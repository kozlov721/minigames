#ifndef TUI_H
#define TUI_H

#include <wchar.h>

void shift_cursor(int);
void custom_boxed_message(int x_shift, char *str, char *chars[]);
void boxed_message(int x_shift, char *str);
int center_str(int x_shift, int cols, char *str);
void get_xy_shift(int rows, int cols, int *x_shift, int *y_shift);
void render_board(int rows,
        int cols,
        int x_o,
        int board[rows][cols],
        void (*show_cell)(int));

void render(int rows,
        int cols,
        int x_o,
        int y_o,
        int board[rows][cols],
        char *str,
        void (*show_cell)(int));

#endif /* TUI_H */
