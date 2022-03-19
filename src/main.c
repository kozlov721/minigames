#include <curses.h>
#include <signal.h>
#include <stdlib.h>

#include "snake.h"
#include "minesweeper.h"

#define WIDTH  20
#define HEIGHT 20

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
    nodelay(stdscr, TRUE);
    /* nodelay(stdscr, FALSE); */
    clearok(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
}

int main() {
    init_screen();
    /* run_minesweeper(WIDTH, HEIGHT, 5); */
    run_snake(WIDTH, HEIGHT);
    endwin();
}
