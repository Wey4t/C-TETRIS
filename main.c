#include <stdio.h>
#include <time.h>
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include "window.h"
#include "block.h"

#define GRID_X  40
#define GRID_Y 20
#define GRID_LINE '*'
#define GAME_WINDOW_POS_Y 3
#define GAME_WINDOW_POS_X 3
#define NEXT_BLOCK_WINDOW_POS_X 20
#define NEXT_BLOCK_WINDOW_POS_Y 10
#define HIGHEST_POINT_POS_Y 3
#define HIGHEST_POINT_POS_X (GAME_WINDOW_POS_X + WINDOW_X + 4)
#define POINT_POS_Y 5
#define POINT_POS_X (GAME_WINDOW_POS_X + WINDOW_X + 4)
#define BLOCK_GENERATE_POS_X (GAME_WINDOW_POS_X + WINDOW_X)/2
#define BLOCK_GENERATE_POS_Y GAME_WINDOW_POS_Y-2
#define DRAW 1
#define ERASE 2 
#define BLOCK_POINT 4
#define LINE_POINT 100
#define TIME 0.5 
#define FILE_NAME "save.txt"

// Color pairs for ncurses
#define COLOR_GAME_BORDER 1
#define COLOR_BLOCK_ACTIVE 2
#define COLOR_BLOCK_PLACED 3
#define COLOR_UI_TEXT 4
#define COLOR_SCORE 5
#define COLOR_NEXT_BLOCK 6

// Game windows
WINDOW *game_win;
WINDOW *next_win;
WINDOW *score_win;
WINDOW *info_win;

int block_curr_pos_x = BLOCK_GENERATE_POS_X;
int block_curr_pos_y = BLOCK_GENERATE_POS_Y;
 
extern char *I;
extern char *J;
extern char *L;
extern char *O;
extern char *S;
extern char *T;
extern char *Z;

char grid[GRID_Y][GRID_X];
int line[GRID_Y];
int highestPoint = 0; 
int current_point = 0;
char temp_block[BLOCK_X*BLOCK_Y];
int done = 0;

// Function declarations
void init_ncurses();
void cleanup_ncurses();
void init_colors();
void create_windows();
void destroy_windows();
void grid_init();
void draw_game();
void draw_ui();
void draw_next_block_window();
char *block_generator(int type, int pos_y, int pos_x);
void block_drawer(char *block, int pos_y, int pos_x, int flag);
void getPoint();
void eraseLine(int start, int end);
int block_done(char *block);
int islost();
void swap(char *a, char *b);
int cal_line_point(int fact);
void move_right_block(char *block);
void move_left_block(char *block);
void fall_down(char *block);
int down(char *block);
void rotate(char *block);
void show_instructions();
void show_game_over();
void load_high_score();
void save_high_score();
char *create_next_block_data(int type);

