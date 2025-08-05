#ifndef READWRITE_H
#define READWRITE_H

#include <iostream>
#include <unistd.h>
#include <assert.h>
#include "structures.h"
#include <fcntl.h>

const size_t max_msg = 4096;

static bool isItParsable(Connection *conn){
    size_t len;
    if(conn -> incoming_buffer.size() < 4){
        return false; // read more from kernels buffer
    }
    memcpy(&len, conn -> incoming_buffer.data(), 4);

    if(len > max_msg){
        conn -> want_close = true;
        return false;
    }

    if(4 + len > conn -> incoming_buffer.size()) return false; // want to read more from kernel buffer

    // writting the whole msg to client
    conn -> outgoing_buffer.insert(conn -> outgoing_buffer.end(), &len, &len + 4);
    conn -> outgoing_buffer.insert(conn -> outgoing_buffer.end(), &conn -> incoming_buffer[4],&conn -> incoming_buffer[4] + len);

    conn -> incoming_buffer.erase(conn -> incoming_buffer.begin(), conn -> incoming_buffer.begin() + 4 + len);
    return true;
}

static void set_fd_nb(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}


class ReadWrite{

    public:

    static void Myread(Connection *conn){
        uint8_t r_buf[64 * 1024]; // a buffer of 64kb
        ssize_t size = read(conn -> fd, r_buf, sizeof(r_buf));
        if(size <= 0){
            conn -> want_close = true;
            return;
        }
        // insert the r_buf in incoming_buf
        conn->incoming_buffer.insert(conn -> incoming_buffer.end(), r_buf, r_buf + size);
        isItParsable(conn);

        if(conn -> outgoing_buffer.size() > 0){
            conn -> want_read = false;
            conn -> want_write = true;
        }
    }

    static void Mywrite(Connection *conn){
        // assuming the write buffer is fully empty
        size_t written = write(conn -> fd, &conn -> outgoing_buffer, sizeof(conn -> outgoing_buffer));
        if(written < 0) {
            conn -> want_close = true;
            return ;
        }

        conn -> outgoing_buffer.erase(conn -> outgoing_buffer.begin(), conn -> outgoing_buffer.begin() + written);

        if(conn -> outgoing_buffer.size() == 0){
            conn -> want_read = true;
            conn -> want_write = false;
        }
    }

    static Connection* Myaccept(int fd){
        struct sockaddr_in client_addr = {};
        socklen_t addr_len = sizeof(client_addr);

        int connfd = accept(fd, (struct sockaddr*) &client_addr, &addr_len);
        if(connfd < 0){
            errors::die("error : accept()");
            return NULL;
        }

        set_fd_nb(connfd);
        struct Connection conn = {};
        conn.fd = connfd;
        conn.want_read = true;
        return &conn;
    }

};

#endif
