#ifndef strops_h
#define strops_h

#include <vector>
#include <string>
#include "Console.h"
#include <cmath>
#include <iomanip>
#include <locale>
#include <iostream>


std::string JoinArrayPieces(std::string input[]);
std::string JoinArrayPieces(std::vector<std::string> input);
std::string PadString(const std::string& input, char padChar, size_t desiredLength);
std::string ExtractPaddedChars(const std::string& input, char padChar);
std::vector<std::string> SplitString(std::string str, std::string delim);
void ltrim(std::string& s);
void rtrim(std::string& s);
// Overridden by boost::trim
std::string TrimString(std::string s);
std::string ToUpper(std::string s);
std::string ToLower(std::string s);
bool CompareCharNumbers(const unsigned char* number1, const unsigned char* number2);
std::string ReplaceEscapeSymbols(std::string s);
bool StringStartsWith(std::string str, std::string substr);
bool CharStrStartsWith(unsigned char* str, char* substr, int len);
std::string CommaLargeNumber(long num);
std::string CommaLargeNumber(unsigned long long int num);
std::string CommaLargeNumberF(float num);
std::string CommaLargeNumberF(double num);
std::string multiplyHexByInteger(const std::string& hexNumber, int multiplier);
std::string divideHexByFloat(const std::string& hexNumber, float divisor);
std::string multiplyHexByFloat(const std::string& hexNumber, float multiplier);
float clampf(float x, float min, float max);
std::string FormatHPS(float input);
double round(float value, int decimal_places);
bool IsVersionGreaterOrEqual(std::string a, std::string b);



template<class T>
std::string FormatWithCommas(T value)
{
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << value;
	return ss.str();
}

#endif
