#include "cvar.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

#include <cstring>
#include <memory>

// Global Convar Handler
ConVar::CConVarHandler conVarHandle;

namespace ConVar
{
	CConVar::CConVar( const char *name, const char *defval, int flags )
	{
		this->name	 = name;
		this->val	 = defval;
		this->defVal = defval;
		this->flags	 = flags;
	}

	void CConVar::SetString( const char *val )
	{
		char *cval = new char[strlen( val ) + 1];
		strcpy( cval, val );
		cval[strlen( val ) + 1] = '\0';

		modified  = ( strcmp( cval, defVal ) != 0 );
		this->val = cval;
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

	const char *CConVar::GetString() { return this->val; }
	int CConVar::GetInt() { return atoi( this->val ); }
	float CConVar::GetFloat() { return atof( this->val ); }
	bool CConVar::GetBool() { return ( strcmp( this->val, "true" ) != 0 ); }

	CConVarHandler::CConVarHandler() { Cvars = {}; }
	CConVarHandler::~CConVarHandler()
	{
		for ( auto &c : Cvars )
			delete c.second;
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

		char *in = new char[strlen( str )];
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
			oToken[strlen( token ) + 1] = '\0';

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

		CConVar *conv = Cvars[cmd];
		if ( conv == nullptr )
		{
			con_error( "Unknown ConVar %s", cmd );
		}
		else
		{
			conv->SetString( args );
		}
	}
} // namespace ConVar