// Resource Packs!

#include <string>
#include <vector>

#pragma once

#define CURRENT_PACK_FORMAT 0x01

namespace resourcePacks
{

	struct packInfo
	{
		std::string name	= "UNKNOWN";
		std::string desc	= "????";
		std::string license = "N/A";

		int format = 0x00;

		bool hasTexture = false;
		bool hasSound	= false;
		bool hasShader	= false;
		bool hasModel	= false;
		bool hasScript	= false; // TODO: Unused, implement when scripting comes

		// Not stored in the toml
		std::string path		 = "";
		bool hasPackPng			 = false; // use default or pack's provided packpng
		std::string internalName = "";
	};

	std::vector<packInfo> ListPacks();

	packInfo GetPackInfo( const char *packname );

	void MountPack( packInfo pack );
} // namespace resourcePacks