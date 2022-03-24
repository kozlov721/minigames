#include "minesweeper.h"
#include "tui.h"

#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FLAT(X) ((int *) X)

#define M_SURR 0x00ff
#define M_MINE 0x0100
#define M_FLAG 0x0200
#define M_HIDD 0x0400

static inline bool is_flag(int);
static inline bool is_mine(int);
static inline bool is_hidden(int);
static inline bool is_revealed(int);
static inline bool check_bounds(int i, int j, int r, int c);
static inline int get_number(int);

static void show_cell(int cell);
static void count_mines(int r, int c, int board[r][c]);
static void reveal_mines(int r, int c, int board[r][c]);
static int toggle_flag(int *cell);
static int reveal(int r, int c, int board[r][c], int i, int j);
static int reveal_one(int *cell);
static void floodfill(int r, int c, int board[r][c], int i, int j);
static bool is_solved(int r, int c, int board[r][c]);
static void init_board(int r, int c, int board[r][c], int mines);

bool is_flag(int cell) {
    return cell & M_FLAG;
}
bool is_mine(int cell) {
    return cell & M_MINE;
}
bool is_hidden(int cell) {
    return cell & M_HIDD;
}
int get_number(int cell) {
    return cell & M_SURR;
}

bool is_revealed(int cell) {
    return !is_hidden(cell);
}

bool check_bounds(int i, int j, int r, int c) {
    return i >= 0 && i < (int) r && j >= 0 && j < (int) c;
}

int count_mines_around(int i, int j, int r, int c, int board[r][c]) {
    int count = 0;
    for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
            if (di == 0 && dj == 0)
                continue;
            if (check_bounds(i + di, j + dj, r, c))
                count += is_mine(board[i + di][j + dj]);
        }
    }
    return count;
}

void count_mines(int rows, int cols, int board[rows][cols]) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j)
            board[i][j] |= count_mines_around(i, j, rows, cols, board);
    }
}

void show_cell(int cell) {
    if (is_flag(cell))
        printf(CYAN"🏴"RESET);
    else if (!is_revealed(cell))
        printf("🟫");
    else if (is_mine(cell))
        printf(RED"💣"RESET);
    else if (get_number(cell) == 0)
        printf("  ");
    else
        printf("%d ", get_number(cell));
}

int reveal(int rows, int cols, int board[rows][cols], int i, int j) {
    int n = reveal_one(&board[i][j]);
    if (n < 0)
        return n;
    if (n == 0)
        floodfill(rows, cols, board, i, j);
    return 0;
}

int reveal_one(int *cell) {
    if (is_flag(*cell) || is_revealed(*cell))
        return -2;
    *cell &= ~M_HIDD;
    if (is_mine(*cell))
        return -1;
    return get_number(*cell);
}

void floodfill(int rows, int cols, int board[rows][cols], int i, int j) {
    if (!check_bounds(i, j, rows, cols))
        return;
    if (get_number(board[i][j]))
        return;
    for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
            if ((di == 0) && (dj == 0))
                continue;
            int n_i = i + di;
            int n_j = j + dj;
            if (check_bounds(n_i, n_j, rows, cols) &&
                    is_hidden(board[n_i][n_j])) {
                if (reveal_one(&board[n_i][n_j]) != -1)
                    floodfill(rows, cols, board, n_i, n_j);
            }
        }
    }
}

int toggle_flag(int *cell) {
    if (is_flag(*cell)) {
        *cell &= ~M_FLAG;
        return -1;
    } else {
        *cell |= M_FLAG;
        return 1;
    }
}

bool is_solved(int rows, int cols, int board[rows][cols]) {
    for (int i = 0; i < rows * cols; ++i) {
        int cell = FLAT(board)[i];
        if (is_hidden(cell) && !is_mine(cell))
            return false;
    }
    return true;
}

void init_board(int rows, int cols, int board[rows][cols], int mines) {
    for (int i = 0; i < rows * cols; ++i) {
        FLAT(board)[i] = M_HIDD;
    }
    while (mines) {
        int i = rand() % rows;
        int j = rand() % cols;
        if (!is_mine(board[i][j])) {
            board[i][j] |= M_MINE | M_HIDD;
            --mines;
        }
    }
    count_mines(rows, cols, board);
}

void reveal_mines(int rows, int cols, int board[rows][cols]) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (is_mine(board[i][j]))
                reveal(rows, cols, board, i, j);
        }
    }
}

void run_minesweeper(int rows, int cols, int mines) {
    int(*board)[cols] = malloc(rows * cols * sizeof(int));
    init_board(rows, cols, board, mines);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    int x_shift, y_shift;
    get_xy_shift(rows, cols, &x_shift, &y_shift);
    MEVENT event;
    int ch = 0;
    char str[15] = { 0 };
    while (!is_solved(rows, cols, board) && ch != 'q') {
        sprintf(str, " Mines: %-3d", mines >= 0 ? mines : 0);
        clear();
        refresh();
        render(rows, cols, x_shift, y_shift, board, str, show_cell);
        if ((ch = getch()) == KEY_MOUSE && getmouse(&event) == OK) {
            int i = event.y - y_shift - 3;
            int j = (event.x - x_shift - 1) / 2;
            if (check_bounds(i, j, rows, cols)) {
                if (event.bstate & BUTTON1_CLICKED) {
                    if (reveal(rows, cols, board, i, j) == -1) {
                        reveal_mines(rows, cols, board);
                        break;
                    }
                } else if (event.bstate & BUTTON3_CLICKED)
                    mines -= toggle_flag(&board[i][j]);
            }
        }
    }
    clear();
    refresh();
    render(rows, cols, x_shift, y_shift, board, str, show_cell);
    char *end_str;
    if (!is_solved(rows, cols, board))
        end_str = "You've lost :(";
    else
        end_str = "Congratulations, you've won!";
    boxed_message(center_str(x_shift, cols, end_str), end_str);
    getchar();
}
