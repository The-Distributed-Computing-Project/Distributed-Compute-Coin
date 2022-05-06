
#include <string>
#include <vector>
#include "strops.h"

//using namespace std;

//string JoinArrayPieces(string input[]);
//vector<string> SplitString(string str, string delim);
//void ltrim(std::string& s);
//void rtrim(std::string& s);
//string trim(std::string s);
//string ToUpper(string s);
//bool StringStartsWith(string str, string substr);

std::string JoinArrayPieces(std::string input[])
{
	std::string outStr = "";
	for (int i = 0; i < sizeof(input) / sizeof(input[0]); i++)
	{
		outStr += input[i];
	}
	return outStr;
}

std::vector<std::string> SplitString(std::string str, std::string delim)
{
	std::vector<std::string> words;

	size_t last = 0;
	size_t next = 0;
	while ((next = str.find(delim, last)) != std::string::npos)
	{
		//cout << s.substr(last, next - last) << endl;  
		words.push_back(str.substr(last, next - last));
		last = next + 1;
	}
	//cout << s.substr(last) << endl;
	words.push_back(str.substr(last));

	//for (const auto& str : words) {
	//	cout << str << endl;
	//}

	return words;
}

// trim from start (in place)
void ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
void rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}
// Overrided by boost::trim
// trim from both ends (in place)
std::string TrimString(std::string s)
{
	std::string sN = s;
	ltrim(sN);
	rtrim(sN);
	return sN;
}

// Convert string to uppercase
std::string ToUpper(std::string s)
{
	std::string sN = s;
	for (auto& c : sN) c = toupper(c);
	return sN;
}

bool StringStartsWith(std::string str, std::string substr)
{
	for (int i = 0; i < substr.length(); i++)
	{
		if (str[i] != substr[i])
			return false;
	}
	return true;
}