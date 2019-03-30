#ifndef S_BACKEND_H
#define S_BACKEND_H




enum BE_BACKEND_CMD
{
    BE_CMD_SWAP_BUFFERS = 0,
    BE_CMD_CLEAR_BUFFER,
    BE_CMD_MATRIX_MODE,
    BE_CMD_LOAD_MATRIX,
    BE_CMD_PUSH_MATRIX,
    BE_CMD_POP_MATRIX,
    BE_CMD_UPDATE_INPUT,
    BE_CMD_SHUTDOWN,
    BE_CMD_TEST,
};



struct backend_cmd_t
{
    unsigned int type;
};

void be_Init();

void be_Shutdown();

void be_RunBackend();


void be_QueueCmd(int type, void *data, int size);

void be_MatrixMode(int mode);

void be_LoadMatrix(float *matrix);

void be_Clear(int mask);

void be_WaitEmptyQueue();




#endif // S_BACKEND_H
