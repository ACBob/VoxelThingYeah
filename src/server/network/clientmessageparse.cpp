#include "network/protocol.hpp"
#include "server.hpp"

#include "cvar_serverside.hpp"

#include "logging.hpp"

#include "specialeffectdef.hpp"

#include "blocks/blockbase.hpp"

namespace protocol
{
	void DealWithPacket( NetworkPacket &p, void *pSide, ENetPeer *pPeer )
	{
		ArchiveBuf buf( p.data );
		Archive<ArchiveBuf> bufAccess( buf );

		CNetworkServer *pServer = reinterpret_cast<CNetworkServer *>( pSide );

		switch ( p.type )
		{
			case ClientPacket::PLAYER_ID: {
				CNetworkPlayer *c = new CNetworkPlayer{ pPeer, nullptr };
				pServer->m_players.push_back( c );

				// we've been given a few things here, decomp them
				uint protocolVersion = 0x0;
				std::string username;
				bufAccess >> protocolVersion;
				bufAccess >> username;

				con_info( "user %s joined", username.c_str() );

				// Ideally impossible (as the server should check it itself)
				if ( protocolVersion != PROTOCOL_VERSION )
				{
					con_error( "User %s attempted to join unsupported protocol version (%#010x) (Expected %#010x)",
							   username.c_str(), protocolVersion, PROTOCOL_VERSION );
					pServer->KickPlayer( pPeer, "Protocol version Mismatch!" );
					return;
				}
				if ( pServer->ClientFromUsername( username.c_str() ) != nullptr )
				{
					con_error( "Duplicate username %s!", username.c_str() );
					pServer->KickPlayer( pPeer, "Someone already has your username. Change it!" );
					return;
				}

				// Send them our info
				SendServerPlayerID( pPeer, false );

				// Create an entity for them
				CEntityPlayer *p = new CEntityPlayer();
				pServer->m_world.AddEntity( p );
				c->m_pEntity  = p;
				c->m_username = username;
				p->m_name	  = c->m_username;

				// Then send it to spawn
				int x = 8 + rand() % 8;
				int z = 8 + rand() % 8;

				p->m_vPosition = Vector3f( x, 20, z );
				p->m_vRotation = Vector3f( 0, 0, 0 );

				SendServerPlayerSpawn( pPeer, "", p->m_vPosition, p->m_vRotation, false );

				for ( CNetworkPlayer *c : pServer->m_players )
				{
					if ( c->m_pPeer == pPeer )
						continue;
					SendServerPlayerSpawn( c->m_pPeer, p->m_name, p->m_vPosition, p->m_vRotation, true );

					SendServerPlayerSpawn( pPeer, c->m_pEntity->m_name, c->m_pEntity->m_vPosition,
										   c->m_pEntity->m_vRotation, false );
					if ( c->m_skinData.size() )
						SendServerPlayerSkin( pPeer, c->m_username, c->m_skinData, c->m_skinRes );
				}

				// Now send them 0,0
				SendServerChunkData( pPeer, &pServer->m_world, Vector3f( 0, 0, 0 ) );
			}
			break;

			case ClientPacket::SET_BLOCK: {
				float x, y, z;
				uint blockType;
				uint16_t val;
				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;
				bufAccess >> blockType;
				bufAccess >> val;

				CBlock *b = pServer->m_world.BlockAtWorldPos( Vector3f( x, y, z ) );

				BLOCKID oldBlockType = b->m_iBlockType;
				if ( true ) // If it's a valid block placement (for now no check)
				{
					b->m_iBlockType = (BLOCKID)blockType;
					b->m_iBlockData	= val;
					b->Update();

					for ( CNetworkPlayer *c : pServer->m_players )
					{
						SendServerUpdateBlock( c->m_pPeer, Vector3f( x, y, z ), BLOCKID( blockType ), val );

						if ( blockType == AIR )
							SendServerSpecialEffect( c->m_pPeer, Vector3f( x, y, z ).Floor(), SPECIALEFFECT_BLOCKBREAK,
													 oldBlockType );
						else
							SendServerSpecialEffect( c->m_pPeer, Vector3f( x, y, z ).Floor(), SPECIALEFFECT_BLOCKPLACE,
													 blockType );
					}
				}
				else
				{
					SendServerUpdateBlock( pPeer, Vector3f( x, y, z ), b->m_iBlockType, b->m_iBlockData );
				}
			}
			break;

			case ClientPacket::PLAYERPOSORT: {
				float x, y, z;
				float pitch, yaw;
				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;
				bufAccess >> pitch;
				bufAccess >> yaw;

				CEntityPlayer *p = pServer->ClientFromPeer( pPeer )->m_pEntity;
				p->m_vPosition	 = Vector3f( x, y, z );
				p->m_vRotation	 = Vector3f( pitch, yaw, 0 );

				for ( CNetworkPlayer *c : pServer->m_players )
				{
					std::string name = p->m_name;
					if ( c->m_pPeer == pPeer )
					{
						name = "";

						// TODO: check if position is legal
						if ( true )
							continue;
					}
					SendServerPlayerPos( c->m_pPeer, name, p->m_vPosition, p->m_vRotation );
				}
			}
			break;

			case ClientPacket::CHATMESSAGE: {
				std::string message;
				bufAccess >> message;

				std::string username = pServer->ClientFromPeer( pPeer )->m_username;

				con_info( "%s: %s", username.c_str(), message.c_str() );

				for ( CNetworkPlayer *c : pServer->m_players )
				{
					SendServerPlayerMessage( c->m_pPeer, username, message );
				}
			}
			break;

			case ClientPacket::PONG: {
				// TODO: update player timetolive
			}
			break;

			case ClientPacket::LEAVE: {
				con_info( "Goodbye" );
				SendServerPlayerDisconnect( pPeer, false );
			}
			break;

			case ClientPacket::SKIN: { // TODO: Validate to some degree (i.e max resolution)
				std::vector<unsigned char> imageData;
				unsigned int skinRes;
				bufAccess >> imageData;
				bufAccess >> skinRes;

				CNetworkPlayer *plyr = pServer->ClientFromPeer( pPeer );
				con_info( "%s has a skin", plyr->m_username.c_str() );

				plyr->m_skinData = imageData;
				plyr->m_skinRes	 = skinRes;

				for ( CNetworkPlayer *c : pServer->m_players )
				{
					if ( c == plyr )
						continue;

					SendServerPlayerSkin( c->m_pPeer, plyr->m_username, plyr->m_skinData, plyr->m_skinRes );
				}
			}
			break;
			case ClientPacket::USE_BLOCK: {
				int x, y, z;
				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;

				// test if the block is useable
				CBlock *b = pServer->m_world.BlockAtWorldPos( Vector3f( x, y, z ) );
				if ( b != nullptr && BlockType(b->m_iBlockType).CanBeUsed() )
					BlockType(b->m_iBlockType).OnUse( (CChunk*)b->m_pChunk, Vector3f(x, y, z), pServer->ClientFromPeer(pPeer)->m_pEntity );
			}
			break;

			default: {
				// Kick player for unknown packet
				con_warning( "Kicking player for unknown packet %#X", p.type );
				char *reason = new char[128];
				sprintf( reason, "Unknown packet %#X", p.type );
				pServer->KickPlayer( pPeer, reason );
				delete[] reason;
			}
			break;
		}
	}
} // namespace protocol