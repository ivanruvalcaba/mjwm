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

#define CATCH_CONFIG_MAIN

#include <iostream>
#include <string>
#include <sstream>

#include "catch.hpp"

#include "menu.h"
#include "representation.h"
#include "transformer.h"

static const std::string fixtures_directory = "test/fixtures/";

class TestTransformer : public amm::TransformerInterface
{
  std::string transform(amm::representation::menu_start *entry)
  {
    std::stringstream stream;
    stream << "Menu start--> name: " << entry->name();
    return stream.str();
  }

  std::string transform(amm::representation::menu_end *entry)
  {
    std::stringstream stream;
    stream << "Menu end--> name: " << entry->name();
    return stream.str();
  }

  std::string transform(amm::representation::subcategory_start *entry)
  {
    std::stringstream stream;
    stream << "Subsection start--> name: " << entry->name() << " icon: " << entry->icon();
    return stream.str();
  }

  std::string transform(amm::representation::subcategory_end *entry)
  {
    std::stringstream stream;
    stream << "Subsection end--> name: " << entry->name();
    return stream.str();
  }

  std::string transform(amm::representation::menu_entry *entry)
  {
    std::stringstream stream;
    stream << "Program--> name: " << entry->name() << " icon: " << entry->icon() << " executable: " << entry->executable();
    return stream.str();
  }
};

static void clear_memory(std::vector<amm::representation::base*> representations)
{
  for (std::vector<amm::representation::base*>::iterator iter = representations.begin(); iter != representations.end(); ++iter) {
    delete *iter;
  }
}


SCENARIO("amm::Menu custom categories", "[menu]") {
  GIVEN("A menu") {
    amm::menu menu;

    WHEN("loaded with custom categories") {
      std::vector<std::string> lines;
      lines.push_back("Accessories:accessories:Utility");
      lines.push_back("Games:games:Game");
      menu.load_custom_categories(lines);

      THEN("it stores them in its subcategories") {
        std::vector<amm::Subcategory> subcategories = menu.subcategories();

        REQUIRE(subcategories.size() == 2);
        REQUIRE(subcategories[0].display_name() == "Accessories");
        REQUIRE(subcategories[1].display_name() == "Games");
      }
    }

    WHEN("given a line beginning with '#'") {
      std::vector<std::string> lines;
      lines.push_back("# Comments");
      lines.push_back("#More:comment:here");
      menu.load_custom_categories(lines);

      THEN("it ignores it the lines begenning with '#'") {
        std::vector<amm::Subcategory> subcategories = menu.subcategories();

        REQUIRE(subcategories.size() == 0);
      }
    }

    WHEN("given a line with less than three tokens") {
      std::vector<std::string> lines;
      lines.push_back("Accessories::Accessories");
      lines.push_back("Game:Games");
      menu.load_custom_categories(lines);

      THEN("it ignores the lines") {
        std::vector<amm::Subcategory> subcategories = menu.subcategories();

        REQUIRE(subcategories.size() == 0);
      }
    }


    WHEN("loaded with custom categories with more than three tokens") {
      std::vector<std::string> lines;
      lines.push_back("Games:games:Game:Fun");
      menu.load_custom_categories(lines);

      THEN("it has multiple classifications in its subcategories") {
        std::vector<amm::Subcategory> subcategories = menu.subcategories();

        REQUIRE(subcategories.size() == 1);
        REQUIRE(subcategories[0].display_name() == "Games");

        std::vector<std::string> classification_names = subcategories[0].classification_names();
        REQUIRE(classification_names.size() == 2);
        REQUIRE(classification_names[0] == "Game");
        REQUIRE(classification_names[1] == "Fun");
      }
    }

    WHEN("given a line with more than three tokens but with some classification names as empty") {
      std::vector<std::string> lines;
      lines.push_back("Games:games:Game::Fun::Frolic");
      menu.load_custom_categories(lines);

      THEN("it ignores the missing classification names") {
        std::vector<amm::Subcategory> subcategories = menu.subcategories();

        REQUIRE(subcategories.size() == 1);

        std::vector<std::string> classification_names = subcategories[0].classification_names();
        REQUIRE(classification_names.size() == 3);
        REQUIRE(classification_names[0] == "Game");
        REQUIRE(classification_names[1] == "Fun");
        REQUIRE(classification_names[2] == "Frolic");
      }
    }

    WHEN("given a line without a single classification name") {
      std::vector<std::string> lines;
      lines.push_back("Games:games:::");
      menu.load_custom_categories(lines);

      THEN("it ignores the lines with missing classification names") {
        std::vector<amm::Subcategory> subcategories = menu.subcategories();

        REQUIRE(subcategories.size() == 0);
      }
    }
  }
}

