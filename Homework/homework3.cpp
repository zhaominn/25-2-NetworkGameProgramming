#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>

char* SERVER_IP = (char*)"172.30.1.78";
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 1;
    }

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serveraddr.sin_port = htons(SERVER_PORT);
    if (connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
        err_quit("connect()");

    printf("서버에 접속했습니다.\n");

    const char* filename = argv[1];

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("파일을 열 수 없습니다: %s\n", filename);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    struct FileInfo info;
    info.filename_len = strlen(filename);
    info.file_size = file_size;

    info.filename_len = htonl(info.filename_len);
    info.file_size = htonl(info.file_size);

    send(sock, (char*)&info, sizeof(info), 0);
    send(sock, filename, strlen(filename), 0);

    printf("파일 전송 시작: %s (%d 바이트)\n", filename, file_size);

    char buffer[BUFFER_SIZE];
    int total_sent = 0;
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        int retval = send(sock, buffer, bytes_read, 0);
        if (retval == SOCKET_ERROR) {
            err_quit("send()");
            break;
        }
        total_sent += retval;
    }

    fclose(fp);

    if (total_sent == file_size) printf("파일 전송 완료.\n");
    else printf("파일 전송 실패.\n");

    closesocket(sock);
    WSACleanup();
    return 0;
}