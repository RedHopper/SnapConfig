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
    const std::array<char, 5> strip_chars{' ', '\t', '\r', '\n', '\v'};

    std::string lstrip(std::string str)
    {
        int count{};

        for (const char a : str) {
            auto f{std::find_if(strip_chars.begin(), strip_chars.end(),
                [a](unsigned char c) { return c == a; }
            )};

            if (f == strip_chars.end())
                break;

            ++count;
        }

        if (!count)
            return str;

        return str.substr(count, str.size() - 1);
    }

    std::string rstrip(std::string str)
    {
        auto index{str.rbegin()};

        while (index != str.rend()) {
            auto f{std::find_if(strip_chars.rbegin(), strip_chars.rend(),
                [index](unsigned char c) { return c == *index; }
            )};

            if (f == strip_chars.rend())
                break;

            ++index;
        }

        const long count{index - str.rbegin()};

        if (!count)
            return str;

        return str.substr(0, str.size() - count);
    }

    std::string strip(std::string str)
    {
        return lstrip(rstrip(str));
    }

    // Checks if given string is of a floating number format i.e. a) has number 
    // in the front; b) has a dot in the middle; c) has a number in the end.
    bool is_float(std::string str)
    {
        str = strip(str);
        auto f{str.find('.')};
        auto count{std::count_if(str.begin(), str.end(),
            [] (unsigned char c) {return c == '.';}
        )};
        auto find_other{std::find_if(str.begin(), str.end(),
            [] (unsigned char c) { return !isdigit(c) && c != '.'; }
        )};

        if (
            f == std::string::npos || !f || f + 1 == str.size()
            || count > 1 || find_other != str.end()
        ) {
            return false;
        }

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
                if (str[0] == c)
                    return true;
            }
            return false;
        }

        return true;
    } 

    bool char_escaped(const std::string& line, const int char_index, const char escape_char = '\\')
    {
        if (char_index - 1 >= 0 && line[char_index - 1] == escape_char)
            return !char_escaped(line, char_index - 1);
        return false;
    }

    bool line_escaped(const std::string& line, const char escape_char = '\\')
    {
        return line.size()
            && line[line.size() - 1] == escape_char
            && !SnapTools::char_escaped(line, line.size() - 1, escape_char);
    }
}

