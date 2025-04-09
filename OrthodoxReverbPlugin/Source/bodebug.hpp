// Boris Dimitrov: A header file that allows for quick and simple debugging
//                 by just printing data to a text file.
//                 How to use it?
//                 Include it in any C++ source file
//                     #include "bodebug.hpp"
//                 and then you can print a message like that
//                     BODEBUG("any message");
//                 or you can print a variable's value like that
//                     BODEBUG("i = " << i);
//                 Basically you can pass whatever you would pass after std::cout <<
#define BODEBUG_ON 1
#if BODEBUG_ON

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

static void _bodebug(const std::string& msg)
{
    std::ofstream file("C:/dev/bodebug.txt", std::ios::app);
    if (!file.is_open())
    {
        std::cout << "ERROR: Failed to open BODEBUG file." << std::endl;
        return;
    }

    file << msg << std::endl;

    file.close();
    if (file.is_open())
    {
        std::cout << "ERROR: Failed to close BODEBUG file." << std::endl;
    }
}

#define BODEBUG(msg) { std::ostringstream oss; oss << msg; _bodebug(oss.str()); }

#endif