SCENARIO("amm::Menu statistics", "[menu]") {
  GIVEN("A menu") {
    amm::menu menu;

    WHEN("successfully parsed") {
      std::vector<std::string> files;
      files.push_back(fixtures_directory + "vlc.desktop");
      files.push_back(fixtures_directory + "mousepad.desktop");
      menu.populate(files);
      amm::Stats stats = menu.stats();

      THEN("it holds the number of files parsed sucessfully") {
        REQUIRE(stats.TotalParsedFiles() == 2);
      }
    }

    WHEN("parsing an unclassified desktop file") {
      std::vector<std::string> files;
      files.push_back(fixtures_directory + "unclassified.desktop");
      files.push_back(fixtures_directory + "mousepad.desktop");
      menu.populate(files);
      amm::Stats stats = menu.stats();

      THEN("it holds the number of unclassified files") {
        REQUIRE(stats.TotalUnclassifiedFiles() == 1);
      }

      THEN("it holds the categories that weren't handled") {
        std::vector<std::string> unhandled_classifications = stats.UnhandledClassifications();

        REQUIRE(unhandled_classifications.size() == 1);
        REQUIRE(unhandled_classifications[0] == "GTK");
      }
    }

    WHEN("parsing a file suppressed with 'NoDisplay=true'") {
      std::vector<std::string> files;
      files.push_back(fixtures_directory + "mousepad.desktop");
      files.push_back(fixtures_directory + "suppressed.desktop");
      menu.populate(files);
      amm::Stats stats = menu.stats();

      THEN("it holds the number of suppressed files") {
        REQUIRE(stats.TotalSuppressedFiles() == 1);
      }
    }

    WHEN("parsing a file with missing entries") {
      std::vector<std::string> files;
      files.push_back(fixtures_directory + "vlc.desktop");
      files.push_back(fixtures_directory + "missing.desktop");
      menu.populate(files);
      amm::Stats stats = menu.stats();

      THEN("it holds the a list of unparsed files") {
        std::vector<std::string> unparsed_files = stats.UnparsedFiles();

        REQUIRE(unparsed_files.size() == 1);
        REQUIRE(unparsed_files[0] == fixtures_directory + "missing.desktop");
      }
    }

    WHEN("parsing a file with missing and suppressed entries") {
      std::vector<std::string> files;
      files.push_back(fixtures_directory + "suppressed.desktop");
      files.push_back(fixtures_directory + "suppressedinvalid.desktop");
      files.push_back(fixtures_directory + "missing.desktop");
      menu.populate(files);
      amm::Stats stats = menu.stats();

      THEN("missing entries excludes suppressed files") {
        std::vector<std::string> unparsed_files = stats.UnparsedFiles();

        REQUIRE(unparsed_files.size() == 1);
        REQUIRE(unparsed_files[0] == fixtures_directory + "missing.desktop");
      }
    }
  }
}

SCENARIO("amm::Menu representations", "[menu]") {
  GIVEN("A menu") {
    amm::menu menu;

    WHEN("transformed to a representations") {
      std::vector<std::string> files;
      files.push_back(fixtures_directory + "vlc.desktop");
      files.push_back(fixtures_directory + "mousepad.desktop");

      menu.populate(files);
      std::vector<amm::representation::base*> representations = menu.representations();
      TestTransformer test_transformer;

      THEN("it stores menu start, subcategory start, menu entry, subcategory end and menu end") {
        REQUIRE(representations.size() == 8);
        REQUIRE(representations[0]->visit(test_transformer) == "Menu start--> name: Menu start");
        REQUIRE(representations[1]->visit(test_transformer) == "Subsection start--> name: Accessories icon: accessories");
        REQUIRE(representations[2]->visit(test_transformer) == "Program--> name: Mousepad icon: accessories-text-editor executable: mousepad %F");
        REQUIRE(representations[3]->visit(test_transformer) == "Subsection end--> name: Accessories end");
        REQUIRE(representations[4]->visit(test_transformer) == "Subsection start--> name: Multimedia icon: multimedia");
        REQUIRE(representations[5]->visit(test_transformer) == "Program--> name: VLC media player icon: vlc executable: /usr/bin/vlc --started-from-file %U");
        REQUIRE(representations[6]->visit(test_transformer) == "Subsection end--> name: Multimedia end");
        REQUIRE(representations[7]->visit(test_transformer) == "Menu end--> name: Menu end");

        clear_memory(representations);
      }
    }

    WHEN("transformed to a representations with icon service") {
      std::vector<std::string> files;
      files.push_back(fixtures_directory + "vlc.desktop");
      files.push_back(fixtures_directory + "mousepad.desktop");

      amm::IconService icon_service;
      icon_service.set_extension(".xpm");
      menu.register_icon_service(icon_service);

      menu.populate(files);
      std::vector<amm::representation::base*> representations = menu.representations();
      TestTransformer test_transformer;

      THEN("it adds the icon name to icons for subcategory and menu-entry") {
        REQUIRE(representations.size() == 8);
        REQUIRE(representations[0]->visit(test_transformer) == "Menu start--> name: Menu start");
        REQUIRE(representations[1]->visit(test_transformer) == "Subsection start--> name: Accessories icon: accessories.xpm");
        REQUIRE(representations[2]->visit(test_transformer) == "Program--> name: Mousepad icon: accessories-text-editor.xpm executable: mousepad %F");
        REQUIRE(representations[3]->visit(test_transformer) == "Subsection end--> name: Accessories end");
        REQUIRE(representations[4]->visit(test_transformer) == "Subsection start--> name: Multimedia icon: multimedia.xpm");
        REQUIRE(representations[5]->visit(test_transformer) == "Program--> name: VLC media player icon: vlc.xpm executable: /usr/bin/vlc --started-from-file %U");
        REQUIRE(representations[6]->visit(test_transformer) == "Subsection end--> name: Multimedia end");
        REQUIRE(representations[7]->visit(test_transformer) == "Menu end--> name: Menu end");

        clear_memory(representations);
      }
    }
  }
}
