/**
 * @file config.h
 * @brief defination of Config class
 *
 * @author king
 * @date 2018-02-11
 */


#ifndef __CONFIG_H__
#define __CONFIG_H__


#include "json/json.h"


/**
 * @class Config
 * @brief configuration class
 *
 * Operation of configuration file.
 *
 */
class Config : public Json::Value
{
public:
	/**
	 * Contructor
	 * @param path configuration file path
	 */
	Config(const std::string path);

	/**
	 * Load configuration file
	 * @return return true if success, otherwise false.
	 */
	bool Load();

	/**
	 * Reload configuration file
	 * @return same as @ref Load
	 */
	bool Reload();

	/**
	 * Save configuration file
	 * @return same as @ref Load
	 */
	bool Save();

	/**
	 * Save configuration file to special path
	 * @return same as @ref Load
	 */
	bool SaveAs(const std::string path);

private:
	//! configuration file path
	std::string file_path;
};


#endif //__CONFIG_H__

