// Protocol bby

#pragma once

#include "netarchive.hpp"
#include "world/world.hpp"
#include "archive.h"

#include "enet/enet.h"

#define PROTOCOL_VERSION 0x01

struct NetworkPacket
{
	NetworkPacket() :
		_buf(data),
		_bufAccess(_buf)
	{

	}

	unsigned int type;
	ArchiveIntermediary data;
	ArchiveBuf _buf;
	Archive<ArchiveBuf> _bufAccess;

	template <typename S>
	void serialize(S& s)
	{
		s & type & data & server;
	};

	bool server;
};

// Both ClientPacket and ServerPacket are compiled into the Client/Server
// So that they know how to speak to each other

// *FROM* Client
// *TO* Server
struct ClientPacket : public NetworkPacket
{
	enum type_t
	{
		// Identifies the player
		/*
			{
				clientProtocol,
				setUsername,
			}
		*/
		PLAYER_ID    = 0x00,
		// We wish to set a block
		/*
			{
				X, Y, Z,
				blockID
			}
		*/
		SET_BLOCK    = 0x01,
		// We wish to set our position and orientation
		/*
			{
				X, Y, Z,
				pitch, yaw
			}
		*/
		PLAYERPOSORT = 0x02,
		// Talky time
		/*
			{
				message
			}
		*/
		CHAT_MESSAGE = 0x03,
		// Sent back to the server in response to KEEPALIVE
		/*
			Sorry nothing
		*/
		PONG = 0x04,
	};

	template <typename S>
	void serialize(S& s)
	{
		s & type & data & false;
	};
};

// *FROM* Server
// *TO* Client
struct ServerPacket : public NetworkPacket
{
	enum type_t
	{
		// Some info the player needs from the server
		//! DO NOT CONFUSE WITH ClientPacket::PLAYER_ID
		// TODO: Better name
		/*
			{
				serverProtocol,
				serverName,
				serverDesc,
				isOp
			}
		*/
		PLAYER_ID    = 0x00,
		// Sent occasionally to make sure that other clients are still connected
		// Doesn't contain any data of its' own
		/*
			Sorry nothing
		*/
		KEEPALIVE    = 0x01,
		// Sends the chunk data at X,Y,Z
		// TODO: Compression?
		/*
			{
				X,Y,Z,
				amountOfBlocks (Typically 4096, just how many blocks there are in a chunk),
				chunkData[amountOfBlocks] (1D array of BlockTypes)
			}
		*/
		CHUNKDATA    = 0x02,
		// Updates block at position
		// Used to confirm a block break or to tell the clients that a block has changed due to physics
		// Reserved for singular blocks, if you have modified a large portion of the world (say, water)
		// Then just re-send them the chunk
		/*
			{
				X,Y,Z
				blockType
			}
		*/
		UPDATE_BLOCK = 0x03,
		// Spawn Player
		// Reserved for *FIRST JOIN* or *RESPAWN*
		/*
			{
				X,Y,Z,
				pitch,
				yaw
			}
		*/
		PLAYER_SPAWN = 0x04,
		// When a client sends a message, send this out to all clients (including original sender)
		/*
			{
				playerUsername,
				message
			}
		*/
		PLAYER_MESSAGE = 0x05,
		// Tells the player we no longer serve their kind 'round these parts
		// Can be sent as a kick (just set the flag) or a disconnect confirmation
		// There isn't really much use for the regular disconnect confirmation as 
		/*
			{
				isKick,
				kickReason (string message)
			}
		*/
		PLAYER_DISCONNECT = 0x06
	};

	template <typename S>
	void serialize(S& s)
	{
		s & type & data & true;
	};
};

namespace protocol
{
	void DealWithPacket(ClientPacket *p, void *side);
	void DealWithPacket(ServerPacket *p, void *side);

	// side is a badly named pointer to whoever called this
	// Be them client or server
	// What they're interpereted as is down to the packet's server flag
	void UncompressAndDealWithPacket(ArchiveIntermediary packetData, void *side);

	// Pew pew
	void SendPacket(ENetPeer *peer, ClientPacket &p);
	void SendPacket(ENetPeer *peer, ServerPacket &p);

#ifdef CLIENTEXE
	ServerPacket GetPacketBack(ENetPacket *p);
#elif SERVEREXE
	ClientPacket GetPacketBack(ENetPacket *p);
#endif
}