#include <stdint.h>
#include <stdbool.h>

#ifndef HW02_MINESWEEPER_H
#define HW02_MINESWEEPER_H


#define MAX_SIZE 99
#define MIN_SIZE 3

#define GAME_CONTINUE 0
#define GAME_WON 1
#define GAME_DEFEAT 2

void run_minesweeper(int width, int height, int mines);

#endif //HW02_MINESWEEPER_H
