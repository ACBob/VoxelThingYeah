#include "client.hpp"

#include "logging.hpp"

#include "sound/soundmanager.hpp"

CNetworkClient::CNetworkClient()
{
	m_pEnetHost = enet_host_create( NULL, 1, 1, 0, 0 );

	if ( m_pEnetHost == NULL )
	{
		con_error( "Couldn't create ENet client object" );
		return;
	}

	if ( enet_host_compress_with_range_coder( m_pEnetHost ) != 0 )
	{
		con_error( "Couldn't enable compression!" );
		return;
	}
}
CNetworkClient::~CNetworkClient() { enet_host_destroy( m_pEnetHost ); }

bool CNetworkClient::WorkingClient() { return m_pEnetHost != NULL; }

bool CNetworkClient::Connect( const char *address, int port )
{
	if ( enet_address_set_host_ip( &m_addr, address ) != 0 )
	{
		con_error( "Could not set the IP" );
		return false;
	}
	m_addr.port = port;

	con_info( "Connecting to %s:%i", address, port );

	m_pPeer = enet_host_connect( m_pEnetHost, &m_addr, 1, 0 );
	if ( !m_pPeer )
	{
		con_error( "No peers available." );
		return false;
	}

	con_info( "We've found a peer!" );
	con_info( "Giving server 2.5 seconds to respond..." );
	if ( enet_host_service( m_pEnetHost, &e, 2500 ) > 0 && e.type == ENET_EVENT_TYPE_CONNECT )
	{
		con_info( "Hello! We've connected to a server!" );
		con_info( "Sending the neccesary information" );
		protocol::SendClientPlayerID( e.peer );
		m_pPeer		 = e.peer;
		m_bConnected = true;
		return true;
	}
	else
	{
		con_error( "Nobody said hello back :(" );
		enet_peer_reset( m_pPeer );
		m_pPeer = nullptr;
		return false;
	}
}
void CNetworkClient::Disconnect()
{
	if ( !m_bConnected || m_pPeer == nullptr )
	{
		con_warning( "Disconnect without connection" );
		return;
	}
	con_info( "Disconnecting" );
	protocol::SendClientLeave( m_pPeer );

	while ( enet_host_service( m_pEnetHost, &e, 0 ) > 0 )
	{
		switch ( e.type )
		{
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy( e.packet );
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				con_info( "Disconnected. Bye bye server! :)" );
				break;
		}
	}

	enet_peer_reset( m_pPeer );
	m_pPeer		 = nullptr;
	m_bConnected = false;
}

void CNetworkClient::SpecialEffectHandle( Vector3f pos, SpecialEffect specialEffect, int attrib )
{
	if ( m_pParticleMan == nullptr )
		return;

	switch ( specialEffect )
	{
		case SPECIALEFFECT_BLOCKBREAK: {
			// Attrib is taken to be the block id
			BLOCKID b = (BLOCKID)attrib;

			// Air has no break effect
			if ( b == AIR )
				return;

			BlockTexture tex = GetDefaultBlockTextureSide( b, NORTH );

			ParticleDef blockBreak = PARTICLE_BREAKBLOCK;
			// TODO: these aren't small pieces of the block anymore, they are the block
			blockBreak.vUVOffsetMin = { tex.z / 64.0f, tex.w / 64.0f, tex.x / 16.0f, tex.y / 16.0f };
			blockBreak.vUVOffsetMax = { tex.z / 64.0f, tex.w / 64.0f, tex.x / 16.0f + 1/32.0f, tex.y / 16.0f + 1/32.0f };
			blockBreak.pTexture = materialSystem::LoadTexture( "terrain.png" );

			soundSystem::PlayBreakSound( b, pos + Vector3f( 0.5, 0.5, 0.5 ) );

			for ( int x = 0; x < 4; x++ )
				for ( int y = 0; y < 4; y++ )
					for ( int z = 0; z < 4; z++ )
						m_pParticleMan->CreateParticle( pos + Vector3f( x / 4.0f, y / 4.0f, z / 4.0f ), blockBreak );
		}
		break;
		case SPECIALEFFECT_BLOCKPLACE: {
			// Attrib is taken to be the block id
			BLOCKID b = (BLOCKID)attrib;

			// Air has no place effect
			if ( b == AIR )
				return;
			soundSystem::PlayPlaceSound( b, pos + Vector3f( 0.5, 0.5, 0.5 ) );
		}
		break;
		default:
			con_error( "Unknown Effect %#010x!", specialEffect );
			break;
	}
}

void CNetworkClient::Update()
{
	if ( !m_bConnected )
		return;

	ENetEvent e;
	if ( !m_pPeer )
		return; // Can't update w/out a pPeer

	while ( enet_host_service( m_pEnetHost, &e, 0 ) > 0 )
	{
		switch ( e.type )
		{
			case ENET_EVENT_TYPE_RECEIVE: {
				protocol::UncompressAndDealWithPacket(
					ArchiveIntermediary( e.packet->data, e.packet->data + e.packet->dataLength ), this, m_pPeer );
			}
			break;
			case ENET_EVENT_TYPE_DISCONNECT: {
				con_info( "Oh, the server has said goodbye... :(" );
				Disconnect();
				return;
			}
			break;
		}
	}

	// We may not have a local world and yet still be connected.
	if ( m_pLocalPlayer != nullptr )
	{
		Vector3f cP = ( m_pLocalPlayer->m_vPosition / Vector3f( CHUNKSIZE_X, CHUNKSIZE_Y, CHUNKSIZE_Z ) ).Floor();
		// MAGIC NUMBER: 4 == render distance
		// TODO: tie to cvar
		m_pLocalWorld->m_chunks.erase(
			std::remove_if( m_pLocalWorld->m_chunks.begin(), m_pLocalWorld->m_chunks.end(),
							[cP]( auto &&c ) { return ( ( cP - c.get()->m_vPosition ).Magnitude() > 6 ); } ),
			m_pLocalWorld->m_chunks.end() );
	}
}
