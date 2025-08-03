#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <iostream>
#include <vector>
#include <stdint.h>

struct Connection{

    int fd = -1;

    bool want_read = false;
    bool want_write = false;
    bool want_close = false;

    std :: vector <uint8_t> incoming_buffer;
    std :: vector <uint8_t> outgoing_buffer;

};

#endif