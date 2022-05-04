
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

void Console::PrintColored(std::string text, std::string fgColor, std::string bgColor)
{
#if WINDOWS
	auto fg = dye::white(text);
	if (fgColor == blackFGColor)
		fg = dye::black(text);
	else if (fgColor == redFGColor)
		fg = dye::black(text);
	else if (fgColor == greenFGColor)
		fg = dye::green(text);
	else if (fgColor == yellowFGColor)
		fg = dye::yellow(text);
	else if (fgColor == blueFGColor)
		fg = dye::blue(text);
	else if (fgColor == magentaFGColor)
		fg = dye::magenta(text);
	else if (fgColor == cyanFGColor)
		fg = dye::cyan(text);
	else if (fgColor == whiteFGColor)
		fg = dye::white(text);
	std::cout << fg;
#else
	cout << fgColor + bgColor + name + resetColor;
#endif
}

void Console::NetworkPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Network", cyanFGColor, "");
	Console::PrintColored("]  - ", yellowFGColor, "");
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
	Console::PrintColored("]  - ", yellowFGColor, "");
}
void Console::MiningErrorPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Mining-Error", redFGColor, "");
	Console::PrintColored("]  - ", yellowFGColor, "");
}
void Console::RustPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Rust", magentaFGColor, "");
	Console::PrintColored("]  - ", yellowFGColor, "");
}
void Console::CompilerErrorPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Rust-Error", redFGColor, "");
	Console::PrintColored("]  - ", yellowFGColor, "");
}
void Console::BlockCheckerPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Block-Checker", blackFGColor, cyanBGColor);
	Console::PrintColored("]  - ", yellowFGColor, "");
}
void Console::DebugPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Debug", yellowFGColor, "");
	Console::PrintColored("]  - ", yellowFGColor, "");
}
void Console::ErrorPrint()
{
	Console::PrintColored("[", yellowFGColor, "");
	Console::PrintColored("Error", redFGColor, "");
	Console::PrintColored("]  - ", yellowFGColor, "");
}

void Console::WriteLine()
{
	std::cout << std::endl;
}
void Console::WriteLine(std::string message)
{
	std::cout << message << std::endl;
}
void Console::WriteLine(std::string message, std::string fgColor, std::string bgColor)
{
	Console::PrintColored(message, fgColor, bgColor);
	Console::WriteLine();
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
	Console::PrintColored(message, color, "");
}
void Console::Write(std::string message, std::string fgColor, std::string bgColor)
{
	Console::PrintColored(message, fgColor, bgColor);
}
//void Console::WriteDialogueAuthor(std::string coloredType)
//{
//	Console::PrintColored(coloredType, "", "");
//}

std::string Console::ReadLine()
{
	std::string s;
	std::cin >> s;
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
