#include "network.hpp"

#include "logging.hpp"

namespace network
{
	bool Init()
	{
		if ( enet_initialize() != 0 )
		{
			con_error( "ENet failed to initialise" );
			return false;
		}
		return true;
	}
	void Uninit() { enet_deinitialize(); }
} // namespace network
