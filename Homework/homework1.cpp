#include <stdio.h>

void checkHostEndianness()
{
    unsigned short test_word = 0x0102;

    unsigned char* byte_ptr = (unsigned char*)&test_word;

    printf("호스트의 바이트 정렬 방식은 ... ");

    if (*byte_ptr == 0x02)
        printf(" Little Endian (리틀 엔디안)\n");
    else if (*byte_ptr == 0x01)
        printf(" Big Endian (빅 엔디안)\n");
    else
        printf("결과를 확인할 수 없습니다.\n");
}

int main(int argc, char* argv[])
{
    checkHostEndianness();

    return 0;
}
