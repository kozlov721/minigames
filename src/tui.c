#include "tui.h"

#include <stdio.h>
#include <string.h>
#include <curses.h>

void shift_cursor(int n) {
    for (int i = 0; i < n; ++i)
        printf(" ");
}

void boxed_message(int x_shift, char *message) {
    char *chars[] = {"╭", "─", "╮", "│", "╰", "╯"};
    custom_boxed_message(x_shift, message, chars);
}

void get_xy_shift(int rows, int cols, int *x_shift, int *y_shift) {
    *x_shift = (getmaxx(stdscr) - cols * 2) / 2;
    *y_shift = (getmaxy(stdscr) - rows - 4) / 2;
}

void custom_boxed_message(int x_shift, char *message, char *chars[]) {
    shift_cursor(x_shift);

    printf("%s", chars[0]);
    for (size_t i = 0; i < strlen(message) + 4; ++i)
        printf("%s", chars[1]);
    printf("%s\n\r", chars[2]);
    shift_cursor(x_shift);
    printf("%s  %s  %s\n\r", chars[3], message, chars[3]);

    shift_cursor(x_shift);
    printf("%s", chars[4]);
    for (size_t i = 0; i < strlen(message) + 4; ++i)
        printf("%s", chars[1]);
    printf("%s\n\r", chars[5]);
}

int center_str(int x_shift, int cols, char *str) {
    return x_shift + cols - strlen(str) / 2 - 2;
}

void render_board(int rows,
        int cols,
        int x_o,
        int board[rows][cols],
        void (*show_cell)(int)) {
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

void render(int rows,
        int cols,
        int x_o,
        int y_o,
        int board[rows][cols],
        char *str,
        void (*show_cell)(int)) {

    for (int i = 0; i < y_o; ++i)
        printf("\n\r");

    int score_x_shift = center_str(x_o, cols, str);
    shift_cursor(score_x_shift);
    printf("╭");
    for (int i = 0; i < (int) strlen(str) + 2; ++i)
        printf("─");
    printf("╮\n\r");
    shift_cursor(score_x_shift);
    printf("│ %s │\n\r", str);

    shift_cursor(x_o);
    printf("╭");
    for (int i = 0; i < cols * 2; ++i) {
        if (i + x_o == score_x_shift - 1 ||
                i + x_o == score_x_shift + (int) strlen(str) + 2)
            printf("┴");
        else
            printf("─");
    }
    render_board(rows, cols, x_o, board, show_cell);
}