class SnapConfig {
protected:
    bool new_line_escaped{};
    short error{};
    bool strip_value{};

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
        wrong_type,
        init,
    };

    std::string config_file;
    std::vector<Variable> config_variables;
    std::vector<Variable> default_variables;
    static const char default_separating_char{':'};
    static const char default_escape_char{'\\'};
    const char escape_char{};
    const char separating_char{};
    const char comment_char{'#'};

    std::array<const Escapable, 4> escapables{
        Escapable{escape_char, escape_char},
        Escapable{'n', '\n'},
        Escapable{'r', '\r'},
        Escapable{'t', '\t'}
    };

    SnapConfig(const std::string& config_file,
               const char separating_char = default_separating_char,
               const char escape_char = default_escape_char
    ) : escape_char{escape_char}, separating_char{separating_char}, error{Error::no_error}
    {
        read_config(config_file);
    }

    // Returns error code. Additional processing can be added
    short get_error()
    {
        return error;
    }

    // Checks for some of the possible errors, like equality of the separating
    // character and the escape character, existence of the config file.
    void base_error_check()
    {
        if (separating_char == escape_char) {
            error = Error::init;
            debug("escape char and separating char can't hold the same value",
                "fatal error");
        }

        if (!std::filesystem::exists(config_file)) {
            error = Error::init;
            const std::string abs_path{std::filesystem::absolute(config_file)};
            debug("file '" + abs_path + "' doesn't exist.",
                "fatal error");
        }
    }

    // Checks wether config file was read and no fatal
    // error occured during initializing step
    bool is_config_init()
    {
        return error != Error::init;
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
        strip_value = true;
        new_line_escaped = false;

        for (const char a : config_line) {
            if (
                !get_value && (last_char == separating_char
                || a == separating_char && iterator == config_line.size() - 1)
            ) {
                get_value = true;
                key_name = SnapTools::strip(key_name);

                if (!key_name.size()) {
                    debug("config line number " + std::to_string(line_number)
                        + " is missing a variable's key name");

                    return Variable();
                }

                if (a == separating_char) {
                    strip_value = false;
                    last_char = a;
                    continue;
                }
            }
            
            if (get_value)
                value += a;
            else if (a != separating_char)
                key_name += a;

            last_char = a;
            ++iterator;
        }

        if (!key_name.size()) {
            debug("config line number " + std::to_string(line_number)
                + " is missing a variable's key name");
            return Variable();
        }
        else if (!get_value) {
            debug("config line number " + std::to_string(line_number)
                + " is missing a separating character");
            return Variable();
        }

        if (SnapTools::line_escaped(value, escape_char)) {
            new_line_escaped = true;
            value.pop_back();
        }
        if (strip_value)
            value = SnapTools::strip(value);

        return {key_name, value};
    }

    // Replaces all escapable chars with its replacement
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

    // Reads the config file
    void read_config(const std::string& config_file)
    {
        this->config_file = config_file;
        base_error_check();

        if (error)
            return;

        config_variables.clear();
        std::fstream file;
        file.open(config_file);
        
        // File existence is checked in the class constructor. Thus if
        // we still can't open the file, user has no access to it.
        if (!file.is_open()) {
            if (error)
                return;

            std::string abs_path = std::filesystem::absolute(config_file);
            debug("user has no access to file '" + abs_path + "'",
                "fatal error");
            error = Error::init;

            return;
        }

        std::string cache;
        Variable cache_var{};
        int count_lines{};

        while (getline(file, cache)) {
            ++count_lines;

            if (new_line_escaped) {
                new_line_escaped = SnapTools::line_escaped(cache, escape_char);
                cache = strip_value ? SnapTools::strip(cache) : cache;
                cache_var.value += "\n" + cache;

                if (new_line_escaped)
                    cache_var.value.pop_back();
            }
            else if (cache.size() && cache[0] != comment_char)
                cache_var = process_line(cache, count_lines);
            else
                continue;

            if (!new_line_escaped) {
                if (cache_var.key.size())
                    config_variables.push_back(cache_var);

                cache_var = Variable{};
            }
        }

        file.close();

        for (Variable& var : config_variables)
            format_string(var.value);
    }

    // Returns a variable's value. If required_variable is set to true and the
    // config isn't able to find a variable, then it will block itself. Showing
    // to a parent program that some *crucial* config variable wasn't found
    std::string get(const std::string& key,
                    const bool verbose = false)
    {
        if (!is_config_init()) {
            if (verbose) {
                debug("can't get key '" + key + "' due to"
                    " a prior fatal error");
            }
            return "";
        }

        error = Error::no_error;
        auto f{std::find_if(config_variables.begin(), config_variables.end(),
            [key] (const Variable& var) { return var.key == key; }
        )};

        if (f == config_variables.end()) {
            const std::string default_variable{get_default(key)};

            if (verbose)
                debug("variable '" + key + "' not found");

            return default_variable;
        }

        return f->value;
    }

    // Transforms found variable to long int
    long int get_lint(const std::string& key,
                      const bool verbose = false)
    {
        try {
            const std::string result{get(key, verbose)};

            if (error)
                return 0;

            if (!SnapTools::is_int(result)) {
                if (verbose)
                    debug("key '" + key + "' is not an int type");
                error = Error::wrong_type;
                return 0;
            }

            return std::stoi(result);
        }
        catch (std::out_of_range ofr) {
            if (verbose) {
                debug("variable \"" + key + "\" is way too big"
                      " and can't fit info long int");
            }
            error = Error::wrong_type;
            return 0;
        }
    }

    // Transforms found variable to double
    double get_double(const std::string& key,
                      const bool verbose = false)
    {
        const std::string result{get(key, verbose)};

        if (error)
            return 0;

        if (!SnapTools::is_float(result)) {
            if (verbose)
                debug("requested key '" + key + "' is not a floating type");
            error = Error::wrong_type;
            return 0;
        }

        try {
            return std::stod(result);
        }
        catch (std::out_of_range ofr) {
            if (verbose) {
                debug("variable \"" + key + "\" is"
                      " way too big and can't fit into double");
            }
            error = Error::wrong_type;
            return 0;
        }
    }

    // Calls get_double and then casts return value to float. Infinity may occur
    float get_float(const std::string& key,
                    const bool verbose = false)
    {
        return static_cast<float>(get_double(key, verbose));
    }

    // Calls get_lint and then casts return value to int. Infinity may occur
    int get_int(const std::string& key,
                const bool verbose = false)
    {
        return static_cast<int>(get_lint(key, verbose));
    }

    // Returns the default variable's value, if one was set later.
    // If no default value was found, returns empty string and sets error.
    std::string get_default(const std::string& key)
    {
        auto f{std::find_if(default_variables.begin(), default_variables.end(),
            [key] (const Variable& var) { return var.key == key; }
        )};

        if (f == default_variables.end()) {
            error = Error::variable_not_found;
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

    template<typename T>
    bool set_default(const std::string& key, const T& value)
    {
        return set_default(key, std::to_string(value));
    }

    void debug(const std::string& message,
               const std::string& message_type = "error",
               const char end_char = '\n')
    {
        std::cerr << "SnapConfig[" << message_type << "]: " << message << end_char;
    }
};
