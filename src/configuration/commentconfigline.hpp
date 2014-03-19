/*
 * Arch Linux Installer
 * Copyright (C) 2014  Branislav Holý <brano.holy@gmail.com>
 *
 * This file is part of Arch Linux Installer.
 *
 * Arch Linux Installer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arch Linux Installer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arch Linux Installer.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
