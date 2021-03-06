// Quake-Style Console Variable System
#include <map>

#include <cstring>

#include <vector>

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

	class CConVar
	{
	  public:
		CConVar( const char *name, const char *defval, int flags );

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

		// Returns if the value has been changed
		bool IsModified() { return m_bModified; };
		bool ShouldArchive() { return m_iFlags & CVAR_ARCHIVE; }

		// Returns the name of the variable
		const char *GetName() { return m_cName; };

	  private:
		int m_iFlags;

		// Does the value differ from its' regular one
		bool m_bModified;

		// Converted from to other types :trollface:
		const char *m_cVal;
		const char *m_cDefVal;

		// Name
		const char *m_cName;
	};

	// Function pointer for the console command
	typedef void ( *ConsoleCommandFunc )( const char *args );

	class CConCmd
	{
	  public:
		// Takes the name, expected arguments and the function to call
		CConCmd( const char *name, const char *expectedArgs, ConsoleCommandFunc func );

		// Executes the command
		// (handles the arguments and calls the function)
		void Execute( const char *args );

	  private:
		const char *m_cName;
		const char *m_cExpectedArgs;
		// int m_iFlags;

		// Execution function
		void ( *m_pFunc )( const char *args );
	};

	class CConVarHandler
	{
	  private:
		// Name -> ConVar

		// https://stackoverflow.com/a/4157729
		struct cmp_str
		{
			bool operator()( const char *a, const char *b ) const { return strcmp( a, b ) < 0; }
		};

		std::map<const char *, CConVar *, cmp_str> Cvars;
		std::map<const char *, CConCmd *, cmp_str> Cmds;

		void HandleConvarTokens( const char *cmd, const char *arg );

	  public:
		CConVarHandler();
		~CConVarHandler();

		void WriteCFG( const char *cfgName = "config.cfg" );

		CConVar *DeclareConvar( const char *name, const char *defVal, int flags = F::CVAR_NOFLAGS );
		CConCmd *DeclareConCmd( const char *name, const char *expectedArgs, ConsoleCommandFunc func );

		// Find Cvar by name
		CConVar *FindConVar( const char *name );
		// Find Cmd by name
		CConCmd *FindConCmd( const char *name );

		CConVar *operator[]( const char *name ) { return FindConVar( name ); }

		void Parse( const char *str );

		// List all cvars
		std::vector<CConVar *> ListConVars();
	};
} // namespace ConVar

// Global Convar Handler
extern ConVar::CConVarHandler conVarHandle;