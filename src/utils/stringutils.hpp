#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <vector>

using namespace std;

namespace utils
{
	class StringUtils
	{
		public:
			static bool startsWith(const string& str, const string& prefix);
			static vector<string> split(const string& str, char delim);
			static string join(char glue, const vector<string>& values);
			static string trim(const string& str);
	};
}

#endif // STRINGUTILS_H
