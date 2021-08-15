#include "cvar.h"

#include <cstring>

namespace ConVar
{
	std::map<const char*, ConVar*> Cvars;

	ConVar::ConVar(const char* name, const char* defval, ConVarFlag flags)
	{
		this->name = name;
		this->val = defval;
		this->defVal = defval;
		this->flags = flags;
	}

	void ConVar::SetString(const char* val)
	{
		modified = (strcmp(val, defVal) != 0);
		this->val = val;
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
}