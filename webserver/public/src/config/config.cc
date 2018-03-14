#include "config/config.h"

#include <fstream>


Config::Config(const std::string path)
{
	file_path = path;
}

bool Config::Load()
{
	Json::Reader reader;
	std::ifstream file(file_path.c_str());
	if (file)
		return reader.parse(file, *this);
	else
		return false;
}

bool Config::Reload()
{
	this->clear();
	return this->Load();
}

bool Config::Save()
{
	return SaveAs(file_path);
}

bool Config::SaveAs(const std::string path)
{
	Json::StyledStreamWriter writer;

	std::ofstream config_file(path.c_str(), std::ios::out | std::ios::trunc);
	if (config_file)
	{
		writer.write(config_file, *this);
		return true;
	}
	else
	{
		return false;
	}
}
