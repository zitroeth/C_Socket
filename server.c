#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
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
int scan_num(int limit);
int drop_disc(int board[][BOARD_SIZE_HORIZ], int drop_col, int diskType);
indexBound set_indexbound(int row, int col);
int check_gamewin(int board[][BOARD_SIZE_HORIZ]);
int move_algo(int board[][BOARD_SIZE_HORIZ]);
void reset_board(int board[][BOARD_SIZE_HORIZ]);
int simulate(int board[][BOARD_SIZE_HORIZ], int row, int col);
int simulate_twice(int board[][BOARD_SIZE_HORIZ], int drop_col);

int main(void) {
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int board[BOARD_SIZE_VERT][BOARD_SIZE_HORIZ] = {0};
    char sendbuf[5];
    int drop_col, recvNum, recvOpt;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            system("cls");
            printf("Bytes received: %d\n", iResult);
            recvOpt = recvbuf[0] - '0';
            recvNum = recvbuf[2] - '0';
            if (check_gamewin(board) != ENEMY_NUM)
                drop_disc(board, recvNum, PLAYER_NUM);
            display_board(board);

            if (recvOpt == 1) {
                drop_col = move_algo(board);
                drop_disc(board, drop_col, ENEMY_NUM);
                system("cls");
                display_board(board);
                snprintf(sendbuf, 5, "1 %d", drop_col);
                iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    WSACleanup();
                    return 1;
                }
            } else if (recvOpt == 2) {
                while (1) {
                    system("cls");
                    display_board(board);
                    drop_col = scan_num(BOARD_SIZE_HORIZ);
                    if (drop_disc(board, drop_col, ENEMY_NUM)){
                        system("cls");
                        display_board(board);
                        break;
                        }
                }
                snprintf(sendbuf, 5, "2 %d", drop_col);
                iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    WSACleanup();
                    return 1;
                }
                printf("\nBytes sent: %d\n", iSendResult);
            } else if (recvOpt == 3) {
                if (check_gamewin(board) == 3)
                    printf("\nGame ended in a draw!\nAwaiting client input...");
                else
                    printf("\nPlayer %d wins!\nAwaiting client input...", check_gamewin(board));
                reset_board(board);
            }
        } else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}

void reset_board(int board[][BOARD_SIZE_HORIZ]) {
    for (int x = 0; x < BOARD_SIZE_VERT; x++) {
        for (int y = 0; y < BOARD_SIZE_HORIZ; y++) {
            board[x][y] = 0;
        }
    }
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

    for (row = 0; row < BOARD_SIZE_VERT; row++) {
        for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
            currIB = set_indexbound(row, col);

            if (currIB.east) {
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

            if (currIB.west) {
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

            if (currIB.east == 1 &&
                currIB.north == 1) {
                if (board[row][col] == PLAYER_NUM && board[row - 1][col + 1] == 0 && board[row - 2][col + 2] == PLAYER_NUM && board[row][col + 1] != 0)
                    return col + 1;
            }

            if (currIB.east == 1 &&
                currIB.south == 1) {
                if (board[row][col] == PLAYER_NUM && board[row + 1][col + 1] == 0 && board[row + 2][col + 2] == PLAYER_NUM && board[row + 2][col + 1] != 0)
                    return col + 1;
            }

            if (currIB.west == 1 &&
                currIB.south == 1) {
                if (board[row][col] == PLAYER_NUM && board[row + 1][col - 1] == 0 && board[row + 2][col - 2] == PLAYER_NUM && board[row + 2][col - 1] != 0)
                    return col - 1;
            }

            if (currIB.west == 1 &&
                currIB.north == 1) {
                if (board[row][col] == PLAYER_NUM && board[row - 1][col - 1] == 0 && board[row - 2][col - 2] == PLAYER_NUM && board[row][col - 1] != 0)
                    return col - 1;
            }
        }
    }

    for (row = 0; row < BOARD_SIZE_VERT; row++) {
        for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
            currIB = set_indexbound(row, col);

            if (currIB.east) {
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

            if (currIB.west) {
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

            if (currIB.east == 1 &&
                currIB.north == 1) {
                if (board[row][col] == PLAYER_NUM && board[row - 1][col + 1] == PLAYER_NUM && board[row - 2][col + 2] == 0 && board[row - 1][col + 2] != 0)
                    return col + 2;
            }

            if (currIB.east == 1 &&
                currIB.south == 1) {
                if (board[row][col] == PLAYER_NUM && board[row + 1][col + 1] == PLAYER_NUM && board[row + 2][col + 2] == 0 && board[row + 3][col + 2] != 0)
                    return col + 2;
            }

            if (currIB.west == 1 &&
                currIB.south == 1) {
                if (board[row][col] == PLAYER_NUM && board[row + 1][col - 1] == PLAYER_NUM && board[row + 2][col - 2] == 0 && board[row + 3][col - 2] != 0)
                    return col - 2;
            }

            if (currIB.west == 1 &&
                currIB.north == 1) {
                if (board[row][col] == PLAYER_NUM && board[row - 1][col - 1] == PLAYER_NUM && board[row - 2][col - 2] == 0 && board[row - 1][col - 2] != 0)
                    return col - 2;
            }
        }
    }

    for (row = 0; row < BOARD_SIZE_VERT; row++) {
        for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
            currIB = set_indexbound(row, col);

            if (currIB.east) {
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

            if (currIB.west) {
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

            if (currIB.east == 1 &&
                currIB.north == 1) {
                if (board[row][col] == ENEMY_NUM &&
                    board[row - 1][col + 1] == ENEMY_NUM &&
                    board[row - 2][col + 2] == 0 &&
                    board[row - 1][col + 2] != 0)
                    return col + 2;
            }

            if (currIB.east == 1 &&
                currIB.south == 1) {
                if (board[row][col] == ENEMY_NUM &&
                    board[row + 1][col + 1] == ENEMY_NUM &&
                    board[row + 2][col + 2] == 0 &&
                    board[row + 3][col + 2] != 0)
                    return col + 2;
            }

            if (currIB.west == 1 &&
                currIB.south == 1) {
                if (board[row][col] == ENEMY_NUM &&
                    board[row + 1][col - 1] == ENEMY_NUM &&
                    board[row + 2][col - 2] == 0 &&
                    board[row + 3][col - 2] != 0)
                    return col - 2;
            }

            if (currIB.west == 1 &&
                currIB.north == 1) {
                if (board[row][col] == ENEMY_NUM &&
                    board[row - 1][col - 1] == ENEMY_NUM &&
                    board[row - 2][col - 2] == 0 &&
                    board[row - 1][col - 2] != 0)
                    return col - 2;
            }
        }
    }

    int i, drop_col;

    while (1) {
        drop_col = rand() % BOARD_SIZE_HORIZ;
        if (simulate_twice(board, drop_col)) {
            return drop_col;
        }
    }
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