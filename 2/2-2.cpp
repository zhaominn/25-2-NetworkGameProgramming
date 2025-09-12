#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment (lib,"WS2_32.LIB")
#pragma comment (lib, "MSWSock.LIB")

#include <stdio.h>

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

int main(int argc, char* argv[])
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	printf("[알림] 윈속 초기화 성공\n");

	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	printf("[알림] 소켓 생성 성공\n");

	/*
	AF_INET : IPv4 인터넷 프로토콜
	SOCK_STREAM : 연결형(스트림) 소켓
	0 : 기본 프로토콜(TCP)

	AF_INET과 SOCK_STREAM 조합에 대한 기본 프로토콜이 TCP로 정해져 있기 때문입니다.
	따라서 0을 사용하면 운영체제(OS)가 알아서 TCP를 선택해 줍니다.

	1. 코드의 명확성: 
	0 대신 IPPROTO_TCP라고 명시적으로 적으면, 코드를 처음 보는 사람도 
	"이 소켓은 TCP를 사용하는구나"라고 즉시 파악할 수 있습니다. 
	가독성을 높이는 목적이 가장 큽니다.

	2. 특수한 경우: 하나의 주소 체계와 소켓 타입에 여러 프로토콜이 존재할 수 있는 
	드문 경우에는 원하는 프로토콜을 명확하게 지정해야 합니다. 
	하지만 TCP/IP 프로토콜군에서는 거의 해당되지 않습니다.
	*/

	// 소켓 닫기
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}