#include "defs.h"
#include "aes.h"

int main()
{
    printf("printf test.\n");
    AES_C aes;
    //char key[17] = "1234567890ABCDEF";
    //char p[33] = "1234567890ABCDEF1234567890ABCDEF";
    byte key[17] = {(byte)0x3C,(byte)0xA1,(byte)0x0B,(byte)0x21,(byte)0x57,(byte)0xF0 ,(byte)0x19 ,(byte)0x16 ,(byte)0x90 ,(byte)0x2E ,(byte)0x13 ,(byte)0x80 ,(byte)0xAC ,(byte)0xC1 ,(byte)0x07 ,(byte)0xBD,(byte)0x0} ;
    char p[17] = "1234567890ABCDEF";

    printf("key: %s\n", key);
    printf("origin text: %s\n", p);
    /*
    for(int i = 0; i < 32; i++){
        printf("0x%02x ", *(p + i) & 0xff);
    }
    printf("\n");
    */
    //printf("\nWArray\n");
    aes.encode(p, key);

    
    printf("encode text: %s\n", p);
    for(int i = 0; i < 16; i++){
        printf("0x%02x ", *(p + i) & 0xff);
    }
    
    //printf("\nWArray\n");
   
    aes.decode(p, key);
    printf("\n");
    printf("decode text: %s\n", p);
    printf("\n");
    /*
    
    for(int i = 0; i < 32; i++){
        printf("0x%02x ", *(p + i) & 0xff);
    }
    printf("\n");
    */
    return 0;
}
