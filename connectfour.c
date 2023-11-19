#include <stdio.h>
#include <stdlib.h>
#define BOARD_SIZE_VERT 6
#define BOARD_SIZE_HORIZ 7
#define PLAYER_NUM 1
#define ENEMY_NUM 2
typedef struct {
    int x_bound;
    int y_bound;
    int nx_bound;
    int ny_bound;
} indexBound;

void display_board(int board[][BOARD_SIZE_HORIZ]);
int scan_num();
int drop_disc(int board[][BOARD_SIZE_HORIZ], int drop_col, int diskType);
indexBound set_indexbound(int row, int col);
int check_gamewin(int board[][BOARD_SIZE_HORIZ]);
int move_algo(int board[][BOARD_SIZE_HORIZ]);

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

    while (1) {
        
        display_board(board);
        inputNum = scan_num(BOARD_SIZE_HORIZ);
        drop_disc(board, inputNum, PLAYER_NUM);
        algo = move_algo(board);
        printf("\nalgo %d\n", algo);
        drop_disc(board, algo, ENEMY_NUM);
        win = check_gamewin(board);
        if (win > 0)
            break;
        //system("cls");
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
    } else
        return valid;
}

indexBound set_indexbound(int row, int col) {
    indexBound newIB;

    ((col + 4) <= BOARD_SIZE_HORIZ) ? (newIB.x_bound = 1) : (newIB.x_bound = 0);  // x-axis right
    (col >= 3) ? (newIB.nx_bound = 1) : (newIB.nx_bound = 0);                      // x-axis left
    ((row + 4) <= BOARD_SIZE_VERT) ? (newIB.ny_bound = 1) : (newIB.ny_bound = 0); // y-axis down
    (row >= 3) ? (newIB.y_bound = 1) : (newIB.y_bound = 0);                       // y-axis up

    return newIB;
}

