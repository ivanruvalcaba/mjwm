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

#include "amm.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "vectorx.h"
#include "filex.h"
#include "messages.h"
#include "amm_options.h"
#include "system_environment.h"
#include "command_line_options_parser.h"
#include "icon_search/icon_search_interface.h"
#include "icon_search/xdg_search.h"
#include "icon_search/caching_search.h"
#include "qualified_icon_theme.h"
#include "desktop_entry_file_search.h"
#include "stats.h"
#include "menu.h"
#include "transformer/jwm.h"

namespace amm {

Amm::Amm()
{
    actual_searcher_ = NULL;
}

Amm::~Amm()
{
    if (actual_searcher_ != NULL) {
        delete actual_searcher_;
        actual_searcher_ = NULL;
    }
}

void Amm::validateEnvironment()
{
    if (!environment_.isValid()) {
        std::cerr << messages::homeNotSet() << std::endl;
        exit(2);
    }
}

void Amm::loadCommandLineOption(int argc, char **argv)
{
    options_ = CommandLineOptionsParser().parse(argc, argv, environment_.home());
    if (!options_.is_parsed) {
        std::cerr << messages::optionError();
        exit(2);
    }
    std::vector<std::string> deprecations = options_.deprecations;
    if (deprecations.size() > 0) {
        std::cerr << VectorX(deprecations).join("\n") << "\tProceeding..." << std::endl;
    }
    if (options_.is_help) {
        std::cout << messages::help();
        exit(0);
    }
    if (options_.is_version) {
        std::cout << messages::version();
        exit(0);
    }
    if (options_.summary_type != "short" && options_.summary_type != "normal" && options_.summary_type != "long") {
        std::cout << messages::badSummaryType(options_.summary_type);
        exit(2);
    }
}

void Amm::readCategories()
{
    std::string category_file_name = options_.category_file_name;
    std::vector<std::string> category_lines;

    if (category_file_name != "") {
        if (FileX(category_file_name).load(&category_lines)) {
            menu_.loadCustomCategories(category_lines);
        } else {
            std::cerr << messages::badCategoryFile(category_file_name) << std::endl;
            exit(1);
        }
    }
}

void Amm::registerIconService()
{
    if (options_.is_iconize) {
        QualifiedIconTheme theme(environment_, options_.icon_theme_name);
        actual_searcher_ = new icon_search::XdgSearch(48, theme);
        icon_search::IconSearchInterface *icon_searcher = new icon_search::CachingSearch(*actual_searcher_);
        menu_.registerIconService(*icon_searcher);
    }
}

void Amm::readDesktopEntryFiles()
{
    std::vector<std::string> input_directory_names = options_.input_directory_names;

    DesktopEntryFileSearch service;
    if (options_.override_default_directories) {
        service.registerDirectories(input_directory_names);
    } else {
        service.registerDefaultDirectories();
    }
    service.resolve();

    std::vector<std::string> bad_paths = service.badPaths();
    if (bad_paths.size() > 0) {
        std::cerr << "These paths couldn't be opened: " << VectorX(bad_paths).join(", ");
        std::cerr << std::endl << "Proceeding..." << std::endl;
    }
    desktop_entry_file_names_ = service.desktopEntryFileNames();
}

void Amm::populate()
{
    menu_.populate(desktop_entry_file_names_);
    if (menu_.summary().totalParsedFiles() == 0) {
        std::cerr << messages::noValidDesktopEntryFiles() << std::endl;
        exit(1);
    }
    menu_.sort();
}

void Amm::writeOutputFile()
{
    std::string output_file_name = options_.output_file_name;
    std::ofstream output_file(output_file_name.c_str());
    if (!output_file.good()) {
        std::cerr << messages::badOutputFile(output_file_name) << std::endl;
        exit(1);
    }

    transformer::Jwm jwm_transformer;
    std::vector<representation::RepresentationInterface*> representations = menu_.representations();

    output_file << messages::autogeneratedByAmm();

    for (std::vector<representation::RepresentationInterface*>::const_iterator representation = representations.begin(); representation != representations.end(); ++representation) {
        output_file << (*representation)->visit(jwm_transformer) << std::endl;
    }

    output_file.close();

    for (std::vector<representation::RepresentationInterface*>::iterator representation = representations.begin(); representation != representations.end(); ++representation) {
        delete *representation;
    }
}

void Amm::printSummary()
{
    std::cout << menu_.summary().details(options_.summary_type);
    std::cout << "Created " << options_.output_file_name << std::endl;
}

} // namespace amm
