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

        void advance_next_in();

        void advance_next_out();

        int add_next(void *data);

        int add_next_no_advance(void *data);

        void *get_next();

        void *get_next_no_advance();

        int available();
};


#endif // RING_BUFFER_H
