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
    signal(SIGTERM, quit);
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, FALSE);
    clearok(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
}

void render_options(int selected, char *title, int n, char* options[n]) {
    int cols = getmaxx(stdscr);
    int rows = getmaxy(stdscr);
    move(rows / 4, 0);
    wrefresh(stdscr);
    char *chars[] = {"╭", "─", "╮", "│", "╰", "╯"};
    char *sel_chars[] = {"╔", "═", "╗", "║", "╚", "╝"};
    custom_boxed_message((cols - strlen(title)) / 2, title, chars);
    for (int i = 0; i < n; ++i)
        custom_boxed_message((cols - strlen(options[i])) / 2,
                options[i], i == selected ? sel_chars : chars);

}

int modulo(int x, int n) {
    return (x % n + n) % n;
}

void show_options(int *game, int *rows, int *cols) {
    char *games[] = {"Minesweeper", "   Snake   "};
    int sizes_values[] = {14, 22, 36};
    char *sizes[] = {"  14x14  ", "  22x22  ", "  36x36  "};
    int selected_size = 0;
    bool game_selected = false;
    while (true) {
        clear();
        if (!game_selected)
            render_options(*game, "Which game do you wanna play?", 2, games);
        else
            render_options(selected_size, "Select the size of the game", 3, sizes);
        switch(getch()) {
        case KEY_DOWN:
        case 'j':
        case 's':
            if (!game_selected)
                *game = modulo(*game + 1, 2);
            else
                selected_size = modulo(selected_size + 1, 3);
            break;
        case KEY_UP:
        case 'k':
        case 'w':
            if (!game_selected)
                *game = modulo(*game - 1, 2);
            else
                selected_size = modulo(selected_size - 1, 3);
            break;
        case KEY_RIGHT:
        case 'l':
        case 'd':
        case '\n':
            if (!game_selected)
                game_selected = true;
            else {
                *rows = sizes_values[selected_size];
                *cols = sizes_values[selected_size];
                return;
            }
            break;
        case KEY_LEFT:
        case 'h':
        case 'a':
        case 'q':
            if (!game_selected)
                quit(0);
            game_selected = false;
        }
        usleep(10000);
    }
}

int main() {
    srand(time(NULL));
    init_screen();
    int game, rows, cols;
    show_options(&game, &rows, &cols);

    clear();
    refresh();
    erase();

    if (game != MINESWEEPER)
        nodelay(stdscr, TRUE);

    if (game == SNAKE)
        run_snake(rows, cols);
    else
        run_minesweeper(rows, cols, rows * cols / 6);
    endwin();
}
