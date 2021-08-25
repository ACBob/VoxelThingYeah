#include "protocol.hpp"

#include "network.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

#ifdef CLIENTEXE
#include "cvar_clientside.hpp"
#elif SERVEREXE
#include "cvar_serverside.hpp"
#endif

namespace protocol
{
	void SendPacket(ENetPeer *peer, ClientPacket &p)
	{
		ArchiveBuf buf;
		Archive<ArchiveBuf> bufAccess(buf);
		p.data = p.buffer.str();
		bufAccess << p;

		ArchiveIntermediary g = buf.str();

		ENetPacket *packet = enet_packet_create(&g.begin()[0], g.size(), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
	}
	void SendPacket(ENetPeer *peer, ServerPacket &p)
	{
		ArchiveBuf buf;
		Archive<ArchiveBuf> bufAccess(buf);
		p.data = p.buffer.str();
		bufAccess << p;

		ArchiveIntermediary g = buf.str();

		ENetPacket *packet = enet_packet_create(&g.begin()[0], g.size(), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
	}

	void UncompressAndDealWithPacket(ArchiveIntermediary packetData, void *side, ENetPeer *peer)
	{
		NetworkPacket p;
		ArchiveBuf buf(packetData);
		Archive<ArchiveBuf> bufAccess(buf);

		bufAccess >> p;

		if (p.server)
			DealWithPacket(reinterpret_cast<ServerPacket*>(&p), side);
		else
			DealWithPacket(reinterpret_cast<ClientPacket*>(&p), side, peer);
	}

#ifdef CLIENTEXE
	void DealWithPacket(ServerPacket *p, void *side)
	{
		ArchiveBuf buf(p->data);
		Archive<ArchiveBuf> bufAccess(buf);

		network::Client *client = reinterpret_cast<network::Client*>(side);

		switch (p->type)
		{
			case ServerPacket::PLAYER_ID:
			{
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
				if (protocolVersion != PROTOCOL_VERSION)
				{
					con_error("Attempt to join unsupported protocol version (%#010x) (Expected %#010x)", protocolVersion, PROTOCOL_VERSION);
					client->Disconnect();
					return;
				}

				con_info("Connected to server %s.", serverName.c_str());


				// TODO: do something with this info
			}
			break;

			case ServerPacket::KEEPALIVE:
			{
				// TODO: Immediately send a message back
				ClientPacket pp;
				pp.type = ClientPacket::PONG;
			}
			break;

			case ServerPacket::CHUNKDATA:
			{
				World::PortableChunkRepresentation crep;
				bufAccess >> crep;

				// Woot, data!
				client->localWorld->UsePortable(crep);
			}
			break;

			case ServerPacket::UPDATE_BLOCK:
			{
				int x, y, z;
				uint blockType;
				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;
				bufAccess >> blockType;

				// Woot, data!
				// TODO: make sure the server isn't being malicious
				Block *b = client->localWorld->BlockAtWorldPos(Vector(x,y,z));
				b->blockType = (blocktype_t)blockType;
				b->Update();
			}
			break;

			case ServerPacket::PLAYER_SPAWN:
			{
				int x, y, z;
				float pitch, yaw;
				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;
				bufAccess >> pitch;
				bufAccess >> yaw;

				// TODO:
			}
			break;

			case ServerPacket::PLAYER_MESSAGE:
			{
				std::string username;
				std::string message;
				bufAccess >> username;
				bufAccess >> message;

				// TODO:
			}
			break;

			case ServerPacket::PLAYER_DISCONNECT:
			{
				bool isKick = false;
				std::string message;

				bufAccess >> isKick;
				bufAccess >> message;

				if (isKick)
				{
					con_warning("Kicked from server for: %s", message.c_str());
					client->Disconnect();
				}
			}
			break;
			
			default:
			{
				con_error("Unknown packet of type %#010x", p->type);
			}
			break;
		}
	}
#elif SERVEREXE
	void DealWithPacket(ServerPacket *p, void *side)
	{
		con_error("DealWithPacket(ServerPacket) called on server!!!");
	}
#endif

#ifdef SERVEREXE
	void DealWithPacket(ClientPacket *p, void *side, ENetPeer *peer)
	{
		ArchiveBuf buf(p->data);
		Archive<ArchiveBuf> bufAccess(buf);

		network::Server *server = reinterpret_cast<network::Server*>(side);

		switch (p->type)
		{
			case ClientPacket::PLAYER_ID:
			{
				// we've been given a few things here, decomp them
				uint protocolVersion = 0x0;
				std::string username;
				bufAccess >> protocolVersion;
				bufAccess >> username;

				// Ideally impossible (as the server should check it itself)
				if (protocolVersion != PROTOCOL_VERSION)
				{
					con_error("User %s attempted to join unsupported protocol version (%#010x) (Expected %#010x)", username.c_str(), protocolVersion, PROTOCOL_VERSION);
					server->KickPlayer(peer, "Protocol version Mismatch!");
					return;
				}
				
				// Send them our info
				ServerPacket pp;
				pp.type = ServerPacket::PLAYER_ID;
				Archive<ArchiveBuf> bufAcc = pp.GetAccess();
				bufAcc << PROTOCOL_VERSION;
				bufAcc << std::string(sv_name->GetString());
				bufAcc << std::string(sv_desc->GetString());
				bufAcc << false;

				SendPacket(peer, pp);

				// TODO: do something with this info
			}
			break;

			case ClientPacket::SET_BLOCK:
			{
				int x, y, z;
				uint blockType;
				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;
				bufAccess >> blockType;

				// TODO:
			}
			break;

			case ClientPacket::PLAYERPOSORT:
			{
				int x, y, z;
				float pitch, yaw;
				bufAccess >> x;
				bufAccess >> y;
				bufAccess >> z;
				bufAccess >> pitch;
				bufAccess >> yaw;

				// TODO:
			}
			break;

			case ClientPacket::CHAT_MESSAGE:
			{
				std::string message;
				bufAccess >> message;

				con_info("%s", message.c_str());

				// TODO:
			}
			break;

			case ClientPacket::PONG:
			{
				// TODO: update player timetolive
			}
			break;

			case ClientPacket::LEAVE:
			{
				ServerPacket p;
				Archive<ArchiveBuf> bufAccess = p.GetAccess();
				p.type = ServerPacket::PLAYER_DISCONNECT;
				
				bufAccess << false;
				bufAccess << std::string("");
				con_info("Goodbye");

				SendPacket(peer, p);
			}
			break;

			default:
			{
				con_error("Unknown packet of type %#010x", p->type);
			}
			break;
		}
	}
#elif CLIENTEXE
	void DealWithPacket(ClientPacket *p, void *side, ENetPeer *peer)
	{
		con_error("DealWithPacket(ClientPacket) called on client!!!");
	}
#endif
}