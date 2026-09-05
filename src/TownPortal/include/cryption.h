
#ifndef _cryption_h_
#define _cryption_h_

TOWN_PORTAL_API void Encrypt(BYTE *szSrc, int len);
TOWN_PORTAL_API void Decrypt(BYTE *szSrc, int len);
TOWN_PORTAL_API DWORD MakeDigest( BYTE *szSrc, int len );
#endif