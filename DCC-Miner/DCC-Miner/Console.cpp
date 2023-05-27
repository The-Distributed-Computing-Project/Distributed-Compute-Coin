
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
#include "include/color.hpp"


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

std::string Console::colorText(std::string name, std::string color) {
	return color + name + resetColor;
}
std::string Console::colorText(std::string name, std::string fgColor, std::string bgColor)
{
	return fgColor + bgColor + name + resetColor;
}

void Console::PrintColored(std::string text, std::string fgColor, std::string bgColor)
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
	printQueue.push(fgColor + bgColor + name + resetColor);
	ConsoleQueueHandle();
	#else
	cout << fgColor + bgColor + name + resetColor;
	#endif
#endif
}

void Console::NetworkPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Network", cyanFGColor, "");
	Console::PrintColored("]        - ", yellowFGColor, "");
}
void Console::NetworkErrorPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Network-Error", redFGColor, "");
	Console::PrintColored("]  - ", yellowFGColor, "");
}
void Console::MiningPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Mining", greenFGColor, "");
	Console::PrintColored("]         - ", yellowFGColor, "");
}
void Console::MiningErrorPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Mining-Error", redFGColor, "");
	Console::PrintColored("]   - ", yellowFGColor, "");
}
void Console::RustPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Rust", magentaFGColor, "");
	Console::PrintColored("]           - ", yellowFGColor, "");
}
void Console::CompilerErrorPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Rust-Error", redFGColor, "");
	Console::PrintColored("]     - ", yellowFGColor, "");
}
void Console::BlockCheckerPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Block-Checker", greenFGColor, cyanBGColor);
	Console::PrintColored("]  - ", yellowFGColor, "");
}
void Console::DebugPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Debug", yellowFGColor, "");
	Console::PrintColored("]          - ", yellowFGColor, "");
}
void Console::SystemPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("System", blueFGColor, "");
	Console::PrintColored("]         - ", yellowFGColor, "");
}
void Console::ErrorPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Error", redFGColor, "");
	Console::PrintColored("]          - ", yellowFGColor, "");
}

void Console::WriteLine()
{
	#if MULTITHREADED_SAFE
	printQueue.push("\n");
	ConsoleQueueHandle();
	#else
	std::cout << std::endl;
	#endif
}
void Console::WriteLine(std::string message)
{
	#if MULTITHREADED_SAFE
	printQueue.push(message + "\n");
	ConsoleQueueHandle();
	#else
	std::cout << message << std::endl;
	#endif
}
void Console::WriteLine(std::string message, std::string fgColor, std::string bgColor)
{
	Console::PrintColored(message, fgColor, bgColor);
	Console::WriteLine();
}

void Console::Write()
{
	//std::cout;
	ConsoleQueueHandle();
}
void Console::Write(std::string message)
{
	#if MULTITHREADED_SAFE
	printQueue.push(message);
	ConsoleQueueHandle();
	#else
	std::cout << message;
	#endif
}
void Console::Write(std::string message, std::string color)
{
	Console::PrintColored(message, color, "");
}
void Console::Write(std::string message, std::string fgColor, std::string bgColor)
{
	Console::PrintColored(message, fgColor, bgColor);
}
void Console::WriteIndented(std::string message, std::string fgColor, std::string bgColor, int indents)
{
	std::string ind = "";
	for (size_t i = 0; i < indents; i++)
		ind += "\t";
	Console::PrintColored(ind + "  " + message, fgColor, bgColor);
}
void Console::WriteBulleted(std::string message, std::string fgColor, std::string bgColor, int indents, std::string bullet)
{
	std::string ind = "";
	for (size_t i = 0; i < indents; i++)
		ind += "\t";
	Console::PrintColored(ind + bullet + " " + message, fgColor, bgColor);
}
void Console::WriteBulleted(std::string message, std::string fgColor, std::string bgColor, int indents)
{
	std::string ind = "";
	for (size_t i = 0; i < indents; i++)
		ind += "\t";
	Console::PrintColored(ind + "- " + message, fgColor, bgColor);
}
void Console::WriteBulleted(std::string message, int indents, std::string bullet)
{
	std::string ind = "";
	for (size_t i = 0; i < indents; i++)
		ind += "\t";
	Console::PrintColored(ind + bullet + " " + message, "", "");
}
void Console::WriteBulleted(std::string message, int indents)
{
	std::string ind = "";
	for (size_t i = 0; i < indents; i++)
		ind += "\t";
	Console::PrintColored(ind + "- " + message, "", "");
}
//void Console::WriteDialogueAuthor(std::string coloredType)
//{
//	Console::PrintColored(coloredType, "", "");
//}

std::string Console::ReadLine()
{
	std::string s;
	std::getline(std::cin, s);
	return s;
}

void Console::ExitError(std::string errMessage)
{
	Console console;
	console.WriteLine(errMessage);
	std::cout << "Press Enter to Continue";
	std::cin.ignore();
	exit(1);
}
