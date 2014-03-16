#include "commentconfigline.hpp"

using namespace configuration;

using namespace std;

CommentConfigLine::CommentConfigLine()
{
	hasComment = false;
}

void CommentConfigLine::parseLine(const string& line)
{
	string::size_type hashPos = line.find('#');
	if(hashPos != string::npos)
	{
		hasComment = true;
		comment = line.substr(hashPos + 1);
	}
}

string CommentConfigLine::writeToLine()
{
	if(hasComment)
	{
		return '#' + comment;
	}

	return "";
}

string CommentConfigLine::getComment() const
{
	return comment;
}

void CommentConfigLine::setComment(const string& comment)
{
	this->comment = comment;
	hasComment = true;
}

void CommentConfigLine::removeComment()
{
	comment = "";
	hasComment = false;
}
