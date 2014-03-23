
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

#ifndef __amm_messages__
#define __amm_messages__

namespace amm
{
	namespace messages
	{
		std::string help();
		std::string version();
		std::string option_error();
		std::string autogenerated_by_amm();
		std::string summary(size_t total_files, size_t total_parsed_files, size_t total_unclassified_parsed_files, std::vector<std::string> unparsed_files);
	}
}

#endif
