#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h> 


void printHostInfo(const char* domainName)
{
    struct hostent* host = gethostbyname(domainName);
    if (host == NULL) {
        fprintf(stderr, "'%s' 호스트를 찾을 수 없습니다.\n", domainName);
        fprintf(stderr, "Winsock Error Code: %d\n", WSAGetLastError());
        return;
    }

    printf("공식 이름: %s\n", host->h_name);

    // 3. 모든 별명(Aliases) 출력
    printf("별명 목록:\n");
    if (host->h_aliases[0] == NULL) {
        printf("    (별명 없음)\n");
    }
    else {
        for (int i = 0; host->h_aliases[i] != NULL; ++i) {
            printf("    %d. %s\n", i + 1, host->h_aliases[i]);
        }
    }

    // 4. 모든 IPv4 주소 출력
    printf("IPv4 주소 목록:\n");
    if (host->h_addr_list[0] == NULL) {
        printf("    (주소 없음)\n");
    }
    else {
        if (host->h_addrtype == AF_INET) { // 주소 타입이 IPv4인지 확인
            for (int i = 0; host->h_addr_list[i] != NULL; ++i) {
                struct in_addr* addr = (struct in_addr*)host->h_addr_list[i];
                printf("    %d. %s\n", i + 1, inet_ntoa(*addr));
            }
        }
    }
}

int main(int argc, char* argv[])
{
    // 1. 명령행 인수 확인
    if (argc != 2) {
        fprintf(stderr, "사용법: %s <도메인 이름>\n", argv[0]);
        return 1;
    }

    // Windows 소켓 초기화
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup 실패.\n");
        return 1;
    }

    // 2. 핵심 기능 함수 호출
    printHostInfo(argv[1]);

    // Windows 소켓 종료 처리
    WSACleanup();

    return 0;
}