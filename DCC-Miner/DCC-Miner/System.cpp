
#include "System.h"



// Execute a command in the main thread and print the output
std::string ExecuteCommand(const char* cmd)
{
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) {
		throw std::runtime_error("_popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
		std::cout << buffer.data();
	}

	return "";
}

// Execute a process in an asynchronous background thread
boost::process::child ExecuteAsync(std::string cmd, bool printOutput)
{
	try
	{
		namespace bp = boost::process;
		std::vector<std::string> splitCommand = SplitString(cmd, " ");
		std::string command = splitCommand[0];
		std::string args;
		for (int i = 1; i < sizeof(splitCommand) / sizeof(splitCommand[0]); i++)
		{
			args += splitCommand[i] + " ";
		}
		bp::child c(cmd, ::boost::process::windows::create_no_window);

		return c;
	}
	catch (const std::exception& e)
	{
		return boost::process::child();
	}

	return boost::process::child();
}
