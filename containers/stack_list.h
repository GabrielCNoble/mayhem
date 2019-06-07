#ifndef STACK_LIST_H
#define STACK_LIST_H


class stack_list_t
{
    public:

    void *buffer;
    int *free_list;
    int size;
    int cursor;
    int free_top;
    int elem_size;

    stack_list_t();

    stack_list_t(int elem_size, int init_size);

    ~stack_list_t();

    void init(int elem_size, int init_size);

    void resize(int new_size);

    int add(void *data);

    void remove(int index);

    void *get(int index);
};


#endif // STACK_LIST_H
