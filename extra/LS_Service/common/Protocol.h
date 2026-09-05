#pragma once



////////////////////////////////////////////////////////////////////////////////////////
// 네트워크 프로토콜 정의
////////////////////////////////////////////////////////////////////////////////////////
enum Protocol
{
	BEGIN_CLIENT_COMMAND = 0xa000,

	C_PING,						S_PING,

	C_SERVICES,					S_SERVICES,
	C_TRANSFER,					S_TRANSFER,
	C_REMOVE,					S_REMOVE,
	C_INSTALL,					S_INSTALL,
	C_PLAY,						S_PLAY,
	C_BACKUP,					S_BACKUP,
	C_REBOOT,					S_REBOOT,
	C_RESOURCE,					S_RESOURCE,

	END_CLIENT_COMMAND
};

#pragma pack(1)

const uint32 G_MAXFILESIZE = 1024 * 1024 * 5;


// 결과값
struct Result
{
	Result()
	{
		result = FALSE;
	}
	BOOL result;
};

// 핑
struct csPing
{
};

struct scPing
{
	scPing()
	{
		code = 0;
	}
	uint32 code;
};


// 서비스 목록요청
struct Service
{
	Service()
	{
		running = FALSE;
		console = FALSE;
		ZeroMemory(service, sizeof(service));
	}

	BOOL running;
	BOOL console;
	TCHAR service[32];
};

struct scServices
{
	scServices()
	{
		count = 0;
	}
	uint8 count;
};


// 파일 전송
enum TransferTypes
{
	TRANSFER_BEGIN = 1,
	TRANSFER_DO,
	TRANSFER_END
};

struct csTransfer
{
	csTransfer()
	{
		type	= 0;
		index	= 0;
	}
	uint8 type;
	uint8 index;
};

struct csTransferBegin : csTransfer
{
	csTransferBegin()
	{
		length	= 0;
		ZeroMemory(file, sizeof(file));
		ZeroMemory(folder, sizeof(folder));
	}
	uint32 length;
	TCHAR file[64];
	TCHAR folder[256];
	BYTE digest[16];
};

struct csTransferDo : csTransfer
{
	csTransferDo()
	{
		length	= 0;
	}
	uint32 length;
};

struct csTransferEnd : csTransfer
{
	csTransferEnd()
	{
		length = 0;
	}
	uint32 length;
};

struct scTransfer : Result
{
	scTransfer()
	{
		type	= 0;
		index	= 0;
	}
	uint8 type;
	uint8 index;
};

struct scTransferFail : scTransfer
{
	scTransferFail()
	{
		ZeroMemory(file, sizeof(file));
	}
	TCHAR file[256];
};

// 파일 삭제
struct csRemove
{
	csRemove()
	{
		ZeroMemory(folder, sizeof(folder));
	}
	TCHAR folder[256];
};

struct scRemove : Result
{
};


// 인스톨
struct Install
{
	Install()
	{
		install = TRUE;
		ZeroMemory(service, sizeof(service));
	}
	BOOL install;
	TCHAR service[256];
};

struct csInstall : Install
{
	csInstall()
	{
		ZeroMemory(file, sizeof(file));
		ZeroMemory(script, sizeof(script));
	}
	TCHAR file[256];
	TCHAR script[256];
};

struct csUninstall : Install
{
	csUninstall()
	{
		clear = FALSE;
	}
	BOOL clear;
};

struct scInstall : Result
{
	scInstall()
	{
	}
};

// 시작
struct Play
{
	Play()
	{
		play = TRUE;
		isDemon = FALSE;
		ZeroMemory(service, sizeof(service));
	}
	BOOL play;
	BOOL isDemon;
	TCHAR service[256];
};

struct csPlay : Play
{
	csPlay()
	{
	}
};

struct csStop : Play
{
};

struct scPlay : Result
{
	scPlay()
	{
	}
};


// 백업
struct csBackup
{
	csBackup()
	{
		ZeroMemory(source, sizeof(source));
		ZeroMemory(target, sizeof(target));
	}
	TCHAR source[256];
	TCHAR target[256];
};

struct scBackup : Result
{
};


// 윈도우 재부팅
struct csReboot
{
	csReboot()
	{
		option = 0;
	}
	uint32 option;
};

struct scReboot : Result
{
};

// 성능모니터
struct scResource : Result
{
	scResource()
	{
		cpu		= 0;
		disk	= 0;
		memory	= 0;
		network	= 0;
	}
	uint32 cpu;
	uint32 disk;
	uint32 memory;
	uint32 network;
};

#pragma pack()
