#include "textconfigline.hpp"

using namespace configuration;

void TextConfigLine::parseLine(const string& line)
{
	this->line = line;
}

string TextConfigLine::writeToLine()
{
	return line;
}
