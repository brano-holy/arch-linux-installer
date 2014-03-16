#include "varconfigline.hpp"

#include "../utils/stringutils.hpp"

using namespace configuration;
using namespace utils;

VarConfigLine::VarConfigLine()
{
}

VarConfigLine::VarConfigLine(const string& name)
{
	this->name = name;
}

void VarConfigLine::parseLine(const string& line)
{
	CommentConfigLine::parseLine(line);

	string varLine;
	string::size_type hashPos = line.find('#');
	if(hashPos != string::npos) varLine = line.substr(0, hashPos);
	else varLine = line;

	varLine = StringUtils::trim(varLine);
	string::size_type startValPos = varLine.find("=\"") + 2;
	// string::size_type endValPos = varLine.find("\"", startValPos) - 1;

	name = StringUtils::trim(varLine.substr(0, startValPos - 2));
	value = varLine.substr(startValPos, varLine.length() - startValPos - 1);
}

string VarConfigLine::writeToLine()
{
	string comment = CommentConfigLine::writeToLine();
	if(comment.length() > 0) comment = ' ' + comment;

	return name + "=\"" + value + '"' + comment;
}

string VarConfigLine::getName()
{
	return name;
}

string VarConfigLine::getValue()
{
	return value;
}

void VarConfigLine::setValue(string value)
{
	this->value = value;
}
