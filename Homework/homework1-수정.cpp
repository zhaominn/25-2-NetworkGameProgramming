#include <stdio.h>
#include <windows.h> // LOBYTE, HIBYTE 매크로를 위해 포함

void checkEndianWithMacros()
{
    unsigned short test_word = 0x0102;
    // 메모리의 첫 바이트를 가리키는 포인터
    unsigned char* first_byte_ptr = (unsigned char*)&test_word;

    printf("호스트의 바이트 정렬 방식은 ... ");

    // 메모리의 첫 바이트가 test_word의 하위 바이트(0x02)와 같은가?
    if (*first_byte_ptr == LOBYTE(test_word)) {
        printf("Little Endian (리틀 엔디안)\n");
    }
    // 메모리의 첫 바이트가 test_word의 상위 바이트(0x01)와 같은가?
    else if (*first_byte_ptr == HIBYTE(test_word)) {
        printf("Big Endian (빅 엔디안)\n");
    }
}

int main(void)
{
    checkEndianWithMacros();
    return 0;
}