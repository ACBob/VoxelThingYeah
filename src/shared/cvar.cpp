#include "cvar.hpp"

#include "logging.hpp"

#include <cstring>
#include <memory>
#include <cstdlib>

#include <string>

#include "types.hpp"

#include "filesystem.hpp"

// Global Convar Handler
ConVar::CConVarHandler conVarHandle;

namespace ConVar
{
	CConVar::CConVar( const char *name, const char *defval, int flags )
	{
		m_cName	  = name;
		m_cVal	  = defval;
		m_cDefVal = defval;
		m_iFlags  = flags;
		m_bModified = false;
	}

	void CConVar::SetString( const char *val )
	{
		char *cval = new char[strlen( val ) + 1]();
		strcpy( cval, val );

		m_bModified = ( strcmp( cval, m_cDefVal ) != 0 );
		m_cVal		= cval;
	}
	void CConVar::SetInt( int val )
	{
		char buf[256];
		snprintf( buf, 256, "%d", val );
		SetString( buf );
	}
	void CConVar::SetFloat( float val )
	{
		char buf[256];
		snprintf( buf, 256, "%f", val );
		SetString( buf );
	}
	void CConVar::SetBool( bool val ) { SetString( val ? "true" : "false" ); }

	const char *CConVar::GetString() { return m_cVal; }
	int CConVar::GetInt() { return atoi( m_cVal ); }
	float CConVar::GetFloat() { return atof( m_cVal ); }
	bool CConVar::GetBool() { return ( strcmp( m_cVal, "true" ) == 0 ); }

	CConVarHandler::CConVarHandler() { Cvars = {}; }
	CConVarHandler::~CConVarHandler()
	{
		for ( auto &c : Cvars )
			delete c.second;
	}

	void CConVarHandler::WriteCFG()
	{
		std::string out;

		for ( auto &c : Cvars )
		{
			if ( !c.second->ShouldArchive() )
				continue;
			if ( !c.second->IsModified() )
				continue;

			out += c.first;
			out += ' ';
			out += c.second->GetString();
			out += '\n';
		}

		bool bSuccess = false;
		fileSystem::WriteFile( "config.cfg", (uchar_t *)out.c_str(), out.size(), bSuccess );
		if ( !bSuccess )
			con_error( "Failed to write config.cfg..." );
	}

	CConVar *CConVarHandler::DeclareConvar( const char *name, const char *defVal, int flags )
	{
		CConVar *c	= new CConVar( name, defVal, flags );
		Cvars[name] = c;

		return c;
	}

	CConVar *CConVarHandler::FindConVar( const char *name ) { return Cvars[name]; }

	void CConVarHandler::Parse( const char *str )
	{
		// Don't try parsing empty string
		if ( strlen( str ) == 0 )
			return;

		char *in = new char[strlen( str ) + 1];
		strcpy( in, str );

		char *token	 = new char[1];
		token[0]	 = '\0';
		char *oToken = new char[1];
		oToken[0]	 = '\0';
		char *saveptr;
		const char sep[4] = "\n; ";
		token			  = strtok_r( in, sep, &saveptr );

		while ( token != NULL )
		{
			con_debug( "%s", token );

			if ( oToken != nullptr && strlen( token ) && strlen( oToken ) )
			{
				ParseConvarTokens( oToken, token );

				if ( oToken != nullptr )
				{
					delete[] oToken;
					oToken = nullptr;
				}

				token = strtok_r( NULL, sep, &saveptr );
				continue;
			}

			if ( oToken != nullptr )
			{
				delete[] oToken;
				oToken = nullptr;
			}
			oToken = new char[strlen( token ) + 1];
			strcpy( oToken, token );

			token = strtok_r( NULL, sep, &saveptr );
		}

		delete[] oToken;
		delete[] token;
		delete[] in;
	}

	void CConVarHandler::ParseConvarTokens( const char *cmd, const char *args )
	{
		// It's empty, bum run, don't do anything
		if ( strlen( cmd ) == 0 && strlen( args ) == 0 )
			return;

		con_debug( "SET %s TO %s", cmd, args );

		if ( Cvars.find(cmd) == Cvars.end() )
		{
			con_error( "Unknown ConVar %s", cmd );
		}
		else
		{
			Cvars[cmd]->SetString( args );
		}
	}
} // namespace ConVar