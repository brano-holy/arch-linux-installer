#ifndef UTILS_SYSTEMUTILS_HPP
#define UTILS_SYSTEMUTILS_HPP

#include <string>

using namespace std;

namespace utils
{
	class SystemUtils
	{
	public:
		static int csystem(string cmd);
		static string ssystem(string cmd, int bufferSize = 256);

		static int getRAMSize(char &unit);
		static int alignSize(int minSize, int aligment = 2048);
	};
}

#endif // UTILS_SYSTEMUTILS_H
