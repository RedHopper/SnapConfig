![SnapConfig logo](snap_logo.png)
# What is SnapConfig?
**SnapConfig** is single header config reader library for C++. Its main goal is to provide great performance and customizability yet using as little space as possible. It has simplistic syntax, trying to stick as much as possible to plain text. That's why there is no specific file format associated with SnapConfig and it's solely your decision to choose an extension for your configuration files.

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

Output of `make example && ./example`: *You can clone SnapConfig from the GitHub repository https://github.com/RedHopper/SnapConfig. SnapConfig is licensed under GNU GPL 3, and one of its goals is to stay under 1000 lines!*
# Documentation
Everything you need to know about SnapConfig is [here](DOCS.md)
# Roadmap
Project's roadmap:
* Add array support
* Add more unit tests
* Optimize and refactor

# Contribution
Any contribution or feedback is very welcome. You can contact me at whitesurfer@protonmail.com
