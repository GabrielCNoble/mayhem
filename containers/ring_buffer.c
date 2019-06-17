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

void ring_buffer_t::advance_next_in()
{
    if(this->available())
    {
        this->next_in = (this->next_in + 1) % this->elem_count;
    }
}

void ring_buffer_t::advance_next_out()
{
    if(this->available())
    {
        this->next_out = (this->next_out + 1) % this->elem_count;
    }
}

int ring_buffer_t::add_next(void *data)
{
    int index = -1;

    if(this->available())
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


int ring_buffer_t::add_next_no_advance(void *data)
{
    int index = -1;

    if(this->available())
    {
        if(data)
        {
            memcpy(this->buffer + this->next_in * this->elem_size, data, this->elem_size);
        }

        index = this->next_in;
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

void *ring_buffer_t::get_next_no_advance()
{
    void *rtrn = NULL;

    if(this->next_out != this->next_in)
    {
        /* only get the next element, without advancing the 'out' cursor... */
        rtrn = (char *)this->buffer + this->elem_size * this->next_out;
    }

    return rtrn;
}

int ring_buffer_t::available()
{
    int in = this->next_in;

    if(in < this->next_out)
    {
        /* the 'next_in' position has wrapped around the buffer, so to properly compare
        'next_in' and 'next_out' we add the buffer size to 'next_in', so we always have
        it ahead of 'next_out'. No risk of catastrophic overflow here. When 'next_out'
        wraps around, doing this won't be necessary... */
        in += this->elem_count;
    }

    return this->elem_count - abs(this->next_out - in) - 1;
}









