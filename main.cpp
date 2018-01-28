#include "defs.h"
#include "aes.h"

int main()
{
    printf("printf test.\n");
    AES_C aes;
    aes.print();
    char key[17] = "1234567890ABCDEF";
    char p[33] = "1234567890ABCDEF1234567890ABCDEF";
    printf("key: %s\n", key);
    printf("origin text: %s\n", p);
    for(int i = 0; i < 32; i++){
        printf("0x%02x ", *(p + i) & 0xff);
    }
    printf("\n");
    aes.encode(p, key);
    printf("encode text: %s\n", p);
    for(int i = 0; i < 32; i++){
        printf("0x%02x ", *(p + i) & 0xff);
    }
    printf("\n");
    return 0;
}
