#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define SERVER_PORT 9000
#define BUFFER_SIZE 4096

#pragma pack(push, 1)
struct FileInfo {
    int filename_len;
    int file_size;
};
#pragma pack(pop)

void err_quit(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (char*)&lpMsgBuf, 0, NULL);
    MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}


void err_display(const char* msg) {
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s\n", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVER_PORT);
    if (bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
        err_quit("bind()");

    if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR)
        err_quit("listen()");

    printf("서버가 %d 포트에서 클라이언트 접속을 기다립니다...\n", SERVER_PORT);

    while (1) {
        struct sockaddr_in clientaddr;
        int addrlen = sizeof(clientaddr);
        SOCKET client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            continue;
        }

        printf("\n클라이언트 접속: %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        // ===== [중요] 이 부분이 누락되었습니다! =====
        struct FileInfo info;
        int retval = recv(client_sock, (char*)&info, sizeof(info), MSG_WAITALL);
        if (retval != sizeof(info)) {
            printf("헤더 수신 실패 또는 연결 끊김\n");
            closesocket(client_sock);
            continue;
        }

        info.filename_len = ntohl(info.filename_len);
        info.file_size = ntohl(info.file_size);

        char filename[256];
        if (info.filename_len >= sizeof(filename)) {
            printf("파일 이름이 너무 깁니다.\n");
            closesocket(client_sock);
            continue;
        }
        retval = recv(client_sock, filename, info.filename_len, MSG_WAITALL);
        if (retval != info.filename_len) {
            printf("파일 이름 수신 실패 또는 연결 끊김\n");
            closesocket(client_sock);
            continue;
        }
        filename[info.filename_len] = '\0';
        // ==========================================

        printf("수신할 파일: %s (크기: %d 바이트)\n", filename, info.file_size);

        FILE* fp = fopen(filename, "wb");
        if (fp == NULL) {
            perror("파일 열기 실패");
            closesocket(client_sock);
            continue;
        }

        char buffer[BUFFER_SIZE];
        int total_received = 0;
        int current_percent = 0;

        printf("수신률:   0%%");

        while (total_received < info.file_size) {
            int bytes_to_receive = min(BUFFER_SIZE, info.file_size - total_received);
            retval = recv(client_sock, buffer, bytes_to_receive, 0);
            if (retval <= 0) break;

            fwrite(buffer, 1, retval, fp);
            total_received += retval;

            int percent = (int)((double)total_received * 100 / info.file_size);

            if (percent > current_percent) {
                current_percent = percent;
                printf("\r수신률: %3d%%", current_percent);
                fflush(stdout);
            }
        }
        printf("\n");

        fclose(fp);

        if (total_received == info.file_size) printf("파일 수신 완료.\n");
        else printf("파일 수신 실패. (수신된 크기: %d 바이트)\n", total_received);

        closesocket(client_sock);
        printf("클라이언트 접속 종료.\n");
    }

    closesocket(listen_sock);
    WSACleanup();
    return 0;
}