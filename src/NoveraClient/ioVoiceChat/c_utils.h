#ifndef UTILS_H
#define UTILS_H

#include "c_types.h"

typedef struct _biquad {

  S2byte negb12;
  S2byte a12;
  S2byte negb11;
  S2byte a11;
  S2byte a0;
  S2byte dn_2;
  S2byte dn_1;
  
} t_biquad;

#define abs16(data) ((data < 0) ? -data : data)
#define MIN_16 0x8000
#define MAX_16 0x7fff

//#define TRUE	true
//#define FALSE	false

extern S2byte sature16(S4byte data);
                         
extern void bqProcess (t_biquad *bq, S2byte *datain, S2byte *dataout, int n);

extern void bqInit (t_biquad *bq, S2byte a0, S2byte a1, S2byte a2, S2byte b1, S2byte b2); 
                       
extern void calcPower ( int length, U4byte *datain, float *logout); // formerly called log10_32()

#endif
