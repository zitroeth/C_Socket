#include <stdio.h>
#define BOARD_SIZE_VERT 6
#define BOARD_SIZE_HORIZ 7
#define PLAYER_NUM 1

void display_board(int board[][BOARD_SIZE_HORIZ]);
int scan_num();
void drop_disc(int board[][BOARD_SIZE_HORIZ]);
int check_gamewin(int board[][BOARD_SIZE_HORIZ]);

int main()
{

    int board[BOARD_SIZE_VERT][BOARD_SIZE_HORIZ] = {

        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0, 0, 0}};

    display_board(board);

    if (check_gamewin(board) == 0)
        printf("no winners");

    return 0;
}

void display_board(int board[][BOARD_SIZE_HORIZ])
{
    int i, j;
    for (j = 0; j < BOARD_SIZE_HORIZ; j++)
        printf("  %d ", j);
    printf("\n");
    for (i = 0; i < BOARD_SIZE_VERT; i++)
    {
        printf(" +");
        for (j = 0; j < BOARD_SIZE_HORIZ; j++)
            printf("---+");
        printf("\n");
        printf("%d|", i);
        for (j = 0; j < BOARD_SIZE_HORIZ; j++)
        {
            switch (board[i][j])
            {
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

    printf("+");
    for (j = 0; j < BOARD_SIZE_HORIZ; j++)
        printf("---+");
    printf("\n");
    for (j = 0; j < BOARD_SIZE_HORIZ; j++)
        printf("  %d ", j);
    printf("\n");
}

int scan_num()
{
    int num, c;
    printf("\nEnter a number (0-6): ");
    while ((scanf("%d", &num) == 0 || num > 6 || num < 0))
        ;
    return num;
}

void drop_disc(int board[][BOARD_SIZE_HORIZ])
{
    int drop_col, valid = 0, i;

    do
    {
        if (valid)
            break;
        drop_col = scan_num();
        for (i = 5; i >= 0; --i)
        {
            if (board[i][drop_col] == 0)
            {
                valid = 1;
                break;
            }
            else
            {
                valid = 0;
            }
            if (i == 0 && valid == 0)
                printf("\nInvalid!!\n");
        }
    } while (valid == 1);

    for (i = 5; i >= 0; --i)
    {
        if (board[i][drop_col] == 0)
        {
            board[i][drop_col] = PLAYER_NUM;
            (check_gamewin(board)) ? (printf("\nPlayer %d wins!\n", PLAYER_NUM)) : (printf(""));
            break;
        }
    }
}

int check_gamewin(int board[][BOARD_SIZE_HORIZ])
{
    int row, col, x_bound, y_bound, ny_bound;

    for (row = 0; row < BOARD_SIZE_VERT; row++)
    {
        for (col = 0; col < BOARD_SIZE_HORIZ; col++)
        {
            ((col + 4) <= BOARD_SIZE_HORIZ) ? (x_bound = 1) : (x_bound = 0);  // x-axis right
            ((row + 4) <= BOARD_SIZE_VERT) ? (ny_bound = 1) : (ny_bound = 0); // y-axis down
            (row >= 3) ? (y_bound = 1) : (y_bound = 0);                       // y-axis up

            if (x_bound == 1)
            {
                if (
                    board[row][col] == PLAYER_NUM &&
                    board[row][col + 1] == PLAYER_NUM &&
                    board[row][col + 2] == PLAYER_NUM &&
                    board[row][col + 3] == PLAYER_NUM)
                {
                    printf("\nWin by x->right\n");
                    return 1;
                }
            }

            if (ny_bound == 1)
            {
                if (
                    board[row][col] == PLAYER_NUM &&
                    board[row + 1][col] == PLAYER_NUM &&
                    board[row + 2][col] == PLAYER_NUM &&
                    board[row + 3][col] == PLAYER_NUM)
                {
                    printf("\nWin by y->down\n");
                    return 1;
                }
            }

            if (x_bound == 1 && ny_bound == 1)
            {
                if (
                    board[row][col] == PLAYER_NUM &&
                    board[row + 1][col + 1] == PLAYER_NUM &&
                    board[row + 2][col + 2] == PLAYER_NUM &&
                    board[row + 3][col + 3] == PLAYER_NUM)
                {
                    printf("\nWin by xy->down right\n");
                    return 1;
                }
            }

            if (x_bound == 1 && y_bound == 1)
            {
                if (
                    board[row][col] == PLAYER_NUM &&
                    board[row - 1][col + 1] == PLAYER_NUM &&
                    board[row - 2][col + 2] == PLAYER_NUM &&
                    board[row - 3][col + 3] == PLAYER_NUM)
                {
                    printf("\nWin by xy->up right\n");
                    return 1;
                }
            }
        }
    }
    return 0;
}