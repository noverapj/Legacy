
#ifndef _cryption_h_
#define _cryption_h_

void  Encrypt( BYTE *szSrc, int len );
void  Decrypt( BYTE *szSrc, int len );
DWORD MakeDigest( BYTE *szSrc, int len );
DWORD MakeDigestSHA256( BYTE *szSrc, int len );
#endif
