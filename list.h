#ifndef LIST_H
#define LIST_H




class list_t
{
    public:

    void *buffer;
    int size;
    int cursor;
    unsigned int elem_size;

    list_t();

    list_t(int elem_size, int init_size);

    ~list_t();

    void init(int elem_size, int init_size);

    void resize(int new_size);

    int add(void *data);

    void remove(int index);

    void remove_move(int index);

    void *get(int index);

    void * operator [] (int index);
};


#endif // LIST_H
