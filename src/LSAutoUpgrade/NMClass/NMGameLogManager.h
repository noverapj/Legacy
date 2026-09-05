#ifndef __NMGAMELOGMANAGER_H__4F99E9A2_DBE8_49a8_A65C_5D345EAA02BA___
#define __NMGAMELOGMANAGER_H__4F99E9A2_DBE8_49a8_A65C_5D345EAA02BA___

namespace NMGameLogManager
{
	BOOL	Initialize
	(
			IN LPCTSTR			szGameName				// 32
	,		IN LPCTSTR			szBaseURL				// 256
	,		IN LPCTSTR			szSessionID				// 32
	);

	BOOL	Finalize
	(
			void
	);

	BOOL	WriteStageLog
	(
			IN INT32			nStage
	,		IN LPCTSTR			szComment				// 256
	);

	BOOL	WriteErrorLog
	(
			IN INT32			nErrorType
	,		IN LPCTSTR			szComment				// 256
	);

	LPCTSTR	GetSessionID
	(
			IN	LPTSTR			szSessionIDOut			// 32
	);
}

#endif	//	#ifndef __NMGAMELOGMANAGER_H__4F99E9A2_DBE8_49a8_A65C_5D345EAA02BA___