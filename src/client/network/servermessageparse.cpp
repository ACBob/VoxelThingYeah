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

		con_critical("Protocol not implemented!");
	}
} // namespace protocol