#include "client.hpp"
#include "network/protocol.hpp"

#include "cvar_clientside.hpp"

#include "logging.hpp"

#include "specialeffectdef.hpp"

namespace protocol
{
	void DealWithPacket( NetworkPacket &p, void *side, ENetPeer *pPeer )
	{
		ArchiveBuf buf( p.data );
		Archive<ArchiveBuf> bufAccess( buf );

		CNetworkClient *client = reinterpret_cast<CNetworkClient *>( side );

		switch ( p.type )
		{
			case ServerPacket::PLAYER_ID: {
				// we've been given a few things here, decomp them
				uint protocolVersion = 0x0;
				std::string serverName;
				std::string serverDesc;
				bool isOp = false;
				bufAccess >> protocolVersion;
				bufAccess >> serverName;
				bufAccess >> serverDesc;
				bufAccess >> isOp;

				// Ideally impossible (as the server should check it itself)
				if ( protocolVersion != PROTOCOL_VERSION )
				{
					con_error( "Attempt to join unsupported protocol version (%#010x) (Expected %#010x)",
							   protocolVersion, PROTOCOL_VERSION );
					client->Disconnect();
					return;
				}

				con_info( "Connected to server %s.", serverName.c_str() );
				cl_servername->SetString( serverName.c_str() );

				// TODO: do something with this info
			}
			break;

			case ServerPacket::KEEPALIVE: {
				// TODO: Immediately send a message back
				ClientPacket pp;
				pp.type = ClientPacket::PONG;
			}
			break;

			case ServerPacket::CHUNKDATA: {
				ChunkData crep;
				int numBlocks;
				bufAccess >> crep.x;
				bufAccess >> crep.y;
				bufAccess >> crep.z;
				bufAccess >> numBlocks;
				bufAccess >> crep.m_iBlocks;
				bufAccess >> crep.m_iValue;

				// con_debug("ChunkData at <%d,%d,%d>", crep.x, crep.y, crep.z);
				// con_info("%d Blocks", numBlocks);

				// Woot, data!
				client->m_pLocalWorld->LoadFromData( crep );
			}
			break;

			case ServerPacket::UPDATE_BLOCK: {
				float x, y, z;
				uint16_t val;
				uint blockType;
				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;
				bufAccess >> blockType;
				bufAccess >> val;

				// Woot, data!
				// TODO: make sure the server isn't being malicious.. Somehow
				BLOCKID b = std::get<0>(client->m_pLocalWorld->GetBlockAtWorldPos( CVector( x, y, z ) ));
				if ( b != BLCK_NONE )
				{
					// con_info( "Update Block At <%f,%f,%f>", x, y, z );
					client->m_pLocalWorld->SetBlockAtWorldPos( CVector( x, y, z), (BLOCKID)blockType, val );
				}
			}
			break;

			case ServerPacket::PLAYER_SPAWN: {
				std::string username;
				float x, y, z;
				float pitch, yaw;
				bool joined;
				bufAccess >> username;
				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;
				bufAccess >> pitch;
				bufAccess >> yaw;
				bufAccess >> joined;

				// Empty username is taken to mean us
				if ( username == "" )
				{
					// Then it's us
					con_info( "Spawning at <%f,%f,%f> <%f,%f>", x, y, z, pitch, yaw );
					client->m_pLocalPlayer->m_vPosition = CVector( x, y, z );
					client->m_pLocalPlayer->m_vRotation = CVector( pitch, yaw, 0 );
				}
				else
				{
					con_info( "Player %s at <%f,%f,%f>", username.c_str(), x, y, z );

					if ( joined )
						client->m_chatBuffer.push_back( username + " joined." );

					if ( client->m_pLocalWorld->GetEntityByName( username.c_str() ) != nullptr )
					{
						CEntityPlayer *plyr =
							(CEntityPlayer *)client->m_pLocalWorld->GetEntityByName( username );
						plyr->m_vPosition = CVector( x, y, z );
						plyr->m_vRotation = CVector( pitch, yaw, 0 );
					}
					else
					{
						// New player
						CEntityPlayer *plyr = (CEntityPlayer*)client->m_pLocalWorld->AddEntity( std::make_unique<CEntityPlayer>() );
						plyr->m_vPosition	= CVector( x, y, z );
						plyr->m_vRotation	= CVector( pitch, yaw, 0 );
						plyr->m_name		= username;

					}
				}
			}
			break;

			case ServerPacket::PLAYERPOSORT: {
				std::string username;
				float x, y, z;
				float pitch, yaw;
				bufAccess >> username;
				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;
				bufAccess >> pitch;
				bufAccess >> yaw;

				// Empty username is taken to mean us
				if ( username == "" )
				{
					client->m_pLocalPlayer->m_vPosition = CVector( x, y, z );
				}
				else
				{
					if ( client->m_pLocalWorld->GetEntityByName( username.c_str() ) != nullptr )
					{
						CEntityPlayer *plyr =
							(CEntityPlayer *)client->m_pLocalWorld->GetEntityByName( username.c_str() );
						plyr->m_vPosition = CVector( x, y, z );
						plyr->m_vRotation = CVector( pitch, yaw, 0 );
					}
				}
			}
			break;

			case ServerPacket::CHATMESSAGE: {
				std::string username;
				std::string message;
				bufAccess >> username;
				bufAccess >> message;

				con_info( "%s: %s", username.c_str(), message.c_str() );

				client->m_chatBuffer.push_back( username + ": " + message );
			}
			break;

			case ServerPacket::PLAYER_DISCONNECT: {
				bool isKick = false;
				std::string message;

				bufAccess >> isKick;
				bufAccess >> message;

				if ( isKick )
				{
					con_warning( "Kicked from server for: %s", message.c_str() );
					cl_kickreason->SetString( message.c_str() );
					client->Disconnect();
				}
			}
			break;

			case ServerPacket::TIMEOFDAY: {
				int ticks;

				bufAccess >> ticks;

				client->m_pLocalWorld->m_iWorldTime = ticks;
			}
			break;
			case ServerPacket::PLAYERLEAVE: {
				std::string username;
				bufAccess >> username;

				if ( username == "" || username == client->m_pLocalPlayer->m_name )
				{
					con_warning( "Attempt from server to PLAYERLEAVE us...?" );
					return;
				}

				CEntityPlayer *e = (CEntityPlayer *)client->m_pLocalWorld->GetEntityByName( username.c_str() );
				if ( e != nullptr )
				{
					e->m_bIsKilled = true;
					client->m_chatBuffer.push_back( username + " has left the game." );
				}
			}
			break;
			case ServerPacket::PLAYERSKIN: {
				std::string username;
				std::vector<unsigned char> imageData;
				unsigned int resolution = 0;

				bufAccess >> username;
				bufAccess >> imageData;
				bufAccess >> resolution;

				con_info( "Handling %s' skin", username.c_str() );

				CEntityPlayer *e = (CEntityPlayer *)client->m_pLocalWorld->GetEntityByName( username.c_str() );
				if ( e != nullptr )
				{
					e->m_pMdl->SetTexture( materialSystem::LoadTexture( imageData, resolution * 8, resolution * 8 ) );
				}
			}
			break;
			case ServerPacket::SPECIALEFFECT: {
				float x, y, z;
				int effectId;
				int effectAttrib;

				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;
				bufAccess >> effectId;
				bufAccess >> effectAttrib;

				client->SpecialEffectHandle( CVector( x, y, z ), (SpecialEffect)effectId, effectAttrib );
			}
			break;

			default: {
				con_error( "Unknown packet of type %#010x", p.type );
			}
			break;
		}
	}
} // namespace protocol