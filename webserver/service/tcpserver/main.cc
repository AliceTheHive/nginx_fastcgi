#include <errno.h>
#include <getopt.h>
#include <signal.h>

#include <fstream>
#include <iostream>
#include <string>

#include "common.h"
#include "config.h"
#include "log.h"
#include "pollthread.h"
#include "tcp_constant.h"
#include "tcp_listener.h"


static int gs_argc = 0;
static char **gs_argv = NULL;
static bool gs_help = false;
static bool gs_version = false;
static bool gs_stop = false;
static bool gs_reload = false;
static bool gs_delete_pidfile = false;
static std::string gs_config = kDefaultConfig;
static std::string gs_pidfile = kDefaultPidFile;
static CPollThread *gs_listen_thread = NULL;
static CPollThread *gs_work_thread = NULL;
static CTcpListener *gs_listener = NULL;
static CClientProcess *gs_client_process = NULL;
static CWorkProcess *gs_work_process = NULL;


static void ShowHelp()
{
	std::cout << std::setw(4) << "" << std::setw(24) << std::left << "-c|--config=CONF" << "config file." << std::endl;
	std::cout << std::setw(4) << "" << std::setw(24) << std::left << "-h|--help" << "print this." << std::endl;
	std::cout << std::setw(4) << "" << std::setw(24) << std::left << "-v|--version" << "print version." << std::endl;
	std::cout << std::setw(4) << "" << std::setw(24) << std::left << "-V|--Version" << "print version." << std::endl;
}

static void ShowVersion()
{
	std::cout << PROGRAM_NAME << "-" << VERSION << std::endl;
	std::cout << std::setw(4) << "" << "compile time: " << __DATE__ << " " << __TIME__ << std::endl;
}

static void catch_signal(int32_t signal)
{
	switch(signal)
	{
	case SIGUSR1:
	{
		log_error("catch [%d] signal, for reload.", signal);
		gs_reload = true;
		gs_delete_pidfile = false;
		break;
	}
	case SIGTERM:
	case SIGINT:
	case SIGQUIT:
	case SIGHUP:
	{
		log_error("catch [%d] signal, for stop.", signal);
		gs_stop = true;
		gs_delete_pidfile = true;
		break;
	}
	case SIGPIPE:
	{
		log_error("catch [%d] signal, nothing.", signal);
		break;
	}
	default:
	{
		log_error("catch [%d] signal, nothing.", signal);
		break;
	}
	}
}

static bool SetSignal()
{
	if (SIG_ERR == signal(SIGTERM, catch_signal))
	{
		log_error("set TERM signal handler error. errno [%d], strerror [%s].", errno, strerror(errno));
		return false;
	}
	if (SIG_ERR == signal(SIGINT, catch_signal))
	{
		log_error("set INT signal handler error. errno [%d], strerror [%s].", errno, strerror(errno));
		return false;
	}
	if (SIG_ERR == signal(SIGQUIT, catch_signal))
	{
		log_error("set QUIT signal handler error. errno [%d], strerror [%s].", errno, strerror(errno));
		return false;
	}
	if (SIG_ERR == signal(SIGHUP, catch_signal))
	{
		log_error("set HUP signal handler error. errno [%d], strerror [%s].", errno, strerror(errno));
		return false;
	}
	if (SIG_ERR == signal(SIGUSR1, catch_signal))
	{
		log_error("set reload signal handler error. errno [%d], strerror [%s].", errno, strerror(errno));
		return false;
	}
	if (SIG_ERR == signal(SIGPIPE, catch_signal))
    {
		log_error("set EPIPE signal handler error. errno [%d], strerror [%s].", errno, strerror(errno));
		return false;
	}
	if (SIG_ERR == signal(SIGCHLD, catch_signal))
    {
		log_error("set ECHLD signal handler error. errno [%d], strerror [%s].", errno, strerror(errno));
		return false;
	}

	sigset_t sst;
	sigemptyset(&sst);
	sigaddset(&sst, SIGSEGV);
	sigaddset(&sst, SIGBUS);
	sigaddset(&sst, SIGTERM);
	sigaddset(&sst, SIGINT);
	sigaddset(&sst, SIGQUIT);
	sigaddset(&sst, SIGHUP);
	sigaddset(&sst, SIGUSR1);
	sigaddset(&sst, SIGUSR2);
	sigaddset(&sst, SIGABRT);
	sigaddset(&sst, SIGILL);
	sigaddset(&sst, SIGPIPE);
	sigaddset(&sst, SIGCHLD);
	sigaddset(&sst, SIGFPE);
	sigaddset(&sst, SIGPROF);
	sigprocmask(SIG_UNBLOCK, &sst, &sst);

	return true;
}

