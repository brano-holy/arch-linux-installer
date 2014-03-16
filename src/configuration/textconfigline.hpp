#ifndef CONFIGURATION_TEXTCONFIGLINE_HPP
#define CONFIGURATION_TEXTCONFIGLINE_HPP

#include "configline.hpp"

namespace configuration {
	class TextConfigLine : public ConfigLine
	{
	protected:
		string line;

	public:
		virtual void parseLine(const string& line);
		virtual string writeToLine();
	};
}

#endif // CONFIGURATION_TEXTCONFIGLINE_HPP
