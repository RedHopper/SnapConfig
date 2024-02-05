/* 
 * This file is part of SnapConfig.
 *
 * Here we read some arbitrary variables from test config file.
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

#include"../SnapConfig.h"
#include<iostream>

#ifdef _WIN32
#include<windows.h>
#endif

#ifdef linux
#include<unistd.h>
#endif

int main()
{
    SnapConfig config{"config.txt"};

    std::cout << "Config language: " << config.get("language") << "\n";
    std::cout << "This line is not stripped: " << config.get("not_stripped") << "\n";
    std::cout << "Multiline: " << config.get("multiline") << "\n";
    double avg_ct{config.get_double("average_computation_time")};
    std::cout << "Average computation time (no context): " << avg_ct << "\n";
    int sleep_seconds{config.get_int("time_to_sleep")};
    std::cout << "Now sleeping for " << sleep_seconds << " seconds\n";
    sleep(sleep_seconds);

    return 0;
}
