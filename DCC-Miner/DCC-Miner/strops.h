#ifndef strops_h
#define strops_h

std::string JoinArrayPieces(std::string input[]);
std::vector<std::string> SplitString(std::string str, std::string delim);
void ltrim(std::string& s);
void rtrim(std::string& s);
// Overrided by boost::trim
//string trim(std::string s);
std::string ToUpper(std::string s);
bool StringStartsWith(std::string str, std::string substr);

#endif
