// Resource Packs!

#include <vector>
#include <string>

#define CURRENT_PACK_FORMAT 0x01

namespace resourcePacks {

	struct packInfo {
		std::string name = "UNKNOWN";
		std::string desc = "????";
		std::string license = "N/A";
		
		int format = 0x00;

		bool hasTexture = false;
		bool hasSound = false;
		bool hasShader = false;
		bool hasModel = false;
		bool hasScript = false; // TODO: Unused, implement when scripting comes

		// Not stored in the toml
		std::string path = "";
	};

	std::vector<const char*> ListPacks();

	packInfo GetPackInfo(const char* packname);

	void MountPack(packInfo pack);
}