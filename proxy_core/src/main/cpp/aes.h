//
// Created by Administrator on 2020/3/27.
//

#ifndef DEXSHELL_AES_H
#define DEXSHELL_AES_H

#endif //DEXSHELL_AES_H
/************************************************************************/
/*AES：          AES-128,AES-192，AES-256
/*Author:       chenweiliang
/*Version:      1.0
/*Note:         The input data must be 128-bit.
/*              To see all information output,please uncommet the macro
/*              definition "#define DEBUG"
/************************************************************************/

#include <string.h>
#include <stdio.h>

#ifndef DEBUG
//#define DEBUG
#endif // !DEBUG

typedef unsigned char byte;

//length of secret key
enum KEYLENGTH
{
    KEY_LENGTH_16BYTES, KEY_LENGTH_24BYTES, KEY_LENGTH_32BYTES
};

class AES
{
public:
    AES();
    AES(const byte key[],enum KEYLENGTH keyBytes);
    virtual ~AES();
    void encrypt(const byte data[16], byte out[16]);
    void decrypt(const byte data[16], byte out[16]);
private:
    //
    int mNb;

    //word length of the secret key used in one turn
    int mNk;

    //number of turns
    int mNr;

    //the secret key,which can be 16bytes，24bytes or 32bytes
    byte mKey[32];

    //the extended key,which can be 176bytes,208bytes,240bytes
    byte mW[60][4];

    static byte sBox[];
    static byte invSBox[];
    //constant
    static byte rcon[];
    void setKey(const byte key[], const int keyBits);

    void subBytes(byte state[][4]);
    void shiftRows(byte state[][4]);
    void mixColumns(byte state[][4]);
    void addRoundKey(byte state[][4], byte w[][4]);

    void invSubBytes(byte state[][4]);
    void invShiftRows(byte state[][4]);
    void invMixColumns(byte state[][4]);

    void keyExpansion();

    //
    byte GF28Multi(byte s, byte a);

    void rotWord(byte w[]);
    void subWord(byte w[]);

    //get the secret key
    void getKeyAt(byte key[][4],int i);

#ifdef DEBUG
    void print128(byte state[16]);
    void print128(byte state[4][4]);
#endif // DEBUG
};