static bool CreatePidFile(const std::string &pidfile)
{
	std::ofstream pidfile_ostream;
	pidfile_ostream.open(pidfile.c_str(), std::ios::out | std::ios::trunc);
	if(true == pidfile_ostream.is_open())
	{
		pidfile_ostream << getpid();
		pidfile_ostream.close();
	}
	else
	{
		log_error("Open pid file [%s] failed, errno [%d], strerror [%s].", pidfile.c_str(), errno, strerror(errno));
		return false;
	}

	return true;
}

static int32_t GetInheritedSocket(const std::string ip, uint16_t port)
{
	int32_t inherited_socket = -1;
	const char *inherited_env = getenv(kInheritedSocketName.c_str());
	if(NULL != inherited_env)
	{
		inherited_socket = atoi(inherited_env);
		sockaddr_in listen_addr;
		socklen_t length = sizeof(sockaddr_in);
		if(0 == getsockname(inherited_socket, reinterpret_cast<sockaddr *>(&listen_addr), &length))
		{
			const std::string inherited_ip = inet_ntoa(listen_addr.sin_addr);
			const int16_t inherited_port = ntohs(listen_addr.sin_port);
			if(inherited_ip != ip || inherited_port != port)
			{
				inherited_socket = -1;
				log_error("Get inherited socket success, but ip or port is not same. config ip [%s], config port [%u], inherited ip [%s], inherited port [%u].", ip.c_str(), port, inherited_ip.c_str(), inherited_port);
			}
		}
		else
		{
			inherited_socket = -1;
			log_error("Get inherited socket [%d] name failed, errno [%d], strerror [%s].", inherited_socket, errno, strerror(errno));
		}
	}

	return inherited_socket;
}

static bool InitListenServer(const CConfig &config)
{
	if(config.isMember("listen") && config["listen"].isObject())
	{
		Json::Value &listen = config["listen"];
		if(listen.isMember("ip") && listen["ip"].isString()
		   && listen.isMember("port") && listen["port"].isUInt())
		{
			int32_t inherited_socket = GetInheritedSocket(listen["ip"].asString(), listen["port"].asUInt());
			if(-1 == inherited_socket)
			{
				gs_listener = new CTcpListener(listen["ip"].asString(), listen["port"].asUInt());
			}
			else
			{
				gs_listener = new CTcpListener(inherited_socket);
			}

			if(NULL == gs_listener)
			{
				log_error("Not enough memory for new CTcpListener.");
				return false;
			}

			if(0 != gs_listener->Listen())
			{
				log_error("The listener listen failed.");
				return false;
			}
			
			if(0 != gs_listen_thread->Attach(gs_listener))
			{
				log_error("The listener attach listen thread failed.");
				return false;
			}
			
			log_debug("Create listener success.");
			return true;
		}
		else
		{
			log_error("Can not find valid ip or port field within config.");
			return false;
		}
	}
	else
	{
		log_error("Can not find valid listen field within config.");
		return false;
	}

	return true;
}

static bool InitWorkProcess(const CConfig &config)
{
	gs_client_process = new CClientProcess(gs_listen_thread);
	if(NULL == gs_client_process)
	{
		log_error("Not enough memory for new CClientProcess.");
		return false;
	}
	
	gs_work_process = new CWorkProcess(gs_work_thread);
	if(NULL == gs_work_process)
	{
		log_error("Not enough memory for new CWorkProcess.");
		return false;
	}

	gs_client_process->BindDispatcher(gs_work_process);

	return true;
}

