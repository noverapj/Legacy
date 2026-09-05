#pragma once

#define MAX_TOKEN_BUFFER_SIZE	32
#define TOKEN_SPLIT_CHAR		_T(' ')

enum BatchCommandType
{
	BCT_NONE = -1,
	BCT_PLAY = 0,
	BCT_STOP,
	BCT_SYNC,
	BCT_INSTALL,
	BCT_UNINSTALL,
	BCT_BACKUP,
	BCT_REMOVE,
	BCT_WAIT,
	BCT_QUIT,
	BCT_END,
};

enum BatchOptionType
{
	BOT_SERVICE_WAIT = 0,
	BOT_ALL_SERVICE,
	//BOT_WATCHDOG,
	//BOT_APPLYALL,
	//BOT_RECURSIVE,

	// 여기까지 비트처리 옵션들
	//====================
	// 여기부터 기타 옵션들
	BOT_TABINDEX,
	BOT_END,
};

enum CommandStateType
{
	CST_NONE = 0,
	CST_BEFORE_EXCUTE,
	CST_WAITING,
	CST_EXCUTE_DONE,
	CST_FAIL,
	CST_SYNC_CHECK,
	CST_ALREADY_EXCUTE,
	CST_UNKNOWN_COMMAND,	// 나와선 안될 오류
	CST_END,
};

enum CommandErrorType
{
	CET_NONE = 0,
	CET_NOERROR,
	CET_WRONG_COMMAND_LINE,			// 명령어 잘못됨
	CET_WRONG_NO_LAUNCHER,			// 런쳐 없음 - 나와선 안될 오류
	// common Error
	CET_ERROR_NO_COMMAND,			// 커맨드 없음
	CET_ERROR_NO_SERVICENAME,		// 서비스 이름 없음
	CET_ERROR_WRONG_COMMAND,		// 잘못된 명령어
	CET_ERROR_WRONG_SERVICENAME,	// 잘못된 서비스 이름
	CET_ERROR_WRONG_TABINDEX,		// 잘못된 탭인덱스	
	CET_ERROR_NO_NEED_SERVICENAME,	// '/a' 옵션과 서비스 이름 중첩

	// Play Error
	// Stop Error
	// Uninstall Error
	// Install Error
	CET_ERROR_INSTALL_NO_INIFILE,
	CET_ERROR_INSTALL_NO_EXEFILE,
	// Sync Error

	CET_END
};

#define BITON(x, y)		( x |= (1 << y) )
#define BITOFF(x, y)	( x &= !(1 << y ) )
#define BITCHK(x, y)	( x & (1 << y) )

typedef std::vector<int>		VECINT;
typedef std::vector<CString>	VECCSTRING;
