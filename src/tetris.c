#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

#include "tetris.h"
#include "tui.h"

#define EMPTY   0
#define BLOCK_I 1
#define BLOCK_J 2
#define BLOCK_L 3
#define BLOCK_S 4
#define BLOCK_Z 5
#define BLOCK_T 6
#define BLOCK_O 7

static void show_cell(int);

void show_cell(int cell) {
    if (cell == EMPTY)
        printf("  ");
    else
        printf("██");
}


void run_tetris(int rows, int cols) {
    int(*board)[cols] = malloc(rows * cols * sizeof(int));
    int x_shift, y_shift;
    char str[15] = { 0 };
    int score = 0;
    while (true) {
        get_xy_shift(rows, cols, &x_shift, &y_shift);
        clear();
        sprintf(str, "Score : %-3d", score);
        render(rows, cols, x_shift, y_shift, board, str, show_cell);
    }
    free(board);
}
