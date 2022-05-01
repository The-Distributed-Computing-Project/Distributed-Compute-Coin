
#include <iostream>
#include <string>
#include <vector>

class Console
{
public:
	// Foreground colors
	const std::string blackFGColor = "\x1B[30m";
	const std::string redFGColor = "\x1B[31m";
	const std::string greenFGColor = "\x1B[32m";
	const std::string yellowFGColor = "\x1B[33m";
	const std::string blueFGColor = "\x1B[34m";
	const std::string magentaFGColor = "\x1B[35m";
	const std::string cyanFGColor = "\x1B[36m";
	const std::string whiteFGColor = "\x1B[37m";
	const std::string brightBlackFGColor = "\x1B[90m";
	const std::string brightRedFGColor = "\x1B[91m";
	const std::string brightGreenFGColor = "\x1B[92m";
	const std::string brightYellowFGColor = "\x1B[93m";
	const std::string brightBlueFGColor = "\x1B[94m";
	const std::string brightMagentaFGColor = "\x1B[95m";
	const std::string brightCyanFGColor = "\x1B[96m";
	const std::string brightWhiteFGColor = "\x1B[97m";

	//Background colors
	const std::string blackBGColor = "\x1B[40m";
	const std::string redBGColor = "\x1B[41m";
	const std::string greenBGColor = "\x1B[42m";
	const std::string yellowBGColor = "\x1B[43m";
	const std::string blueBGColor = "\x1B[44m";
	const std::string magentaBGColor = "\x1B[45m";
	const std::string cyanBGColor = "\x1B[46m";
	const std::string whiteBGColor = "\x1B[47m";
	const std::string brightBlackBGColor = "\x1B[100m";
	const std::string brightRedBGColor = "\x1B[101m";
	const std::string brightGreenBGColor = "\x1B[102m";
	const std::string brightYellowBGColor = "\x1B[103m";
	const std::string brightBlueBGColor = "\x1B[104m";
	const std::string brightMagentaBGColor = "\x1B[105m";
	const std::string brightCyanBGColor = "\x1B[106m";
	const std::string brightWhiteBGColor = "\x1B[107m";
	
	// Reset colors
	const std::string resetColor = "\033[0m";

	std::string colorText(std::string name, std::string color) {
		return color + name + resetColor;
	}
	std::string colorText(std::string name, std::string fgColor, std::string bgColor) {
		return fgColor + bgColor + name + resetColor;
	}

	std::string Network()
	{
		return colorText("Network", cyanFGColor);
	}
	std::string NetworkError()
	{
		return colorText("Network-Error", redFGColor);
	}
	std::string Mining()
	{
		return colorText("Mining", greenFGColor);
	}
	std::string MiningError()
	{
		return colorText("Mining-Error", redFGColor);
	}
	std::string Rust()
	{
		return colorText("Rust", magentaFGColor);
	}
	std::string CompilerError()
	{
		return colorText("Rust-Error", redFGColor);
	}
	std::string BlockChecker()
	{
		return colorText("Block-Checker", blackFGColor, cyanBGColor);
	}
	std::string Debug()
	{
		return colorText("Debug", yellowFGColor);
	}
	std::string Error()
	{
		return colorText("Error", redFGColor);
	}

	void WriteLine()
	{
		std::cout << std::endl;
	}
	void WriteLine(std::string message)
	{
		std::cout << message << std::endl;
	}
	void WriteLine(std::string message, std::string coloredType)
	{
		Write(colorText("[", yellowFGColor));
		Write(coloredType);
		Write(colorText("]  - ", yellowFGColor));

		WriteLine(message);
	}

	void Write()
	{
		std::cout;
	}
	void Write(std::string message)
	{
		std::cout << message;
	}
	void Write(std::string message, std::string color)
	{
		Write(colorText("[", yellowFGColor));
		Write(colorText(message, color));
		Write(colorText("]  - ", yellowFGColor));
	}
	void Write(std::string message, std::string fgColor, std::string bgColor)
	{
		Write(colorText("[", yellowFGColor));
		Write(colorText(message, fgColor, bgColor));
		Write(colorText("]  - ", yellowFGColor));
	}
	void WriteDialogueAuthor(std::string coloredType)
	{
		Write(colorText("[", yellowFGColor));
		Write(coloredType);
		Write(colorText("]  - ", yellowFGColor));
	}

	std::string ReadLine()
	{
		std::string s;
		std::cin >> s;
		return s;
	}
};
