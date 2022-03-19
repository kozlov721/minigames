#include "tui.h"

#include <stdio.h>
#include <stdbool.h>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define EMPTY 0x00
#define BODY  0x01
#define FOOD  0x02

#define M_TYPE  0x0f
#define M_DIR   0xf0
#define M_PDIR  0xf00
#define M_UP    0x10
#define M_DOWN  0x20
#define M_LEFT  0x40
#define M_RIGHT 0x80

static inline int modulo(int x, int n);
static inline bool is_empty(int cell);
static inline bool is_food(int cell);
static inline int get_dir(int cell);

static inline bool is_up(int dx, int dy);
static inline bool is_down(int dx, int dy);
static inline bool is_left(int dx, int dy);
static inline bool is_right(int dx, int dy);

static void show_cell(int cell);

int modulo(int x, int n) {
    return (x % n + n) % n;
}

bool is_empty(int cell) {
    return (cell & M_TYPE) == EMPTY;
}

bool is_body(int cell) {
    return (cell & M_TYPE) == BODY;
}

bool is_food(int cell) {
    return (cell & M_TYPE) == FOOD;
}

int get_dir(int cell) {
    return cell & M_DIR;
}

int get_prev_dir(int cell) {
    return (cell & M_PDIR) >> 4;
}

void show_cell(int cell) {
    int prev_dir = get_prev_dir(cell);
    int dir = get_dir(cell);
    if (is_empty(cell))
        printf("  ");
    else if (is_food(cell))
        printf("🍎");
    else if (prev_dir == M_RIGHT && dir == M_UP)
        printf("╝ ");
    else if (prev_dir == M_LEFT  && dir == M_UP)
        printf("╚═");
    else if (prev_dir == M_RIGHT && dir == M_DOWN)
        printf("╗ ");
    else if (prev_dir == M_LEFT  && dir == M_DOWN)
        printf("╔═");
    else if (prev_dir == M_UP    && dir == M_LEFT)
        printf("╗ ");
    else if (prev_dir == M_DOWN  && dir == M_LEFT)
        printf("╝ ");
    else if (prev_dir == M_UP    && dir == M_RIGHT)
        printf("╔═");
    else if (prev_dir == M_DOWN  && dir == M_RIGHT)
        printf("╚═");
    else if (dir == M_DOWN || dir == M_UP)
        printf("║ ");
    else
        printf("══");
}

bool    is_up(int d_i, int d_j) { return d_i == -1 && d_j ==  0; }
bool  is_down(int d_i, int d_j) { return d_i ==  1 && d_j ==  0; }
bool  is_left(int d_i, int d_j) { return d_i ==  0 && d_j == -1; }
bool is_right(int d_i, int d_j) { return d_i ==  0 && d_j ==  1; }

int run_snake(int rows, int cols) {

    int delay = 100000;

    int (*board)[cols] = calloc(rows * cols, sizeof(int));
    int h_i = rows / 2;
    int h_j = cols / 2;
    int t_i = h_i;
    int t_j = h_j;
    int d_i = 0;
    int d_j = 1;
    int size = 1;

    board[h_i][h_j] = BODY | M_RIGHT;
    int f_i = rand() % rows;
    int f_j = rand() % cols;
    if (f_i == h_i && f_j == h_j)
        --f_i;
    int x_shift;
    int y_shift;
    board[f_i][f_j] = (board[f_i][f_j] & ~M_TYPE) | FOOD;
    char str[15] = {0};
    while (true) {
        clear();
        sprintf(str, " Score: %-3d", size);
        x_shift = (getmaxx(stdscr) - cols) / 2;
        y_shift = (getmaxy(stdscr) - rows) / 2;
        int prev_dir = get_dir(board[h_i][h_j]) << 4;
        switch (getch()) {
        case 'w':
        case 'k':
        case KEY_UP:
            if (!is_down(d_i, d_j) || size == 1) {
                d_i = -1; d_j = 0;
                board[h_i][h_j] = BODY | M_UP | prev_dir;
            }
            break;
        case 's':
        case 'j':
        case KEY_DOWN:
            if (!is_up(d_i, d_j) || size == 1) {
                d_i = 1; d_j = 0;
                board[h_i][h_j] = BODY | M_DOWN | prev_dir;
            }
            break;
        case 'a':
        case 'h':
        case KEY_LEFT:
            if (!is_right(d_i, d_j) || size == 1) {
                d_i = 0; d_j = -1;
                board[h_i][h_j] = BODY | M_LEFT | prev_dir;
            }
            break;
        case 'd':
        case 'l':
        case KEY_RIGHT:
            if (!is_left(d_i, d_j) || size == 1) {
                d_i = 0; d_j = 1;
                board[h_i][h_j] = BODY | M_RIGHT | prev_dir;
            }
            break;
        case 'q':
            goto end;
        default:
            board[h_i][h_j] &= ~M_PDIR;
        }

        int old_h = board[h_i][h_j];
        h_i = modulo(h_i + d_i, rows);
        h_j = modulo(h_j + d_j, cols);
        if (is_food(board[h_i][h_j])) {
            ++size;
            delay -= 300;
            f_i = rand() % rows;
            f_j = rand() % cols;
            if (f_i == h_i && f_j == h_j)
                f_i = modulo(f_i - 1, rows);
            board[f_i][f_j] = (board[f_i][f_j] & ~M_TYPE) | FOOD;
        } else {
            int prev_tail = board[t_i][t_j];
            if (!is_food(board[t_i][t_j]))
                board[t_i][t_j] = EMPTY;
            if (prev_tail & M_UP)
                t_i = modulo(t_i - 1, rows);
            else if (prev_tail & M_DOWN)
                t_i = modulo(t_i + 1, rows);
            else if (prev_tail & M_LEFT)
                t_j = modulo(t_j - 1, cols);
            else if (prev_tail & M_RIGHT)
                t_j = modulo(t_j + 1, cols);
        }
        if (is_body(board[h_i][h_j]))
            break;
        board[h_i][h_j] = old_h & 0x0ff;
        render(rows, cols, x_shift, y_shift, board, str, show_cell);
        usleep(delay);
    }
end:
    render(rows, cols, x_shift, y_shift, board, str, show_cell);
    char end_str[30] = {0};
    sprintf(end_str, "Game over, score: %d", size);
    boxed_message(center_str(x_shift, cols, end_str), end_str);
    getchar();
    free(board);
    return 0;
}
