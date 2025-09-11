// 교재 실습 2 - 1 InitSocket.cpp를 MAKEWORD를 사용하지 않고 동작하도록 수정하고, 
// 이를 확인하기 위해 wsa 변수의 필드 중 처음 네 개
// (wVersion, wHighVersion, szDescription, szSystemStatus)
// 를 화면에 출력하도록 코드를 수정하시오.

#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment (lib,"WS2_32.LIB")
#pragma comment (lib, "MSWSock.LIB")

#include <stdio.h>

int main(int argc, char* argv[])
{
	WSADATA wsa;
	// 8비트 씩 따로 만들어서 합쳐서 16진수 만들기
	BYTE minor = 2 << 8;
	BYTE major = 2;
	WORD version = minor | major;

	if (WSAStartup(version, &wsa) != 0)
		return 1;

	printf("wVersion: %u.%u\n", LOBYTE(wsa.wVersion), HIBYTE(wsa.wVersion));
	printf("wHighVersion: %u.%u\n", LOBYTE(wsa.wHighVersion), HIBYTE(wsa.wHighVersion));
	printf("szDescription: %s\n", wsa.szDescription);
	printf("szSystemStatus: %s\n", wsa.szSystemStatus);

	WSACleanup();
	return 0;
}