#pragma once

#include <stdlib.h>
#include <stdio.h>


namespace biohash {

struct Buffer {

    Buffer(size_t size);
    ~ Buffer();

    void resize(size_t size);

    char* data = nullptr;
    size_t size = 0;
};

}
