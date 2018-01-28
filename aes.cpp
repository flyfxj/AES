#include "aes.h"

using namespace VAL;

AES_C::AES_C(){
    printf("AES Construction.\n");
}

AES_C::~AES_C(){
    printf("AES Desconstruction.\n");
}

void AES_C::print(){
    printf("AES print.\n");
}

bool AES_C::encode(char *p, char *key){
    //check
    if(NULL == p || NULL == key){
        return false;
    }
    if(16 != strlen(key)){
        printf("the lenth of key is not 16");
        return false;
    }
    int nLen = strlen(p);

    if(0 != nLen % 16){
        printf("the lenth of text % 16 is not 0");
        return false;
    }
    //extend the key.
    if(!extendKey(key)){
        return false;
    }

    byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM];
    for(int k = 0; k < nLen; k += CONST_SZ_NUM_SQURE){
        //get source char array to encode
        memcpy(byteArr, p + k, CONST_SZ_NUM_SQURE);

        for(int i = 0; i < 10 ; i++){
            //a encode round.
            addRoundKey(byteArr, i);
            //bytes exchange.
            subBytes(byteArr);
            //row shift.
            shiftRows(byteArr);
            if(10 == i){
                //another encode round.
                addRoundKey(byteArr, i);
            }else{
                mixColumn(byteArr);
            }
        }
        //copy encode char array to origin c
        memcpy(p + k, byteArr, CONST_SZ_NUM_SQURE);
    }
}

bool AES_C::decode(){

}

void init(){

}

bool AES_C::extendKey(char* pStrOriKey){
    if(NULL == pStrOriKey){
        return false;
    }
    //get origin key array value w[0] w[1] w[2] w[3]
    for(int i = 0; i < CONST_SZ_NUM; i++){
        memcpy(m_nKeyWArr[i], pStrOriKey + i * CONST_SZ_NUM, CONST_SZ_NUM);
    }
    //extend key array value w[4]-w[43]
    for(int i = CONST_SZ_NUM, j = 0; i < 44; i++){
        //if i mod 4 != 0, W[i]=W[i-4] ^ W[i-1] 
        //else W[i]=W[i-4] ^ T(W[i-1]);
        if(0 == j % CONST_SZ_NUM){
            *((bytes4*)m_nKeyWArr[i]) = *((bytes4*)m_nKeyWArr[i - CONST_SZ_NUM]) ^ T(m_nKeyWArr[i - 1], j);
            j++;
        }else{
            *((bytes4*)m_nKeyWArr[i]) = *((bytes4*)m_nKeyWArr[i - CONST_SZ_NUM]) ^ *((bytes4*)m_nKeyWArr[i - 1]);
        }
    }
    return true;
}

void AES_C::splitBytes4toBytesArr(bytes4 num, byte* byteArr){
    if(NULL == byteArr){
        return;
    }
    memcpy(byteArr, &num, CONST_SZ_NUM);
}

bytes4 AES_C::T(byte byteArr[CONST_SZ_NUM], int round){
    byte tmpByteArr[CONST_SZ_NUM];
    memcpy(tmpByteArr, byteArr, CONST_SZ_NUM);
    //left shift 1 byte
    leftLoop4int(tmpByteArr,CONST_SZ_NUM,1);
    //byte exchange.
    for(int i = 0; i < 4 ; i++){
        tmpByteArr[i] = getByteFromSBox(tmpByteArr[i]);
    }
    //get bytes4
    bytes4 result = 0;
    memcpy(&result, tmpByteArr, CONST_SZ_NUM);
    return result ^ VAL::Rcon[round];
}

//replace 1 byte
byte AES_C::getByteFromSBox(byte originByte){
    int row = (int)(originByte >> 4);
    int col = (int)(originByte & 0x0f);
    return VAL::S[row][col];
}

//replace 4 byte
void AES_C::subBytes(byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM]){
    if(NULL == byteArr){
        return;
    }
    for(int i = 0; i < CONST_SZ_NUM; i++){
        for(int j = 0; j < CONST_SZ_NUM; j++){
            byteArr[i][j] = getByteFromSBox(byteArr[i][j]);
        }
    }
}


