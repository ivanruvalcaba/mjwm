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

#include <iostream>
#include <string>
#include <algorithm>

#include "categories.h"

mjwm::categories::categories()
{
	_raw = "";
}


mjwm::categories::categories(std::string raw)
{
	_raw = raw;

	parse();
	sort();
}

void
mjwm::categories::parse()
{
	std::string raw = _raw;
	unsigned int size = raw.length();

	if (size >= 1 && raw.at(size - 1) != DELIM.at(0)) {
		raw += DELIM;
	}

	unsigned long int start = 0U;
	unsigned long int end = raw.find(DELIM);

	while (end != std::string::npos) {
		_categories.push_back(raw.substr(start, end - start));
		start = end + DELIM.length();
		end = raw.find(DELIM, start);
	}
}

void
mjwm::categories::sort()
{
	std::sort(_categories.begin(), _categories.end());
}

bool
mjwm::categories::is_a(std::string type) const
{
	return std::binary_search(_categories.begin(), _categories.end(), type);
}

std::ostream&
mjwm::operator << (std::ostream& stream, const mjwm::categories& categories)
{
	std::vector<std::string> container = categories._categories;

	for(std::vector<std::string>::iterator category = container.begin(); category != container.end(); ++category) {
		stream << *category << "; ";
	}

	return stream;
}