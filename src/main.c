#include <curses.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "snake.h"
#include "minesweeper.h"

#define WIDTH  10
#define HEIGHT 10

#define SNAKE

void quit(int sig) {
    endwin();
    exit(sig);
}

void init_screen() {
    signal(SIGINT, quit);
    signal(SIGSEGV, quit);
    initscr();
    cbreak();
    noecho();
#ifdef SNAKE
    nodelay(stdscr, TRUE);
#else
    nodelay(stdscr, FALSE);
#endif
    clearok(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
}

int main() {
    srand(time(NULL));
    init_screen();
#ifdef SNAKE
    run_snake(WIDTH, HEIGHT);
#else
    run_minesweeper(WIDTH, HEIGHT, WIDTH * HEIGHT / 15); /* 5 */
#endif
    endwin();
}
