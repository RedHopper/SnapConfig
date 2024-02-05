/* 
 * SnapConfig is a C++ single header config reader library.
 * Copyright (C) 2024 Igor Mytsik
 * Contact email: whitesurfer@protonmail.com
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include<algorithm>
#include<array>
#include<cassert>
#include<fstream>
#include<filesystem>
#include<iostream>
#include<vector>

namespace SnapTools {
    std::string lstrip(std::string str, const char strip_char = ' ')
    {
        auto f{std::find_if(str.begin(), str.end(),
            [strip_char] (unsigned char c)
            {
                return c != strip_char;
            })};

        if (f == str.end())
            return "";
        else if (f == str.begin())
            return str;

        const long index{f - str.begin()};

        return str.substr(index, index - 1 - str.size());
    }

    std::string rstrip(std::string str, const char strip_char = ' ')
    {
        auto f{std::find_if(str.rbegin(), str.rend(),
            [strip_char] (unsigned char c)
            {
                return c != strip_char;
            })};

        if (f == str.rend())
            return "";
        else if (f == str.rbegin())
            return str;

        return str.substr(0, abs(f - str.rend()));
    }

    std::string strip(std::string str, const char strip_char = ' ')
    {
        return lstrip(rstrip(str, strip_char), strip_char);
    }

    // Checks if given string is of a floating number format i.e. a) has number 
    // in the front; b) has a dot in the middle; c) has a number in the end.
    bool is_float(std::string str)
    {
        str = strip(str);
        auto f{str.find('.')};
        auto count{std::count_if(str.begin(), str.end(),
            [] (unsigned char c) {return c == '.';})};
        auto find_other{std::find_if(str.begin(), str.end(),
            [] (unsigned char c)
            {
                return !isdigit(c) && c != '.';
            })};

        if (f == std::string::npos || !f || f + 1 == str.size() || count > 1
        || find_other != str.end())
            return false;

        return true;
    }

    bool is_int(std::string str)
    {
        str = SnapTools::strip(str);

        if (!str.size())
            return false;

        std::array<unsigned char, 2> special_chars{'+', '-'};

        auto f{std::find_if(str.begin(), str.end(), [=] (const unsigned char c)
        {
            return std::find(special_chars.begin(), special_chars.end(), c)
                == special_chars.end() && !std::isdigit(c);
        })};

        if (f != str.end())
            return false;

        long count_special_chars{std::count_if(str.begin(), str.end(),
            [=] (const unsigned char c)
            {
                return std::find(special_chars.begin(), special_chars.end(), c)
                    != special_chars.end();
            })};

        if (count_special_chars > 1) {
            return false;
        }
        else if (count_special_chars && str.size() == 1) {
            return false;
        }
        else if (count_special_chars) {
            for (const unsigned char c : special_chars) {
                f = std::find(str.begin(), str.end(), c);
                
                if (f == str.end())
                    continue;

                if (f != str.begin())
                    return false;
            }
        }

        return true;
    } 
}

class SnapConfig {
protected:
    bool new_line_escaped{};
    short error{};
    std::string error_details;

public:
    struct Variable {
        std::string key;
        std::string value;
    };

    struct Escapable {
        const char character{};
        const char replacement{};
    };

    enum Error {
        no_error,
        variable_not_found,
        default_variable_not_found,
        required_variable_not_found,
        wrong_type,
        wrong_type_fatal,
        empty_key,
        no_separator,
        file_doesnt_exist,
        access_error,
        conflicting_separating_and_escape_char,
    };

    std::string config_file;
    std::vector<Variable> config_variables;
    std::vector<Variable> default_variables;
    static const char default_separating_char{':'};
    static const char default_escape_char{'\\'};
    const char escape_char{};
    const char separating_char{};
    const char comment_char{'#'};
    short debug_level;

    // Non-fatal errors
    std::array<const int, 5> clearable_errors {
        Error::variable_not_found,
        Error::default_variable_not_found,
        Error::wrong_type,
        Error::no_separator,
        Error::empty_key,
    };

    enum DebugLevel {
        all,
        minimal
    };

    std::array<const Escapable, 4> escapables{Escapable{escape_char,
        escape_char}, Escapable{'n', '\n'}, Escapable{'r', '\r'},
        Escapable{'t', '\t'}};

    SnapConfig(const std::string& config_file,
    const char separating_char = default_separating_char,
    const char escape_char = default_escape_char) : escape_char{escape_char},
    separating_char{separating_char}, error{} 
    {
        debug_level = DebugLevel::all;
        read_config(config_file);
    }

    // Returns error description. If no error returns empty string.
    std::string get_error_message()
    {
        switch(get_error()) {
        case Error::no_error:
            return "";
        case Error::variable_not_found:
            return "variable \"" + error_details + "\" not found";
        case Error::default_variable_not_found:
            return "default variable \"" + error_details + "\" not found";
        case Error::required_variable_not_found:
            return "fatal: required variable \"" + error_details
                + "\" not found";
        case Error::wrong_type:
            return "variable \"" + error_details + "\" is of a wrong type";
        case Error::wrong_type_fatal:
            return "fatal: required variable \"" + error_details
                + "\" is of a wrong type";
        case Error::no_separator:
            return "config line number " + error_details
                + " is missing a separator character";
        case Error::empty_key:
            return "config line number " + error_details
                + " is missing a variable's key name";
        case Error::file_doesnt_exist:
            return "fatal: config file " + error_details + " doesn't exist";
        case Error::access_error:
            return "fatal: user has no read access to the file " + error_details;
        case Error::conflicting_separating_and_escape_char:
            return "fatal: separator char and escape char are the same";
        default:
            return "undefined error, error details: " + error_details;
        }
    }

    // Returns error code. Additional processing can be added
    short get_error()
    {
        return error;
    }

    // Checks if a config object encountered a fatal error. Fatal errors
    // are made to hinder most of the library functionality. Thus making
    // it clear to a parent program that some crucial config variables are
    // missing or lib isn't able to access its config file etc.
    bool is_fatal_error()
    {
        if (!error)
            return false;

        auto f{std::find(clearable_errors.begin(), clearable_errors.end(), error)};

        if (f == clearable_errors.end())
            return true;

        return false;
    }

    // Clears a non-fatal error. Returns true if the error was cleared and
    // false otherwise (if the error is fatal).
    bool clear_error()
    {
        if (!is_fatal_error()) {
            error = 0;
            return true;
        }
            
        return false;
    }

    // Checks for some of the possible errors, like equality of the separating
    // character and the escape character, existence of the config file.
    void base_error_check()
    {
        if (separating_char == escape_char) {
            std::cerr << "Config error: the esacpe char and the separating char"
               << " have the same value. You should change it in the source file"
               << " and recompile the SnapConfig.\n";
            error = Error::conflicting_separating_and_escape_char;
        }

        if (!std::filesystem::exists(config_file)) {
            error_details = std::filesystem::absolute(config_file);
            error = Error::file_doesnt_exist;
            std::cerr << "Snap error: " << get_error_message() << "\n";
        }
    }

    // Get variable's key name and value from the config line
    Variable process_line(const std::string& config_line,
    const int line_number)
    {
        std::string key_name;
        std::string value;
        int iterator{};
        char last_char{};
        bool get_value{};
        bool strip_value{true};
        new_line_escaped = false;

        assert(!get_error() && "You must first resolve existing error");

        for (const char a : config_line) {
            if ((last_char == separating_char || a == separating_char
            && iterator == config_line.size() - 1) && !get_value) {
                get_value = true;
                key_name = SnapTools::strip(key_name);

                if (!key_name.size()) {
                    error = Error::empty_key;
                    error_details = std::to_string(line_number);

                    return Variable();
                }

                if (a == separating_char) {
                    strip_value = false;
                    last_char = a;
                    continue;
                }
            }
            
            if (!get_value && a != separating_char)
                key_name += a;
            else if (get_value)
                value += a;

            last_char = a;
            ++iterator;
        }

        if (!key_name.size()) {
            error = Error::empty_key;
            error_details = std::to_string(line_number);
            return Variable();
        }
        else if (!get_value) {
            error = Error::no_separator;
            error_details = std::to_string(line_number);
            return Variable();
        }

        if (last_char == escape_char) {
            new_line_escaped = true;
            value.pop_back();
        }
        if (strip_value)
            value = SnapTools::strip(value);

        return {key_name, value};
    }

    // Replaces all escapables chars with its replacement chars.
    void format_string(std::string& str)
    {
        size_t prev_pos{};

        while (true) {
            const size_t f{str.find(std::string(1, escape_char), prev_pos)};

            if (f == std::string::npos)
                break;

            prev_pos = f + 1;

            if (str.size() > f + 1) {
                const char next_char = str[f + 1];
                auto find{std::find_if(escapables.begin(), escapables.end(),
                    [next_char] (const Escapable esc)
                    {
                        return esc.character == next_char;
                    })};

                if (find != escapables.end())
                    str.replace(f, 2, std::string(1, find->replacement));
                else
                    str.erase(f, 1);
            }
            else
                str.erase(f, 1);
        }
    }

    // Rereads the config file
    bool update_config()
    {
        read_config(this->config_file);
        const int error{get_error()};
        
        if (error) {
            std::cerr << "Snap: error updating the config: "
                << get_error_message() << "\n";
            return false;
        }

        return true;
    }

    // Reads the config file
    void read_config(const std::string& config_file)
    {
        this->config_file = config_file;
        base_error_check();
        config_variables.clear();

        std::fstream file;
        file.open(config_file);
        
        // File existence is checked in the class constructor. Thus if
        // we still can't open the file, user has no access to it.
        if (!file.is_open()) {
            if (error)
                return;

            error_details = std::filesystem::absolute(config_file);
            error = Error::access_error;
            std::cerr << "Snap error: " << get_error_message() << "\n";

            return;
        }

        std::string cache;
        Variable cache_var{};
        int count_lines{};

        // TODO: Even though most of the line processing is done in the
        // process_line function, this loop is also responsible for major
        // part of the processing logic. Must be refactored soon.
        while (getline(file, cache)) {
            ++count_lines;

            if (new_line_escaped) {
                cache_var.value += "\n" + cache;

                if (cache.size() && cache[cache.size() - 1] == escape_char) {
                    cache_var.value.pop_back();
                } else {
                    new_line_escaped = false;
                }
            }
            else if (cache.size() && cache[0] != comment_char)
                cache_var = process_line(cache, count_lines);
            else
                continue;

            if (get_error()) {
                debug("Error: "  + get_error_message());
            }
            else if (!new_line_escaped) {
                config_variables.push_back(cache_var);
                cache_var = Variable{};
            }

            clear_error();
        }

        file.close();

        for (Variable& var : config_variables)
            format_string(var.value);
    }

    // Returns a variable's value. If required_variable is set to true and the
    // config isn't able to find a variable, then it will block itself. Showing
    // to a parent program that some *crucial* config variable wasn't found
    std::string get(const std::string& key,
    const bool required_variable = false)
    {
        if (!clear_error()) {
            std::cerr << "Snap error: unable to call get function "
                << "due to the fatal error: " << get_error_message() << "\n";
            return "";
        }

        auto f{std::find_if(config_variables.begin(), config_variables.end(),
            [key] (const Variable& var)
            {
                if (var.key == key)
                    return true;

                return false;
            }
        )};

        if (f == config_variables.end()) {
            const std::string default_var{get_default(key)};
            
            if (!get_error())
                return default_var;

            error_details = key;

            if (required_variable)
                error = Error::required_variable_not_found;
            else
                error = Error::variable_not_found;

            return "";
        }

        return f->value;
    }

    // Transforms found variable to long int
    long int get_lint(const std::string& key,
    const bool required_variable = false)
    {
        const int possible_error{required_variable ? Error::wrong_type_fatal
            : Error::wrong_type};

        try {
            const std::string result{get(key, required_variable)};

            if (get_error())
                return 0;

            if (!SnapTools::is_int(result)) {
                error_details = key;
                error = possible_error;
                return 0;
            }

            return std::stoi(result);
        }
        catch (std::out_of_range ofr) {
            debug("Error: variable \"" + key + "\" is way to big"
                + " and can't fit info long int");
            error = possible_error;
            error_details = key;
            return 0;
        }
    }

    // Transforms found variable to double
    double get_double(const std::string& key,
    const bool required_variable = false)
    {
        const int possible_error{required_variable ? Error::wrong_type_fatal
            : Error::wrong_type};
        const std::string result{get(key, required_variable)};

        if (get_error())
            return 0;
        if (!SnapTools::is_float(result)) {
            error_details = key;
            error = possible_error;
            return 0;
        }

        try {
            return std::stod(result);
        }
        catch (std::out_of_range ofr) {
            debug("Error: variable \"" + key + "\" is"
                + " way too big and can't fit into double");
            error = possible_error;
            error_details = key;
            return 0;
        }
    }

    // Calls get_double and then casts return value to float. Infinity may occur
    float get_float(const std::string& key,
    const bool required_variable = false)
    {
        return static_cast<float>(get_double(key, required_variable));
    }

    // Calls get_lint and then casts return value to int. Infinity may occur
    int get_int(const std::string& key,
    const bool required_variable = false)
    {
        return static_cast<int>(get_lint(key, required_variable));
    }

    // Returns the default variable's value, if one was set later.
    // If no default value was found, returns empty string and sets error.
    std::string get_default(const std::string& key)
    {
        if (!clear_error()) {
            std::cerr << "Snap error: unable to call get_default() function "
                << "due to the fatal error: " << get_error_message() << "\n";
            return "";
        }

        auto f{std::find_if(default_variables.begin(), default_variables.end(),
            [key] (const Variable& var)
            {
                if (var.key == key)
                    return true;

                return false;
            }
        )};

        if (f == default_variables.end()) {
            error = Error::default_variable_not_found;
            error_details = key;

            return "";
        }

        return f->value;
    }

    // Sets the default value for the given key. Returns true if given key
    // already existed in default variables, false if new default variable
    // was created.
    bool set_default(const std::string& key, const std::string& value)
    {
        auto f{std::find_if(default_variables.begin(), default_variables.end(),
            [key] (const Variable& var)
            {
                if (var.key == key)
                    return true;

                return false;
            }
        )};

        const Variable default_var{key, value};

        if (f == default_variables.end()) {
            default_variables.push_back(default_var);

            return false;
        }

        f->value = value;

        return true;
    }

    bool set_default(const std::string& key, const int value)
    {
        return set_default(key, std::to_string(value));
    }

    bool set_default(const std::string& key, const double value)
    {
        return set_default(key, std::to_string(value));
    }

    void debug(const std::string& message,
    const std::string& prefix = "Snap: ",
    const char end_char = '\n')
    {
        if (debug_level == DebugLevel::all) {
            std::cerr << prefix << message << end_char;
        }
    }
};
