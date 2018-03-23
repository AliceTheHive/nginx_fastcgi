#ifndef __VERSION_H__
#define __VERSION_H__


#define STRING_TRANSFER(str) #str
#define STRING(STR) STRING_TRANSFER(STR)


#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_BUILD 0


#define MAIN_VERSION STRING(VERSION_MAJOR) \
					 "." \
					 STRING(VERSION_MINOR) \
					 "." \
					 STRING(VERSION_BUILD)

#ifndef GIT_VERSION
#define GIT_VERSION "00000000"
#endif

#define VERSION MAIN_VERSION \
				"-" \
				GIT_VERSION


#endif
