#include "cvar.h"

#define LOG_LEVEL DEBUG
#include "seethe.h"

#include <cstring>
#include <memory>

// Global Convar Handler
ConVar::ConVarHandler conVarHandle;

namespace ConVar
{
	ConVar::ConVar(const char* name, const char* defval, ConVarFlag flags)
	{
		this->name = name;
		this->val = defval;
		this->defVal = defval;
		this->flags = flags;
	}

	void ConVar::SetString(const char* val)
	{
		char *cval = new char(strlen(val) + 1);
		strcpy(cval, val);
		cval[strlen(val) + 1] = '\0';

		modified = (strcmp(cval, defVal) != 0);
		this->val = cval;
	}
	void ConVar::SetInt(int val)
	{
		char buf[256];
		snprintf(buf, 256, "%d", val);
		SetString(buf);
	}
	void ConVar::SetFloat(float val)
	{
		char buf[256];
		snprintf(buf, 256, "%f", val);
		SetString(buf);
	}
	void ConVar::SetBool(bool val)
	{
		char buf[256];
		snprintf(buf, 256, "%s", val ? "true" : "false");
		SetString(buf);
	}

	const char *ConVar::GetString()
	{
		return this->val;
	}
	int ConVar::GetInt()
	{
		return atoi(this->val);
	}
	float ConVar::GetFloat()
	{
		return atof(this->val);
	}
	bool ConVar::GetBool()
	{
		return (strcmp(this->val, "true") != 0);
	}

	ConVarHandler::ConVarHandler()
	{
		Cvars = {};
	}
	ConVarHandler::~ConVarHandler()
	{
		for (auto &c : Cvars)
			delete c.second;
	}

	ConVar *ConVarHandler::DeclareConvar(const char* name, const char* defVal, ConVarFlag flags)
	{
		ConVar *c = new ConVar(name, defVal, flags);
		Cvars[name] = c;

		return c;
	}

	ConVar *ConVarHandler::FindConVar(const char* name)
	{
		return Cvars[name];
	}

	void ConVarHandler::Parse(const char* str)
	{
		// Don't try parsing empty string
		if (strlen(str) == 0)
			return;

		char c = '\0';
		char lc = '\0';

		char *token = new char(1);
		token[0] = '\0';
		// Despite being called token, it's parsed with "secondToken token"
		char *secondToken = new char(1);
		secondToken[0] = '\0'; 

		bool quoted;
		bool escaping;
		int quotes;
		for (int i = 0; i < strlen(str); i++)
		{
			lc = c;
			c = str[i];

			switch (c)
			{
				case '"':
					quotes++;
					quoted = quotes & 1;
				break;

				case '\\':
					escaping = lc != '\\';
				break;


				case '\n':
				case ' ':
				case ';':
					if (!quoted && !escaping)
					{
						if (strlen(secondToken) == 0)
						{
							secondToken = new char(strlen(token) + 1);
							strcpy(secondToken, token);

							token = new char(1);
							token[0] = '\0';
						}
						else
						{
							ParseConvarTokens(secondToken, token);

							delete token;
							delete secondToken;

							token = new char(1);
							token[0] = '\0';
							secondToken = new char(1);
							secondToken[0] = '\0';
						}
					}			
				break;

				default:
					char *buf = new char(strlen(token) + 2);
					strcpy(buf, token);
					buf[strlen(token)] = c;
					buf[strlen(token) + 1] = '\0';
					delete token;
					token = buf;
				break;
			}
		}

		if (quoted)
		{
			// Only a warning as we can continue anyway
			warning("Syntax warning: Unclosed String!");
		}

		
		ParseConvarTokens(secondToken, token);

		delete token;
		delete secondToken;
	}

	void ConVarHandler::ParseConvarTokens(const char *cmd, const char *args)
	{
		debug("SET %s TO %s", cmd, args);

		ConVar *conv = Cvars[cmd];
		if (conv == nullptr)
		{
			error("Unknown ConVar %s", cmd);
		}
		else
		{
			conv->SetString(args);
		}
	}
}