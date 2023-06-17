#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include "window.h"
#include "block.h"

#define GRID_X  40
#define GRID_Y 20
#define GRID_LINE '*'
#define GAME_WINDOW_POS_Y 3
#define GAME_WINODW_POS_X 3
#define NEXT_BLOCK_WINDOW_POS_X 20
#define NEXT_BLOCK_WINDOW_POS_Y 10
#define HIGHEST_POINT_POS_Y 3
#define HIGHEST_POINT_POS_X (GAME_WINODW_POS_X + WINDOW_X + 4)
#define POINT_POS_Y 5
#define POINT_POS_X (GAME_WINODW_POS_X + WINDOW_X + 4)
#define BLOCK_GENERATE_POS_X (GAME_WINODW_POS_X + WINDOW_X)/2
#define BLOCK_GENERATE_POS_Y GAME_WINDOW_POS_Y-2
#define DRAW 1
#define ERASE 2 
#define BLOCK_POINT 4
#define LINE_POINT 100
#define TIME 1.5 
#define FILE_NAME "save.txt"
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
extern char *next_block_windows; 
extern char *windows_top;
char hP[] = "Higest point:";
char p[] = "Point:";

void grid_init();  
void draw();
char *block_generater(int,int,int);
void block_drawer(char *, int, int, int);
void getPoint();
void eraseLine(int, int);
int block_done();
int done = 0;
int islost();
void swap(char *,char *);
int cal_line_point(int);
char grid_buff[GRID_Y*(GRID_X+1)+2];
char grid[GRID_Y][GRID_X];
int line[GRID_Y];
int highestPoint; 
int current_point;
char temp_block[BLOCK_X*BLOCK_Y];

int main(){
	char key;
	int temp_point = current_point; 
	printf("press 's' to start\n a to right, d to left,\n w to rotate, s to fall.\n");
	do {
		if (_kbhit()){
				key = _getch();	
			}
	} while (key != 's' && key != 'S'); // s for start 
	srand((unsigned)time(NULL));
	system("mode 41, 21");
	while(key != 'q' && key != 'Q'){
	grid_init();
 	draw();
 	char *block = NULL;
 	char *next_block = NULL;
 	key = '0';
	while(!islost()){
		done = 0;
		//char *block = block_generater(4);
		if(!next_block){
			block = block_generater(rand()%7 + 1,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X);
		}else{
			block_drawer(next_block,NEXT_BLOCK_WINDOW_POS_Y+1,NEXT_BLOCK_WINDOW_POS_X+3,ERASE); 
			block = next_block;
			strncpy(temp_block,block,BLOCK_Y*BLOCK_X);
			block_drawer(block,BLOCK_GENERATE_POS_Y,BLOCK_GENERATE_POS_X,DRAW); 
		}
		next_block = block_generater(rand()%7 + 1,NEXT_BLOCK_WINDOW_POS_Y+1,NEXT_BLOCK_WINDOW_POS_X+3);
		draw();
		time_t start;
		time_t end;
		while(!done && !islost()){

			if (_kbhit()){
				key = _getch();
				if(key == 's' || key == 'S') 
					time(&start); 
			}
			time(&end);
			//key = fgetc(stdin);
			switch (key){
				case 'W':
				case 'w':{
					rotate(block);
					break;
				}
				case 'A':
				case 'a':{
					move_left_block(block);
					break;
				}
				case 'D':
				case 'd':{
					move_right_block(block);
					break;
				}
				case ' ':{
					fall_down(block);
					break;
				}
				case 'S':
				case 's':{
					down(block);
					break;
				}
			}
			if(key != 'S' && key != 's'){
				if(difftime(end,start) >= TIME){
					key = 's';
					time(&start);
				}else{
					key = '0';
				}
			}else{
				key = '0';
			}
            system("cls");
			draw();
			getPoint();
			if(current_point > temp_point){
				char *pnt = &grid[POINT_POS_Y][POINT_POS_X + sizeof(p)];
				itoa(current_point, pnt, 10);
			}
			if(islost())
				break; 
		}
	}
	if(current_point > highestPoint){
		highestPoint = current_point;
		FILE *fp = NULL;
		fp = fopen(FILE_NAME, "w");
		if(fp == NULL){
			fprintf(stderr,"file open failed.\n");
		}
		char *buff = (char*)malloc(10*sizeof(char));
		itoa(highestPoint, buff, 10);
		char *end = buff;
		while(*end != '\0') end++;
		fwrite(buff,sizeof(char),end-buff,fp);
		fclose(fp);
		free(buff);
	}
	grid_init();
	char *game_over = &grid[8][4];
	strncpy(game_over, "Game.over.",sizeof("Game.over.")-1);
	game_over += GRID_X;
	strncpy(game_over, "Enter.any",sizeof("Enter.any.")-1);
	game_over += GRID_X;
	strncpy(game_over, ".key.to",sizeof(".key.to")-1);
	game_over += GRID_X;
	strncpy(game_over, "..restart",sizeof("..restart")-1);
	game_over += GRID_X;
	strncpy(game_over, ".or.(q)uit",sizeof(".or.(q)uit")-1);
	Sleep(500); 
	draw();
	Sleep(5000); 
	key =  _getch();
	}
	return 0;
}