int main(){
    int key;
    
    // Initialize ncurses
    init_ncurses();
    init_colors();
    create_windows();
    
    // Show instructions
    show_instructions();
	
    srand((unsigned)time(NULL));
    
    while(1) {
        grid_init();
        load_high_score();
        draw_game();
        draw_ui();
        
        char *block = NULL;
        char *next_block = NULL;
        
        while(!islost()) {
            done = 0;
            
            // Generate blocks
            if(!next_block) {
                block = block_generator(rand()%7 + 1, BLOCK_GENERATE_POS_Y, BLOCK_GENERATE_POS_X);
            } else {
                block = next_block;
                block_drawer(block, BLOCK_GENERATE_POS_Y, BLOCK_GENERATE_POS_X, DRAW); 
            }
            // Generate next block data (don't draw it in main grid)
            next_block = malloc(BLOCK_X*BLOCK_Y*sizeof(char));
            if(next_block) {
                char *temp = NULL;
                int next_type = rand()%7 + 1;
                switch(next_type) {
                    case BLOCK_I: temp = I; break;
                    case BLOCK_J: temp = J; break;
                    case BLOCK_L: temp = L; break;
                    case BLOCK_O: temp = O; break;
                    case BLOCK_S: temp = S; break;
                    case BLOCK_T: temp = T; break;
                    case BLOCK_Z: temp = Z; break;
                    default: break;
                }
                if(temp) {
                    strncpy(next_block, temp, BLOCK_X*BLOCK_Y);
                }
            }
            
            // Draw next block in preview window
            werase(next_win);
            box(next_win, 0, 0);
            mvwprintw(next_win, 0, 2, " NEXT ");
            if(next_block) {
                for(int y = 0; y < BLOCK_Y; y++) {
                    for(int x = 0; x < BLOCK_X; x++) {
                        if(next_block[y*BLOCK_Y + x] == BLOCK_BODY) {
                            wattron(next_win, COLOR_PAIR(COLOR_NEXT_BLOCK));
                            mvwaddch(next_win, y+1, x+1, 'O');
                            wattroff(next_win, COLOR_PAIR(COLOR_NEXT_BLOCK));
                        }
                    }
                }
            }
            wrefresh(next_win);
            
            draw_game();
            draw_ui();
            
            time_t start, end;
            time(&start);
            
            while(!done && !islost()) {
                timeout(50); // 50ms timeout for getch()
                key = getch();
                
                time(&end);
                
                switch(key) {
                    case 'w':
                    case 'W':
                        rotate(block);
                        break;
                    case 'a':
                    case 'A':
                        move_left_block(block);
                        break;
                    case 'd':
                    case 'D':
                        move_right_block(block);
                        break;
                    case 's':
                    case 'S':
                        down(block);
                        time(&start);
                        break;
                    case ' ':
                        fall_down(block);
                        break;
                    case 'q':
                    case 'Q':
                        goto exit_game;
                }
                
                // Auto-fall
                if(difftime(end, start) >= TIME) {
                    down(block);
                    time(&start);
                }
                
                draw_game();
                draw_ui();
                getPoint();
                
                if(islost())
                    break;
            }
        }
        
        // Game over
        show_game_over();
        save_high_score();
        
        // Wait for restart or quit
        timeout(-1); // Blocking input
        key = getch();
        if(key == 'q' || key == 'Q')
            break;
    	clear();
		
	
    }
    
exit_game:
    cleanup_ncurses();
    return 0;
}

void init_ncurses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0); // Hide cursor
    
    // Check if terminal supports colors
    if(has_colors()) {
        start_color();
    }
}

void cleanup_ncurses() {
    destroy_windows();
    endwin();
}

void init_colors() {
    if(has_colors()) {
        init_pair(COLOR_GAME_BORDER, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_BLOCK_ACTIVE, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_BLOCK_PLACED, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_UI_TEXT, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_SCORE, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOR_NEXT_BLOCK, COLOR_RED, COLOR_BLACK);
    }
}

void create_windows() {
    // Main game window
    game_win = newwin(WINDOW_Y, WINDOW_X, GAME_WINDOW_POS_Y, GAME_WINDOW_POS_X);
    
    // Next block preview window (make it bigger to show blocks properly)
    next_win = newwin(6, 8, NEXT_BLOCK_WINDOW_POS_Y, NEXT_BLOCK_WINDOW_POS_X);
    
    // Score window
    score_win = newwin(6, 20, HIGHEST_POINT_POS_Y, HIGHEST_POINT_POS_X);
    
    // Info window for controls
    info_win = newwin(10, 25, 1, 45);
}

void destroy_windows() {
    if(game_win) delwin(game_win);
    // Next block preview window (make it bigger to show blocks properly)
    if(score_win) delwin(score_win);
    if(info_win) delwin(info_win);
}

void grid_init() {
    // Info window for controls
    memset(line, 0, sizeof(int) * GRID_Y);
    memset(grid, ' ', GRID_X * GRID_Y);
    
    // Initialize game boundaries
    for(int y = 0; y < GRID_Y; y++) {
        for(int x = 0; x < GRID_X; x++) {
            if(!x || !y || x == GRID_X-1 || y == GRID_Y-1) {
                grid[y][x] = GRID_LINE;
            } else if(y >= GAME_WINDOW_POS_Y && y < GAME_WINDOW_POS_Y + WINDOW_Y
                    && x >= GAME_WINDOW_POS_X && x < GAME_WINDOW_POS_X + WINDOW_X) {
                if(x == GAME_WINDOW_POS_X || x == GAME_WINDOW_POS_X + WINDOW_X - 1) {
                    grid[y][x] = WALL;
                } else if(y == GAME_WINDOW_POS_Y + WINDOW_Y - 1) {
                    grid[y][x] = WALL;
                } else {
                    grid[y][x] = BACKGROUND;
                }
            }
        }
    }
}

