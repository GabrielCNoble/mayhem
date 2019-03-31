#include "ring_buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ring_buffer_t::ring_buffer_t()
{
    this->buffer = NULL;
    this->elem_count = 0;
    this->elem_size = 0;
    this->next_in = 0;
    this->next_out = 0;
}

ring_buffer_t::ring_buffer_t(int elem_size, int elem_count)
{
    this->init(elem_size, elem_count);
}

ring_buffer_t::~ring_buffer_t()
{
    if(this->buffer)
    {
        free(this->buffer);
    }
}

void ring_buffer_t::init(int elem_size, int elem_count)
{
    this->elem_size = elem_size;
    this->elem_count = elem_count;
    this->buffer = calloc(this->elem_size, this->elem_count);
    this->next_in = 0;
    this->next_out = 0;
}

int ring_buffer_t::add_next(void *data)
{
    int index = -1;

    if(this->avaliable())
    {
        if(data)
        {
            memcpy(this->buffer + this->next_in * this->elem_size, data, this->elem_size);
        }

        index = this->next_in;

        this->next_in = (this->next_in + 1) % this->elem_count;
    }

    return index;
}

void *ring_buffer_t::get_next()
{
    void *rtrn = NULL;

    if(this->next_out != this->next_in)
    {
        rtrn = (char *)this->buffer + this->elem_size * this->next_out;
        this->next_out = (this->next_out + 1) % this->elem_count;
    }

    return rtrn;
}

int ring_buffer_t::avaliable()
{
    int in = next_in;

    if(in < this->next_out)
    {
        in += this->elem_count;
    }

    return this->elem_count - abs(this->next_out - in) - 1;
}









