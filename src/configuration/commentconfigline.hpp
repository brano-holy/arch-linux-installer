#ifndef TEXTCONFIGLINE_H
#define TEXTCONFIGLINE_H

#include "configline.hpp"

namespace configuration
{
	class CommentConfigLine : public ConfigLine
	{
	private:
		bool hasComment;
		string comment;

	public:
		CommentConfigLine();

		virtual void parseLine(const string& line);
		virtual string writeToLine();

		string getComment() const;
		void setComment(const string& comment);
		void removeComment();

	};
}

#endif // TEXTCONFIGLINE_H
