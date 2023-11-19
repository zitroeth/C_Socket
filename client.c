#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
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
int scan_num(int limit);
int drop_disc(int board[][BOARD_SIZE_HORIZ], int drop_col, int diskType);
indexBound set_indexbound(int row, int col);
int check_gamewin(int board[][BOARD_SIZE_HORIZ]);
void reset_board(int board[][BOARD_SIZE_HORIZ]);

int main(int argc, char **argv) {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char sendbuf[5];
    char recvbuf[DEFAULT_BUFLEN];
    int iResult, inputNum = 3, drop_col, recvNum;
    int recvbuflen = DEFAULT_BUFLEN;
    int board[BOARD_SIZE_VERT][BOARD_SIZE_HORIZ] = {0};

    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                               ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Sending connect four data
    do {
        if (inputNum == 3) {
            system("cls");
            printf("\n[1] Single player\n[2] Multiplayer\n[0] Exit\n");
            inputNum = scan_num(3);
            system("cls");
            display_board(board);
        }

        switch (inputNum) {
        case 1:
            drop_col = scan_num(BOARD_SIZE_HORIZ);
            drop_disc(board, drop_col, PLAYER_NUM);

            system("cls");
            display_board(board);

            if (check_gamewin(board) == 1 || check_gamewin(board) == 2) {
                printf("\nPlayer %d wins!\n[1] Continue\n[0] Exit", check_gamewin(board));
                snprintf(sendbuf, 5, "3 %d", drop_col);
                iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
                if (iResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ConnectSocket);
                    WSACleanup();
                    return 1;
                }

                if (scan_num(2)) {
                    reset_board(board);
                    inputNum = 3;

                } else {
                    inputNum = 0;
                }
            } else {
                snprintf(sendbuf, 5, "1 %d", drop_col);

                iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
                if (iResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ConnectSocket);
                    WSACleanup();
                    return 1;
                }

                iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
                if (iResult > 0) {
                    printf("Bytes received: %d\n", iResult);
                    recvNum = recvbuf[2] - '0';
                    drop_disc(board, recvNum, ENEMY_NUM);
                    system("cls");
                    display_board(board);

                    if (check_gamewin(board) == 1 || check_gamewin(board) == 2) {
                        printf("\nPlayer %d wins!\n[1] Continue\n[0] Exit", check_gamewin(board));
                        snprintf(sendbuf, 5, "3 %d", drop_col);
                        iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
                        if (iResult == SOCKET_ERROR) {
                            printf("send failed with error: %d\n", WSAGetLastError());
                            closesocket(ConnectSocket);
                            WSACleanup();
                            return 1;
                        }
                        if (scan_num(2)) {
                            reset_board(board);
                            inputNum = 3;
                        } else {
                            inputNum = 0;
                        }
                    }
                } else if (iResult == 0) {
                    printf("Connection closed\n");
                    inputNum = 0;
                } else {
                    printf("recv failed with error: %d\n", WSAGetLastError());
                    inputNum = 0;
                }
            }
            break;
        case 2:
            drop_col = scan_num(BOARD_SIZE_HORIZ);
            drop_disc(board, drop_col, PLAYER_NUM);

            system("cls");
            display_board(board);

            if (check_gamewin(board) == 1 || check_gamewin(board) == 2) {
                printf("\nPlayer %d wins!\n[1] Continue\n[0] Exit", check_gamewin(board));
                snprintf(sendbuf, 5, "3 %d", drop_col);
                iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
                if (iResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ConnectSocket);
                    WSACleanup();
                    return 1;
                }

                if (scan_num(2)) {
                    reset_board(board);
                    inputNum = 3;

                } else {
                    inputNum = 0;
                }
            } else {
                snprintf(sendbuf, 5, "2 %d", drop_col);

                iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
                if (iResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ConnectSocket);
                    WSACleanup();
                    return 1;
                }

                iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
                if (iResult > 0) {
                    printf("Bytes received: %d\n", iResult);
                    recvNum = recvbuf[2] - '0';
                    drop_disc(board, recvNum, ENEMY_NUM);
                    system("cls");
                    display_board(board);

                    if (check_gamewin(board) == 1 || check_gamewin(board) == 2) {
                        printf("\nPlayer %d wins!\n[1] Continue\n[0] Exit", check_gamewin(board));
                        snprintf(sendbuf, 5, "3 %d", drop_col);
                        iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);

                        if (iResult == SOCKET_ERROR) {
                            printf("send failed with error: %d\n", WSAGetLastError());
                            closesocket(ConnectSocket);
                            WSACleanup();
                            return 1;
                        }
                        if (scan_num(2)) {
                            reset_board(board);
                            inputNum = 3;
                        } else {
                            inputNum = 0;
                        }
                    }
                } else if (iResult == 0) {
                    printf("Connection closed\n");
                    inputNum = 0;
                } else {
                    printf("recv failed with error: %d\n", WSAGetLastError());
                    inputNum = 0;
                }
            }
            break;
        }

    } while (inputNum != 0);

    iResult = send(ConnectSocket, sendbuf, 0, 0);
    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while (iResult > 0);

    // cleanup
    closesocket(ConnectSocket);
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

int scan_num(int limit) {
    int num, c, result;
    do {
        printf("\nEnter option (0-%d): ", limit - 1);
        result = scanf("%d", &num);
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        printf("\nNUM = %d\n", num);
    } while (result == 0 || (num > (limit - 1)) || num < 0);
    return num;
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
    (col >= 3) ? (newIB.nx_bound = 1) : (newIB.nx_bound = 0);                     // x-axis left
    ((row + 4) <= BOARD_SIZE_VERT) ? (newIB.ny_bound = 1) : (newIB.ny_bound = 0); // y-axis down
    (row >= 3) ? (newIB.y_bound = 1) : (newIB.y_bound = 0);                       // y-axis up

    return newIB;
}

int check_gamewin(int board[][BOARD_SIZE_HORIZ]) {
    int row, col;
    indexBound currIB;
    for (int i = 1; i <= 2; i++) {
        for (row = 0; row < BOARD_SIZE_VERT; row++) {
            for (col = 0; col < BOARD_SIZE_HORIZ; col++) {
                currIB = set_indexbound(row, col);

                if (currIB.x_bound == 1) {
                    if (
                        board[row][col] == i &&
                        board[row][col + 1] == i &&
                        board[row][col + 2] == i &&
                        board[row][col + 3] == i) {
                        printf("\nWin by move->right\n");
                        return i;
                    }
                }

                if (currIB.ny_bound == 1) {
                    if (
                        board[row][col] == i &&
                        board[row + 1][col] == i &&
                        board[row + 2][col] == i &&
                        board[row + 3][col] == i) {
                        printf("\nWin by move->down\n");
                        return i;
                    }
                }

                if (currIB.x_bound == 1 && currIB.ny_bound == 1) {
                    if (
                        board[row][col] == i &&
                        board[row + 1][col + 1] == i &&
                        board[row + 2][col + 2] == i &&
                        board[row + 3][col + 3] == i) {
                        printf("\nWin by move->down right\n");
                        return i;
                    }
                }

                if (currIB.nx_bound == 1 && currIB.ny_bound == 1) {
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