#ifndef strops_h
#define strops_h

#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm> 
#include "Console.h"
#include <cmath>
#include <math.h>
#include <iomanip>
#include <locale>
#include <iostream>
#include "BaseConverter.h"

#define ERRORMSG( msg ) std::cerr << "\n\n[" << __FILE__ << ", at line: " << __LINE__ << "] " << msg << "  ";

std::string JoinArrayPieces(std::string input[]);
std::string JoinArrayPieces(std::vector<std::string> input);
std::string PadString(const std::string& input, char padChar, size_t desiredLength);
std::string PadStringRight(const std::string& input, char padChar, size_t desiredLength);
std::string ExtractPaddedChars(const std::string& input, char padChar);
std::vector<std::string> SplitString(std::string str, std::string delim);
bool StringContains(std::string str, std::string check);
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
std::string longDivision(const std::string& number, uint32_t divisor);
std::string hexLongDivision(const std::string& number, uint32_t divisor);
std::string divideHexByFloat(const std::string& hexNumber, float divisor);
std::string shiftHexNumber(const std::string& hexNumber, int digits);
std::string addHexNumbers(const std::string& hexNumberA, const std::string& hexNumberB);
std::string multiplyHexByFloat(const std::string& hexNumber, float multiplier);
float clampf(float x, float min, float max);
std::string FormatHPS(float input);
double round(float value, int decimal_places);
bool IsVersionGreaterOrEqual(std::string a, std::string b);
void addOneToHexStr(char* num, int len);
void addOneToHexStrInRange(char* num, int start, int len);
void csubstr(char* arr, char* outArr, int begin, int len, int totalLen, int& actualSize);
void cConcatInt(char* arr, char* outArr, int len, int x);
std::string SplitGetLastAfterChar(std::string str, std::string delim);

template<class T>
std::string FormatWithCommas(T value)
{
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << value;
	return ss.str();
}

#endif
