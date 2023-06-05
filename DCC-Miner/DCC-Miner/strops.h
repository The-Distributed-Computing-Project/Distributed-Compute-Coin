#ifndef strops_h
#define strops_h

#include <vector>
#include <string>

std::string JoinArrayPieces(std::string input[]);
std::string JoinArrayPieces(std::vector<std::string> input);
std::vector<std::string> SplitString(std::string str, std::string delim);
void ltrim(std::string& s);
void rtrim(std::string& s);
// Overrided by boost::trim
std::string TrimString(std::string s);
std::string ToUpper(std::string s);
std::string ToLower(std::string s);
std::string ReplaceEscapeSymbols(std::string s);
bool StringStartsWith(std::string str, std::string substr);
bool CharStrStartsWith(unsigned char* str, char* substr, int len);
std::string CommaLargeNumber(int num);
std::string CommaLargeNumber(float num);

#endif
