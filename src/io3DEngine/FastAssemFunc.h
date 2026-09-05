

#ifndef _FastAssemFunc_h_
#define _FastAssemFunc_h_

void SSEMemcpy( void *pDest, const void *pSrc, int iBytes );
void MMXMemcpy( void *pDest, const void *pSrc, int iBytes );

#endif