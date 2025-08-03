#include <iostream>
#include <sys/socket.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "readwrite.h"
#include "error_handling.h"

const size_t max_size = 4096;
int32_t err;


static int32_t query(const int &fd, const char *message){

    uint32_t len = (uint32_t)strlen(message);
    if(len > max_size) return -1;
    char write_buf[4 + max_size];

    if(len > max_size) {
        errors :: msg("write() : too long message");
        return -1;
    }
    // creating a write byte stream msg
    memcpy(write_buf, &len, 4);
    memcpy(&write_buf[4], message, len);

    // writing all
    err = ReadWrite::writefull(fd, write_buf, 4 + len);
    if(err){
        errors :: msg("write() error");
        return err;
    }

    
    //reading all byte stream
    char read_buf[4 + max_size + 1];
    errno = 0;
    err = ReadWrite::readfull(fd, read_buf, 4); //getting length of message
    if(err) {
        errors :: msg(err == 0 ? "EOF":"read() error"); 
        return err;
    }

    memcpy(&len, read_buf, 4);
    if(len > max_size){
        errors :: msg("read() : too long message");
        return -1;
    }
    err = ReadWrite::readfull(fd, &read_buf[4], len);
    if(err) {
        errors :: msg("read() error");
        return err;
    }

    std::cout << "Server says: " << &read_buf[4] << std::endl;

    return 0;
}

int main(){

    int fd = socket(AF_INET, SOCK_STREAM ,0);
    if(fd < 0){errors :: die("socket()");}

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR ,&val, sizeof(val));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    socklen_t my_addrlen = sizeof(addr);
    int err_code = connect(fd, (struct sockaddr *) &addr, my_addrlen);
    if(err_code < 0){errors :: die("connect() error");}
    else {std::clog << "connected "<< std::endl;}

    err = query(fd, "Hello1");
    if(err){
        goto DONE;
    }
    err = query(fd, "Hello2");
    if(err){
        goto DONE;
    }

DONE:
    close(fd);
    return 0;
}