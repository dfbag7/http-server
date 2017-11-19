#include <unistd.h>
#include <syslog.h>
#include <signal.h>

#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "HttpServer.hpp"
#include "FileHandler.hpp"

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
	std::string addr;
	std::string port;
	std::string documentRoot;

	// Declare the supported options
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce this help message")
		("host,h", po::value<std::string>(&addr), "host IP address")
		("port,p", po::value<std::string>(&port), "port")
		("directory,d", po::value<std::string>(&documentRoot), "document root directory");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if(vm.count("help"))
	{
		std::cout << desc << std::endl;
		return 1;
	}

	if( !vm.count("host") )
	{
		std::cerr << "Host IP address was not specified" << std::endl;
		return 1;
	}

	if( !vm.count("port") )
	{
		std::cerr << "Port number was not specified" << std::endl;
		return 1;
	}

	if( !vm.count("directory") )
	{
		std::cerr << "Document root directory was not specified" << std::endl;
		return 1;
	}

	std::cout << "IP address:    " << addr << std::endl
	          << "Port:          " << port << std::endl
	          << "Document root: " << documentRoot << std::endl;

	try
	{
		boost::asio::io_service ioService;

		// Initialize the server before becoming a daemon. If the process is
		// started from a shell, this means any errors will be reported back 
		// to the user.
		HttpServer(ioService, addr, port, FileHandler(documentRoot))();

		// Register signal handlers so that the daemon may be shut down. You may
		// also want to register for other signals, such as SIGHUP to trigger a
		// re-read of a configuration file.
		boost::asio::signal_set signals(ioService, SIGINT, SIGTERM);
		signals.async_wait(boost::bind(&boost::asio::io_service::stop, &ioService));


//#if false


		// Inform the io_service that we are about to become a daemon.
		// The io_service cleans up any internal resources, such as threads,
		// that may interfere with forking.
		ioService.notify_fork(boost::asio::io_service::fork_prepare);

		// Fork the process and have the parent exit. If the process was started
		// from a shell, this returns control to the user. Forking a new process
		// is also a prerequisite for the subsequent call to setsid().
		if(pid_t pid = fork())
		{
			if(pid > 0)
			{
				// We're in the parent process and need to exit.
				//
				// When the exit() function is used, the program terminates without
				// invoking local variables' destructors. Only global variables are
				// destroyed. As the io_service object is a local variable, this means
				// we do not have to call:
				//
				//   ioService.notify_fork(io_service::fork_parent);
				//
				// However, this line should be added before each call to exit() 
				// if using a global io_service object. An additional call:
				//
				//   ioService.notify_fork(io_service::fork_prepare);
				//
				// should also precede the second fork().
				exit(0);
			}
			else
			{
				syslog(LOG_ERR | LOG_USER, "First fork failed: %m");
				return 1;
			}
		}

		// Make the process a new session leader. This detaches it from the terminal.
		setsid();

		// A process inherits its working directory from its parent. This could be
		// on a mounted filesystem, which means that the running daemon would
		// prevent this filesystem fom being unmounted. Changing to the root
		// directory avoids this problem.
		chdir("/");

		// The file mode creation mask is also inherited from the parent process.
		// We don't want to restrict the permissions on files created by 
		// the daemon, so the mask is cleared.
		umask(0);

		// A second fork ensures the process cannot acquire a controlling terminal.
		if(pid_t pid = fork())
		{
			if(pid > 0)
			{
				exit(0);
			}
			else
			{
				syslog(LOG_ERR | LOG_USER, "Second fork failed: %m");
				return 1;
			}
		}

		// Close the standard streams. This decouples the daemon from 
		// the terminal that started it.

		// We don't want the daemon to have any standard input.
		int nullFd = open("/dev/null", O_RDONLY);
		if(nullFd < 0)
		{
			syslog(LOG_ERR | LOG_USER, "Unable to open /dev/null: %m");
			return 1;
		}
		if(dup2(nullFd, STDIN_FILENO) < 0)
		{
			syslog(LOG_ERR | LOG_USER, "Unable to dup2 dev/null input: %m");
			return 1;
		}

		// Send standard output to a log file.
		const char* output = "/tmp/final.out";
		const int flags = O_WRONLY | O_CREAT | O_APPEND;
		const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

		int outFd = open(output, flags, mode);

		if(outFd < 0)
		{
			syslog(LOG_ERR | LOG_USER, "Unable to open output file %s: %m", output);
			return 1;
		}

		if(dup2(outFd, STDOUT_FILENO) < 0)
		{
			syslog(LOG_ERR | LOG_USER, "Unable to dup2 stdout descriptor: %m");
			return 1;
		}

		if(dup2(outFd, STDERR_FILENO) < 0)
		{
			syslog(LOG_ERR | LOG_USER, "Unable to dup2 stderr descriptor: %m");
			return 1;
		}

		// Inform the io_service that we have finished becoming a daemon.
		// The io_service uses this opportunity to create any internal
		// file descriptors that need to be private to the new process.
		ioService.notify_fork(boost::asio::io_service::fork_child);


//#endif

		// The io_service can now be used normally.
		syslog(LOG_INFO | LOG_USER, "Daemon started");
		ioService.run();
		syslog(LOG_INFO | LOG_USER, "Daemon stopped");
	}	
	catch(std::exception& ex)
	{
		std::cerr << "Got exception: " << ex.what() << std::endl;
		return 2;
	}

	return 0;
}
