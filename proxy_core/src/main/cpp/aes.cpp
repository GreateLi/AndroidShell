//
// Created by Administrator on 2020/3/27.
//

/************************************************************************/
/*AES：          AES-128,AES-192，AES-256
/*Author:       chenweiliang
/*Version:      1.0
/*Note:         The input data must be 128-bit.
/************************************************************************/
#include "aes.h"

byte AES::sBox[] =
        { /*  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f */
                0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, /*0*/
                0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, /*1*/
                0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, /*2*/
                0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, /*3*/
                0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, /*4*/
                0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, /*5*/
                0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, /*6*/
                0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, /*7*/
                0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, /*8*/
                0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, /*9*/
                0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, /*a*/
                0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, /*b*/
                0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, /*c*/
                0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, /*d*/
                0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, /*e*/
                0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16  /*f*/
        };
byte AES::invSBox[] =
        { /*  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f  */
                0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, /*0*/
                0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, /*1*/
                0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, /*2*/
                0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, /*3*/
                0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, /*4*/
                0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, /*5*/
                0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, /*6*/
                0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, /*7*/
                0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, /*8*/
                0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, /*9*/
                0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, /*a*/
                0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, /*b*/
                0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, /*c*/
                0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, /*d*/
                0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, /*e*/
                0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d  /*f*/
        };

byte AES::rcon[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };

AES::AES()
{
    //If no key is input,use the default key
    int keyBits = 128;
    byte key[16] = {
            0x0f, 0x15, 0x71, 0xc9,
            0x47, 0xd9, 0xe8, 0x59,
            0x0c, 0xb7, 0xad, 0xd6,
            0xaf, 0x7f, 0x67, 0x98
    };
    setKey(key, keyBits);
    keyExpansion();
}

AES::AES(const byte key[],enum KEYLENGTH keyBytes){
    int keyBits;
    if (keyBytes == KEYLENGTH::KEY_LENGTH_16BYTES){
        keyBits = 128;
    }
    else if (keyBytes==KEYLENGTH::KEY_LENGTH_24BYTES){
        keyBits == 192;
    }
    else if (keyBytes==KEYLENGTH::KEY_LENGTH_32BYTES){
        keyBits == 256;
    }
    setKey(key,keyBits);
    keyExpansion();
}

void AES::setKey(const byte key[], const int keyBits){
    mNb = 4;
    if (keyBits == 128){
        mNk = 4;
        mNr = 10;
    }
    else if (keyBits == 192){
        mNk = 6;
        mNr = 12;
    }
    else if (keyBits == 256){
        mNk = 8;
        mNr = 14;
    }
    memcpy(mKey,key,mNk*4);
}

void AES::keyExpansion(){
    //the first mNk words will be filled in mW derictly
    for (int i = 0; i < mNk; i++){
        for (int j = 0; j < 4; j++){
            //arranged vertically
            mW[i][j] = mKey[j+i*4];
        }
    }

    //generate the secret key words
    for (int i = mNk; i < mNb*(mNr + 1); i++){
        //last secret key word
        byte pre_w[4];

        for (int k = 0; k < 4; k++){
            pre_w[k] = mW[i-1][k];
        }

        if(i%mNk == 0){
            rotWord(pre_w);
            subWord(pre_w);
            pre_w[0] = pre_w[0] ^= rcon[i / mNk - 1];
        }
        else if ((mNk>6)&&(i%mNk==4)){
            subWord(pre_w);
        }

        for (int k = 0; k < 4; k++){
            mW[i][k] = pre_w[k] ^ mW[i-mNk][k];
        }
    }

}

void AES::subBytes(byte state[][4])
{
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            state[i][j] = sBox[state[i][j]];
        }
    }
}

void AES::shiftRows(byte state[][4])
{
    byte t[4];
    for (int i = 1; i < 4; i++){
        for (int j = 0; j < 4; j++){
            t[j] = state[i][(i + j) % 4];
        }
        for (int j = 0; j < 4; j++){
            state[i][j] = t[j];
        }
    }
}

void AES::mixColumns(byte state[][4])
{
    byte t[4];
    for (int j = 0; j < 4; j++){
        for (int i = 0; i < 4; i++){
            t[i] = state[i][j];
        }
        for (int i = 0; i < 4; i++){
            state[i][j] = GF28Multi(t[i], 0x02)
                          ^ GF28Multi(t[(i + 1) % 4], 0x03)
                          ^ GF28Multi(t[(i + 2) % 4], 0x01)
                          ^ GF28Multi(t[(i + 3) % 4], 0x01);
        }
    }
}

