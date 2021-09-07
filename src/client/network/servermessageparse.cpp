#include "client.hpp"
#include "network/protocol.hpp"

#include "cvar_clientside.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

namespace protocol
{
	void DealWithPacket( NetworkPacket &p, void *side, ENetPeer *peer )
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
				CWorld::PortableChunkRepresentation crep;
				int numBlocks;
				bufAccess >> crep.x;
				bufAccess >> crep.y;
				bufAccess >> crep.z;
				bufAccess >> numBlocks;
				bufAccess >> crep.blocks;

				// con_info("ChunkData at <%d,%d,%d>", crep.x, crep.y, crep.z);
				// con_info("%d Blocks", numBlocks);

				// Woot, data!
				client->localWorld->UsePortable( crep );
			}
			break;

			case ServerPacket::UPDATE_BLOCK: {
				float x, y, z;
				uint blockType;
				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;
				bufAccess >> blockType;

				// Woot, data!
				// TODO: make sure the server isn't being malicious
				CBlock *b = client->localWorld->BlockAtWorldPos( CVector( x, y, z ) );
				if ( b != nullptr )
				{
					con_info( "Update Block At <%f,%f,%f>", x, y, z );
					b->blockType = blocktype_t( blockType );
					b->Update();
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
					client->localPlayer->position = CVector( x, y, z );
					client->localPlayer->rotation = CVector( pitch, yaw, 0 );
				}
				else
				{
					con_info( "Player %s at <%f,%f,%f>", username.c_str(), x, y, z );

					if ( joined )
						client->chatBuffer.push_back( username + " joined." );

					if ( client->localWorld->GetEntityByName( username.c_str() ) != nullptr )
					{
						CEntityPlayer *plyr = (CEntityPlayer *)client->localWorld->GetEntityByName( username.c_str() );
						plyr->position	   = CVector( x, y, z );
						plyr->rotation	   = CVector( pitch, yaw, 0 );
					}
					else
					{
						// New player
						CEntityPlayer *plyr = new CEntityPlayer();
						plyr->position	   = CVector( x, y, z );
						plyr->rotation	   = CVector( pitch, yaw, 0 );
						plyr->name		   = username;

						client->localWorld->AddEntity( plyr );
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
					client->localPlayer->position = CVector( x, y, z );
				}
				else
				{
					if ( client->localWorld->GetEntityByName( username.c_str() ) != nullptr )
					{
						CEntityPlayer *plyr = (CEntityPlayer *)client->localWorld->GetEntityByName( username.c_str() );
						plyr->position	   = CVector( x, y, z );
						plyr->rotation	   = CVector( pitch, yaw, 0 );
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

				client->chatBuffer.push_back( username + ": " + message );
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
					client->Disconnect();
				}
			}
			break;

			case ServerPacket::TIMEOFDAY: {
				int ticks;

				bufAccess >> ticks;

				client->localWorld->timeOfDay = ticks;
			}
			break;

			default: {
				con_error( "Unknown packet of type %#010x", p.type );
			}
			break;
		}
	}
} // namespace protocol