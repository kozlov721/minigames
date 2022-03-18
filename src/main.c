#include <curses.h>
#include <signal.h>
#include <stdlib.h>

#include "snake.h"

#define WIDTH  25
#define HEIGHT 25

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
    keypad(stdscr, TRUE);
}

int main() {
    init_screen();
    run_snake(WIDTH, HEIGHT);
    endwin();
}
