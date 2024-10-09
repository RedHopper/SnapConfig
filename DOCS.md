# Documentation
## Getting variables from config file
* Get string:
```c_cpp
SnapConfig config("config.txt");

std::string program_name = config.get("name");
std::cout << "Program's name is: " << program_name << "\n";
```

* Get int:
```c_cpp
int sleep_time = config.get_int("seconds_to_sleep");
long int big_num = config.get_lint("big_num");
sleep(sleep_time);
```

* Floating point types:
```c_cpp
double average_grade = config.get_double("average_grade");

float water_boiling_temp = config.get_float("boiling_temp");
```

## Formatting
### Variable's value
* By default value is stripped
* You can change this behavior by writing not one, but two separating chars "::" after variable's name
* You are able to make multiline variables by putting the escape char at the end of the string
* You are able to escape some characters like: \n, \r and \t (this list is expandable)
* Empty variables *are* allowed

Example:

*variables.txt*
```
var1: stripped     
var2:: This value is not stripped 
var3: SnapCofig also has several escape characters\: \n\r\t\\
var4: You can make\
mulitline\
variables!
empty_variable:
```
### Variable's name
* You can not escape chars in them. Hence this - "tes\t: Var", will be treated as: variable with name "tes\\t" and value "Var"
* Spaces in names *are* allowed
* Names *are* stripped

*names.txt*
```
Hello world: hello world!
  spaces  : As much space as you need
```
*names.cpp*
```c_cpp
std::cout << config.get("Hello world") << "\n"; // "hello world!"
std::cout << config.get("spaces") << "\n"; // "As much space as you need"
```

## Comments
* You can create a comment by using "#" character at the beginning of the line
* If "#" isn't in the beginning, it will be treated as a regular character
```
# This is a valid comment line
variable: value # This is NOT a valid comment line
```

## Default values
*config.txt*
```
level_name: Space wreckers
```
*main.cpp*
```c_cpp
config.set_default("difficulty_level", "normal");
std::cout << config.get("difficulty_level") << "\n"; // Everything's fine, using the default value
// For other types
config.set_default("velocity", 12);
config.set_default("floating", 23.23);
```
**Note**: SnapConfig prioritizes existing variables over default ones

## Checking for errors
* If `get...()` function wasn't able to get a value for name you provided `get_error()` can be used to get code of last error
* Any new `get...()` call will overwrite any non-fatal error value
* Fatal errors can occur during initialization process and will write an error to stderr. To check that config file was successfully read: `if (snap.get_error() != SnapConfig::Error::init) ...`
* By default SnapConfig won't output non-fatal error messages. You can force it to do so by passing `true` as a second variable to a `get...()` function

*config.txt*
```
name: Simon
age: 31
favorite_book: Everything Bad Is Good For You
```

*main.cpp*
```c_cpp
SnapConfig config("config.txt");
std::string username = config.get("username"); // Will return an empty string, nothing will be printed in terminal

if (config.get_error()) {
    std::cerr << "Error: 'username' variable doesn't exist!\n";
}

std::string book = config.get("favorite_book");
std::cout << config.get_error() << "\n"; // Will output "0"

float height = config.get_float("height", true) // Will print error in terminal
```

## More features
* You can change separating char and escape char when initializing SnapConfig. So if you don't like semicolon as a separating char, you can set any other. For example:

*config.txt*
```
friend = Brad
likes = pizza
```
*main.cpp*
```c_cpp
SnapConfig config("config.txt", '=');
std::cout << "My friend " << config.get("friend") << " likes " << config.get("likes") << "!\n";
```
Following program will output: My friend Brad likes pizza!
