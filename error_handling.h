#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <cstdlib>
#include <string>
#include <iostream>
#include <cerrno>
#include <cstring>

class errors{
    public :
    static void die(const std::string &msg){
        int error = errno;
        std::cerr << "Error:" << error << strerror(errno) << ":"<< msg << std::endl;
        abort();
    }

    static void msg(const std::string &message){
        std::cerr << message << std::endl;
    }
};
#endif