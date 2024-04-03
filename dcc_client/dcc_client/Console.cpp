
#if defined(__unix__)
#define UNIX true
#define WINDOWS false
#elif defined(_MSC_VER)
#define UNIX false
#define WINDOWS true
#endif

#define MULTITHREADED_SAFE false


#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <sstream>
#include "Console.h"

#if WINDOWS
#include "include/color.hpp"
#endif

namespace console{
	
	#if MULTITHREADED_SAFE
	std::queue<std::ostream> printQueue;
	
	
	void ConsoleQueueHandle(){
		while (!printQueue.empty())
		{
			// Output front of the queue
			std::stringbuf str;
			printQueue.front().rdbuf(&str); // uses str
			std::cout << str.str();
			// Pop the queue item
			printQueue.pop();
		}	
	}
	#endif
	
	std::string colorText(std::string name, std::string color) {
		return color + name + resetColor;
	}
	std::string colorText(std::string name, std::string fgColor, std::string bgColor)
	{
		return fgColor + bgColor + name + resetColor;
	}
	
	void PrintColored(std::string text, std::string fgColor, std::string bgColor)
	{
	#if WINDOWS
		auto fg = dye::white(text);
		if (fgColor == blackFGColor)
			fg = dye::black(text);
		else if (fgColor == redFGColor)
			fg = dye::red(text);
		else if (fgColor == greenFGColor)
			fg = dye::green(text);
		else if (fgColor == yellowFGColor)
			fg = dye::yellow(text);
		else if (fgColor == blueFGColor)
			fg = dye::blue(text);
		else if (fgColor == magentaFGColor)
			fg = dye::purple(text);
		else if (fgColor == cyanFGColor)
			fg = dye::aqua(text);
		else if (fgColor == whiteFGColor)
			fg = dye::white(text);
		#if MULTITHREADED_SAFE
		std::stringstream ss;
		ss << fg;
		printQueue.push((std::ostream)fg);
		ConsoleQueueHandle();
		#else
		std::cout << fg;
		#endif
	#else
		#if MULTITHREADED_SAFE
		printQueue.push(fgColor + bgColor + text + resetColor);
		ConsoleQueueHandle();
		#else
		std::cout << fgColor + bgColor + text + resetColor;
		#endif
	#endif
	}
	
	void NetworkPrint()
	{
		PrintColored("[", yellowFGColor, "");
		PrintColored("Network", cyanFGColor, "");
		PrintColored("]        - ", yellowFGColor, "");
	}
	void NetworkErrorPrint()
	{
		PrintColored("[", yellowFGColor, "");
		PrintColored("Network-Error", redFGColor, "");
		PrintColored("]  - ", yellowFGColor, "");
	}
	void MiningPrint()
	{
		PrintColored("[", yellowFGColor, "");
		PrintColored("Mining", greenFGColor, "");
		PrintColored("]         - ", yellowFGColor, "");
	}
	void MiningErrorPrint()
	{
		PrintColored("[", yellowFGColor, "");
		PrintColored("Mining-Error", redFGColor, "");
		PrintColored("]   - ", yellowFGColor, "");
	}
	void DockerPrint()
	{
		PrintColored("[", yellowFGColor, "");
		PrintColored("Docker", magentaFGColor, "");
		PrintColored("]         - ", yellowFGColor, "");
	}
	void CompilerErrorPrint()
	{
		PrintColored("[", yellowFGColor, "");
		PrintColored("Docker-Error", redFGColor, "");
		PrintColored("]   - ", yellowFGColor, "");
	}
	void BlockchainPrint()
	{
		PrintColored("[", yellowFGColor, "");
		PrintColored("Blockchain", greenFGColor, "");
		PrintColored("]     - ", yellowFGColor, "");
	}
	void DebugPrint()
	{
		PrintColored("[", yellowFGColor, "");
		PrintColored("Debug", yellowFGColor, "");
		PrintColored("]          - ", yellowFGColor, "");
	}
	void SystemPrint()
	{
		PrintColored("[", yellowFGColor, "");
		PrintColored("System", blueFGColor, "");
		PrintColored("]         - ", yellowFGColor, "");
	}
	void ErrorPrint()
	{
		PrintColored("[", yellowFGColor, "");
		PrintColored("Error", redFGColor, "");
		PrintColored("]          - ", yellowFGColor, "");
	}
	
	void WriteLine()
	{
		#if MULTITHREADED_SAFE
		printQueue.push("\n");
		ConsoleQueueHandle();
		#else
		std::cout << std::endl;
		#endif
	}
	void WriteLine(std::string message)
	{
		#if MULTITHREADED_SAFE
		printQueue.push(message + "\n");
		ConsoleQueueHandle();
		#else
		std::cout << message << std::endl;
		#endif
	}
	void WriteLine(std::string message, std::string fgColor, std::string bgColor)
	{
		PrintColored(message, fgColor, bgColor);
		WriteLine();
	}
	
	void Write()
	{
		//std::cout;
	#if MULTITHREADED_SAFE
		ConsoleQueueHandle();
	#endif
	}
	void Write(std::string message)
	{
		#if MULTITHREADED_SAFE
		printQueue.push(message);
		ConsoleQueueHandle();
		#else
		std::cout << message;
		#endif
	}
	void Write(std::string message, std::string color)
	{
		PrintColored(message, color, "");
	}
	void Write(std::string message, std::string fgColor, std::string bgColor)
	{
		PrintColored(message, fgColor, bgColor);
	}
	void WriteIndented(std::string message, std::string fgColor, std::string bgColor, int indents)
	{
		std::string ind = "";
		for (size_t i = 0; i < indents; i++)
			ind += "\t";
		PrintColored(ind + "  " + message, fgColor, bgColor);
	}
	void WriteBulleted(std::string message, std::string fgColor, std::string bgColor, int indents, std::string bullet)
	{
		std::string ind = "";
		for (size_t i = 0; i < indents; i++)
			ind += "\t";
		PrintColored(ind + bullet + " " + message, fgColor, bgColor);
	}
	void WriteBulleted(std::string message, std::string fgColor, std::string bgColor, int indents)
	{
		std::string ind = "";
		for (size_t i = 0; i < indents; i++)
			ind += "\t";
		PrintColored(ind + "- " + message, fgColor, bgColor);
	}
	void WriteBulleted(std::string message, int indents, std::string bullet)
	{
		std::string ind = "";
		for (size_t i = 0; i < indents; i++)
			ind += "\t";
		PrintColored(ind + bullet + " " + message, "", "");
	}
	void WriteBulleted(std::string message, int indents)
	{
		std::string ind = "";
		for (size_t i = 0; i < indents; i++)
			ind += "\t";
		PrintColored(ind + "- " + message, "", "");
	}
	void WriteLineCharArrayOfLen(char* message, int len)
	{
		for (size_t i = 0; i < len; i++)
			std::cout << message[i];
	
		std::cout << std::endl;
	}
	//void WriteDialogueAuthor(std::string coloredType)
	//{
	//	PrintColored(coloredType, "", "");
	//}
	
	std::string ReadLine()
	{
		std::string s;
		std::getline(std::cin, s);
		return s;
	}
	
	void ExitError(std::string errMessage)
	{
		WriteLine(errMessage);
		std::cout << "Press Enter to Continue";
		std::cin.ignore();
		exit(1);
	}
	
	// Print a connection error dialog
	void ConnectionError()
	{
		NetworkErrorPrint();
		WriteLine("Failed To Connect");
	}

}
