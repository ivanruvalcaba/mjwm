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

#ifndef __amm_jwm_transform__
#define __amm_jwm_transform__

#include <string>

#include "../icon_service.h"
#include "../desktop_file.h"
#include "../subcategory.h"
#include "menu_entry.h"

namespace amm
{
	namespace jwm
	{
		// Understands convertion of a FreeDesktop .desktop file into a Program entry in JWM configuration file
		class transformer
		{
		private:
			amm::icon_service _icon_service;
			std::string remove_field_code(std::string input) const;

		public:
			void register_icon_service(amm::icon_service icon_service);
			amm::jwm::menu_entry transform(std::vector<amm::subcategory> subcategories, amm::subcategory unclassified_subcategory);
			amm::jwm::menu_entry transform(amm::desktop_file desktop_file);
		};
	}
}

#endif