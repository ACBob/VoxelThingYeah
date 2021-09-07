// Protocol bby

#pragma once

#include "netarchive.hpp"
#include "world/world.hpp"
#include "archive.h"

#include "enet/enet.h"

#define PROTOCOL_VERSION 0x01

struct NetworkPacket
{
	unsigned int type;
	ArchiveBuf buffer;
	ArchiveIntermediary data;

	Archive<ArchiveBuf> GetAccess()
	{
		return Archive<ArchiveBuf>(buffer);
	}

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
				X,Y,Z,
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
		CHATMESSAGE = 0x03,
		// Sent back to the server in response to KEEPALIVE
		/*
			Sorry nothing
		*/
		PONG = 0x04,
		// Sent to the server to say goodbye
		/*
			Sorry nothing
		*/
		LEAVE = 0x05,
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
				playerUsername,
				X,Y,Z, (floats)
				pitch, yaw (floats)
			}
		*/
		PLAYER_SPAWN = 0x04,
		// Move Pkl
		// Reserved for *FIRST JOIN* or *RESPAWN*
		/*
			{
				playerUsername,
				X,Y,Z, (floats)
				pitch, yaw, (floats)
				joined (True if the player joined the game)
			}
		*/
		PLAYERPOSORT = 0x05,
		// When a client sends a message, send this out to all clients (including original sender)
		/*
			{
				playerUsername,
				message
			}
		*/
		CHATMESSAGE = 0x06,
		// Tells the player we no longer serve their kind 'round these parts
		// Can be sent as a kick (just set the flag) or a disconnect confirmation
		// There isn't really much use for the regular disconnect confirmation as 
		/*
			{
				isKick,
				kickReason (string message)
			}
		*/
		PLAYER_DISCONNECT = 0x07,
		// Tells the time of day to the client
		// Without this time does not change on individual clients
		/*
			{
				ticks
			}
		*/
		TIMEOFDAY = 0x08
	};

	template <typename S>
	void serialize(S& s)
	{
		s & type & data & true;
	};
};

namespace protocol
{
	void DealWithPacket(NetworkPacket &p, void *side, ENetPeer *peer);

	// side is a badly named pointer to whoever called this
	// Be them client or server
	// What they're interpereted as is down to the packet's server flag
	void UncompressAndDealWithPacket(ArchiveIntermediary packetData, void *side, ENetPeer *p = nullptr);

	// Pew pew
	void SendPacket(ENetPeer *peer, Packet &p);

	/****************************************************/
	/******************* SERVER *************************/
	/****************************************************/
	void SendServerPlayerID           (ENetPeer *peer, bool isOp);
	void SendServerChunkData          (ENetPeer *peer, World *world, Vector pos);
	void SendServerChunkDataFromRep   (ENetPeer *peer, World::PortableChunkRepresentation crep);
	void SendServerUpdateBlock        (ENetPeer *peer, Vector pos, blocktype_t blockType);
	void SendServerPlayerSpawn        (ENetPeer *peer, std::string username, Vector pos, Vector rot, bool join);
	void SendServerPlayerPos          (ENetPeer *peer, std::string username, Vector pos, Vector rot);
	void SendServerPlayerMessage      (ENetPeer *peer, std::string username, std::string message);
	void SendServerPlayerDisconnect   (ENetPeer *peer, bool isKick, std::string reason = "");
	void SendServerTimeOfDay          (ENetPeer *peer, int ticks);
	
	/****************************************************/
	/******************* CLEINT *************************/
	/****************************************************/
	void SendClientPlayerID           (ENetPeer *peer);
	void SendClientSetBlock           (ENetPeer *peer, Vector pos, blocktype_t blockType);
	void SendClientPlayerPos          (ENetPeer *peer, Vector pos, Vector rot);
	void SendClientChatMessage        (ENetPeer *peer, std::string message);
	void SendClientLeave              (ENetPeer *peer);
}