int check_gamewin(int board[][BOARD_SIZE_HORIZ]) {
    int row, col;
    indexBound currIB;

    for (row = 0; row < BOARD_SIZE_VERT; row++) {
        for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
            currIB = set_indexbound(row, col);

            if (currIB.x_bound == 1) {
                if (
                    board[row][col] == 1 &&
                    board[row][col + 1] == 1 &&
                    board[row][col + 2] == 1 &&
                    board[row][col + 3] == 1) {
                    printf("\nWin by move->right\n");
                    return 1;
                } else if (
                    board[row][col] == 2 &&
                    board[row][col + 1] == 2 &&
                    board[row][col + 2] == 2 &&
                    board[row][col + 3] == 2) {
                    printf("\nWin by move->right\n");
                    return 2;
                }
            }

            if (currIB.ny_bound == 1) {
                if (
                    board[row][col] == 1 &&
                    board[row + 1][col] == 1 &&
                    board[row + 2][col] == 1 &&
                    board[row + 3][col] == 1) {
                    printf("\nWin by move->down\n");
                    return 1;
                } else if (
                    board[row][col] == 2 &&
                    board[row + 1][col] == 2 &&
                    board[row + 2][col] == 2 &&
                    board[row + 3][col] == 2) {
                    printf("\nWin by move->down\n");
                    return 2;
                }
            }

            if (currIB.x_bound == 1 && currIB.ny_bound == 1) {
                if (
                    board[row][col] == 1 &&
                    board[row + 1][col + 1] == 1 &&
                    board[row + 2][col + 2] == 1 &&
                    board[row + 3][col + 3] == 1) {
                    printf("\nWin by move->down right\n");
                    return 1;
                } else if (
                    board[row][col] == 2 &&
                    board[row + 1][col + 1] == 2 &&
                    board[row + 2][col + 2] == 2 &&
                    board[row + 3][col + 3] == 2) {
                    printf("\nWin by move->down right\n");
                    return 2;
                }
            }

            if (currIB.nx_bound == 1 && currIB.ny_bound == 1) {
                if (
                    board[row][col] == 1 &&
                    board[row + 1][col - 1] == 1 &&
                    board[row + 2][col - 2] == 1 &&
                    board[row + 3][col - 3] == 1) {
                    printf("\nWin by move->down left\n");
                    return 1;
                } else if (
                    board[row][col] == 2 &&
                    board[row + 1][col - 1] == 2 &&
                    board[row + 2][col - 2] == 2 &&
                    board[row + 3][col - 3] == 2) {
                    printf("\nWin by move->down left\n");
                    return 2;
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
    
    for (row = 0; row < BOARD_SIZE_VERT; row++) {
        for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
            currIB = set_indexbound(row, col);

            if (currIB.x_bound) {
                if (board[row][col] == ENEMY_NUM &&
                    board[row][col + 1] == ENEMY_NUM &&
                    board[row][col + 2] == ENEMY_NUM &&
                    board[row][col + 3] == 0) {
                    if (row == (BOARD_SIZE_VERT - 1)) {
                        return col + 3;
                    } else if (row < (BOARD_SIZE_VERT - 1)) {
                        if (board[row + 1][col + 3] != 0) {
                            return col + 3;
                        }
                    }
                }
            }

            if (currIB.nx_bound) {
                if (board[row][col] == ENEMY_NUM &&
                    board[row][col - 1] == ENEMY_NUM &&
                    board[row][col - 2] == ENEMY_NUM &&
                    board[row][col - 3] == 0) {
                    if (row == (BOARD_SIZE_VERT - 1)) {
                        return col - 3;
                    } else if (row < (BOARD_SIZE_VERT - 1)) {
                        if (board[row + 1][col - 3] != 0) {
                            return col - 3;
                        }
                    }
                }
            }

            if (currIB.x_bound == 1 && currIB.y_bound == 1) {
                if (board[row][col] == ENEMY_NUM &&
                    board[row - 1][col + 1] == ENEMY_NUM &&
                    board[row - 2][col + 2] == ENEMY_NUM &&
                    board[row - 3][col + 3] == 0 &&
                    board[row - 2][col + 3] != 0)
                    return col + 3;
            }

            if (currIB.x_bound == 1 && currIB.ny_bound == 1) {
                if (board[row][col] == ENEMY_NUM &&
                    board[row + 1][col + 1] == ENEMY_NUM &&
                    board[row + 2][col + 2] == ENEMY_NUM &&
                    board[row + 3][col + 3] == 0 &&
                    (row == (BOARD_SIZE_VERT - 1) || board[row + 4][col + 3] != 0))
                    return col + 3;
            }

            if (currIB.nx_bound == 1 && currIB.ny_bound == 1) {
                if (board[row][col] == ENEMY_NUM &&
                    board[row + 1][col - 1] == ENEMY_NUM &&
                    board[row + 2][col - 2] == ENEMY_NUM &&
                    board[row + 3][col - 3] == 0 &&
                    (row == (BOARD_SIZE_VERT - 1) || board[row + 4][col - 3] != 0))
                    return col - 3;
            }

            if (currIB.nx_bound == 1 && currIB.y_bound == 1) {
                if (board[row][col] == ENEMY_NUM &&
                    board[row - 1][col - 1] == ENEMY_NUM &&
                    board[row - 2][col - 2] == ENEMY_NUM &&
                    board[row - 3][col - 3] == 0 &&
                    board[row - 2][col - 3] != 0)
                    return col - 3;
            }

            if (currIB.ny_bound == 1) {
                if (board[row][col] == 0 &&
                    board[row + 1][col] == ENEMY_NUM &&
                    board[row + 2][col] == ENEMY_NUM &&
                    board[row + 3][col] == ENEMY_NUM)
                    return col;
            }
        }
    }
    
    for (row = 0; row < BOARD_SIZE_VERT; row++) {
        for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
            currIB = set_indexbound(row, col);

            if (currIB.x_bound) {
                if (board[row][col] == PLAYER_NUM &&
                    board[row][col + 1] == PLAYER_NUM &&
                    board[row][col + 2] == PLAYER_NUM &&
                    board[row][col + 3] == 0) {
                    if (row == (BOARD_SIZE_VERT - 1)) {
                        return col + 3;
                    } else if (row < (BOARD_SIZE_VERT - 1)) {
                        if (board[row + 1][col + 3] != 0) {
                            return col + 3;
                        }
                    }
                }
            }

            if (currIB.nx_bound) {
                if (board[row][col] == PLAYER_NUM &&
                    board[row][col - 1] == PLAYER_NUM &&
                    board[row][col - 2] == PLAYER_NUM &&
                    board[row][col - 3] == 0) {
                    if (row == (BOARD_SIZE_VERT - 1)) {
                        return col - 3;
                    } else if (row < (BOARD_SIZE_VERT - 1)) {
                        if (board[row + 1][col - 3] != 0) {
                            return col - 3;
                        }
                    }
                }
            }

            if (currIB.x_bound == 1 && currIB.y_bound == 1) {
                if (board[row][col] == PLAYER_NUM &&
                    board[row - 1][col + 1] == PLAYER_NUM &&
                    board[row - 2][col + 2] == PLAYER_NUM &&
                    board[row - 3][col + 3] == 0 &&
                    board[row - 2][col + 3] != 0)
                    return col + 3;
            }

            if (currIB.x_bound == 1 && currIB.ny_bound == 1) {
                if (board[row][col] == PLAYER_NUM &&
                    board[row + 1][col + 1] == PLAYER_NUM &&
                    board[row + 2][col + 2] == PLAYER_NUM &&
                    board[row + 3][col + 3] == 0 &&
                    (row == (BOARD_SIZE_VERT - 1) || board[row + 4][col + 3] != 0))
                    return col + 3;
            }

            if (currIB.nx_bound == 1 && currIB.ny_bound == 1) {
                if (board[row][col] == PLAYER_NUM &&
                    board[row + 1][col - 1] == PLAYER_NUM &&
                    board[row + 2][col - 2] == PLAYER_NUM &&
                    board[row + 3][col - 3] == 0 &&
                    (row == (BOARD_SIZE_VERT - 1) || board[row + 4][col - 3] != 0))
                    return col - 3;
            }

            if (currIB.nx_bound == 1 && currIB.y_bound == 1) {
                if (board[row][col] == PLAYER_NUM &&
                    board[row - 1][col - 1] == PLAYER_NUM &&
                    board[row - 2][col - 2] == PLAYER_NUM &&
                    board[row - 3][col - 3] == 0 &&
                    board[row - 2][col - 3] != 0)
                    return col - 3;
            }

            if (currIB.ny_bound == 1) {
                if (board[row][col] == 0 &&
                    board[row + 1][col] == PLAYER_NUM &&
                    board[row + 2][col] == PLAYER_NUM &&
                    board[row + 3][col] == PLAYER_NUM)
                    return col;
            }
        }
    }
    
    for (row = 0; row < BOARD_SIZE_VERT; row++) {
        for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
            currIB = set_indexbound(row, col);

            if (currIB.x_bound) {
                if (board[row][col] == PLAYER_NUM && 
                    board[row][col + 1] == 0 && 
                    board[row][col + 2] == PLAYER_NUM) {
                    if (row == (BOARD_SIZE_VERT - 1)) {
                        return col + 1;
                    } else if (row < (BOARD_SIZE_VERT - 1)) {
                        if (board[row + 1][col + 1] != 0) {
                            return col + 1;
                        }
                    }
                }
            }

            if (currIB.nx_bound) {
                if (board[row][col] == PLAYER_NUM && 
                    board[row][col - 1] == 0 && 
                    board[row][col - 2] == PLAYER_NUM) {
                    if (row == (BOARD_SIZE_VERT - 1)) {
                        return col - 1;
                    } else if (row < (BOARD_SIZE_VERT - 1)) {
                        if (board[row + 1][col - 1] != 0) {
                            return col - 1;
                        }
                    }
                }
            }

            if (currIB.x_bound == 1 && 
                currIB.y_bound == 1) {
                if (board[row][col] == PLAYER_NUM && board[row - 1][col + 1] == 0 && board[row - 2][col + 2] == PLAYER_NUM && board[row][col + 1] != 0)
                    return col + 1;
            }

            if (currIB.x_bound == 1 && 
                currIB.ny_bound == 1) {
                if (board[row][col] == PLAYER_NUM && board[row + 1][col + 1] == 0 && board[row + 2][col + 2] == PLAYER_NUM && board[row + 2][col + 1] != 0)
                    return col + 1;
            }

            if (currIB.nx_bound == 1 && 
                currIB.ny_bound == 1) {
                if (board[row][col] == PLAYER_NUM && board[row + 1][col - 1] == 0 && board[row + 2][col - 2] == PLAYER_NUM && board[row + 2][col - 1] != 0)
                    return col - 1;
            }

            if (currIB.nx_bound == 1 && 
                currIB.y_bound == 1) {
                if (board[row][col] == PLAYER_NUM && board[row - 1][col - 1] == 0 && board[row - 2][col - 2] == PLAYER_NUM && board[row][col - 1] != 0)
                    return col - 1;
            }
        }
    }
    
    for (row = 0; row < BOARD_SIZE_VERT; row++) {
        for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
            currIB = set_indexbound(row, col);
            
            if (currIB.x_bound) {
                if (board[row][col] == PLAYER_NUM && 
                    board[row][col + 1] == PLAYER_NUM && 
                    board[row][col + 2] == 0) {
                    if (row == (BOARD_SIZE_VERT - 1)) {
                        return col + 2;
                    } else if (row < (BOARD_SIZE_VERT - 1)) {
                        if (board[row + 1][col + 2] != 0) {
                            return col + 2;
                        }
                    }
                }
            }
            
            if (currIB.nx_bound) {
                if (board[row][col] == PLAYER_NUM && 
                    board[row][col - 1] == PLAYER_NUM && 
                    board[row][col - 2] == 0) {
                    if (row == (BOARD_SIZE_VERT - 1)) {
                        return col - 2;
                    } else if (row < (BOARD_SIZE_VERT - 1)) {
                        if (board[row + 1][col - 2] != 0) {
                            return col - 2;
                        }
                    }
                }
            }

            if (currIB.x_bound == 1 && 
                currIB.y_bound == 1) {
                if (board[row][col] == PLAYER_NUM && board[row - 1][col + 1] == PLAYER_NUM && board[row - 2][col + 2] == 0 && board[row - 1][col + 2] != 0)
                    return col + 2;
            }

            if (currIB.x_bound == 1 && 
                currIB.ny_bound == 1) {
                if (board[row][col] == PLAYER_NUM && board[row + 1][col + 1] == PLAYER_NUM && board[row + 2][col + 2] == 0 && board[row + 3][col + 2] != 0)
                    return col + 2;
            }

            if (currIB.nx_bound == 1 && 
                currIB.ny_bound == 1) {
                if (board[row][col] == PLAYER_NUM && board[row + 1][col - 1] == PLAYER_NUM && board[row + 2][col - 2] == 0 && board[row + 3][col - 2] != 0)
                    return col - 2;
            }

            if (currIB.nx_bound == 1 && 
                currIB.y_bound == 1) {
                if (board[row][col] == PLAYER_NUM && board[row - 1][col - 1] == PLAYER_NUM && board[row - 2][col - 2] == 0 && board[row - 1][col - 2] != 0)
                    return col - 2;
            }
        }
    }
    /*
    for (row = 0; row < BOARD_SIZE_VERT; row++) {
        for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
            currIB = set_indexbound(row, col);
            if (currIB.ny_bound == 1) {
                if (board[row][col] == 0 &&
                    board[row + 1][col] == ENEMY_NUM &&
                    board[row + 2][col] == ENEMY_NUM) {
                    return col;
                }
            }
        }
    }
    */
   
   for (row = 0; row < BOARD_SIZE_VERT; row++) {
        for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
            currIB = set_indexbound(row, col);

            if (currIB.x_bound) {
                if (board[row][col] == ENEMY_NUM && 
                    board[row][col + 1] == ENEMY_NUM && 
                    board[row][col + 2] == 0) {
                    if (row == (BOARD_SIZE_VERT - 1)) {
                        return col + 2;
                    } else if (row < (BOARD_SIZE_VERT - 1)) {
                        if (board[row + 1][col + 2] != 0) {
                            return col + 2;
                        }
                    }
                }
            }

            if (currIB.nx_bound) {
                if (board[row][col] == ENEMY_NUM && 
                    board[row][col - 1] == ENEMY_NUM && 
                    board[row][col - 2] == 0) {
                    if (row == (BOARD_SIZE_VERT - 1)) {
                        return col - 2;
                    } else if (row < (BOARD_SIZE_VERT - 1)) {
                        if (board[row + 1][col - 2] != 0) {
                            return col - 2;
                        }
                    }
                }
            }

            if (currIB.x_bound == 1 && 
                currIB.y_bound == 1) {
                if (board[row][col] == ENEMY_NUM && 
                    board[row - 1][col + 1] == ENEMY_NUM && 
                    board[row - 2][col + 2] == 0 && 
                    board[row - 1][col + 2] != 0)
                    return col + 2;
            }

            if (currIB.x_bound == 1 && 
                currIB.ny_bound == 1) {
                if (board[row][col] == ENEMY_NUM && 
                    board[row + 1][col + 1] == ENEMY_NUM && 
                    board[row + 2][col + 2] == 0 && 
                    board[row + 3][col + 2] != 0)
                    return col + 2;
            }

            if (currIB.nx_bound == 1 && 
                currIB.ny_bound == 1) {
                if (board[row][col] == ENEMY_NUM && 
                    board[row + 1][col - 1] == ENEMY_NUM && 
                    board[row + 2][col - 2] == 0 && 
                    board[row + 3][col - 2] != 0)
                    return col - 2;
            }

            if (currIB.nx_bound == 1 && 
                currIB.y_bound == 1) {
                if (board[row][col] == ENEMY_NUM && 
                    board[row - 1][col - 1] == ENEMY_NUM && 
                    board[row - 2][col - 2] == 0 && 
                    board[row - 1][col - 2] != 0)
                    return col - 2;
            }
        }
    }
}