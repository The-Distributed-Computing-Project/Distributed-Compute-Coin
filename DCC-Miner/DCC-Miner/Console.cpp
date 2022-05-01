
#include <iostream>
#include <string>
#include <vector>

class Console
{
public:

	std::string blackFGColor = "\x1B[30m";
	std::string redFGColor = "\x1B[31m";
	std::string greenFGColor = "\x1B[32m";
	std::string yellowFGColor = "\x1B[33m";
	std::string blueFGColor = "\x1B[34m";
	std::string magentaFGColor = "\x1B[35m";
	std::string cyanFGColor = "\x1B[36m";
	std::string whiteFGColor = "\x1B[37m";

	std::string brightBlackFGColor = "\x1B[90m";
	std::string brightRedFGColor = "\x1B[91m";
	std::string brightGreenFGColor = "\x1B[92m";
	std::string brightYellowFGColor = "\x1B[93m";
	std::string brightBlueFGColor = "\x1B[94m";
	std::string brightMagentaFGColor = "\x1B[95m";
	std::string brightCyanFGColor = "\x1B[96m";
	std::string brightWhiteFGColor = "\x1B[97m";

	const std::string resetColor = "\033[0m";

	std::string colorText(std::string name, std::string color) {
		return color + name + resetColor;
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
	std::string Compiler()
	{
		return colorText("Rust Compiler", magentaFGColor);
	}
	std::string CompilerError()
	{
		return colorText("Rust Compiler-Error", redFGColor);
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
	void Write(std::string message, std::string coloredType)
	{
		Write(colorText("[", yellowFGColor));
		Write(coloredType);
		Write(colorText("]  - ", yellowFGColor));

		Write(message);
	}

	std::string ReadLine()
	{
		std::string s;
		std::cin >> s;
		return s;
	}
};
