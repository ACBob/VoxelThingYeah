#include "packs.hpp"

#include <cstring>

#include "filesystem.hpp"
#include "tomlcpp.hpp"

#include "types.hpp"

#include "logging.hpp"

std::vector<const char*>resourcePacks::ListPacks()
{
	return fileSystem::List("/packs/");
}

resourcePacks::packInfo resourcePacks::GetPackInfo(const char *pck)
{
	char *path = new char[strlen(pck) + 8];
	strcpy(path, "/packs/");
	strcat(path, pck);
	path[strlen(pck) + 8] = 0;

	if (!fileSystem::Exists(path))
	{
		delete[] path;
		return packInfo();
	}

	char *manifestPath = new char[strlen(path) + 15];
	strcpy(manifestPath, path);
	strcat(manifestPath, "/manifest.toml");
	manifestPath[strlen(path) + 15] = 0;

	int64_t fl				  = 0;
	bool bSuccess			  = false;
	const uchar_t *manifest   = fileSystem::LoadFile( manifestPath, fl, bSuccess );

	if (!bSuccess)
	{
		con_error("Could not load manifest for %s!!!", path);
		delete[] path;
		delete[] manifestPath;
		return packInfo();
	}

	packInfo info;
	info.path = path;

	toml::Result data = toml::parse( (char *)manifest);
	auto root = data.table->getTable("pack-info");

	std::string name;
	std::string desc;
	std::string license;
	int format;

	auto str = root->getString("name");
	name = str.first;
	if (!str.first)
	{
		con_warning("%s does not have a name in its' manifest, resorting to path...", pck);
		name = pck;
	}
	str = root->getString("desc");
	desc = str.second;
	if (!str.first)
	{
		con_warning("%s lacks a description, convention says it should have one!", name.c_str());
		desc = "";
	}
	str = root->getString("license");
	license = str.second;
	if (!str.first)
	{
		con_warning("%s lacks a license, convention says it should have one!", name.c_str());
		license = "N/A";
	}
	auto number = root->getInt("format");
	format = number.second;
	if (!number.first)
	{
		con_error("%s lacks a format, it should have one!... Assuming current.", name.c_str());
		format = CURRENT_PACK_FORMAT;
	}

	info.name = name;
	info.desc = desc;
	info.license = license;
	info.format = format;

	delete[] path;
	delete[] manifestPath;
	return info;
}

void resourcePacks::MountPack(packInfo inf)
{
	// Invalid pack, silently fail
	if (inf.format == 0x00)
	{
		return;
	}

	fileSystem::Mount(("files/" + inf.path.substr(1) + "/assets").c_str(), "/assets", true);
}