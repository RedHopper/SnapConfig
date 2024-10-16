/* 
 * This file is part of SnapConfig.
 *
 * Unit testing library that is being used here: 
 * Catch2 <https://github.com/catchorg/Catch2>
 *
 * SnapConfig is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or any later version. This
 * program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include<catch2/catch_amalgamated.hpp>
#include"../SnapConfig.h"

TEST_CASE("SnapTools are tested", "[tools]") {
    REQUIRE(SnapTools::strip("  test  ") == "test");
    REQUIRE(SnapTools::rstrip("  test") == "  test");
    REQUIRE(SnapTools::lstrip("test  ") == "test  ");
    REQUIRE(SnapTools::rstrip("  test  ") == "  test");
    REQUIRE(SnapTools::lstrip("  test  ") == "test  ");
    REQUIRE(SnapTools::strip("       Test Value           ") == "Test Value");
    REQUIRE(SnapTools::is_int("30") == true);
    REQUIRE(SnapTools::is_int("+891") == true);
    REQUIRE(SnapTools::is_int("-239") == true);
    REQUIRE(SnapTools::is_int("6944") == true);
    REQUIRE(SnapTools::is_int("2384-") == false);
    REQUIRE(SnapTools::is_int("1398+") == false);
    REQUIRE(SnapTools::is_int("  23   ") == true);
    REQUIRE(SnapTools::is_int("  -864   ") == true);
    REQUIRE(SnapTools::is_int("  +344   ") == true);
    REQUIRE(SnapTools::is_int("3-44") == false);
    REQUIRE(SnapTools::is_int("54 1") == false);
    REQUIRE(SnapTools::char_escaped("Hello \\world", 7));
    REQUIRE(SnapTools::char_escaped("Hello \\\\world", 7));
    REQUIRE(!SnapTools::char_escaped("Hello \\\\\\world", 8));
    REQUIRE(!SnapTools::char_escaped("\\", 0));
    REQUIRE(SnapTools::line_escaped("Hello world!\\\\\\"));
    REQUIRE(!SnapTools::line_escaped("Hello world!\\\\"));
}

TEST_CASE("SnapConfig are tested", "[snapconfig]") {
    SnapConfig config{"test.conf"};
    REQUIRE(!config.get_error());

    REQUIRE(config.get_int("num") == 8021);
    REQUIRE(config.get("not_stripped") == " This string isn't stripped ");
    REQUIRE(config.get("multiline") == "This variable is multiline\n"
         "and each line is stripped.");
    REQUIRE(config.get("multiline2") == "  This multiline variable  "
        "\n is not stripped. ");
    REQUIRE(config.get("escaped") == "You can escape some chars: \n,\t,\r");
    REQUIRE(config.get("escape_escape") == "And you can escape escape char: \\n");
    REQUIRE(!config.get_error());

    config.get("nonexistent_var");
    REQUIRE(config.get_error() == SnapConfig::Error::variable_not_found);

    const std::string default_value{"Default value"};
    config.set_default("default_var", default_value);
    REQUIRE(config.get("default_var") == default_value);

    config.get_default("not_stripped");
    REQUIRE(config.get_error() == SnapConfig::Error::variable_not_found);

    config.get_int("not_stripped");
    REQUIRE(config.get_error() == SnapConfig::wrong_type);

    REQUIRE(config.get("empty_variable") == "");
    REQUIRE(!config.get_error());

    REQUIRE(config.get("spaces in name") == "Spaces in variable name are fine");
    REQUIRE(!config.get_error());

    REQUIRE(config.get("empty_variable2") == "");
    REQUIRE(!config.get_error());

    const double floating_default{53923.2314};
    config.set_default("floating", floating_default);
    REQUIRE(!config.get_error());

    const double floating{config.get_double("floating")};
    REQUIRE(!config.get_error());
    REQUIRE(floating == floating_default);

    REQUIRE(config.config_variables.size() == 10);
}
