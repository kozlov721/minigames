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
static void render(int r, int c, int x_o, int y_o, int board[r][c]);

static int count_prop(int i, int j, int r, int c, int[r][c], bool (*f)(int));

static void postprocess(int r, int c, int board[r][c]);
static bool is_solved(int r, int c, int board[r][c]);

static void reveal_mines(int r, int c, int board[r][c]);

static void toggle_flag(int *cell);
static int reveal_cell(int r, int c, int board[r][c], int i, int j);
static void reveal_floodfill(int r, int c, int board[r][c], int i, int j);
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

int count_prop(int i, int j, int r, int c, int board[r][c], bool (*f)(int)) {
    int count = 0;
    for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
            if (di == 0 && dj == 0)
                continue;
            if (check_bounds(i + di, j + dj, r, c))
                count += f(board[i + di][j + dj]);
        }
    }
    return count;
}

void postprocess(int rows, int cols, int board[rows][cols]) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j)
            board[i][j] |= count_prop(i, j, rows, cols, board, is_mine);
    }
}

void show_cell(int cell) {
    if (is_flag(cell))
        printf("🚩");
    else if (!is_revealed(cell))
        printf("⬛");
    else if (is_mine(cell))
        printf("💣");
    else if (get_number(cell) == 0)
        printf("  ");
    else
        printf("%d ", get_number(cell));
}

void render(int rows, int cols, int x_o, int y_o, int board[rows][cols]) {
    for (int i = 0; i < y_o; ++i)
        printf("\n\r");
    shift_cursor(x_o);
    printf("╭");
    for (int i = 0; i < cols * 2; ++i)
        printf("─");
    printf("╮\n\r");
    for (int i = 0; i < rows; ++i) {
        shift_cursor(x_o);
        printf("│");
        for (int j = 0; j < cols; ++j)
            show_cell(board[i][j]);
        printf("│\n\r");
    }
    shift_cursor(x_o);
    printf("╰");
    for (int i = 0; i < cols * 2; ++i)
        printf("─");
    printf("╯\n\r");
}

int reveal_single(int *cell) {
    if (is_flag(*cell) || is_revealed(*cell))
        return -1;
    *cell &= ~M_HIDD;
    if (is_mine(*cell))
        return 1;
    return 0;
}

void reveal_floodfill(int rows, int cols, int board[rows][cols], int i, int j) {
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
                if (reveal_single(&board[n_i][n_j]) != 1)
                    reveal_floodfill(rows, cols, board, n_i, n_j);
            }
        }
    }
}

int reveal_cell(int rows, int cols, int board[rows][cols], int i, int j) {
    if (!check_bounds(i, j, rows, cols))
        return -1;
    int n = reveal_single(&board[i][j]);
    if (n)
        return n;
    if (get_number(board[i][j]) == 0)
        reveal_floodfill(rows, cols, board, i, j);
    return 0;
}

void toggle_flag(int *cell) {
    if (is_flag(*cell))
        *cell &= ~M_FLAG;
    else
        *cell |= M_FLAG;
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
    postprocess(rows, cols, board);
}

void reveal_mines(int rows, int cols, int board[rows][cols]) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (is_mine(board[i][j]))
                reveal_cell(rows, cols, board, i, j);
        }
    }
}

void run_minesweeper(int rows, int cols, int mines) {
    int(*board)[rows] = malloc(rows * cols * sizeof(int));
    init_board(rows, cols, board, mines);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    int ww, wh;
    getmaxyx(stdscr, wh, ww);
    int x_offset = (ww - cols) / 2;
    int y_offset = (wh - rows) / 2;
    MEVENT event;
    bool lost = false;
    while (!is_solved(rows, cols, board)) {
        clear();
        refresh();
        render(rows, cols, x_offset, y_offset, board);
        switch (getch()) {
        case 'q':
            lost = true;
            goto end;
        case KEY_MOUSE:
            if (getmouse(&event) == OK) {
                int i = event.y - y_offset - 1;
                int j = (event.x - x_offset - 1) / 2;
                if (!check_bounds(i, j, rows, cols))
                    break;
                if (event.bstate & BUTTON1_CLICKED) {
                    if (reveal_cell(rows, cols, board, i, j) == 1) {
                        lost = true;
                        reveal_mines(rows, cols, board);
                        goto end;
                    }
                } else if (event.bstate & BUTTON3_CLICKED)
                    toggle_flag(&board[i][j]);
            }
            break;
        }
        usleep(100000);
    }
end:
    clear();
    refresh();
    render(rows, cols, x_offset, y_offset, board);
    if (lost)
        boxed_message(x_offset, "You've lost :(");
    else
        boxed_message(x_offset, "Congratulations, you've won!");
    getchar();
}
