#include "stack_list.h"
#include <stdlib.h>
#include <string.h>

stack_list_t::stack_list_t()
{
    this->buffer = NULL;
    this->free_list = NULL;
    this->elem_size = 0;
    this->free_top = -1;
    this->size = 0;
    this->cursor = 0;
}


stack_list_t::stack_list_t(int elem_size, int init_size)
{
    this->init(elem_size, init_size);
}


stack_list_t::~stack_list_t()
{
    if(this->buffer)
    {
        free(this->buffer);
        free(this->free_list);
    }
}

void stack_list_t::init(int elem_size, int init_size)
{
    this->elem_size = elem_size;
    this->size = init_size;
    this->cursor = 0;
    this->free_top = -1;
    this->buffer = NULL;

    if(init_size)
    {
        this->buffer = calloc(init_size, elem_size);
        this->free_list = (int *)calloc(init_size, sizeof(int));
    }
}

void stack_list_t::resize(int new_size)
{
    void *new_buffer;

    if(this->size < new_size)
    {
        new_buffer = calloc(this->elem_size, new_size);

        if(this->buffer)
        {
            memcpy(new_buffer, this->buffer, this->elem_size * this->size);
            free(this->buffer);
            free(this->free_list);
        }

        this->buffer = new_buffer;
        this->size = new_size;
        this->free_list = (int *)calloc(this->size, sizeof(int));
    }
}


int stack_list_t::add(void *data)
{
    int index = -1;

    if(this->free_top > -1)
    {
        index = this->free_list[this->free_top];
        this->free_top--;
    }
    else
    {
        if(this->cursor >= this->size)
        {
            this->resize(this->size + 64);
        }

        index = this->cursor;
        this->cursor++;
    }

    if(index >= 0 && data)
    {
        memcpy((char *)this->buffer + index * this->elem_size, data, this->elem_size);
    }

    return index;
}


void stack_list_t::remove(int index)
{
    if(index >= 0 && index < this->cursor)
    {
        this->free_top++;
        this->free_list[this->free_top] = index;
    }
}


void *stack_list_t::get(int index)
{
    void *ret = NULL;

    if(index >= 0 && index < this->cursor)
    {
        ret = (char *)this->buffer + index * this->elem_size;
    }

    return ret;
}







