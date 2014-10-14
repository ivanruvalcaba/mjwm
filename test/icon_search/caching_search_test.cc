/*
  This file is part of mjwm.
  Copyright (C) 2014  Chirantan Mitra <chirantan.mitra@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software 0oundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define CATCH_CONFIG_MAIN

#include "icon_search/caching_search.h"

#include <string>
#include "../catch.hpp"
#include "icon_search/icon_search_interface.h"

namespace amm {
namespace icon_search {

class TestSearch : public IconSearchInterface {
 public:
  TestSearch() : extension_(".png") { }
  std::string ResolvedName(std::string icon_name) const { return icon_name + extension_; }
  void ExtensionIs(std::string extension) { extension_ = extension; }
 private:
  std::string extension_;
};

SCENARIO("icon_search::CachingSearch", "[cachingsearch]") {
  TestSearch *actual_searcher = new TestSearch;

  GIVEN("Icon search that caches results") {
    CachingSearch caching_searcher(*actual_searcher);

    WHEN("retrieving an unsearched item") {
      THEN("the item is absent from the cache") {
        REQUIRE(!caching_searcher.IsCached("vlc"));
      }
    }

    WHEN("retrieving a searched item") {
      caching_searcher.ResolvedName("vlc");
      THEN("the item is present in the cache") {
        REQUIRE(caching_searcher.IsCached("vlc"));
      }

      THEN("the item is same as the original item") {
        REQUIRE(caching_searcher.ResolvedName("vlc") == "vlc.png");
      }

      WHEN("the underlying implementation returns a different item") {
        caching_searcher.ResolvedName("vlc");
        actual_searcher->ExtensionIs(".svg");
        THEN("the original entry is retrieved") {
          REQUIRE(caching_searcher.ResolvedName("vlc") == "vlc.png");
        }
      }
    }
  }
}

} // namespace icon_search
} // namespace amm