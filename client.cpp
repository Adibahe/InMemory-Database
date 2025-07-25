#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

void die(const std::string &msg){
    int error = errno;
    std::cerr << "Error:" << error << strerror(errno) << ":"<< msg;
    abort();
}

void msg(const std::string &message){
    std::cerr << message;
}

int main(){

    int fd = socket(AF_INET, SOCK_STREAM ,0);
    if(fd < 0){die("socket()");}

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR ,&val, sizeof(val));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    socklen_t my_addrlen = sizeof(addr);
    int err_code = connect(fd, (struct sockaddr *) &addr, my_addrlen);
    if(err_code < 0){die("connect()");}
    else {std::clog << "connected "<< std::endl;}

    // writting and reading msg's
    char send_buf[] = "Hello";
    ssize_t code = send(fd, send_buf, sizeof(send_buf) - 1, 0);
    if(code < 0) {die("send()");}

    char recv_buf[64] = {};
    code = recv(fd, recv_buf, sizeof(recv_buf) - 1, 0);
    if(code < 0) {die("recv()");}
    std::cout << "server says: " << recv_buf << std::endl;

    return 0;
}