void AES::addRoundKey(byte state[][4], byte w[][4])
{
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            state[i][j] ^= w[i][j];
        }
    }
}

void AES::invSubBytes(byte state[][4])
{
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            state[i][j] = invSBox[state[i][j]];
        }
    }
}

void AES::invShiftRows(byte state[][4])
{
    byte t[4];
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            t[j] = state[i][(j-i + 4) % 4];
        }
        for (int j = 0; j < 4; j++){
            state[i][j] = t[j];
        }
    }
}

void AES::invMixColumns(byte state[][4])
{
    byte t[4];
    //calculate columns by columns
    for (int j = 0; j < 4; j++){
        for (int i = 0; i < 4; i++){
            t[i] = state[i][j];
        }
        for (int i = 0; i < 4; i++){
            state[i][j] = GF28Multi(t[i], 0x0e)
                          ^ GF28Multi(t[(i+1)%4],0x0b)
                          ^ GF28Multi(t[(i+2)%4],0x0d)
                          ^ GF28Multi(t[(i+3)%4],0x09);
        }
    }
}



void AES::rotWord(byte w[])
{
    byte t;
    t = w[0];
    w[0] = w[1];
    w[1] = w[2];
    w[2] = w[3];
    w[3] = t;
}

void AES::subWord(byte w[])
{
    for (int i = 0; i < 4; i++){
        w[i] = sBox[w[i]];
    }
}

//calculate the least significant byte only for we only need
//the least significant byte
byte AES::GF28Multi(byte s,byte a){
    byte t[4];
    byte result = 0;
    t[0] = s;

    //calculate s*{02}，s*{03}，s*{04}
    for (int i = 1; i < 4; i++){
        t[i] = t[i - 1] << 1;
        if (t[i - 1] & 0x80){
            t[i] ^= 0x1b;
        }
    }
    //multiply a and s bit by bit and sum together
    for (int i = 0; i < 4; i++){
        if ((a >> i) & 0x01){
            result ^= t[i];
        }
    }

    return result;
}

void AES::encrypt(const byte data[16], byte out[16])
{
    byte state[4][4];
    byte key[4][4];
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            state[i][j] = data[i+j*4];
        }
    }
#ifdef DEBUG
    printf("plaintext：\n");
    print128(state);
    printf("--------------------------Round 0 begin:--------------------------\n");
#endif // DEBUG
    getKeyAt(key, 0);

    addRoundKey(state, key);
#ifdef DEBUG
    printf("addRoundKey：\n");
    print128(state);
#endif // DEBUG
    for (int i = 1; i <= mNr; i++){
#ifdef DEBUG
        printf("--------------------------Round %d begin：--------------------------\n",i);
#endif // DEBUG
        subBytes(state);
#ifdef DEBUG
        printf("subBytes：\n");
        print128(state);
#endif // DEBUG
        shiftRows(state);
#ifdef DEBUG
        printf("shiftRows：\n");
        print128(state);
#endif // DEBUG

        if (i != mNr){
            mixColumns(state);
#ifdef DEBUG
            printf("mixColumns：\n");
            print128(state);
#endif // DEBUG
        }
        getKeyAt(key, i);
        addRoundKey(state, key);
#ifdef DEBUG
        printf("addRoundKey：\n");
        print128(state);
#endif // DEBUG
    }

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            out[i+j*4] = state[i][j];
        }
    }
}

void AES::decrypt(const byte data[16], byte out[16])
{
    byte state[4][4];
    byte key[4][4];
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            state[i][j] = data[i+j*4];
        }
    }

    getKeyAt(key, mNr);
    addRoundKey(state,key);

    for (int i = (mNr - 1); i >= 0; i--){
        invShiftRows(state);
        invSubBytes(state);
        getKeyAt(key, i);
        addRoundKey(state,key);
        if (i){
            invMixColumns(state);
        }
    }

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            out[i + j * 4] = state[i][j];
        }
    }
}

#ifdef DEBUG
void AES::print128(unsigned char state[16]){
    for (int i = 0; i < 16; i++){
        printf("%02hhx ", state[i]);
    }
}

void AES::print128(unsigned char state[4][4]){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            printf("%02hhx ", state[i][j]);
        }
        printf("\n");
    }
}
#endif // DEBUG


//get the secret key for round "index",which will
//be arranged vetically
void AES::getKeyAt(byte key[][4], int index){
    for (int i = index*4; i < index*4+4; i++){
        for (int j = 0; j < 4; j++){
            key[j][i-index*4] = mW[i][j];
        }
    }
#ifdef DEBUG
    printf("secret key for round %d\n", index);
    print128(key);
#endif // DEBUG

}

AES::~AES()
{
}
