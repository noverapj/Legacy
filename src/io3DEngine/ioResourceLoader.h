

#ifndef _ioResourceLoader_h_
#define _ioResourceLoader_h_

class ioStream;
class ioDataProcessor;
class ioPac;

#include "ioHashString.h"

struct LoadRequest
{
	std::string m_szFileName;
	ioStream *m_pStream;
	ioDataProcessor *m_pProcessor;
	void *m_pParam;
	bool m_bLoadPiece;
};

class __EX ioResourceLoader
{
protected:
	ioHashString m_DivideLoadingName;
	ioHashString m_DivideDeleteListToken;

	typedef std::deque< LoadRequest > RequestQueue;
	RequestQueue m_RequestQueue;

	CRITICAL_SECTION m_RequestSection;
	CRITICAL_SECTION m_LoadInternalLock;
	HANDLE m_hQueueSemaphore;
	HANDLE m_hLoadThread;

	bool m_bNowRunningThread;
	bool m_bLoadThreadDone;
	bool m_bDone;

protected:
	bool m_bLoadPackFile;
	bool m_bBeforeThreadLoading;

protected:
	ioHashString m_szLastFailedFileName;

public:
	void Initialize( bool bLoadInPack );
	bool AddPackFile( const char *szFileName , const char *szDeleteListToken);
	bool RemovePackFile( const char *szFileName );
	void ClearDeleteFileList();

public:	// 3개 한묶음 Begin으로 시작해서 End로 끝내줘야함
	bool BeginDividePackFile( const char *szPackFileName, int iDivideCount , const char *szDeleteListToken);
	bool LoadDividePackFile();
	void EndDividePackFile();

public:
	bool IsFileExist( const char *szFileName, bool bInPackOnly = false );

public:
	inline bool IsLoadInPack() const { return m_bLoadPackFile; }
	inline bool IsNowRunningThread() const { return m_bNowRunningThread; }

public:
	void ResumeLoadingThread();
	void StopLoadingThread();

private:
	void InitializeThreadObjects();
	void DestroyThreadObjects();

public:
	bool LoadStream( const char *szFileName,
					 ioStream *pStream,
					 ioDataProcessor *pProcessor = NULL,	// for ThreadLoading
					 void *pParam = NULL,					// for ThreadLoading
					 bool bLoadPiece = false );

private:
	bool LoadInternal( const char *szFileName, ioStream *pStream, bool bLoadPiece );

public:
	int LoadingThreadProc();

public:
	// for custom sound
	bool PacPieceFileDataInMem( IN const char *szFileName , OUT DWORD &rdwDataSize );
	bool GetPacPieceFileData( OUT BYTE *pPacData, IN DWORD dwPacSize );
	bool UnPacPieceFileByMem( IN const char *szFileName, IN BYTE *pPacData , IN DWORD dwDataSize );

public:
	const ioHashString &GetLastFailedFileName(){ return m_szLastFailedFileName; }
	void  ClearLastFailedFileName();

public:
	int GetPackFileMemSize( IN const char* szFileName );

public:
	static ioResourceLoader& GetInstance();
	static void ReleaseInstance();

protected:
	static ioResourceLoader *m_pLoader;

public:
	ioResourceLoader();
	~ioResourceLoader();
};

#define g_ResourceLoader ioResourceLoader::GetInstance()

#endif


