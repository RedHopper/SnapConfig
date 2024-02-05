# What is SnapConfig?
SnapConfig is single header config reader library for C++. It's main goal is to provide great performance and customizability yet using as little space as possible. It has a nice simplistic syntax, trying to stick as much as possible to **plain text**. That's why there is no any specific file format associated with SnapConfig and it's solely your decision to choose an extension for your configuration files.

# Example
*config.txt*:
```
lib_name: SnapConfig
gitrepo: https\://github.com/RedHopper/SnapConfig
max_lines: 1000
license: GNU GPL 3
```

*example.cpp*:
```
#include<iostream>
#include<SnapConfig.h>

int main()
{
    SnapConfig config("config.txt");
    std::cout << "You can clone " << config.get("lib_name") << " from the "
        << "GitHub repository " << config.get("gitrepo") << ". " << config.get("lib_name")
        << " is licensed under " << config.get("license") << ", and one of its"
        << " goals is to stay under " << config.get("max_lines") << " lines!\n";
}
```

Output of the `make example && ./example`: *You can clone SnapConfig from the GitHub repository https://github.com/RedHopper/SnapConfig. SnapConfig is licensed under GNU GPL 3, and one of its goals is to stay under 1000 lines!*

# Roadmap
Project's roadmap:
* Add array support
* Add more unit tests
* Optimization and refactoring

# Contribution
Any contribution or feedback is very welcome. You can contact me at whitesurfer@protonmail.com
