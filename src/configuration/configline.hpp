#ifndef CONFIGLINE_H
#define CONFIGLINE_H

#include <string>

using namespace std;

namespace configuration
{
	class ConfigLine
	{
	public:
		virtual void parseLine(const string& line) = 0;
		virtual string writeToLine() = 0;

	};
}

#endif // CONFIGLINE_H
