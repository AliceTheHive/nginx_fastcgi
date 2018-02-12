#include "log/log.h"


int main(int argc, char **argv)
{
	_init_log_("log_example", "./log");
	_set_log_level_(7);
	_set_log_console_(false);
	log_debug("log [%s] test.", "debug");
	log_error("log [%s] test.", "error");

	return 0;
}
