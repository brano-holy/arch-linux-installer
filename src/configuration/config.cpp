#include "config.hpp"

#include <fstream>

#include "commentconfigline.hpp"
#include "varconfigline.hpp"
#include "textconfigline.hpp"

#include "../utils/stringutils.hpp"
#include "../utils/memutils.hpp"

using namespace configuration;
using namespace utils;

Config::Config()
{

}

Config::Config(string configPath)
{
	load(configPath);
}

Config::~Config()
{
	MemUtils::deleteVector(content);
}

bool Config::load(string configPath)
{
	ifstream file(configPath);

	if(!file.is_open())
	{
		return false;
	}
	path = configPath;

	string line;
	while(getline(file, line))
	{
		string trimLine = StringUtils::trim(line);
		string::size_type hashPos = trimLine.find('#');

		string::size_type signPos;
		if(hashPos == string::npos)
		{
			signPos = trimLine.find("=\"");
		}
		else
		{
			string varPart = trimLine.substr(0, hashPos);
			signPos = varPart.find("=\"");
		}

		ConfigLine *configLine;
		if(signPos != string::npos)
		{
			configLine = new VarConfigLine();
			varContent.push_back((VarConfigLine *)configLine);
		}
		else if(hashPos != string::npos)
		{
			configLine = new CommentConfigLine();
		}
		else
		{
			configLine = new TextConfigLine();
		}
		configLine->parseLine(line);

		content.push_back(configLine);
	}

	return true;
}

bool Config::save()
{
	return save(path);
}

bool Config::save(string configPath)
{
	ofstream file(configPath);

	if(!file.is_open())
	{
		return false;
	}

	for(vector<ConfigLine*>::size_type i = 0; i < content.size(); i++)
	{
		file << content.at(i)->writeToLine() << endl;
	}

	return true;
}

string Config::getValue(string name)
{
	for(vector<VarConfigLine*>::size_type i = 0; i < varContent.size(); i++)
	{
		if(varContent.at(i)->getName() == name)
		{
			return varContent.at(i)->getValue();
		}
	}

	return "";
}

void Config::setValue(string name, string value)
{
	bool found = false;
	for(vector<VarConfigLine*>::size_type i = 0; i < varContent.size(); i++)
	{
		if(varContent.at(i)->getName() == name)
		{
			varContent.at(i)->setValue(value);
			found = true;
			break;
		}
	}

	if(!found)
	{
		addValue(name, value);
	}
}

void Config::addValue(string name, string value)
{
	VarConfigLine *varLine = new VarConfigLine(name);
	varLine->setValue(value);
	content.push_back(varLine);
	varContent.push_back(varLine);
}

void Config::addValue(string name, string value, string comment)
{
	VarConfigLine *varLine = new VarConfigLine(name);
	varLine->setValue(value);
	varLine->setComment(comment);
	content.push_back(varLine);
	varContent.push_back(varLine);
}

vector<string> Config::getValues(string name)
{
	return StringUtils::split(getValue(name), ' ');
}

void Config::setValues(string name, vector<string> values)
{
	setValue(name, StringUtils::join(' ', values));
}

void Config::addValues(string name, vector<string> values)
{
	addValue(name, StringUtils::join(' ', values));
}

void Config::addValues(string name, vector<string> values, string comment)
{
	addValue(name, StringUtils::join(' ', values), comment);
}
