#include<iostream>
#include <sys/socket.h>
#include <unistd.h>

struct my_sockaddr{
    u_int16_t port;
    
}

int main(){

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    return 1;
}