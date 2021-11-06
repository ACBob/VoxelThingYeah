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

	CConCmd::CConCmd( const char *name, const char *expectedArgs, ConsoleCommandFunc func )
	{
		m_cName			= name;
		m_cExpectedArgs = expectedArgs;
		m_pFunc			= func;
	}

	void CConCmd::Execute( const char *args )
	{
		if ( !m_pFunc )
			return;

		// Test if the arguments are correct
		// expected args is a string of the form:
		// "ifcb"
		// where i = int, f = float, c = char/string, b = bool

		// First, test if the number of args is correct
		int numArgs = 0;
		const char *p = args;
		while ( p[0] != NULL )
		{
			if ( p[0] == ' ' )
				numArgs++;
			p++;
		}

		if ( numArgs != strlen(m_cExpectedArgs))
		{
			con_error( "Wrong number of arguments for command %s (Got %d, expected %d)", m_cName, numArgs, strlen(m_cExpectedArgs) );
			return;
		}

		// Now, test if the args are correct
		// Testing the type of each argument
		// Hop through the given args
		p = args;
		int argTest = 0;
		while ( p[0] != NULL )
		{
			if (p[0] == ' ')
				argTest++;
			
			// Test if the current argument is of the correct type
			switch ( m_cExpectedArgs[argTest] )
			{
				case 'i':
					// The argument to test ends at a space, so test if the char up to the space is a number
					// Minus sign
					if (p[0] == '-')
						p++;
					while ( p[0] != ' ' && p[0] != NULL )
					{
						if ( !isdigit( p[0] ) )
						{
							con_error( "Argument %d of command %s is not an integer", argTest, m_cName );
							return;
						}
						p++;
					}
					break;
				break;

				case 'f':
					// The argument to test ends at a space, so test if the char up to the space is a number
					// Minus sign
					if (p[0] == '-')
						p++;
					while ( p[0] != ' ' && p[0] != NULL )
					{
						if ( !isdigit( p[0] ) && p[0] != '.' )
						{
							con_error( "Argument %d of command %s is not a float", argTest, m_cName );
							return;
						}
						p++;
					}
				break;

				case 'c':
					// String
					// TODO: Quote marks
					while ( p[0] != ' ' && p[0] != NULL )
					{
						p++;
					}					
				break;

				case 'b':
					// Boolean
					// accepted as 1 or 0, and also true or false
					if ( strcmp( p, "true" ) == 0 || strcmp( p, "false" ) == 0 )
					{
						p += 4;
					}
					else
					{
						if ( strcmp( p, "1" ) != 0 && strcmp( p, "0" ) != 0 )
						{
							con_error( "Argument %d of command %s is not a boolean", argTest, m_cName );
							return;
						}
						p++;
					}
				break;
			}
		}

		// If we got here, the arguments are correct!
		// Call the function (it will extract the arguments from the string itself)
		m_pFunc( args );
	}

	CConVarHandler::CConVarHandler() { Cvars = {}; }
	CConVarHandler::~CConVarHandler()
	{
		for ( auto &c : Cvars )
			delete c.second;
	}

	void CConVarHandler::WriteCFG(const char* cfgName)
	{
		std::string out;

		for ( auto &c : Cvars )
		{
			if ( !c.second->ShouldArchive() )
				continue;
			if ( !c.second->IsModified() )
				continue;

			out += c.first;
			out += " \"";

			// We need to make sure quotes are escaped
			// As well as semi-colons
			std::string val(c.second->GetString());
			for (size_t i = 0; i < val.length(); i++)
			{
				if (val[i] == '"') // Escape quotes
					out += "\\\"";
				else if (val[i] == '\'') // Escape single quotes
					out += "\\\'";
				else if (val[i] == ';') // Escape semi-colons
					out += "\\;";
				else if (val[i] == '\\') // Escape backslashes
					out += "\\\\";
				else
					out += val[i]; // Just append the character otherwise
			}

			out += "\";\n";
		}

		bool bSuccess = false;
		fileSystem::WriteFile( cfgName, (uchar_t *)out.c_str(), out.size(), bSuccess );
		if ( !bSuccess )
			con_error( "Failed to write config.cfg..." );
	}

	CConVar *CConVarHandler::DeclareConvar( const char *name, const char *defVal, int flags )
	{
		CConVar *c	= new CConVar( name, defVal, flags );
		Cvars[name] = c;

		return c;
	}

	CConCmd *CConVarHandler::DeclareConCmd( const char *name, const char *expectedArgs, ConsoleCommandFunc func )
	{
		CConCmd *c	= new CConCmd( name, expectedArgs, func );
		Cmds[name] = c;

		return c;
	}

	CConVar *CConVarHandler::FindConVar( const char *name ) { return Cvars[name]; }
	CConCmd *CConVarHandler::FindConCmd( const char *name ) { return Cmds[name]; }

	void CConVarHandler::Parse( const char *str )
	{
		// Don't try parsing empty string
		if ( strlen( str ) == 0 )
			return;
		
		// Tokens can be separated by spaces, tabs, or newlines
		// But tokens can be quoted with " or ' which will ignore whitespace.
		// The whitespace can be escaped with \, and the quotes can be escaped with \
		// We use two tokens to parse the string, one for the name and one for the value
		// Basically it's a key value pair but for commands or variables
		// Like quake!

		// So we have a string like this:
		// "name" "value"
		// Quotes can be ignored for either
		// But to set a value that *has* quotes;
		// "cl_examplemsg" "\"HELLO\""
		// would set cl_examplemsg to "HELLO"

		// There's also a comment character; #
		// This function handles files, so we ignore from # to newline and start over

		// We take a copy of the string so we can modify it
		char *in = new char[strlen( str ) + 1];
		char *start = in;
		strcpy( in, str );

		// This looks like unamanagable quake code
		// It probably is, it was (mostly) given to me by GitHub Co-pilot, I just touched it up a bit (it didn't exactly conform to what I was expecting)
		while ( in[0] != '\0' )
		{
			// Skip whitespace
			while ( in[0] == ' ' || in[0] == '\t' || in[0] == '\n' )
				in++;

			// Skip comments
			if ( in[0] == '#' )
			{
				while ( in[0] != '\n' )
					in++;
				continue;
			}

			// Get the name
			char *name = in;
			// Skip until we hit a whitespace or a quote (or semi-colon)
			while ( in[0] != ' ' && in[0] != '\t' && in[0] != '\n' && in[0] != ';' && in[0] != '\0' )
				in++;
			if (in[0] == '\0')
			{
				// Maybe it's a command with no arguments?
				HandleConvarTokens(name, "");
				break;
			}
			in[0] = '\0';
			in++;

			// Skip whitespace
			while ( in[0] == ' ' || in[0] == '\t' || in[0] == '\n' )
				in++;

			// Get the value
			char *val = in;
			// If it's not a string, skip until we hit a whitespace or semi-colon
			if (val[0] != '\"' && val[0] != '\'')
			{
				while ( in[0] != ' ' && in[0] != '\t' && in[0] != '\n' && in[0] != ';' && in[0] != '\0' )
					in++;
				in[0] = '\0';
				in++;
			}
			// It's a string!
			else
			{
				in ++;

				char quote = val[0];
				val ++;

				while ( in[0] != quote && in[0] != '\0' )
				{
					in++;
					if ( in[0] == '\\' && in[1] == quote ) // skip escaped quotes (and the backslash)
						in += 2; // TODO: the backslash is still technically part of the string, this causes the string to spiral out of control over successive restarts
				}
				if ( in[0] == '\0' )
				{
					con_error( "Unterminated String (%s)", val );
					break;
				}
				in[0] = '\0';
				in++;
			}

			HandleConvarTokens( name, val );

			// Skip whitespace
			while ( in[0] == ' ' || in[0] == '\t' || in[0] == '\n' || in[0] == ';' )
				in++;

			// Skip comments
			if ( in[0] == '#' )
			{
				while ( in[0] != '\n' && in[0] != '\0' )
					in++;
				continue;
			}
		}

		delete[] start;
	}

	void CConVarHandler::HandleConvarTokens( const char *cmd, const char *args )
	{
		// It's empty, bum run, don't do anything
		if ( strlen( cmd ) == 0 )
			return;

		con_debug( "SET %s TO %s", cmd, args );

		if ( Cvars.find(cmd) == Cvars.end() )
		{
			if ( Cmds.find(cmd) == Cmds.end() )
			{
				con_error( "Unknown command or variable: %s", cmd );
				return;
			}
			else
			{
				// It's a command!
				Cmds[cmd]->Execute( args );
				return;
			}
		}
		else
		{
			// It's a variable!
			Cvars[cmd]->SetString( args );
		}
	}

	std::vector<CConVar *> CConVarHandler::ListConVars()
	{
		std::vector<CConVar *> ret;
		for ( auto it = Cvars.begin(); it != Cvars.end(); it++ )
		{
			ret.push_back( it->second );
		}

		return ret;
	}
} // namespace ConVar