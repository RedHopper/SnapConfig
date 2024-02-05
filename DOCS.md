# Documentation
This file provides all important information to get started with SnapConfig

## Getting variables from config file
Get string:
```c_cpp
std::string program_name = config.get("name");
std::cout << "Program's name is: " << program_name << "\n";
```

Get int:
```c_cpp
int sleep_time = config.get_int("seconds_to_sleep");
long int big_num = config.get_lint("big_num");
sleep(sleep_time);
```

Floating point types:
```c_cpp
double average_grade = config.get_double("average_grade");

// Beware: this function just casts the get_double() result to float
float water_boiling_temp = config.get_float("boiling_temp");
```

## Formatting
### Variable's value
* By default value is stripped
* You can change this behavior by writing not one, but two separating chars "::" after variable's name
* You are able to escape some characters like: \n, \r and \t (you can expand this list by adding more values to "escapables" array in the source code)
* It is encouraged to escape separating char, though it's not necessary
* You are able to make multiline variables by putting the escape char at the end of the string
* In multiline variables only the first line can be stripped, following lines won't be stripped
* Empty variables are allowed
Check out this example:

variables.txt
```
var1: stripped     
var2:: This value is not stripped 
var3: SnapCofig also has some escape characters\: \n\r\t\\
var4: You can make\
mulitline\
variables!
empty_variable:
```
### Names
Varialbles' names are treated differently:
* You can not escape chars in them. Hence this - "te\st: Var", will be treated as: variable with name "te\\st" and value "Var"
* But you should avoid using '\\' and other escape values like "\n\r\t" in your variables' names. This behavior is subject to change
* Spaces in names are allowed, but discouraged
* It is fine to put spaces before and after variable's name, but they will be stripped

names.txt
```
pos sible: works fine but discouraged
finevar : It's fine
    also_finevar   :    value
```

```c_cpp
std::cout << config.get("pos sible") << "\n"; // "works fine but discouraged"
std::cout << config.get("finevar") << "\n"; // "It's fine"
std::cout << config.get("also_finevar") << "\n"; // "value"
```

## Comments
* You can create a comment by using "#" character at the begginig of the line
* You can only comment out the whole line. Any other occurrances of comment char will be treated like a normal character
```
# This is a valid comment line
variable: value # This is NOT a valid comment line
```

## Default values
config.txt:
```
level_name: Space wreckers
```
main.cpp:
```c_cpp
config.set_default("difficulty_level", "normal");
std::cout << config.get("difficulty_level") << "\n"; // This will not produce an error since "difficulty_level" has a default value
```
Note: SnapConfig prioritizes config variables over default ones

## Checking for errors
* You can get error by calling get_error()
* You can get error as a human readable string by calling get_error_message()
* You can check if fatal error occurred by calling is_fatal_error()

config.txt:
```
name: Simon
age: 31
favorite_book: Everything Bad Is Good For You
```

main.cpp:
```c_cpp
SnapConfig config{"config.txt"};
std::string username = config.get("username");

if (config.get_error()) {
    std::cout << "Error: error occurred in the config: "
        << config.get_error_message() << "\n";
}

```
**Important:** some of errors occurring in SnapCofig are fatal. After such error config will stop working and will return null values on get... functions. Some of fatal errors are:
* config file doesn't exist
* user has no read access to a config file
* separating char and escape char have the same value.
You can also make get... function to create a fatal error if variable wasn't found by passing _true_ as a second argument:  `config.get("very_important_variable", true)`.

## Changing debug level
You can change amount of debug messages by setting the debug_level variable to SnapConfig::DebugLevel::minimal. It will make the config to display only fatal errors.

## More features
* You can change separating char and escape char when initializing SnapConfig
So if you don't like semicolon as a separating char, you can set any other. For example:

config.txt
```
friend = Brad
likes = pizza
```
main.cpp
```c_cpp
SnapConfig config("config.txt", '=');
std::cout << "My friend " << config.get("friend") << " likes " << config.get("likes") << "!\n";
```
Following program will output: My friend Brad likes pizza!
