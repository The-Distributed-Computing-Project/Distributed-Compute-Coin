
#if defined(__unix__)
#define UNIX true
#define WINDOWS false
#elif defined(_MSC_VER)
#define UNIX false
#define WINDOWS true
#endif


#include <iostream>
#include <string>
#include <vector>
#include "Console.h"
#include "include/color.hpp"

std::string Console::colorText(std::string name, std::string color) {
	return color + name + resetColor;
}
std::string Console::colorText(std::string name, std::string fgColor, std::string bgColor)
{
#if WINDOWS
	auto fg = dye::white(name);
	if (fgColor == blackFGColor)
		fg = dye::black(name);
	else if (fgColor == redFGColor)
		fg = dye::black(name);
	return fg;
#else
	return fgColor + bgColor + name + resetColor;
#endif
}

std::string Console::Network()
{
	return Console::colorText("Network", cyanFGColor);
}
std::string Console::NetworkError()
{
	return Console::colorText("Network-Error", redFGColor);
}
std::string Console::Mining()
{
	return Console::colorText("Mining", greenFGColor);
}
std::string Console::MiningError()
{
	return Console::colorText("Mining-Error", redFGColor);
}
std::string Console::Rust()
{
	return Console::colorText("Rust", magentaFGColor);
}
std::string Console::CompilerError()
{
	return Console::colorText("Rust-Error", redFGColor);
}
std::string Console::BlockChecker()
{
	return Console::colorText("Block-Checker", blackFGColor, cyanBGColor);
}
std::string Console::Debug()
{
	return Console::colorText("Debug", yellowFGColor);
}
std::string Console::Error()
{
	return Console::colorText("Error", redFGColor);
}

void Console::WriteLine()
{
	std::cout << std::endl;
}
void Console::WriteLine(std::string message)
{
	std::cout << message << std::endl;
}
void Console::WriteLine(std::string message, std::string coloredType)
{
	Console::Write(colorText("[", yellowFGColor));
	Console::Write(coloredType);
	Console::Write(colorText("]  - ", yellowFGColor));

	Console::WriteLine(message);
}

void Console::Write()
{
	std::cout;
}
void Console::Write(std::string message)
{
	std::cout << message;
}
void Console::Write(std::string message, std::string color)
{
	Console::Write(colorText("[", yellowFGColor));
	Console::Write(colorText(message, color));
	Console::Write(colorText("]  - ", yellowFGColor));
}
void Console::Write(std::string message, std::string fgColor, std::string bgColor)
{
	Console::Write(colorText("[", yellowFGColor));
	Console::Write(colorText(message, fgColor, bgColor));
	Console::Write(colorText("]  - ", yellowFGColor));
}
void Console::WriteDialogueAuthor(std::string coloredType)
{
	Console::Write(colorText("[", yellowFGColor));
	Console::Write(coloredType);
	Console::Write(colorText("]  - ", yellowFGColor));
}

std::string Console::ReadLine()
{
	std::string s;
	std::cin >> s;
	return s;
}

void Console::ExitError(std::string errMessage, std::string coloredType)
{
	Console console;
	console.WriteLine(errMessage, coloredType);
	std::cout << "Press Enter to Continue";
	std::cin.ignore();
	exit(1);
}
