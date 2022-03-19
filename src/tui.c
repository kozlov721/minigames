#include "tui.h"

#include <stdio.h>
#include <string.h>

void shift_cursor(int n) {
    for (int i = 0; i < n; ++i)
        printf(" ");
}

void boxed_message(int x_shift, char *message) {
    shift_cursor(x_shift);
    printf("╭");
    for (size_t i = 0; i < strlen(message) + 4; ++i)
        printf("─");
    printf("╮\n\r");
    shift_cursor(x_shift);
    printf("│  %s  │\n\r", message);

    shift_cursor(x_shift);
    printf("╰");
    for (size_t i = 0; i < strlen(message) + 4; ++i)
        printf("─");
    printf("╯\n\r");
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

    /* sprintf(str, " Mines: %-3d", mines >= 0 ? mines : 0); */
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
