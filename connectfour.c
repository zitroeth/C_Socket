#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define BOARD_SIZE_VERT 6
#define BOARD_SIZE_HORIZ 7
#define PLAYER_NUM 1
#define ENEMY_NUM 2
typedef struct {
    int north;
    int east;
    int south;
    int west;
} indexBound;

void display_board(int board[][BOARD_SIZE_HORIZ]);
int scan_num();
int drop_disc(int board[][BOARD_SIZE_HORIZ], int drop_col, int diskType);
indexBound set_indexbound(int row, int col);
int check_gamewin(int board[][BOARD_SIZE_HORIZ]);
int move_algo(int board[][BOARD_SIZE_HORIZ]);
int simulate(int board[][BOARD_SIZE_HORIZ], int row, int col);
int simulate_twice(int board[][BOARD_SIZE_HORIZ], int drop_col);
int simulate_doubleatk(int board[][BOARD_SIZE_HORIZ], int drop_col, int player);
int simulate_doubleatkgap(int board[][BOARD_SIZE_HORIZ], int drop_col, int player);

int main() {
    int board[BOARD_SIZE_VERT][BOARD_SIZE_HORIZ] = {
        /*
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0}
        */
        0};
    int inputNum, win, algo;
    srand(time(NULL));

    while (1) {

        display_board(board);

        while (1) {
            inputNum = scan_num(BOARD_SIZE_HORIZ);
            if (drop_disc(board, inputNum, PLAYER_NUM))
                break;
        }

        while (1) {
            algo = move_algo(board);
            if (drop_disc(board, algo, ENEMY_NUM))
                break;
        }

        win = check_gamewin(board);
        if (win > 0)
            break;
        system("cls");
    };

    system("cls");

    if (win == 1) {
        display_board(board);
        printf("Player %d win!", 1);
    } else if (win == 2) {
        display_board(board);
        printf("Player %d win!", 2);
    } else if (win == 3) {
        display_board(board);
        printf("draw");
    }

    scan_num(BOARD_SIZE_HORIZ);
    return 0;
}

void display_board(int board[][BOARD_SIZE_HORIZ]) {
    int i, j;
    printf(" ");
    for (j = 0; j < BOARD_SIZE_HORIZ; j++)
        printf("  %d ", j);
    printf("\n");
    for (i = 0; i < BOARD_SIZE_VERT; i++) {
        printf(" +");
        for (j = 0; j < BOARD_SIZE_HORIZ; j++)
            printf("---+");
        printf("\n");
        printf(" |", i);
        for (j = 0; j < BOARD_SIZE_HORIZ; j++) {
            switch (board[i][j]) {
            case 1:
                printf(" X |");
                break;
            case 2:
                printf(" 0 |");
                break;
            default:
                printf("   |");
            }
        }
        printf("\n");
    }

    printf(" +");
    for (j = 0; j < BOARD_SIZE_HORIZ; j++)
        printf("---+");
    printf("\n ");
    for (j = 0; j < BOARD_SIZE_HORIZ; j++)
        printf("  %d ", j);
    printf("\n");
}

int scan_num(int limit) {
    int num, c, result;
    do {
        printf("\nEnter a number (0-%d): ", limit - 1);
        result = scanf("%d", &num);
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        printf("\nNUM = %d\n", num);
    } while (result == 0 || (num > (limit - 1)) || num < 0);
    return num;
}

int drop_disc(int board[][BOARD_SIZE_HORIZ], int drop_col, int diskType) {
    int valid = 0, i;

    for (i = (BOARD_SIZE_VERT - 1); i >= 0; i--) {
        if (board[i][drop_col] == 0) {
            valid = 1;
            break;
        } else
            valid = 0;
    }
    if (valid) {
        for (i = (BOARD_SIZE_VERT - 1); i >= 0; i--) {
            if (board[i][drop_col] == 0) {
                board[i][drop_col] = diskType;
                return valid;
            }
        }
    } else {
        return valid;
    }
}

indexBound set_indexbound(int row, int col) {
    indexBound newIB;

    ((col + 4) <= BOARD_SIZE_HORIZ) ? (newIB.east = 1) : (newIB.east = 0);  // x-axis right
    (col >= 3) ? (newIB.west = 1) : (newIB.west = 0);                       // x-axis left
    ((row + 4) <= BOARD_SIZE_VERT) ? (newIB.south = 1) : (newIB.south = 0); // y-axis down
    (row >= 3) ? (newIB.north = 1) : (newIB.north = 0);                     // y-axis up

    return newIB;
}