void draw_game() {
    // Clear and redraw game window
    werase(game_win);
    box(game_win, 0, 0);
    
    // Draw the game field
    for(int y = 1; y < WINDOW_Y - 1; y++) {
        for(int x = 1; x < WINDOW_X - 1; x++) {
            char ch = grid[y + GAME_WINDOW_POS_Y][x + GAME_WINDOW_POS_X];
            
            if(ch == BLOCK_BODY) {
                wattron(game_win, COLOR_PAIR(COLOR_BLOCK_ACTIVE));
                mvwaddch(game_win, y, x, 'O');
                wattroff(game_win, COLOR_PAIR(COLOR_BLOCK_ACTIVE));
            } else if(ch == BLOCK_DONE) {
                wattron(game_win, COLOR_PAIR(COLOR_BLOCK_PLACED));
                mvwaddch(game_win, y, x, '#');
                wattroff(game_win, COLOR_PAIR(COLOR_BLOCK_PLACED));
            } else if(ch == BACKGROUND) {
                mvwaddch(game_win, y, x, ' ');
            }
        }
    }
    
    wrefresh(game_win);
}

void draw_ui() {
    // Score window
    werase(score_win);
    box(score_win, 0, 0);
    wattron(score_win, COLOR_PAIR(COLOR_UI_TEXT));
    mvwprintw(score_win, 1, 2, "TETRIS");
    mvwprintw(score_win, 2, 2, "Score:");
    wattron(score_win, COLOR_PAIR(COLOR_SCORE));
    mvwprintw(score_win, 2, 10, "%d", current_point);
    wattroff(score_win, COLOR_PAIR(COLOR_SCORE));
    wattron(score_win, COLOR_PAIR(COLOR_UI_TEXT));
    mvwprintw(score_win, 4, 2, "High:");
    wattron(score_win, COLOR_PAIR(COLOR_SCORE));
    mvwprintw(score_win, 4, 10, "%d", highestPoint);
    wattroff(score_win, COLOR_PAIR(COLOR_SCORE));
    wattroff(score_win, COLOR_PAIR(COLOR_UI_TEXT));
    wrefresh(score_win);
    
    // Info window
    werase(info_win);
    box(info_win, 0, 0);
    mvwprintw(info_win, 0, 2, " CONTROLS ");
    mvwprintw(info_win, 2, 2, "W/w - Rotate");
    mvwprintw(info_win, 3, 2, "A/a - Move Left");
    mvwprintw(info_win, 4, 2, "D/d - Move Right");
    mvwprintw(info_win, 5, 2, "S/s - Soft Drop");
    mvwprintw(info_win, 6, 2, "Space - Hard Drop");
    mvwprintw(info_win, 7, 2, "Q/q - Quit");
    wrefresh(info_win);
}

void show_instructions() {
    clear();
    mvprintw(LINES/2 - 1, COLS/2 - 10, "Press any key to start");
    mvprintw(LINES/2 + 1, COLS/2 - 15, "Use WASD to control pieces");
    mvprintw(LINES/2 + 2, COLS/2 - 10, "Press Q to quit anytime");
    refresh();
    getch();
    clear();
}

void show_game_over() {
    WINDOW *game_over_win = newwin(8, 30, LINES/2 - 4, COLS/2 - 15);
    box(game_over_win, 0, 0);
    wattron(game_over_win, COLOR_PAIR(COLOR_SCORE));
    mvwprintw(game_over_win, 2, 10, "GAME OVER");
    mvwprintw(game_over_win, 4, 5, "Final Score: %d", current_point);
    if(current_point > highestPoint) {
        mvwprintw(game_over_win, 5, 6, "NEW HIGH SCORE!");
    }
    wattroff(game_over_win, COLOR_PAIR(COLOR_SCORE));
    mvwprintw(game_over_win, 7, 3, "Press any key to restart");
    mvwprintw(game_over_win, 8, 8, "or Q to quit");
    wrefresh(game_over_win);
    delwin(game_over_win);
}

void load_high_score() {
    FILE *fp = fopen(FILE_NAME, "r");
    if(fp) {
        fscanf(fp, "%d", &highestPoint);
        fclose(fp);
    }
}

void save_high_score() {
    if(current_point > highestPoint) {
        highestPoint = current_point;
        FILE *fp = fopen(FILE_NAME, "w");
        if(fp) {
            fprintf(fp, "%d", highestPoint);
            fclose(fp);
        }
    }
}

