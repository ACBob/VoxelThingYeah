#include "network/protocol.hpp"
#include "server.hpp"

#include "cvar_serverside.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

namespace protocol
{
	void DealWithPacket( NetworkPacket &p, void *side, ENetPeer *peer )
	{
		ArchiveBuf buf( p.data );
		Archive<ArchiveBuf> bufAccess( buf );

		NetworkServer *server = reinterpret_cast<NetworkServer *>( side );

		switch ( p.type )
		{
			case ClientPacket::PLAYER_ID: {
				NetworkPlayer *c = new NetworkPlayer{ peer, nullptr };
				server->players.push_back( c );

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
					server->KickPlayer( peer, "Protocol version Mismatch!" );
					return;
				}
				if ( server->ClientFromUsername( username.c_str() ) != nullptr )
				{
					con_error( "Duplicate username %s!", username.c_str() );
					server->KickPlayer( peer, "Someone already has your username. Change it!" );
					return;
				}

				// Send them our info
				SendServerPlayerID( peer, false );

				// Create an entity for them
				EntityPlayer *p = new EntityPlayer();
				server->world.AddEntity( p );
				c->entity	= p;
				c->username = username;
				p->name		= c->username;

				// Then send it to spawn
#ifdef __linux__
				int x = 8 + random() % 8;
				int z = 8 + random() % 8;
#elif _WIN32
				int x = 8 + rand() % 8;
				int z = 8 + rand() % 8;
#endif
				p->position = Vector( x, 10, z );
				p->rotation = Vector( 0, 0, 0 );

				SendServerPlayerSpawn( peer, "", p->position, p->rotation, false );

				for ( NetworkPlayer *c : server->players )
				{
					if ( c->peer == peer )
						continue;
					SendServerPlayerSpawn( c->peer, p->name, p->position, p->rotation, true );
					SendServerPlayerSpawn( peer, c->entity->name, c->entity->position, c->entity->rotation, false );
				}

				// Now send them 0,0
				SendServerChunkData( peer, &server->world, Vector( 0, 0, 0 ) );
			}
			break;

			case ClientPacket::SET_BLOCK: {
				float x, y, z;
				uint blockType;
				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;
				bufAccess >> blockType;

				Block *b = server->world.BlockAtWorldPos( Vector( x, y, z ) );
				if ( true ) // If it's a valid block placement (for now no check)
				{
					b->blockType = (blocktype_t)blockType;
					con_info( "Update Block At <%f,%f,%f>", x, y, z );
					b->Update();
				}
				blockType = b->blockType;

				for ( NetworkPlayer *c : server->players )
				{
					SendServerUpdateBlock( c->peer, Vector( x, y, z ), blocktype_t( blockType ) );
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

				EntityPlayer *p = server->ClientFromPeer( peer )->entity;
				p->position		= Vector( x, y, z );
				p->rotation		= Vector( pitch, yaw, 0 );

				for ( NetworkPlayer *c : server->players )
				{
					std::string name = p->name;
					if ( c->peer == peer )
					{
						name = "";

						// TODO: check if position is legal
						if ( true )
							continue;
					}
					SendServerPlayerPos( c->peer, name, p->position, p->rotation );
				}
			}
			break;

			case ClientPacket::CHATMESSAGE: {
				std::string message;
				bufAccess >> message;

				std::string username = server->ClientFromPeer( peer )->username;

				con_info( "%s: %s", username.c_str(), message.c_str() );

				for ( NetworkPlayer *c : server->players )
				{
					SendServerPlayerMessage( c->peer, username, message );
				}
			}
			break;

			case ClientPacket::PONG: {
				// TODO: update player timetolive
			}
			break;

			case ClientPacket::LEAVE: {
				con_info( "Goodbye" );
				SendServerPlayerDisconnect( peer, false );
			}
			break;

			default: {
				con_error( "Unknown packet of type %#010x", p.type );
			}
			break;
		}
	}
} // namespace protocol