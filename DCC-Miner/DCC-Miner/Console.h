#ifndef console_h
#define console_h

#include <queue>

// Declare the global print queue
extern std::queue<std::ostream> printQueue;


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

	std::string colorText(std::string name, std::string color);
	std::string colorText(std::string name, std::string fgColor, std::string bgColor);

	void PrintColored(std::string text, std::string fgColor, std::string bgColor);

	void NetworkPrint();
	void NetworkErrorPrint();
	void MiningPrint();
	void MiningErrorPrint();
	void RustPrint();
	void CompilerErrorPrint();
	void BlockCheckerPrint();
	void DebugPrint();
	void SystemPrint();
	void ErrorPrint();

	void WriteLine();
	void WriteLine(std::string message);
	void WriteLine(std::string message, std::string coloredType);
	void WriteLine(std::string message, std::string fgColor, std::string bgColor);

	void Write();
	void Write(std::string message);
	void Write(std::string message, std::string color);
	void Write(std::string message, std::string fgColor, std::string bgColor);
	void WriteDialogueAuthor(std::string coloredType);
	void WriteIndented(std::string message, std::string fgColor, std::string bgColor, int indents);
	void WriteBulleted(std::string message, std::string fgColor, std::string bgColor, int indents, std::string bullet);
	void WriteBulleted(std::string message, std::string fgColor, std::string bgColor, int indents);
	void WriteBulleted(std::string message, int indents, std::string bullet);
	void WriteBulleted(std::string message, int indents);

	void WriteLineCharArrayOfLen(char* message, int len);

	std::string ReadLine();

	void ExitError(std::string errMessage);

	
	//Console();
};


#endif
