// Quake-Style Console Variable System
#include <map>

#include <cstring>

#pragma once
namespace ConVar
{
	enum ConVarFlag {
		// 0
		CVAR_NOFLAGS = 0,
		// Save the value of the variable between runs
		CVAR_ARCHIVE = 1 << 0,
		// Do not allow the player to set, used for stuff like server info in the client or hardware info
		CVAR_READONLY = 1 << 1,
		// Keep until a restart of the server (or client in the scope of it)
		CVAR_SESSION = 1 << 2,
		// Cheat only
		CVAR_CHEAT = 1 << 3,
	};

	// Shorthand
	typedef ConVarFlag F;

	class ConVar
	{
	  public:
		ConVar( const char *name, const char *defval, int flags );

		// Gets value as (c) string
		const char *GetString();
		// Gets value as float
		float GetFloat();
		// Gets value as integer
		int GetInt();
		// Gets value as boolean
		bool GetBool();

		// Sets value as (c) string
		void SetString( const char *val );
		// Sets value as float
		void SetFloat( float val );
		// Sets value as integer
		void SetInt( int val );
		// Sets value as boolean
		void SetBool( bool val );

	  private:
		int flags;

		// Does the value differ from its' regular one
		bool modified;

		// Converted from to other types :trollface:
		const char *val;
		const char *defVal;

		// Name
		const char *name;
	};

	class ConVarHandler
	{
	  private:
		// Name -> ConVar

		// https://stackoverflow.com/a/4157729
		struct cmp_str
		{
			bool operator()( const char *a, const char *b ) const { return strcmp( a, b ) < 0; }
		};

		std::map<const char *, ConVar *, cmp_str> Cvars;

		void ParseConvarTokens( const char *cmd, const char *arg );

	  public:
		ConVarHandler();
		~ConVarHandler();

		ConVar *DeclareConvar( const char *name, const char *defVal, int flags = F::CVAR_NOFLAGS );

		// Find Cvar by name
		ConVar *FindConVar( const char *name );

		ConVar *operator[]( const char *name ) { return FindConVar( name ); }

		void Parse( const char *str );
	};
} // namespace ConVar

// Global Convar Handler
extern ConVar::ConVarHandler conVarHandle;