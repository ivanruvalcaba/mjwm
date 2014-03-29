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
#include <vector>
#include <cstdlib>

#include "vectorx.h"
#include "messages.h"
#include "command_line_options.h"
#include "icon_service.h"
#include "desktop_file_names.h"
#include "representation.h"
#include "menu.h"
#include "jwm/transformer.h"

namespace amm
{
	class main
	{
	private:
		int _argc;
		char **_argv;

		amm::command_line_options _command_line_options;
		amm::menu _menu;
		std::vector<std::string> _desktop_file_names;

	public:
		main(int argc, char **argv);
		void load_command_line_option();
		void register_icon_service();
		void read_categories();
		void read_desktop_files();
		void populate();
		void write_output_file();
		void print_summary();
	};
}

amm::main::main(int argc, char **argv)
{
	_argc = argc;
	_argv = argv;
}

void
amm::main::load_command_line_option()
{
	_command_line_options = amm::command_line_options();
	if (!_command_line_options.parse(_argc, _argv)) {
		std::cerr << amm::messages::option_error();
		exit(2);
	}
	std::vector<std::string> deprecations = _command_line_options.deprecations();
	if (deprecations.size() > 0) {
		std::cerr << amm::vectorx(deprecations).join("\n") << std::endl << "Proceeding..." << std::endl;
	}
	if (_command_line_options.is_help()) {
		std::cout << amm::messages::help() << std::endl;
		exit(0);
	}
	if (_command_line_options.is_version()) {
		std::cout << amm::messages::version() << std::endl;
		exit(0);
	}
}

void
amm::main::read_categories()
{
	std::string category_file_name = _command_line_options.category_file_name();
	std::vector<std::string> category_lines;

	if (category_file_name != "") {
		std::ifstream category_file(category_file_name.c_str());
		if (category_file.good()) {
			std::string line;
			while (std::getline(category_file, line)) {
				category_lines.push_back(line);
			}
			_menu.load_custom_categories(category_lines);
			category_file.close();
		} else {
			std::cerr << amm::messages::bad_category_file(category_file_name) << std::endl;
			exit(1);
		}
	}
}

void
amm::main::register_icon_service()
{
	amm::icon_service icon_service;
	icon_service.register_extension(_command_line_options.icon_extension());
	_menu.register_icon_service(icon_service);
}

void
amm::main::read_desktop_files()
{
	std::vector<std::string> input_directory_names = _command_line_options.input_directory_names();

	amm::desktop_file_names desktop_file_names;
	desktop_file_names.register_directories_with_default_fallback(input_directory_names);
	desktop_file_names.resolve();

	std::vector<std::string> bad_paths = desktop_file_names.bad_paths();
	if (bad_paths.size() > 0) {
		std::cerr << "These paths couldn't be opened: " << amm::vectorx(bad_paths).join(", ");
		std::cerr << std::endl << "Proceeding..." << std::endl;
	}
	_desktop_file_names = desktop_file_names.all();
}

void
amm::main::populate()
{
	_menu.populate(_desktop_file_names);
	if (_menu.total_parsed_files() == 0) {
		std::cerr << amm::messages::no_valid_desktop_files() << std::endl;
		exit(1);
	}
	_menu.sort();
}

void
amm::main::write_output_file()
{
	std::string output_file_name = _command_line_options.output_file_name();
	std::ofstream output_file(output_file_name.c_str());
	if (!output_file.good()) {
		std::cerr << amm::messages::bad_output_file(output_file_name) << std::endl;
		exit(1);
	}

	output_file << amm::messages::autogenerated_by_amm();
	amm::transformer::jwm jwm_transformer;
	std::vector<amm::representation::base*> representations = _menu.representations();
	for (std::vector<amm::representation::base*>::iterator iter = representations.begin(); iter != representations.end(); ++iter) {
		output_file << (*iter)->visit(jwm_transformer) << std::endl;
	}
	for (std::vector<amm::representation::base*>::iterator iter = representations.begin(); iter != representations.end(); ++iter) {
		delete *iter;
	}

	output_file.close();
}

void
amm::main::print_summary()
{
	std::cout << amm::messages::summary(
		_desktop_file_names.size(),
		_menu.total_parsed_files(),
		_menu.total_unclassified_parsed_files(),
		_menu.unparsed_file_names()
	);
}

int main(int argc, char *argv[])
{
	amm::main operation(argc, argv);
	operation.load_command_line_option();
	operation.register_icon_service();
	operation.read_categories();
	operation.read_desktop_files();
	operation.populate();
	operation.write_output_file();
	operation.print_summary();
}
