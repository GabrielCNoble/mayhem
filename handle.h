#ifndef HANDLES_H
#define HANDLES_H




struct player_handle_t
{
    unsigned int player_index;
};

#define INVALID_PLAYER_INDEX 0xffffffff
#define PLAYER_HANDLE(player_index) (struct player_handle_t){player_index}
#define INVALID_PLAYER_HANDLE PLAYER_HANDLE(INVALID_PLAYER_INDEX)


#endif // HANDLES_H