// Block manipulation functions (same logic, but using ncurses drawing)
void block_drawer(char *block, int pos_y, int pos_x, int flag) {
    for(int y = 0; y < BLOCK_Y; y++) {
        for(int x = 0; x < BLOCK_X; x++) {
            if(block[y*BLOCK_Y + x] == BLOCK_BODY || block[y*BLOCK_Y + x] == BLOCK_DONE) {
                if(flag == DRAW) {
                    grid[y+pos_y-1][x+pos_x] = block[y*BLOCK_Y + x];
                } else if(flag == ERASE) {
                    grid[y+pos_y-1][x+pos_x] = BACKGROUND;
                }
            }
        }
    }
}

// Creates block data and draws it in the main game grid
char *block_generator(int type, int pos_y, int pos_x) {
    char *block = malloc(BLOCK_X*BLOCK_Y*sizeof(char));
    if(!block) return NULL;
    
    char *temp = NULL;
    switch(type) {
        case BLOCK_I: temp = I; break;
        case BLOCK_J: temp = J; break;
        case BLOCK_L: temp = L; break;
        case BLOCK_O: temp = O; break;
        case BLOCK_S: temp = S; break;
        case BLOCK_T: temp = T; break;
        case BLOCK_Z: temp = Z; break;
        default: break;
    }
    
    if(temp) {
        strncpy(block, temp, BLOCK_X*BLOCK_Y);
        block_drawer(block, pos_y, pos_x, DRAW);
    }
    
    return block;
}

// Creates block data WITHOUT drawing it anywhere (for next block preview)
char *create_next_block_data(int type) {
    char *block = malloc(BLOCK_X*BLOCK_Y*sizeof(char));
    if(!block) return NULL;
    
    char *temp = NULL;
    switch(type) {
        case BLOCK_I: temp = I; break;
        case BLOCK_J: temp = J; break;
        case BLOCK_L: temp = L; break;
        case BLOCK_O: temp = O; break;
        case BLOCK_S: temp = S; break;
        case BLOCK_T: temp = T; break;
        case BLOCK_Z: temp = Z; break;
        default: break;
    }
    
    if(temp) {
        strncpy(block, temp, BLOCK_X*BLOCK_Y);
        // DON'T draw it anywhere - this is just data for preview
    }
    
    return block;
}

// Rest of the game logic functions remain the same...
int block_done(char *block) {
    for(int y = 0; y < BLOCK_Y; y++) {
        for(int x = 0; x < BLOCK_X; x++) {
            if((y*BLOCK_Y + x) < 16 && block[y*BLOCK_Y + x] == 'O') {
                block[y*BLOCK_Y + x] = BLOCK_DONE;
                line[y+block_curr_pos_y-GAME_WINDOW_POS_Y]++; 
            }
        }
    }
    done = 1;
    return 1;
}

void move_right_block(char *block) {
    for(int x = BLOCK_X-1; x >= 0; x--) {
        for(int y = 0; y < BLOCK_Y; y++) {
            if(block[y*BLOCK_X + x] == BLOCK_BODY && 
               (grid[y+block_curr_pos_y-1][x+block_curr_pos_x+1] == WALL ||
                grid[y+block_curr_pos_y-1][x+block_curr_pos_x+1] == BLOCK_DONE)) {
                return;
            }
        }
    }
    block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
    block_drawer(block, block_curr_pos_y, ++block_curr_pos_x, DRAW); 
}

void move_left_block(char *block) {
    for(int x = 0; x < BLOCK_X; x++) {
        for(int y = 0; y < BLOCK_Y; y++) {
            if(block[y*BLOCK_X + x] == BLOCK_BODY && 
               (grid[y+block_curr_pos_y-1][x+block_curr_pos_x-1] == WALL ||
                grid[y+block_curr_pos_y-1][x+block_curr_pos_x-1] == BLOCK_DONE)) {
                return;
            }
        }
    }
    block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
    block_drawer(block, block_curr_pos_y, --block_curr_pos_x, DRAW); 
}

void fall_down(char *block) {
    while(down(block) != 1) {
        usleep(60000);
        draw_game();
    }
}

