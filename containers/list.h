#ifndef LIST_H
#define LIST_H




class list_t
{
    public:

    void *buffer;
    unsigned int size;
    unsigned int cursor;
    unsigned int elem_size;

    list_t();

    list_t(int elem_size, int init_size);

    ~list_t();

    void init(unsigned int elem_size, unsigned int init_size);

    void resize(unsigned int new_size);

    int add(void *data);

    void remove(unsigned int index);

    void remove_move(unsigned int index);

    void *get(unsigned int index);

    void * operator [] (int index);
};


#endif // LIST_H
