#ifndef VAD_H
#define VAD_H

#include "c_types.h"

extern void vadInit();
extern boolean vadProcess(S2byte *data, int length);

#endif