void grid_init(){
	int x,y;
	grid_buff[GRID_Y*(GRID_X+1)+1] = '\0';
	current_point = 0;
	memset(line, 0, sizeof(int)*GRID_Y); 
	memset(grid,' ',GRID_X*GRID_Y);
	for(y = 0; y < GRID_Y; ++y){
		for(x = 0; x < GRID_X; ++x){
			if(!x || !y || x == GRID_X-1 | y == GRID_Y-1){
				grid[y][x] = GRID_LINE;
			}else if(y >= GAME_WINDOW_POS_Y && y < GAME_WINDOW_POS_Y + WINDOW_Y
				    && x >= GAME_WINODW_POS_X && x < GAME_WINODW_POS_X + WINDOW_X){
				grid[y][x] = windows_base[(y-GAME_WINDOW_POS_Y) * WINDOW_X + (x-GAME_WINODW_POS_X)];
			}else if(y >= NEXT_BLOCK_WINDOW_POS_Y && y < NEXT_BLOCK_WINDOW_POS_Y + NEXT_BLOCK_WINDOW_Y
				    && x >= NEXT_BLOCK_WINDOW_POS_X && x < NEXT_BLOCK_WINDOW_POS_X + NEXT_BLOCK_WINDOW_X){
				grid[y][x] = next_block_windows[(y-NEXT_BLOCK_WINDOW_POS_Y) * NEXT_BLOCK_WINDOW_X + (x-NEXT_BLOCK_WINDOW_POS_X)];    
			}else if(y >= GAME_WINDOW_POS_Y-2 && y < GAME_WINDOW_POS_Y
				    && x >= GAME_WINODW_POS_X && x < GAME_WINODW_POS_X + WINDOW_X){
				grid[y][x] = windows_top[(y-(GAME_WINDOW_POS_Y-2))* WINDOW_X  + (x-GAME_WINODW_POS_X)];    
			}
		}
	}
	char *hight_point = &grid[HIGHEST_POINT_POS_Y][HIGHEST_POINT_POS_X];
	char *point = &grid[POINT_POS_Y][POINT_POS_X];
	strncpy(hight_point, hP,sizeof(hP));
	strncpy(point,p,sizeof(p));
	*(point + sizeof(p)) = '0';
	*(hight_point + sizeof(hP)) = '0';
	char *temp = hight_point + sizeof(hP);
	FILE *fp =NULL;
	fp = fopen(FILE_NAME, "a+");
	if(fp == NULL){
		fclose(fp);
		return;
	}
	else{
		char c; 
		while((c = fgetc(fp))!= EOF){
			*(hight_point + sizeof(hP))= c;
			hight_point++;
		}
		hight_point = '\0';
	}
	highestPoint = atoi(temp);
	fclose(fp);
	
}
void draw(){
	int x,y;
	char *buff = grid_buff;
	for(y = 0; y < GRID_Y; ++y){
		for(x = 0; x < GRID_X; ++x){
			if(x == 0 && y != 0){
				*buff++ = '\n';
			}
			if(grid[y][x] == '\0')
				*buff++ = ' '; 
			else
				*buff++ = grid[y][x];
		}
	}
	*(buff) = '\n';
	system("cls");
	printf("%s",grid_buff);
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
char *block_generater(int type, int pos_y, int pos_x){
	char *block = malloc(BLOCK_X*BLOCK_Y*sizeof(char));
	if(!block)
		return;
	char *temp = NULL;
	switch (type){
		case BLOCK_I:
			block_drawer(I,pos_y,pos_x, DRAW);
			temp = I;
			break;
		case BLOCK_J:
			block_drawer(J,pos_y,pos_x, DRAW);
			temp = J;
			break;
		case BLOCK_L:
			block_drawer(L,pos_y,pos_x, DRAW);
			temp = L;
			break;
		case BLOCK_O:
			block_drawer(O,pos_y,pos_x, DRAW);
			temp = O;
			break;
		case BLOCK_S:
			block_drawer(S,pos_y,pos_x, DRAW);
			temp = S;
			break;
		case BLOCK_T:
			block_drawer(T,pos_y,pos_x, DRAW);
			temp = T;
			break;
		case BLOCK_Z:
			block_drawer(Z,pos_y,pos_x, DRAW);
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
				line[y+block_curr_pos_y-GAME_WINDOW_POS_Y]++; 
			}
		}
	}
	done = 1;
}

