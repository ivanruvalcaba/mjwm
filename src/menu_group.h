/*
    This file is part of mjwm.
    Copyright (C) 2014  Chirantan Mitra <chirantan.mitra@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __amm_menu_group__
#define __amm_menu_group__

#include <iostream>
#include <string>
#include <vector>

#include "desktop_file.h"
#include "menu_subcategory.h"
#include "transform/jwm.h"

namespace amm
{
	// Understands how a directory with FreeDesktop .desktop files is converted to a JWM menu section
	// TODO - multiple responsibilities - split class
	class menu_group
	{
	private:
		std::vector<std::string> _desktop_file_names;
		std::string _icon_extension; // TODO : find using a icon service
		std::string _error;
		bool _parsed;

		std::vector<amm::menu_subcategory> _menu_subcategories;
		amm::menu_subcategory _unclassified_menu_subcategory;

		bool classify(amm::desktop_file entry);
		void create_categories();

	public:
		menu_group(std::vector<std::string> desktop_file_names, std::string icon_extension);
		void populate();
		bool is_valid() const; // TODO : replace with stats - caller decides on error message
		std::string error() const;  // TODO : replace with stats - caller decides on error message
		void sort();
		void write(std::string output_filename);
	};
}

#endif
