#ifndef __COMMON_H__
#define __COMMON_H__


#include <string>


#ifndef PROGRAM_NAME
#define PROGRAM_NAME "tcpclient"
#endif


const std::string kDefaultConfig = "../conf/" PROGRAM_NAME ".json";
const std::string kDefaultPidFile = "./" PROGRAM_NAME ".pid";


#endif