void move_right_block(char *block){
	int x,y;
	for(x = BLOCK_X-1; x >= 0; --x){
		for(y = 0; y < BLOCK_Y; ++y){
			if(block[y*BLOCK_X + x] == BLOCK_BODY && (grid[y+block_curr_pos_y-1][x+block_curr_pos_x+1] == WALL
			|| grid[y+block_curr_pos_y-1][x+block_curr_pos_x+1] == BLOCK_DONE)){
				return;
			}
		}
	}
	block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
	block_drawer(block, block_curr_pos_y, ++block_curr_pos_x, DRAW); 
} 
void move_left_block(char *block){
	int x,y;
	for(x = 0; x < BLOCK_X; ++x){
		for(y = 0; y < BLOCK_Y; ++y){
			if(block[y*BLOCK_X + x] == BLOCK_BODY && (grid[y+block_curr_pos_y-1][x+block_curr_pos_x-1] == WALL
			|| grid[y+block_curr_pos_y-1][x+block_curr_pos_x-1] == BLOCK_DONE))
				return;
		}
	}
	block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
	block_drawer(block, block_curr_pos_y, --block_curr_pos_x, DRAW); 
}
void fall_down(char *block){
	while(down(block) != 1){
		Sleep(60);
		draw();
	}
		
}
int down(char *block){
	int x,y;
	for(y = BLOCK_Y -1 ; y >= 0; --y){
		for(x = 0; x < BLOCK_X; ++x){
			if(block[y*BLOCK_Y + x] == BLOCK_BODY && (grid[y+block_curr_pos_y][x+block_curr_pos_x] == WALL
			|| grid[y+block_curr_pos_y][x+block_curr_pos_x] == BLOCK_DONE)){
				block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
				current_point += BLOCK_POINT;
				block_done(block);
				block_drawer(block, block_curr_pos_y, block_curr_pos_x, DRAW);
				free(block);
				block_curr_pos_y = BLOCK_GENERATE_POS_Y;
				block_curr_pos_x = BLOCK_GENERATE_POS_X;
				return 1;
			}
				
		}
	}
	block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
	block_drawer(block, ++block_curr_pos_y, block_curr_pos_x, DRAW); 
	return 0;
}
void rotate(char *block){
	if(!strncmp(block,O,BLOCK_X*BLOCK_Y)||!strncmp(block,S,BLOCK_X*BLOCK_Y))
		return;
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
			if(temp[y*BLOCK_Y + x] == BLOCK_BODY
			&& (grid[y+block_curr_pos_y-1][x+block_curr_pos_x] == GRID_LINE
			|| grid[y+block_curr_pos_y-1][x+block_curr_pos_x] == WALL
			|| grid[y+block_curr_pos_y-1][x+block_curr_pos_x] == BLOCK_DONE 
			|| y+block_curr_pos_y < BLOCK_GENERATE_POS_Y)){
				free(temp);
				return;
			}
		}
	}
	strncpy(temp_block,block,BLOCK_Y*BLOCK_X);
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

void getPoint(){
	int x,y;
	int erasedLineCount = 0;
	int start = 0;
	int end = 0;
	int erased = 0;
	for(y = 0; y < WINDOW_Y; ++y){
		if(line[y] == WINDOW_X-2){
			if(erasedLineCount == 0)
				start = y;
			erased = 1;
			erasedLineCount++;
			end = y;
		}
	}
	if(!erased)
		return;
	if(erased){
		for(y = end; y - erasedLineCount >= 0; --y){
			line[y] = line[y - erasedLineCount];
		}
		eraseLine(start, end);
	}
}
void eraseLine(int start, int end){
	int x,y;
	for(y = start; y <= end; ++y){
		for(x = 0; x < WINDOW_X-2; ++x){
			grid[y+GAME_WINDOW_POS_Y-1][x+GAME_WINODW_POS_X+1] = '='; // wall |  |
		}
	}
	draw();
	for(y = start; y <= end; ++y){
		for(x = 0; x < WINDOW_X-2; ++x){
			grid[y+GAME_WINDOW_POS_Y-1][x+GAME_WINODW_POS_X+1] = BACKGROUND; // wall |  |
		}
	}
	current_point += cal_line_point(end-start+1) * LINE_POINT; 
	Sleep(500);
	draw();
	for(y = end; y > end-start+1; --y){
		for(x = 0; x < WINDOW_X-2; ++x){
			grid[y+GAME_WINDOW_POS_Y-1][x+GAME_WINODW_POS_X+1] = grid[y+GAME_WINDOW_POS_Y-1-(end-start+1)][x+GAME_WINODW_POS_X+1];
		}
	}
	Sleep(500);
	draw();
}

int cal_line_point(int fact){
	int factor = 1;
	while(--fact){
		factor*=2;
	}
	return factor;
}
