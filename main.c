#include <stdio.h>
#include <time.h> 
#include <stdlib.h> 
#include "windows.h"
#include "block.h"

#define GRID_X  30
#define GRID_Y 20
#define GRID_LINE '*'
#define GAME_WINDOW_POS_Y 3
#define GAME_WINODW_POS_X 3
#define BLOCK_GENERATE_POS_X (GAME_WINODW_POS_X + WINDOW_X)/2
#define BLOCK_GENERATE_POS_Y GAME_WINDOW_POS_Y
#define DRAW 1
#define ERASE 2 
int block_curr_pos_x = BLOCK_GENERATE_POS_X;
int block_curr_pos_y = BLOCK_GENERATE_POS_Y;
 
extern char *I;
extern char *J;
extern char *L;
extern char *O;
extern char *S;
extern char *T;
extern char *Z;
extern char *windows_base;
void grid_init();  
void draw();
char *block_generater(int);
void block_drawer(char *, int, int, int);
int block_done();
int done = 0;
char grid[GRID_Y][GRID_X];


int main(){
	char key;
	printf("press 's' to start\n");
	do {
		key = getchar();
	} while (key != 's'); // s for start 
	srand((unsigned)time(NULL));
	grid_init();
	while(1){
		system("cls");
		done = 0;
		char *block = block_generater(rand()%7 + 1);
		while(!done){
			key = getchar();
			switch (key){
				case 'w':{
					
					break;
				}
				case 'a':{
					move_left_block(block);
					break;
				}
				case 'd':{
					move_right_block(block);
					break;
				}
				case ' ':{
					fall_down(block);
					break;
				}
				case 's':{
					down(block);
					break;
				}
			}
		draw();
		}
		key = getchar();
		if(key == 'q')
			break;
	}
	return 0;
}

void grid_init(){
	int x,y;
	for(y = 0; y < GRID_Y; ++y){
		for(x = 0; x < GRID_X; ++x){
			if(!x || !y || x == GRID_X-1 | y == GRID_Y-1){
				grid[y][x] = GRID_LINE;
			}else if(y >= GAME_WINDOW_POS_Y && y < GAME_WINDOW_POS_Y + WINDOW_Y
				    && x >= GAME_WINODW_POS_X && x < GAME_WINODW_POS_X + WINDOW_X){
				grid[y][x] = windows_base[(y-GAME_WINDOW_POS_Y) * WINDOW_Y + (x-GAME_WINODW_POS_X)];
			}
		}
	}
}
void draw(){
	int x,y;
	for(y = 0; y < GRID_Y; ++y){
		for(x = 0; x < GRID_X; ++x){
			putchar(grid[y][x]);
		}
		putchar('\n');
	}
} 

void block_drawer(char *block, int pos_y, int pos_x, int flag){
	int x,y;
	for(y = 0; y < BLOCK_Y; ++y){
		for(x = 0; x < BLOCK_X; ++x){
			if(block[y*BLOCK_Y + x] == BLOCK_BODY){
				if(flag == 1){
					grid[y+pos_y-1][x+pos_x] = block[y*BLOCK_Y + x];
				}else if(flag == 2){
					grid[y+pos_y-1][x+pos_x] = BACKGROUND;
				}
			}
		}
	}
}
char *block_generater(int type){
	switch (type){
		case BLOCK_I:
			block_drawer(I,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			return I;
			break;
		case BLOCK_J:
			block_drawer(J,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			return J;
			break;
		case BLOCK_L:
			block_drawer(L,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			return L;
			break;
		case BLOCK_O:
			block_drawer(O,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			return O;
			break;
		case BLOCK_S:
			block_drawer(S,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			return S;
			break;
		case BLOCK_T:
			block_drawer(T,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			return T;
			break;
		case BLOCK_Z:
			block_drawer(Z,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			return Z;
			break;
		default:
			break;
	}
}

int block_done(char *block){
	int x,y;
	for(y = 0; y < BLOCK_Y; ++y){
		for(x = 0; x < BLOCK_X; ++x){
			if((y*BLOCK_Y + x) < 16 && block[y*BLOCK_Y + x] == 'O'){
				//block[y*BLOCK_Y + x] = '0';
			}
		}
	}
	done = 1;
}
void clean_block(char *block, int pos_y, int pos_x){
	
} 
void move_right_block(char *block){
	int x,y;
	for(x = BLOCK_X-1; x > 0; --x){
		for(y = 0; y < BLOCK_Y; ++y){
			if(block[y*BLOCK_Y + x] == BLOCK_BODY && (grid[y+block_curr_pos_y][x+block_curr_pos_x+1] == WALL
			|| grid[y+block_curr_pos_y][x+block_curr_pos_x+1] == BLOCK_DONE))
				return;
		}
	}
	block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
	block_drawer(block, block_curr_pos_y, ++block_curr_pos_x, DRAW); 
} 
void move_left_block(char *block){
	int x,y;
	for(x = 0; x < BLOCK_X; ++x){
		for(y = 0; y < BLOCK_Y; ++y){
			if(block[y*BLOCK_Y + x] == BLOCK_BODY && grid[y+block_curr_pos_y][x+block_curr_pos_x-1] == WALL
			|| grid[y+block_curr_pos_y][x+block_curr_pos_x+1] == BLOCK_DONE)
				return;
		}
	}
	block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
	block_drawer(block, block_curr_pos_y, --block_curr_pos_x, DRAW); 
}
void fall_down(char *block){
	
}
void down(char *block){
	int x,y;
	for(y = BLOCK_Y -1 ; y > 0; --y){
		for(x = 0; x < BLOCK_X; ++x){
			if(block[y*BLOCK_Y + x] == BLOCK_BODY && grid[y+block_curr_pos_y][x+block_curr_pos_x] == WALL
			|| grid[y+block_curr_pos_y][x+block_curr_pos_x] == BLOCK_DONE){
				block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
				block_done(block);
				block_drawer(block, block_curr_pos_y, block_curr_pos_x, DRAW);
				
				return;
			}
				
		}
	}
	block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
	block_drawer(block, ++block_curr_pos_y, block_curr_pos_x, DRAW); 
}
