#ifndef TETRIS_H_INCLUDED
#define TETRIS_H_INCLUDED

#include <stdbool.h>

#define TETRIS_W		(12)
#define TETRIS_H		(20)


struct tetris {
	char game[TETRIS_W][TETRIS_H];
	bool gameover;
	uint16_t score;
	struct tetris_block {
		char data[4][4];
		uint8_t w;
		uint8_t h;
	} termino;
	int8_t x;
	int8_t y;
};


void tetris_signal_quit(int);

void tetris_init(void);

void tetris_clean(void);

void tetris_print(void);

void tetris_new_block(void);

void tetris_print_block(void);

void tetris_rotate(void);

void tetris_gravity(void);

void tetris_fall(int l);

void tetris_check_lines(void);

bool tetris_hittest(void);
bool tetris_is_game_over(void);
void tetris_move_right(void);
void tetris_move_left(void);
#endif //TETRIS_H_INCLUDED
