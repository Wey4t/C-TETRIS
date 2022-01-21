#include <stdio.h>
#include <time.h> 
#include <stdlib.h> 
#include <string.h>
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
int islost();
char grid[GRID_Y][GRID_X];
void swap(char *,char *);
int main(){
	char key;
	printf("press 's' to start\n a to right, d to left,\n w to rotate, s to fall.\n");
	do {
		key = fgetc(stdin);
	} while (key != 's'); // s for start 
	srand((unsigned)time(NULL));
	system("mode con cols=30 lines=20");
	grid_init();
	while(!islost()){
		done = 0;
		char *block = block_generater(rand()%7 + 1);
		while(!done && !islost()){
			key = fgetc(stdin);
			switch (key){
				case 'w':{
					rotate(block);
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
		system("cls");
		draw();
		}
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
			if(block[y*BLOCK_Y + x] == BLOCK_BODY || block[y*BLOCK_Y + x] == BLOCK_DONE){
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
	char *block = malloc(BLOCK_X*BLOCK_Y*sizeof(char));
	if(!block)
		return;
	char *temp = NULL;
	switch (type){
		case BLOCK_I:
			block_drawer(I,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			temp = I;
			break;
		case BLOCK_J:
			block_drawer(J,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			temp = J;
			break;
		case BLOCK_L:
			block_drawer(L,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			temp = L;
			break;
		case BLOCK_O:
			block_drawer(O,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			temp = O;
			break;
		case BLOCK_S:
			block_drawer(S,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			temp = S;
			break;
		case BLOCK_T:
			block_drawer(T,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			temp = T;
			break;
		case BLOCK_Z:
			block_drawer(Z,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X, DRAW);
			temp = Z;
			break;
		default:
			break;
	}
	strncpy(block,temp,BLOCK_X*BLOCK_Y);
	return block;
}

int block_done(char *block){
	int x,y;
	for(y = 0; y < BLOCK_Y; ++y){
		for(x = 0; x < BLOCK_X; ++x){
			if((y*BLOCK_Y + x) < 16 && block[y*BLOCK_Y + x] == 'O'){
				block[y*BLOCK_Y + x] = BLOCK_DONE;
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
			if(block[y*BLOCK_Y + x] == BLOCK_BODY && (grid[y+block_curr_pos_y][x+block_curr_pos_x-1] == WALL
			|| grid[y+block_curr_pos_y][x+block_curr_pos_x-1] == BLOCK_DONE))
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
			if(block[y*BLOCK_Y + x] == BLOCK_BODY && (grid[y+block_curr_pos_y][x+block_curr_pos_x] == WALL
			|| grid[y+block_curr_pos_y][x+block_curr_pos_x] == BLOCK_DONE)){
				block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
				block_done(block);
				block_drawer(block, block_curr_pos_y, block_curr_pos_x, DRAW);
				free(block);
				block_curr_pos_y = BLOCK_GENERATE_POS_Y;
				block_curr_pos_x = BLOCK_GENERATE_POS_X;
				return;
			}
				
		}
	}
	block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
	block_drawer(block, ++block_curr_pos_y, block_curr_pos_x, DRAW); 
}
void rotate(char *block){
	char *temp = malloc(BLOCK_Y*BLOCK_X*sizeof(char));
	strncpy(temp,block,BLOCK_Y*BLOCK_X);
	int x,y;
	for(y = 0; y < BLOCK_Y/2; ++y){
		for(x = 0; x < BLOCK_X; ++x){
			swap(&temp[y*BLOCK_Y + x], &temp[(BLOCK_Y-y-1)*BLOCK_Y + x]);
		}
	}
	for(y = 0; y < BLOCK_Y; ++y){
		for(x = 0; x < y; ++x){
			swap(&temp[y*BLOCK_Y + x], &temp[x*BLOCK_Y + y]);
		}
	}
	for(x = 0; x < BLOCK_X; ++x){
		for(y = 0; y < BLOCK_Y; ++y){
			if(temp[y*BLOCK_Y + x] == BLOCK_BODY && (grid[y+block_curr_pos_y][x+block_curr_pos_x] == WALL
			|| grid[y+block_curr_pos_y][x+block_curr_pos_x] == BLOCK_DONE || y+block_curr_pos_y < BLOCK_GENERATE_POS_Y)){
				free(temp);
				return;
			}
		}
	}
	block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
	block_drawer(temp, block_curr_pos_y, block_curr_pos_x, DRAW); 
	strncpy(block,temp,BLOCK_Y*BLOCK_X);
	free(temp);
}
void swap(char *a, char *b){
	
	char temp = *a;
	*a = *b;
	*b = temp;
}
int islost(){
	int x;
	for(x = 0; x < WINDOW_X; ++x){
		if(grid[GAME_WINDOW_POS_Y][x] == BLOCK_DONE){
			return 1;
		}
	}
	return 0;
}
