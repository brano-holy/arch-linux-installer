#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>
#include "configline.hpp"
#include "varconfigline.hpp"

using namespace std;

namespace configuration
{
	class Config
	{
		private:
			string path;

			vector<ConfigLine*> content;
			vector<VarConfigLine*> varContent;

		public:
			Config();
			Config(string configPath);

			~Config();

			bool load(string configPath);
			bool save();
			bool save(string configPath);

			string getValue(string name);
			void setValue(string name, string value);
			void addValue(string name, string value);
			void addValue(string name, string value, string comment);

			vector<string> getValues(string name);
			void setValues(string name, vector<string> values);
			void addValues(string name, vector<string> values);
			void addValues(string name, vector<string> values, string comment);
	};
}

#endif // CONFIG_H
