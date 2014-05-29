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
#include "command_line_options_parser.h"
#include "service/icon_service_interface.h"
#include "service/icon/naive_scan.h"
#include "desktop_entry_file_search.h"
#include "stats.h"
#include "menu.h"
#include "transformer/jwm.h"

namespace amm {

Amm::Amm() {
  options_ = new AmmOptions(environment_.Home());
}

Amm::~Amm() {
  delete options_;
}

void Amm::ValidateEnvironment() {
  if (environment_.Home() == "") {
    std::cerr << messages::HomeNotSet() << std::endl;
    exit(2);
  }
}

void Amm::LoadCommandLineOption(int argc, char **argv) {
  CommandLineOptionsParser().Parse(argc, argv, options_);
  if (!options_->is_parsed) {
    std::cerr << messages::OptionError();
    exit(2);
  }
  std::vector<std::string> deprecations = options_->deprecations;
  if (deprecations.size() > 0) {
    std::cerr << VectorX(deprecations).Join("\n") << "\tProceeding..." << std::endl;
  }
  if (options_->is_help) {
    std::cout << messages::Help();
    exit(0);
  }
  if (options_->is_version) {
    std::cout << messages::Version();
    exit(0);
  }
  if (options_->summary_type != "short" && options_->summary_type != "normal" && options_->summary_type != "long") {
    std::cout << messages::BadSummaryType(options_->summary_type);
    exit(2);
  }
}

void Amm::ReadCategories() {
  std::string category_file_name = options_->category_file_name;
  std::vector<std::string> category_lines;

  if (category_file_name != "") {
    if (FileX(category_file_name).Load(&category_lines)) {
      menu_.LoadCustomCategories(category_lines);
    } else {
      std::cerr << messages::BadCategoryFile(category_file_name) << std::endl;
      exit(1);
    }
  }
}

void Amm::RegisterIconService() {
  if (options_->is_iconize) {
    service::IconServiceInterface *icon_service = new service::icon::NaiveScan();
    menu_.RegisterIconService(*icon_service);
  }
}

void Amm::ReadDesktopEntryFiles() {
  std::vector<std::string> input_directory_names = options_->input_directory_names;

  DesktopEntryFileSearch service;
  if (options_->override_default_directories) {
    service.RegisterDirectories(input_directory_names);
  } else {
    service.RegisterDefaultDirectories();
  }
  service.Resolve();

  std::vector<std::string> bad_paths = service.BadPaths();
  if (bad_paths.size() > 0) {
    std::cerr << "These paths couldn't be opened: " << VectorX(bad_paths).Join(", ");
    std::cerr << std::endl << "Proceeding..." << std::endl;
  }
  desktop_entry_file_names_ = service.DesktopEntryFileNames();
}

void Amm::Populate() {
  menu_.Populate(desktop_entry_file_names_);
  if (menu_.Summary().TotalParsedFiles() == 0) {
    std::cerr << messages::NoValidDesktopEntryFiles() << std::endl;
    exit(1);
  }
  menu_.Sort();
}

void Amm::WriteOutputFile() {
  std::string output_file_name = options_->output_file_name;
  std::ofstream output_file(output_file_name.c_str());
  if (!output_file.good()) {
    std::cerr << messages::BadOutputFile(output_file_name) << std::endl;
    exit(1);
  }

  transformer::Jwm jwm_transformer;
  std::vector<RepresentationInterface*> representations = menu_.Representations();

  output_file << messages::AutogeneratedByAmm();

  for (std::vector<RepresentationInterface*>::const_iterator representation = representations.begin(); representation != representations.end(); ++representation) {
    output_file << (*representation)->visit(jwm_transformer) << std::endl;
  }

  output_file.close();

  for (std::vector<RepresentationInterface*>::iterator representation = representations.begin(); representation != representations.end(); ++representation) {
    delete *representation;
  }
}

void Amm::PrintSummary() {
  std::cout << menu_.Summary().Details(options_->summary_type);
  std::cout << "Created " << options_->output_file_name << std::endl;
}

} // namespace amm
