#ifndef RING_BUFFER_H
#define RING_BUFFER_H




class ring_buffer_t
{
    public:

        void *buffer;
        int next_in;
        int next_out;
        int elem_size;
        int elem_count;

        ring_buffer_t();

        ring_buffer_t(int elem_size, int elem_count);

        ~ring_buffer_t();

        void init(int elem_size, int elem_count);

        int add_next(void *data);

        void *get_next();

        int avaliable();
};


#endif // RING_BUFFER_H
