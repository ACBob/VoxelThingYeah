#include "cvar.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

#include <cstring>
#include <memory>

// Global Convar Handler
ConVar::ConVarHandler conVarHandle;

namespace ConVar
{
	ConVar::ConVar(const char* name, const char* defval, int flags)
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

	ConVar *ConVarHandler::DeclareConvar(const char* name, const char* defVal, int flags)
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

		char *in = new char[strlen(str)];
		strcpy(in, str);

		char *token = new char[1];
		token[0] = '\0';
		char *oToken = new char[1];
		oToken[0] = '\0';
		char *saveptr;
		const char sep[4] = "\n; ";
		token = strtok_r(in, sep, &saveptr);

		while (token != NULL)
		{
			con_debug("%s", token);

			if (oToken != nullptr && strlen(token) && strlen(oToken))
			{
				ParseConvarTokens(oToken, token);

				if (oToken != nullptr)
				{
					delete[] oToken;
					oToken = nullptr;
				}
				
				token = strtok_r(NULL, sep, &saveptr);
				continue;
			}

			if (oToken != nullptr)
			{
				delete[] oToken;
				oToken = nullptr;
			}
			oToken = new char[strlen(token)];
			strcpy(oToken, token);

			token = strtok_r(NULL, sep, &saveptr);
		}

		delete[] oToken;
		delete[] token;
		delete[] in;
	}

	void ConVarHandler::ParseConvarTokens(const char *cmd, const char *args)
	{
		// It's empty, bum run, don't do anything
		if (strlen(cmd) == 0 && strlen(args) == 0)
			return;

		con_debug("SET %s TO %s", cmd, args);

		ConVar *conv = Cvars[cmd];
		if (conv == nullptr)
		{
			con_error("Unknown ConVar %s", cmd);
		}
		else
		{
			conv->SetString(args);
		}
	}
}