#include "list.h"

#include "stack_list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

list_t::list_t()
{
    this->buffer = NULL;
    this->elem_size = 0;
    this->size = 0;
    this->cursor = 0;
}


list_t::list_t(int elem_size, int init_size)
{
    this->init(elem_size, init_size);
}


list_t::~list_t()
{
    if(this->buffer)
    {
        free(this->buffer);
    }
}

void list_t::init(unsigned int elem_size, unsigned int init_size)
{
    this->elem_size = elem_size;
    this->size = init_size;
    this->cursor = 0;
    this->buffer = NULL;

    if(init_size)
    {
        this->buffer = calloc(init_size, elem_size);
    }
}

void list_t::resize(unsigned int new_size)
{
    void *new_buffer;

    if(this->size < new_size)
    {
        new_buffer = calloc(this->elem_size, new_size);

        if(this->buffer)
        {
            memcpy(new_buffer, this->buffer, this->elem_size * this->size);
            free(this->buffer);
        }

        this->buffer = new_buffer;
        this->size = new_size;
    }
}


int list_t::add(void *data)
{
    int index = -1;
    if(this->cursor >= this->size)
    {
        this->resize(this->size + 64);
    }

    index = this->cursor;
    this->cursor++;

    if(index >= 0 && data)
    {
        memcpy((char *)this->buffer + index * this->elem_size, data, this->elem_size);
    }

    return index;
}

void list_t::add_multiple(void *data, int count)
{
    int cursor = this->cursor + count;

    if(cursor >= this->size)
    {
        cursor -= this->size;
        this->resize(this->size + cursor);
    }

    if(data)
    {
        memcpy((char *)this->buffer + this->cursor * this->elem_size, data, this->elem_size * count);
    }

    this->cursor += count;
}


void list_t::remove(unsigned int index)
{
    void *src;
    void *dst;

    if(index >= 0 && index < this->cursor)
    {
        this->cursor--;

        if(index < this->cursor)
        {
            dst = (char *)this->buffer + this->elem_size * index;
            src = (char *)this->buffer + this->elem_size * this->cursor;

            memcpy(dst, src, this->elem_size);
        }
    }
}

void list_t::remove_move(unsigned int index)
{
    void *src;
    void *dst;

    if(index >= 0 && index < this->cursor)
    {
        dst = (char *)this->buffer + this->elem_size * index;
        src = (char *)this->buffer + this->elem_size * (index + 1);
        memcpy(dst, src, this->elem_size * (this->cursor - index));
        this->cursor--;
    }
}


void *list_t::get(unsigned int index)
{
    void *ret = NULL;

    if(index >= 0 && index < this->cursor)
    {
        ret = (char *)this->buffer + index * this->elem_size;
    }

    return ret;
}

void * list_t::operator [] (int index)
{
    return this->get(index);
}







