#include "stringutils.hpp"

#include <sstream>

using namespace utils;

bool StringUtils::startsWith(const string& str, const string& prefix)
{
	return (str.substr(0, prefix.length()) == prefix);
}

vector<string> StringUtils::split(const string& str, char delim)
{
	vector<string> parts;
	istringstream iss(str);

	string part;
	while(getline(iss, part, delim))
	{
		parts.push_back(part);
	}

	return parts;
}

string StringUtils::join(char glue, const vector<string>& values)
{
	string str = values[0];
	for(vector<string>::size_type i = 1; i < values.size(); i++)
	{
		str += glue;
		str += values[i];
	}

	return str;
}

string StringUtils::trim(const string& str)
{
	string::size_type firstPos = str.find_first_not_of(' ');
	if(firstPos == string::npos)
	{
		return "";
	}

	string::size_type lastPos = str.find_last_not_of(' ');
	return str.substr(firstPos, lastPos - firstPos + 1);
}
