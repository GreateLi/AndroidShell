#ifndef _RC4_H_
#define _RC4_H_


class rc4
{
public:
   static void rc4_init(unsigned char*s, unsigned char*key, unsigned long Len);
   static void rc4_run(unsigned char*s, unsigned char*Data, unsigned long Len);
};



#endif

