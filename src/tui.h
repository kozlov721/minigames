#ifndef TUI_H
#define TUI_H

#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"
#define RESET "\033[0m"
#define YELLOW "\033[0;33m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define PURPLE "\033[0;35m"
#define BLACK "\033[0;30m"

#define REDB "\033[0;41m"
#define BLUEB "\033[0;44m"
#define GREENB "\033[0;42m"
#define YELLOWB "\033[0;43m"
#define CYANB "\033[0;46m"
#define WHITEB "\033[0;47m"
#define PURPLEB "\033[0;45m"

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