int check_gamewin(int board[][BOARD_SIZE_HORIZ]) {
    int row, col;
    indexBound currIB;
    for (int i = 1; i <= 2; i++) {
        for (row = 0; row < BOARD_SIZE_VERT; row++) {
            for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
                currIB = set_indexbound(row, col);

                if (currIB.east == 1) {
                    if (
                        board[row][col] == i &&
                        board[row][col + 1] == i &&
                        board[row][col + 2] == i &&
                        board[row][col + 3] == i) {
                        printf("\nWin by move->right\n");
                        return i;
                    }
                }

                if (currIB.south == 1) {
                    if (
                        board[row][col] == i &&
                        board[row + 1][col] == i &&
                        board[row + 2][col] == i &&
                        board[row + 3][col] == i) {
                        printf("\nWin by move->down\n");
                        return i;
                    }
                }

                if (currIB.east == 1 && currIB.south == 1) {
                    if (
                        board[row][col] == i &&
                        board[row + 1][col + 1] == i &&
                        board[row + 2][col + 2] == i &&
                        board[row + 3][col + 3] == i) {
                        printf("\nWin by move->down right\n");
                        return i;
                    }
                }

                if (currIB.west == 1 && currIB.south == 1) {
                    if (
                        board[row][col] == i &&
                        board[row + 1][col - 1] == i &&
                        board[row + 2][col - 2] == i &&
                        board[row + 3][col - 3] == i) {
                        printf("\nWin by move->down left\n");
                        return i;
                    }
                }
            }
        }
    }

    for (int i = 0; i < BOARD_SIZE_HORIZ; i++) {
        if (board[0][i] == 0)
            break;
        if (i == (BOARD_SIZE_HORIZ - 1))
            return 3;
    }
    return 0;
}

int move_algo(int board[][BOARD_SIZE_HORIZ]) {
    int row, col;
    indexBound currIB;

    for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
        if (simulate(board, col, ENEMY_NUM))
            return col;
    }

    for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
        if (simulate(board, col, PLAYER_NUM))
            return col;
    }

    for (col = 0; col < 4; col++) {
        if (simulate_doubleatk(board, col, PLAYER_NUM))
            return (rand() % 2) ? col : col + 3;
    }
    
    for (col = 2; col < 5; col++) {
        if (simulate_doubleatkgap(board, col, PLAYER_NUM))
            return col;
    }

   for (col = 0; col < 4; col++) {
        if (simulate_doubleatk(board, col, ENEMY_NUM))
            return (rand() % 2) ? col : col + 3;
    }
    
    for (col = 2; col < 5; col++) {
        if (simulate_doubleatkgap(board, col, ENEMY_NUM))
            return col;
    }

    int drop_col;
    for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
        if (simulate_twice(board, col)) {
            while (1) {
                drop_col = rand() % BOARD_SIZE_HORIZ;
                if (simulate_twice(board, drop_col)) {
                    return drop_col;
                }
            }
        }
    }

    drop_col = rand() % BOARD_SIZE_HORIZ;
    return drop_col;
}

int simulate(int board[][BOARD_SIZE_HORIZ], int drop_col, int player) {
    int boardcpy[BOARD_SIZE_VERT][BOARD_SIZE_HORIZ] = {0};
    memcpy(boardcpy, board, sizeof(int) * BOARD_SIZE_HORIZ * BOARD_SIZE_VERT);
    if (drop_disc(boardcpy, drop_col, player))
        if (check_gamewin(boardcpy) == player)
            return 1;
    return 0;
}

int simulate_twice(int board[][BOARD_SIZE_HORIZ], int drop_col) {
    int boardcpy[BOARD_SIZE_VERT][BOARD_SIZE_HORIZ] = {0};
    memcpy(boardcpy, board, sizeof(int) * BOARD_SIZE_HORIZ * BOARD_SIZE_VERT);

    if (drop_disc(boardcpy, drop_col, ENEMY_NUM)) {
        if (drop_disc(boardcpy, drop_col, PLAYER_NUM)) {
            if (check_gamewin(boardcpy) == PLAYER_NUM)
                return 0;
            else if (check_gamewin(boardcpy) != PLAYER_NUM)
                return 1;
        } else
            return 1;
    }
    return 0;
}

int simulate_doubleatk(int board[][BOARD_SIZE_HORIZ], int drop_col, int player) {
    int boardcpy[BOARD_SIZE_VERT][BOARD_SIZE_HORIZ] = {0};
    int counter = 0;
    memcpy(boardcpy, board, sizeof(int) * BOARD_SIZE_HORIZ * BOARD_SIZE_VERT);

    if (drop_col < 4) {
        if (drop_disc(boardcpy, drop_col, player))
            if (drop_disc(boardcpy, drop_col + 3, player))
                if (check_gamewin(boardcpy) == player)
                    return 1;
    }
    return 0;
}

int simulate_doubleatkgap(int board[][BOARD_SIZE_HORIZ], int drop_col, int player) {
    int boardcpy[BOARD_SIZE_VERT][BOARD_SIZE_HORIZ] = {0};
    int counter = 0;
    memcpy(boardcpy, board, sizeof(int) * BOARD_SIZE_HORIZ * BOARD_SIZE_VERT);

    if (drop_col >= 2 && drop_col <= 4) {
        if (drop_disc(boardcpy, drop_col, player))
            if (drop_disc(boardcpy, drop_col -2, player))
                if (check_gamewin(boardcpy) == player)
                    if (drop_disc(boardcpy, drop_col + 2, player))
                        if (check_gamewin(boardcpy) == player)
                            return 1;
    }
    return 0;
}