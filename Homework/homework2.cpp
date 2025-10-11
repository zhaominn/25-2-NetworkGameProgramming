#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return 1;
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return 1;

    // 1. gethostbyname으로 별명 조회
    struct hostent* host = gethostbyname(argv[1]);
    printf("도메인: %s\n", host->h_name);
    if (host && host->h_aliases[0]) {
        printf("별명:\n");
        for (int i = 0; host->h_aliases[i] != NULL; ++i) {
            printf("\t%s\n", host->h_aliases[i]);
        }
    }

    // 2. getaddrinfo로 IP 주소 조회 ("www." 자동 제거)
    const char* domainForIp = (strncmp(argv[1], "www.", 4) == 0) ? argv[1] + 4 : argv[1];

    struct addrinfo hints, * result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    if (getaddrinfo(domainForIp, NULL, &hints, &result) == 0) {
        printf("주소:\n");
        for (struct addrinfo* p = result; p != NULL; p = p->ai_next) {
            char ip_str[INET_ADDRSTRLEN];
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, sizeof(ip_str));
            // 컴퓨터가 사용하는 이진(binary) 형태의 IP 주소를 사람이 읽을 수 있는 문자열(text) 형태로 변환해주는 함수입니다.
            printf("\t%s\n", ip_str);
        }
        freeaddrinfo(result);
    }

    WSACleanup();
    return 0;
}