static bool InitThread(const CConfig &config)
{
	do
	{
		gs_listen_thread = new CPollThread("listen_thread");
		if(NULL == gs_listen_thread)
		{
			log_error("Not enough memory for new listen_thread CPollThread.");
			break;
		}
		if(false == gs_listen_thread.Start())
		{
			log_error("Listen thread start failed.");
			break;
		}

		gs_work_thread = new CPollThread("work_thread");
		if(NULL == gs_work_thread)
		{
			log_error("Not enough memory for new work_thread CPollThread.");
			break;
		}
		if(false == gs_work_thread.Start())
		{
			log_error("Work thread start failed.");
			break;
		}

		if(false == InitListenServer(config))
		{
			log_error("Init listen server failed.");
			break;
		}

		if(false == InitWorkProcess(config))
		{
			log_error("Init work process failed.");
			break;
		}

		gs_listen_thread->RunningThread();
		gs_work_thread->RunningThread();
		return true;
	} while(0);

	return false;
}

static bool Initialize(const CConfig &config)
{
	if(config.isMember("log_dir") && config["log_dir"].isString())
	{
		_init_log_(PROGRAM_NAME, config["log_dir"].asString().c_str());
	}
	else
	{
		_init_log_(PROGRAM_NAME, "../log/");
	}

	if(config.isMember("log_level") && config["log_level"].isUInt())
	{
		_set_log_level_(config["log_level"].asUInt());
	}
	else
	{
		_set_log_level_(3);
	}

	if(config.isMember("pid_file") && config["pid_file"].isString())
	{
		gs_pidfile = config["pid_file"].asString();
	}

	if(false == SetSignal())
	{
		log_error("Initialize failed, when SetSignal.");
		return false;
	}

	if(config.isMember("daemon") && config["daemon"].isBool() && true == config["daemon"].asBool())
	{
		daemon(1, 0);
	}

	if(false == CreatePidFile())
	{
		log_error("Initialize failed, when CreatePidFile.");
		return false;
	}

	if(false == InitThread())
	{
		log_error("Initialize failed, when InitThread.");
		return false;
	}

	log_debug("Initialize %s %s success.", PROGRAM_NAME, VERSION);
	return true;
}

static void DaemonWait()
{
	while(!gs_stop)
	{
		pause();
	}
}

static void Destroy()
{
	if(NULL != gs_listen_thread)
	{
		gs_listen_thread->Interrupt();
	}

	if(NULL != gs_work_thread)
	{
		gs_work_thread->Interrupt();
	}

	DELETE(gs_listener);
	DELETE(gs_client_process);
	DELETE(gs_work_process);
	DELETE(gs_listen_thread);
	DELETE(gs_work_thread);
	CTaskPipe::DestroyAllPipe();

	log_debug("Destroy.");
}

int main(int argc, char **argv)
{
	const char *short_option = "c:hVv";
	option long_option[] =
	{
		{"config", required_argument, NULL, 'c'},
		{"help", no_argument, NULL, 'h'},
		{"Version", no_argument, NULL, 'V'},
		{"version", no_argument, NULL, 'v'},
		{NULL, no_argument, NULL, 0}
	}

	while(true)
	{
		int32_t opt = getopt_long(argc, argv, short_option, long_option, NULL);
		if(-1 == opt)
		{
			break;
		}
		
		switch(opt)
		{
		case 'c':
			{
				gs_config = optarg;
				break;
			}
		case 'h':
			{
				gs_help = true;
				break;
			}
		case 'V':
		case 'v':
			{
				gs_version = true;
				break;
			}
		case '?':
		default:
			{
				std::cout << "invaild option [" << opt << "]." << std::endl;
				break;
			}
		}
	}

	gs_argc = argc;
	gs_argv = argv;
	if(true == gs_help)
	{
		ShowHelp();
		return 0;
	}
	if(true == gs_version)
	{
		ShowVersion();
		return 0;
	}

	CConfig config(gs_config);
	if(false == config.Load())
	{
		std::cout << "load config [" << gs_config << "] failed, errno [" << errno << "], strerror [" << strerror(errno) << "]." << std::endl;
		return -1;
	}
	if(true == Initialize(config))
	{
		DaemonWait();
	}
	Destroy();
	return 0;
}
