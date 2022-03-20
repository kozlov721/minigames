#include <curses.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "snake.h"
#include "tui.h"
#include "minesweeper.h"

#define ROWS 16
#define COLS 16

#define MINESWEEPER 0
#define SNAKE 1

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
    nodelay(stdscr, FALSE);
    clearok(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
}

void render_options(int selected) {
    int cols = getmaxx(stdscr);
    int rows = getmaxy(stdscr);
    move(rows / 4, 0);
    wrefresh(stdscr);
    char *title = "Which game do you wanna play?";
    char *options[] = {"Minesweeper", "   Snake   "};
    char *chars[] = {"╭", "─", "╮", "│", "╰", "╯"};
    char *sel_chars[] = {"╔", "═", "╗", "║", "╚", "╝"};
    custom_boxed_message((cols - strlen(title)) / 2, title, chars);
    for (int i = 0; i < 2; ++i)
        custom_boxed_message((cols - strlen(options[i])) / 2,
                options[i], i == selected ? sel_chars : chars);

}

int modulo(int x, int n) {
    return (x % n + n) % n;
}

int show_options() {
    int n = 2;
    int selected = 0;
    while (true) {
        clear();
        render_options(selected);
        switch(getch()) {
        case KEY_DOWN:
        case 'j':
        case 's':
            selected = modulo(selected - 1, n);
            break;
        case KEY_UP:
        case 'k':
        case 'w':
            selected = modulo(selected + 1, n);
            break;
        case KEY_RIGHT:
        case 'l':
        case 'd':
        case '\n':
            return selected;
        case 'q':
            quit(0);
        }
        usleep(10000);
    }
}

int main() {
    srand(time(NULL));
    init_screen();
    int game = show_options();

    clear();
    refresh();
    erase();

    if (game != MINESWEEPER)
        nodelay(stdscr, TRUE);

    if (game == SNAKE)
        run_snake(ROWS, COLS);
    else
        run_minesweeper(ROWS, COLS, ROWS * COLS / 15);
    endwin();
}
