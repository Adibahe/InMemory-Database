#ifndef READWRITE_H
#define READWRITE_H

#include <iostream>
#include <unistd.h>

class ReadWrite{

    public:

    static int32_t readfull(const int &conn_fd, char * buf, size_t buf_size){
        
        // used to read byte stream with expected size
        while(buf_size > 0){
            ssize_t n = read(conn_fd, &buf, buf_size);
            if(n <= 0) return -1; //error in reading

            buf_size -= (size_t)n;
            buf += (size_t)n;
        }
        return 0;
    }

    static int32_t writefull(const int &conn_fd, char * buf, size_t buf_size){
    
        // used to write byte stream with expected size
        while(buf_size > 0){
            ssize_t n = write(conn_fd, &buf, buf_size);
            if(n <= 0) return -1; //error in reading

            buf_size -= (size_t)n;
            buf += (size_t)n;
        }
        return 0;
    }
};

#endif
