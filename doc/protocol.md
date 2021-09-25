# Meegreef Protocol Documentation
**For protocol version 0x1.**

Table Of Contents:
 * [Introduction](#introduction)
 * [Joining a Server](#joining-a-server)
 * [Packet Types](#packet-types)

# Introduction
Meegreef makes use of the [ENet](https://github.com/lsalzman/enet) library for handling packet sending and receiving. Players, in-fact, are bound by their ENet Network Peer, so ENet serves as a crucial component.\
All packets are marked at `ENET_PACKET_FLAG_RELIABLE`.

On-top of that, Meegreef uses [a library](https://github.com/voidah/archive) for serializing packets. Note that I don't actually know at this time *what* the library does, so it's probably safest to just use it.

**NOTE: It's very important to remember the server and client make _very_ little distinction between themselves when sending information, leaving it to a single boolean value. IDs will overlap and things will go disastrously wrong if you forget to check the value!**

# Joining a Server
Joining a server is a simple process.

Once you've established connection with the server, you're expected to identify yourself. Immediately send Packet ID `0x00` (PLAYER_ID).\
The server expects the following information:
 * Your protocol version (Integer)
 * Your desired username (std::string)

After this, the server will test your protocol version and username. If your protocol version mis-matches (note, that you don't get to know what version it _is_ before connecting presently) it will kick you from the server and immediately stop talking to you. Duplicate usernames also kick you. Eventually your username will be tested for illegal characters and then kicked as well.

Anytime you receive a `0x07` (PLAYER_DISCONNECT) the server is confirming to you that it's either kicked or disconnected you, and then **immediately** closes the connection. It's likely by the time you've received this packet, the connection's long been closed.

If your protocol version and username seem fine, the server then sends you its' own information. `0x00` (PLAYER_ID) is then sent, containing:
 * Protocol Version (Integer)
 * Server Name (std::string)
 * Server Description (std::string)
 * OP Status (boolean)

You're free to do with this information what you wish. OP Status simply tells you if you've got elevated privileges, and to enable things like flight, special commands, etc. And to show the user that status. The OP Status is stored on the server and always checked when an action is performed that depends on it.

After that, the server then immediately sends a `0x04` (PLAYER_SPAWN). PLAYER_SPAWN is generic, in that it can apply to not only your player (in such case username is empty) or other players joining the game. If username is a value other than your own, or isn't empty, it's safe to assume the server is notifying you of another player. This is only ever sent on a respawn or new spawn - If the username isn't known to the vanilla client, it will spawn another, else set the position. the `joined` boolean is used to denote whether or not to show a join message, which is used for a new player joining vs. the server sending you already joined players.

After sending you your own position, yaw, etc. the server moves on to sending you every other player. It does not test view distance currently.

The last thing it does is send you the chunk data to <0, 0, 0>.\
**NOTE: The server will always send you the data stored in <0, 0, 0>, regardless of if you're near it or can see it. If you cannot see it, it is safe to discard the chunk information.**

# Packet Types
This is going to be a long list of packet types.\
This information mirrors that in the comments of `src/shared/network/protocol.hpp`.

**NOTE: Order is important!**

## Client -> Server

### 0x00 (PLAYER_ID)
Identifies the player.
| Value | Type | Description |
|-------|------|-------------|
| Protocol Version | Integer | The protocol version your client supports. |
| Desired Username | std::string | The desired username for your client. |

## 0x01 (SET_BLOCK)
Notifies the server we want to make a block change.
| Value | Type | Description |
|-------|------|-------------|
| X | Integer | X Coord |
| Y | Integer | Y Coord |
| Z | Integer | Z Coord |
| Block Type | Integer | The block type we want to set. |

### 0x02 (PLAYER_POSORT)
Tells the server where we think we are.
| Value | Type | Description |
|-------|------|-------------|
| X | Integer | X Coord |
| Y | Integer | Y Coord |
| Z | Integer | Z Coord |
| Pitch | Float | Our view Pitch, in degrees |
| Yaw| Float | Our view Yaw, in degrees |

### 0x03 (CHAT_MESSAGE)
Sends the server a chat message.
| Value | Type | Description |
|-------|------|-------------|
| Message | std::string | The message we want to send. |

### 0x04 (PONG)
**NOTE: Unused**

The immediate response to `0x01` (KEEP_ALIVE).\
Contains no data.

### 0x05 (LEAVE)
Notifies the server we want to leave.\
Contains no data.

It's completely safe to close the connection after this, you don't need to wait for conformation.

### 0x06 (SKIN)
Tells the server what skin we have.\
The server processes this, but we don't recieve any feedback. The server will simply not send players the skin.
| Value | Type | Description |
|-------|------|-------------|
| Skin Data | RGBA Array | Texture Data |

## Server -> Client

### 0x00 (PLAYER_ID)
Confusingly Named. Notifies the client on server information.
| Value | Type | Description |
|-------|------|-------------|
| Protocol Version | Integer | What protocol the server is. |
| Name | std::string | The server's name. |
| Description | std::string | The server's description. |
| OP Status | Boolean | Whether or not the client has elevated privileges. |

### 0x01 (KEEP_ALIVE)
**NOTE: Unused**

Pings the client.\
Contains no data.

### 0x02 (CHUNK_DATA)
Sends chunk data to the client.
| Value | Type | Description |
|-------|------|-------------|
| X | Integer | X Coord |
| Y | Integer | Y Coord |
| Z | Integer | Z Coord |
| Amount of Blocks | Integer | How many blocks are in the chunk. Typically 4,096. |
| chunkData | Integer Array | Should be the same size as Amount Of Blocks, flat array of the block IDs in a chunk. |

### 0x03 (UPDATE_BLOCK)
Set the block at the position.
| Value | Type | Description |
|-------|------|-------------|
| X | Integer | X Coord |
| Y | Integer | Y Coord |
| Z | Integer | Z Coord |
| Block ID | Integer | The block to set |

### 0x04 (PLAYER_SPAWN)
Spawns/Respawns the player.
| Value | Type | Description |
|-------|------|-------------|
| Username | std::string | Username. Blank if yourself. |
| X | Float | X Coord |
| Y | Float | Y Coord |
| Z | Float | Z Coord |
| Pitch | Float | View Pitch |
| Yaw | Float | View Yaw |
| Show Join | Boolean | Whether or not to display a join message. |

### 0x05 (PLAYER_POSORT)
Sets the position of a player.
| Value | Type | Description |
|-------|------|-------------|
| Username | std::string | Username. Blank if yourself. |
| X | Float | X Coord |
| Y | Float | Y Coord |
| Z | Float | Z Coord |
| Pitch | Float | View Pitch |
| Yaw | Float | View Yaw |

### 0x06 (CHAT_MESSAGE)
Somebody's sent a message.
| Value | Type | Description |
|-------|------|-------------|
| Mesasge | std::string | What they said. |
| Username | std::string | Who said it. |

### 0x07 (PLAYER_DISCONNECT)
Sent for both kicking and a confirmation of leaving.
| Value | Type | Description |
|-------|------|-------------|
| Is Kick | Boolean | If this is denoting a kick |
| Reason | std::string | if it's a kick, this is the reason why. |

### 0x08 (TIMEOFDAY)
The world's time of day.
| Value | Type | Description |
|-------|------|-------------|
| Ticks | Integer | The tick time of day |

### 0x09 (PLAYER_LEAVE)
Notifies clients that a player has left, and to delete their model or whatever.
| Value | Type | Description |
|-------|------|-------------|
| Username | std::string | Username of the player leaving. |

### 0x0A (PLAYER_SKIN)
Notifies the client of a custom skin another player may have.
| Value | Type | Description |
|-------|------|-------------|
| Username | std::string | Username of the player. |
| Skin Data | RGBA Array | Texture Data |