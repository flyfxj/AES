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
                addRoundKey(byteArr, i + 1);
            }else{
                mixColumn(byteArr);
            }
        }
        //copy encode char array to origin c
        memcpy(p + k, byteArr, CONST_SZ_NUM_SQURE);
    }
}

bool AES_C::decode(char *p, char *key){
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

        for(int i = 10; i > 0 ; i--){
            if(10 == i){
                //another encode round.
                addRoundKey(byteArr, i + 1);
            }else{
                demixColumn(byteArr);
            }
            //row shift.
            deshiftRows(byteArr);
            //bytes exchange.
            desubBytes(byteArr);
            //a encode round.
            addRoundKey(byteArr, i);
        }
        //copy encode char array to origin c
        memcpy(p + k, byteArr, CONST_SZ_NUM_SQURE);
    }
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
    for(int i = 0; i < 44; i++){
        for(int j = 0; j < CONST_SZ_NUM; j ++)
        printf("0x%02x ", m_nKeyWArr[i][j] & 0xff);
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
    leftLoop(tmpByteArr,1);
    //byte exchange.
    for(int i = 0; i < 4 ; i++){
        tmpByteArr[i] = getByteFromSBox(tmpByteArr[i], true);
    }
    //get bytes4
    bytes4 result = 0;
    memcpy(&result, tmpByteArr, CONST_SZ_NUM);
    return result ^ VAL::Rcon[round];
}

//replace 1 byte
byte AES_C::getByteFromSBox(byte originByte, bool bCode){
    int row = (int)(originByte >> 4);
    int col = (int)(originByte & 0x0f);
    if(bCode){
        return VAL::S[row][col];    
    }else{
        return VAL::S1[row][col];
    }
}

void AES_C::replayceBytes(byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM], bool bCode){
    for(int i = 0; i < CONST_SZ_NUM; i++){
        for(int j = 0; j < CONST_SZ_NUM; j++){
            byteArr[i][j] = getByteFromSBox(byteArr[i][j], bCode);
        }
    }
}

//replace 4 byte
void AES_C::subBytes(byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM]){
    replayceBytes(byteArr, true);
}
//decode replace 4 byte
void AES_C::desubBytes(byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM]){
    replayceBytes(byteArr, false);
}

//left shift nShiftPos bytes
void AES_C::leftLoop(byte* byteArray, unsigned int nShiftPos){
    if(NULL == byteArray){
        return;
    }
    unsigned int index = nShiftPos % CONST_SZ_NUM;
    if(0 == index){
        return;
    }
    byte temp[CONST_SZ_NUM];
    memcpy(temp,byteArray,CONST_SZ_NUM);
    for(int i = 0; i < CONST_SZ_NUM; i++){
        byteArray[i] = temp[index];
        index++;
        index = index % CONST_SZ_NUM;
    }
}

void AES_C::shiftRow(byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM], int index, unsigned int shiftPos){
    byte row[CONST_SZ_NUM];
    memcpy(row, byteArr + sizeof(byte) * CONST_SZ_NUM * index, CONST_SZ_NUM);
    leftLoop(row, shiftPos);
    memcpy(byteArr + sizeof(byte) * CONST_SZ_NUM * index, row, CONST_SZ_NUM);

}
//shift Rows
void AES_C::shiftRows(byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM]) {
    //index = 0, no need to shift
    for(int i = 1; i < CONST_SZ_NUM; i ++){
        shiftRow(byteArr, i, (unsigned int)i);
    }
}

//decode shift Rows
void AES_C::deshiftRows(byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM]) {
    //index = 0, no need to shift
    for(int i = 1; i < CONST_SZ_NUM; i ++){
        shiftRow(byteArr, i, (unsigned int)(CONST_SZ_NUM - i));
    }
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

void AES_C::demixColumn(byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM]){
    byte tempArray[CONST_SZ_NUM][CONST_SZ_NUM] = {0};

    memcpy(tempArray, byteArr, CONST_SZ_NUM_SQURE * sizeof(byte));

    for(int i = 0; i < CONST_SZ_NUM; i++){
        for(int j = 0; j < CONST_SZ_NUM; j++){
            byteArr[i][j] = GFMul(VAL::decolM[i][0],tempArray[0][j]) ^ GFMul(VAL::decolM[i][1],tempArray[1][j]) ^ GFMul(VAL::decolM[i][2],tempArray[2][j]) ^ GFMul(VAL::decolM[i][3], tempArray[3][j]);
        }
    }
}


void AES_C::addRoundKey(byte byteArr[CONST_SZ_NUM][CONST_SZ_NUM], int round){
    for(int i = 0; i< CONST_SZ_NUM; i++){
        *((bytes4*)byteArr[i]) = *((bytes4*)byteArr[i]) ^ *((bytes4*)m_nKeyWArr[i + round * CONST_SZ_NUM]);
    }
}
