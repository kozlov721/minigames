#include <stdio.h>
#include <stdbool.h>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define EMPTY 0
#define HEAD  1
#define BODY  2
#define TAIL  3
#define FOOD  4

#define M_TYPE  0x0ff
#define M_UP    0x100
#define M_DOWN  0x200
#define M_LEFT  0x400
#define M_RIGHT 0x800

int modulo(int x, int n){
    return (x % n + n) % n;
}

bool is_empty(int cell) {
    return (cell & M_TYPE) == EMPTY;
}

bool is_food(int cell) {
    return (cell & M_TYPE) == FOOD;
}

void print_x_offset(int n) {
    int i;
    for (i = 0; i < n; ++i)
        printf(" ");
}

void show_cell(int cell) {
    if (is_empty(cell))
        printf("  ");
    else if (is_food(cell))
        printf("🍎");
    else
        printf("██");
}

void render(int width, int height, int board[width][height]) {
    int ww, wh;
    getmaxyx(stdscr, wh, ww);
    int x_offset = (ww - width) / 2;
    int y_offset = (wh - height) / 2;
    int y, x, i;
    for (i = 0; i < y_offset; ++i)
        printf("\n\r");
    print_x_offset(x_offset);
    printf("┏");
    for (i = 0; i < width * 2; ++i)
        printf("━");
    printf("┓\n\r");
    for (y = 0; y < height; ++y) {
        print_x_offset(x_offset);
        printf("┃");
        for (x = 0; x < width; ++x)
            show_cell(board[x][y]);
        printf("┃\n\r");
    }
    print_x_offset(x_offset);
    printf("┗");
    for (i = 0; i < width * 2; ++i)
        printf("━");
    printf("┛\n\r");
}

bool is_up(int dx, int dy) {
    return dx == 0 && dy == -1;
}
bool is_down(int dx, int dy) {
    return dx == 0 && dy == 1;
}
bool is_left(int dx, int dy) {
    return dx == -1 && dy == 0;
}
bool is_right(int dx, int dy) {
    return dx == 1 && dy == 0;
}

int run_snake(int width, int height) {

    int delay = 100000;

    int (*board)[height] = calloc(width * height, sizeof(int));
    int hx = width / 2;
    int hy = height / 2;
    int tx = hx;
    int ty = hy;
    int dx = 1;
    int dy = 0;
    int size = 1;

    board[hx][hy] = BODY | M_RIGHT;
    int fx = rand() % width;
    int fy = rand() % height;
    board[fx][fy] = (board[fx][fy] & ~M_TYPE) | FOOD;
    while (true) {
        clear();

        switch (getch()) {
        case 'w':
        case 'k':
        case KEY_UP:
            if (!is_down(dx, dy) || size == 1) {
                dx = 0; dy = -1;
                board[hx][hy] = BODY | M_UP;
            }
            break;
        case 's':
        case 'j':
        case KEY_DOWN:
            if (!is_up(dx, dy) || size == 1) {
                dx = 0; dy = 1;
                board[hx][hy] = BODY | M_DOWN;
            }
            break;
        case 'a':
        case 'h':
        case KEY_LEFT:
            if (!is_right(dx, dy) || size == 1) {
                dx = -1; dy = 0;
                board[hx][hy] = BODY | M_LEFT;
            }
            break;
        case 'd':
        case 'l':
        case KEY_RIGHT:
            if (!is_left(dx, dy) || size == 1) {
                dx = 1; dy = 0;
                board[hx][hy] = BODY | M_RIGHT;
            }
            break;
        case 'q':
            goto end;
        }

        int old_h = board[hx][hy];
        hx = modulo(hx + dx, width);
        hy = modulo(hy + dy, height);
        if (is_food(board[hx][hy])) {
            ++size;
            delay -= 200;
            fx = rand() % width;
            fy = rand() % height;
            board[fx][fy] = (board[fx][fy] & ~M_TYPE) | FOOD;
        } else {
            int prev_tail = board[tx][ty];
            if (!is_food(board[tx][ty]))
                board[tx][ty] = EMPTY;
            if (prev_tail & M_UP)
                ty = modulo(ty - 1, height);
            else if (prev_tail & M_DOWN)
                ty = modulo(ty + 1, height);
            else if (prev_tail & M_LEFT)
                tx = modulo(tx - 1, width);
            else if (prev_tail & M_RIGHT)
                tx = modulo(tx + 1, width);
            else
                board[0][0] = HEAD;
        }
        if (!is_empty(board[hx][hy]) && !is_food(board[hx][hy]))
            break;
        board[hx][hy] = old_h;
        render(width, height, board);
        usleep(delay);
    }
end:
    printf("Game over, score: %d\n", size);
    free(board);
    return 0;
}