int down(char *block) {
    for(int y = BLOCK_Y-1; y >= 0; y--) {
        for(int x = 0; x < BLOCK_X; x++) {
            if(block[y*BLOCK_Y + x] == BLOCK_BODY && 
               (grid[y+block_curr_pos_y][x+block_curr_pos_x] == WALL ||
                grid[y+block_curr_pos_y][x+block_curr_pos_x] == BLOCK_DONE)) {
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

void rotate(char *block) {
    if(!strncmp(block, O, BLOCK_X*BLOCK_Y) || !strncmp(block, S, BLOCK_X*BLOCK_Y))
        return;
        
    char *temp = malloc(BLOCK_Y*BLOCK_X*sizeof(char));
    strncpy(temp, block, BLOCK_Y*BLOCK_X);
    
    // Rotation matrix transformation
    for(int y = 0; y < BLOCK_Y/2; y++) {
        for(int x = 0; x < BLOCK_X; x++) {
            swap(&temp[y*BLOCK_Y + x], &temp[(BLOCK_Y-y-1)*BLOCK_Y + x]);
        }
    }
    for(int y = 0; y < BLOCK_Y; y++) {
        for(int x = 0; x < y; x++) {
            swap(&temp[y*BLOCK_Y + x], &temp[x*BLOCK_Y + y]);
        }
    }
    
    // Check if rotation is valid
    for(int x = 0; x < BLOCK_X; x++) {
        for(int y = 0; y < BLOCK_Y; y++) {
            if(temp[y*BLOCK_Y + x] == BLOCK_BODY &&
               (grid[y+block_curr_pos_y-1][x+block_curr_pos_x] == GRID_LINE ||
                grid[y+block_curr_pos_y-1][x+block_curr_pos_x] == WALL ||
                grid[y+block_curr_pos_y-1][x+block_curr_pos_x] == BLOCK_DONE ||
                y+block_curr_pos_y < BLOCK_GENERATE_POS_Y)) {
                free(temp);
                return;
            }
        }
    }
    
    block_drawer(block, block_curr_pos_y, block_curr_pos_x, ERASE);
    block_drawer(temp, block_curr_pos_y, block_curr_pos_x, DRAW); 
    strncpy(block, temp, BLOCK_Y*BLOCK_X);
    free(temp);
}

void swap(char *a, char *b) {
    char temp = *a;
    *a = *b;
    *b = temp;
}

int islost() {
    for(int x = 0; x < WINDOW_X; x++) {
        if(grid[GAME_WINDOW_POS_Y][x] == BLOCK_DONE) {
            return 1;
        }
    }
    return 0;
}

void getPoint() {
    int erasedLineCount = 0;
    int start = 0, end = 0;
    int erased = 0;
    
    for(int y = 0; y < WINDOW_Y; y++) {
        if(line[y] == WINDOW_X-2) {
            if(erasedLineCount == 0) start = y;
            erased = 1;
            erasedLineCount++;
            end = y;
        }
    }
    
    if(!erased) return;
    
    for(int y = end; y - erasedLineCount >= 0; y--) {
        line[y] = line[y - erasedLineCount];
    }
    eraseLine(start, end);
}

void eraseLine(int start, int end) {
    // Highlight cleared lines
    for(int y = start; y <= end; y++) {
        for(int x = 0; x < WINDOW_X-2; x++) {
            grid[y+GAME_WINDOW_POS_Y-1][x+GAME_WINDOW_POS_X+1] = '=';
        }
    }
    draw_game();
    usleep(300000);
    
    // Clear the lines
    for(int y = start; y <= end; y++) {
        for(int x = 0; x < WINDOW_X-2; x++) {
            grid[y+GAME_WINDOW_POS_Y-1][x+GAME_WINDOW_POS_X+1] = BACKGROUND;
        }
    }
    
    current_point += cal_line_point(end-start+1) * LINE_POINT;
    
    // Drop lines down
    for(int y = end; y > end-start; y--) {
        for(int x = 0; x < WINDOW_X-2; x++) {
            grid[y+GAME_WINDOW_POS_Y-1][x+GAME_WINDOW_POS_X+1] = 
                grid[y+GAME_WINDOW_POS_Y-1-(end-start+1)][x+GAME_WINDOW_POS_X+1];
        }
    }
    
    draw_game();
}

int cal_line_point(int fact) {
    int factor = 1;
    while(--fact) {
        factor *= 2;
    }
    return factor;
}
