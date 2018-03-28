#include "config/config.h"

#include <fstream>


CConfig::CConfig(const std::string path)
{
	file_path = path;
}

bool CConfig::Load()
{
	Json::Reader reader;
	std::ifstream file(file_path.c_str());
	if (file)
		return reader.parse(file, *this);
	else
		return false;
}

bool CConfig::Reload()
{
	this->clear();
	return this->Load();
}

bool CConfig::Save()
{
	return SaveAs(file_path);
}

bool CConfig::SaveAs(const std::string path)
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
