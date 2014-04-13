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

#ifndef AMM_OPTIONS_H_
#define AMM_OPTIONS_H_

#include <string>
#include <vector>

namespace amm {
struct AmmOptions {
  AmmOptions() : is_parsed(false), is_help(false), is_version(false), is_iconize(false), output_file_name("./automenu") {}

  bool is_parsed;
  bool is_help;
  bool is_version;
  bool is_iconize;
  std::string output_file_name;
  std::vector<std::string> input_directory_names;
  std::string category_file_name;
  std::vector<std::string> deprecations;
};
} // namespace amm

#endif // AMM_AMM_OPTIONS_H_
