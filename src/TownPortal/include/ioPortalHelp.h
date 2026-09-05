

#ifndef _ioPortalHelp_h_
#define _ioPortalHelp_h_

namespace ioPortalHelp
{
	TOWN_PORTAL_API bool InitWinSock();
	TOWN_PORTAL_API void CloseWinSock();

	TOWN_PORTAL_API int CheckSocketError( LPSTR szFileName, int iLineNum );
};

#define MAX_BUFFER		32768 * 2         //8192 * 4
#define MAX_TEMP_BUFF	MAX_BUFFER

#endif