#ifndef BLOCK_H
#define BLOCK_H

#define BLOCK_I 1
#define BLOCK_J 2
#define BLOCK_L 3
#define BLOCK_O 4
#define BLOCK_S 5
#define BLOCK_T 6
#define BLOCK_Z 7
#define BLOCK_X 4
#define BLOCK_Y 4
void rotate(char *block);
void reset_block(char **block);
void move_right_block(char *);
void move_left_block(char *);
void clean_block(char *, int, int);
void fall_down(char *);
void down(char *);


#endif /*  WINDOWS_H */