//left shift nShiftPos bytes
void AES_C::leftLoop4int(byte* byteArray, unsigned int nLen, unsigned int nShiftPos){
    if(NULL == byteArray){
        return;
    }
    byte* temp = new byte[nLen];
    if(NULL == temp){
        return;
    }
    memcpy(temp,byteArray,nLen);
    unsigned int index = nShiftPos % nLen;
    if(0 == index){
        return;
    }
    for(int i = 0; i < nLen; i++){
        byteArray[i] = temp[index];
        index++;
        index = index % nLen;
    }
    delete [] temp;
}


//shift Rows
void AES_C::shiftRows(byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM]) {
    if(NULL == byteArr){
        return;
    }
    byte rowTwo[CONST_SZ_NUM], rowThree[CONST_SZ_NUM], rowFour[CONST_SZ_NUM];
    //复制状态矩阵的第2,3,4行
    memcpy(rowTwo, byteArr + sizeof(byte) * CONST_SZ_NUM * 1, CONST_SZ_NUM);
    memcpy(rowThree, byteArr + sizeof(byte) * CONST_SZ_NUM * 2, CONST_SZ_NUM);
    memcpy(rowFour, byteArr + sizeof(byte) * CONST_SZ_NUM * 3, CONST_SZ_NUM);

    //循环左移相应的位数
    leftLoop4int(rowTwo, CONST_SZ_NUM, 1);
    leftLoop4int(rowThree, CONST_SZ_NUM, 2);
    leftLoop4int(rowFour, CONST_SZ_NUM, 3);

    //把左移后的行复制回状态矩阵中
    memcpy(byteArr + sizeof(byte) * CONST_SZ_NUM * 1, rowTwo, CONST_SZ_NUM);
    memcpy(byteArr + sizeof(byte) * CONST_SZ_NUM * 2, rowThree, CONST_SZ_NUM);
    memcpy(byteArr + sizeof(byte) * CONST_SZ_NUM * 3, rowFour, CONST_SZ_NUM);    

 }

byte AES_C::GFMul2(byte s){
    //get the number of first bit;
    byte a7 = s & 0x80;
    byte result = s << 1;
    if(a7 != 0){
        result = result ^ 0x1b;
    }
    return result;
}

byte AES_C::GFMul3(byte s){
    return GFMul2(s) ^ s;
}

byte AES_C::GFMul4(byte s) {
    return GFMul2(GFMul2(s));
}

byte AES_C::GFMul8(byte s) {
    return GFMul2(GFMul4(s));
}

byte AES_C::GFMul9(byte s) {
    return GFMul8(s) ^ s;
}

byte AES_C::GFMul11(byte s) {
    return GFMul9(s) ^ GFMul2(s);
}

byte AES_C::GFMul12(byte s) {
    return GFMul8(s) ^ GFMul4(s);
}

byte AES_C::GFMul13(byte s) {
    return GFMul12(s) ^ s;
}

byte AES_C::GFMul14(byte s) {
    return GFMul12(s) ^ GFMul2(s);
}

byte AES_C::GFMul(int n, byte s) {
    byte result = 0;
    switch(n){
        case 1:
            result = s;
            break;
        case 2:
            result = GFMul2(s);
            break;
        case 3:
            result = GFMul3(s);
            break;
        case 0x9:
            result = GFMul9(s);
            break;
        case 0xb:
            result = GFMul11(s);
            break;
        case 0xd:
            result = GFMul13(s);
            break;
        case 0xe:
            result = GFMul14(s);
            break;
        default:
            break;
    }
    return result;
}

void AES_C::mixColumn(byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM]){
    byte tempArray[CONST_SZ_NUM][CONST_SZ_NUM] = {0};

    memcpy(tempArray, byteArr, CONST_SZ_NUM_SQURE * sizeof(byte));

    for(int i = 0; i < CONST_SZ_NUM; i++){
        for(int j = 0; j < CONST_SZ_NUM; j++){
            byteArr[i][j] = GFMul(VAL::colM[i][0],tempArray[0][j]) ^ GFMul(VAL::colM[i][1],tempArray[1][j]) ^ GFMul(VAL::colM[i][2],tempArray[2][j]) ^ GFMul(VAL::colM[i][3], tempArray[3][j]);
        }
    }
}

void AES_C::addRoundKey(byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM], int round){
    for(int i = 0; i< CONST_SZ_NUM; i++){
        *((bytes4*)byteArr[i]) = *((bytes4*)byteArr[i]) ^ *((bytes4*)m_nKeyWArr[i + round * CONST_SZ_NUM]);
    }
}
