#ifndef VARCONFIGLINE_H
#define VARCONFIGLINE_H

#include "configline.hpp"
#include "commentconfigline.hpp"

namespace configuration
{
	class VarConfigLine : public CommentConfigLine
	{
	protected:
		string name;
		string value;

	public:
		VarConfigLine();
		VarConfigLine(const string& name);

		virtual void parseLine(const string& line);
		virtual string writeToLine();

		string getName();

		string getValue();
		void setValue(string value);
	};
}

#endif // VARCONFIGLINE_H
