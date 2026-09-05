
#ifndef _cryption_h_
#define _cryption_h_

IOCP_SOCKET_API void  Encrypt( BYTE *szSrc, int len );
IOCP_SOCKET_API void  Decrypt( BYTE *szSrc, int len );
IOCP_SOCKET_API DWORD MakeDigest( BYTE *szSrc, int len );
#endif