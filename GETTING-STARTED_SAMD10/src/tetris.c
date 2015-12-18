#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "tetris.h"
#include "terminal.h"


struct tetris_block blocks[] =
	{
		{{"##", 
		  "##"},
		  2, 2
		},
		{{" # ",
		  "###"},
		  3, 2
		},
		{{"####"},
		  4, 1},
		{{"##",
		  "# ",
		  "# "},
		  2, 3},
		{{"##",
		  " #",
		  " #"},
		  2, 3},
		{{"## ",
		  " ##"},
		  3, 2}
	};


#define TETRIS_PIECES (sizeof(blocks)/sizeof(struct tetris_block))

static struct tetris t;

void tetris_init(void) 
{
	uint8_t x, y;
	t.score = 0;
	t.gameover = false;

	//Clear game table
	for (x = 0; x < TETRIS_W; x++) 
	{
		for ( y = 0; y < TETRIS_H; y++)
			t.game[x][y] = ' ';
	}
}


void tetris_print(void) 
{
	int x,y;
	
	printf(" [SAMD10 TETRIS FOR GEEKTIMES]\n");
	printf(" [========SCORE: %d========]\n", t.score);
	for (y = 0; y < TETRIS_H; y++) 
	{
		terminal_putc('<');
		terminal_putc('!');
		for (x = 0; x < TETRIS_W; x++) 
		{
			if ((x >= t.x) &&
				(y >= t.y) &&
				(x < (t.x+t.termino.w)) &&
				(y < (t.y+t.termino.h)) &&
				(t.termino.data[y-t.y][x-t.x] != ' ')) 
			{
				terminal_putc(t.termino.data[y-t.y][x-t.x]);	
			} 
			else 
			{
				terminal_putc(t.game[x][y]);
			}
			
			if (x != (TETRIS_W - 1)) 
			{
				terminal_putc(' ');	
			}
		}
		terminal_putc('!');
		terminal_putc('>');
		terminal_putc('\n');
	}
	
	terminal_putc('<');
	terminal_putc('!');
	for (x=0; x < 2*TETRIS_W - 1; x++)
		terminal_putc('=');
	terminal_putc('!');
	terminal_putc('>');
	
	if (t.gameover) 
	{
		terminal_putc('\n');
		terminal_putc('*');
		terminal_putc('*');
		terminal_putc('*');
	    terminal_putc('*');
		terminal_putc('*');
		terminal_putc(' ');
		terminal_putc('G');
		terminal_putc('A');
		terminal_putc('M');
		terminal_putc('E');
		terminal_putc(' ');
		terminal_putc('O');
		terminal_putc('V');
		terminal_putc('E');
		terminal_putc('R');
		terminal_putc(' ');
		terminal_putc('*');
		terminal_putc('*');
		terminal_putc('*');
		terminal_putc('*');
		terminal_putc('*');
	}
}


void tetris_new_block(void) 
{
	t.termino = blocks[rand() % TETRIS_PIECES];
	t.x= (TETRIS_W / 2) - (t.termino.w / 2);
	t.y = 0;
	
	if (tetris_hittest()) 
	{
		t.gameover = true;
	}
}


bool tetris_hittest(void) 
{
	if (t.x < 0) 
	{
		return true;	
	} 
	else if ((t.x + t.termino.w) > TETRIS_W) 
	{
		return true;
	}
	
	for (uint8_t x = 0; x < t.termino.w; x++) 
	{
		for (uint8_t y = 0; y < t.termino.h; y++) 
		{			
			if (t.termino.data[y][x] != ' ') 
			{
				int8_t abs_x = t.x + x;
				int8_t abs_y = t.y + y;
				if ((abs_y >= TETRIS_H) ||	(abs_y >= 0 && t.game[abs_x][abs_y] != ' ')) 
				{
					return true;
				}
			}
		}
	}
	
	return false;
}


void tetris_print_block(void) 
{
	for (uint8_t x = 0; x < t.termino.w; x++) 
	{
		for (uint8_t y = 0; y < t.termino.h; y++) 
		{
			if (t.termino.data[y][x] != ' ')
			{
				t.game[t.x + x][t.y + y]=t.termino.data[y][x];
			}
		}	
	}
}


void tetris_rotate(void) 
{
	struct tetris_block b = t.termino;
	struct tetris_block s = b;
	
	int8_t x, y;
	b.w = s.h;
	b.h = s.w;
	
	for (x = 0; x < s.w; x++) 
	{
		for (y = 0; y < s.h; y++) 
		{
			b.data[x][y] = s.data[s.h - y - 1][x];
		}
	}
	
	x = t.x;
	y = t.y;
	t.x -= (b.w - s.w) / 2;
	t.y -= (b.h - s.h) / 2;
	t.termino = b;
	if (tetris_hittest()) 
	{
		t.termino=s;
		t.x = x;
		t.y = y;
	}
}


void tetris_gravity(void) 
{
	t.y++;
	if (tetris_hittest()) 
	{
		t.y--;
		tetris_print_block();
		tetris_new_block();
	}
}


void tetris_fall(int l) 
{
	for (int8_t y = l; y > 0; y--) 
	{
		for (uint8_t x = 0; x < TETRIS_W; x++)
			t.game[x][y] = t.game[x][y - 1];
	}
	
	for (uint8_t x = 0; x < TETRIS_W; x++)
		t.game[x][0] = ' ';
}


void tetris_check_lines(void) 
{
	bool l;
	uint16_t p = 100;
	for (int8_t y = (TETRIS_H - 1); y >= 0; y--) 
	{
		l = true;
		for (uint8_t x = 0; x < TETRIS_W && l; x++) 
		{
			if (t.game[x][y] == ' ') 
			{
				l = false;
			}
		}
		
		if (l) 
		{
			t.score += p;
			p *= 2;
			tetris_fall(y);
			y++;
		}
	}
}


bool tetris_is_game_over(void)
{
	return t.gameover;
}


void tetris_move_right(void)
{
	t.x++;
	if (tetris_hittest()) 
	{
		t.x--;
	}
}

void tetris_move_left(void)
{
	t.x--;
	if (tetris_hittest()) 
	{
		t.x++;
	}
}
