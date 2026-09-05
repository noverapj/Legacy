
//////////////////////////////////////////////////////////////////////
//
// circular_buffer.h: interface for the DNetCircularBuffer class.
//
//////////////////////////////////////////////////////////////////////


#ifndef __GSS_CIRCULARBUFFER_H__
#define __GSS_CIRCULARBUFFER_H__

#include "GFrame.h"

namespace nsGSS
{
	class GNetCircularBuffer
	{
	public:
		/************************************************************************/
		/* Contruction / Destruction                                            */
		/************************************************************************/
		GNetCircularBuffer(INT iBufferSize = MAX_BUF_SIZE);
		~GNetCircularBuffer(void);

	public:
		void				Clear(void);
		void				Rearrange(void);
		INT				Delete(INT iFlushSize);

		PBYTE		FindFirstDataPos(PBYTE pFind, INT iLen);

		static		INT		GetDataBuffer(GNetCircularBuffer *pDNetCirculaBuffer, PBYTE pBuffer, INT iLen);
		static		INT		PutDataBuffer(GNetCircularBuffer *pDNetCirculaBuffer, PBYTE pBuffer, INT iLen);
		static		INT		ViewDataBuffer(GNetCircularBuffer *pDNetCirculaBuffer, PBYTE pBuffer, INT iLen);
		static		INT		ViewDataBuffer(GNetCircularBuffer *pDNetCirculaBuffer, INT iStart, PBYTE pBuffer, INT iLen);

	protected:
		INT					GetData(PBYTE pBuffer, INT iLen);
		INT					PutData(PBYTE pBuffer, INT iLen);
		INT					ViewData(PBYTE pBuffer, INT iLen);
		INT					ViewData(INT iStart, PBYTE pBuffer, INT iLen);

	public:
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		__inline INT		GetDataSize(void)					{ return m_iDataSize; }
		__inline INT        GetReadDataSize(void)               { return m_iReadSize; }
		__inline INT        GetWriteDataSize(void)              { return m_iWriteSize;}
		__inline void			SetDataSize(INT iDataSize)			{ m_iDataSize = iDataSize; }

		__inline INT		GetBufferSize(void)					{ return m_iSize; }
		__inline void			SetBufferSize(INT iSize)		{ m_iSize = iSize; }

		__inline INT		GetEmptySpace(void)					{ return m_iSize - m_iDataSize; }

		__inline PBYTE			GetReadPos(void)					{ return m_pReadPos; }
		__inline PBYTE			GetWritePos(void)					{ return m_pWritePos; }

		__inline void		BufferActive()						{ m_bBufferActive = true; }
		__inline void		BufferDeactive()					{ m_bBufferActive = false; }
		__inline bool		IsBufferActive()					{ return m_bBufferActive; }
			
		/************************************************************************/
		/* Member Variables                                                     */
		/************************************************************************/
	public:
		class Lock			m_CLock;						// For Thread-Sync.
		PBYTE				m_pBuffer;						// Circular Buffer Pointer.

	protected:
		bool				m_bBufferActive;

	private :
		INT					m_iMaxSize;						// Maximum Buffer Size. No Use.
		INT					m_iSize;						// Total size of Circular Buffer now.
		INT					m_iDataSize;					// Total data size in the Circular Buffer now.
		INT                 m_iReadSize;
		INT                 m_iWriteSize;

		PBYTE					m_pReadPos;						// Read position in the buffer.
		PBYTE					m_pWritePos;					// Write position in the buffer.
	};

} // namespace nsGSS
#endif //__GSS_CIRCULARBUFFER_H__
#ifndef __GSS_GUID_GENERATOR_H__
#define	__GSS_GUID_GENERATOR_H__

#include "GFrame.h"

namespace nsGSS 
{
	//////////////////////////////////////////////////////////////
	//	싱글 쓰레드용 
	//
	//	1 Day /sec			: 86400
	//	1 Year /sec			: 31536000
	//	1 Year /60sec		: 52600
	//
	//	기준 시간 : 2010/01/01 00:00:00
	//
	//	Total 8 Byte ( 64bit)
	//	8 Bit	: Server ID			- 0~255
	//	20 Bit	: Time Serial		- 0~1,048,575
	//	12 Bit	: Serial Type		- 0~4,095
	//	24 Bit	: Count				- 0~16,777,215


	class CGUIDGenerator
	{
	public:
		CGUIDGenerator( int iServerID, int iIncreaseTerm = 10*60 );
		~CGUIDGenerator(void);

		void        ResetSerialData();

		BOOL		IncreaseTimeSerial();			// 알아서 호출해 줘야 함.
		DWORDLONG	GetGUID( int iSerialType );

		//--------------------------------------------------------------------
		//	inline Function
		__inline int		GetIncreaseTerm()				{ return INCREASE_TERM; }
		__inline int		GetServerID()					{ return m_iServerID; }
		__inline int		GetTimeSerial()					{ return m_iTimeSerial; }
		__inline int		GetTypeCount( int iSerialType ) { return m_arCount[iSerialType]; }

		//--------------------------------------------------------------------
		//	Static inline Function
		static __inline int		GetStandardTime()
		{
			return STANDARD_TIME;
		}

		static __inline time_t	GetTime( DWORDLONG ulGUID, int iIncreaseTerm )
		{
			return CGUIDGenerator::GetTimeSerial( ulGUID ) * iIncreaseTerm + CGUIDGenerator::GetStandardTime();
		}

		//--------------------------------------------------------------------
		//	Static Function
		static int		GetServerID( DWORDLONG ulGUID );
		static int		GetTimeSerial( DWORDLONG ulGUID );
		static int		GetSerialType( DWORDLONG ulGUID );
		static int		GetCount( DWORDLONG ulGUID );

	private:
		CGUIDGenerator();
		CGUIDGenerator& operator= (CGUIDGenerator& cGUIDGenerator);
		BOOL		InitInstance();
		int			CalcTimeSerial( time_t tTime );

	private:
		enum
		{
			STANDARD_TIME		= 1262271600,

			MAX_SERVER_ID		= 255,
			MAX_TIME_SERIAL		= 1048575,
			MAX_SERIAL_TYPE		= 4095,
			MAX_COUNT			= 16777215,

			SHIFT_CNT_SERVER_ID		= 56,
			SHIFT_CNT_TIME_SERIAL	= 36,
			SHIFT_CNT_SERIAL_TYPE	= 24,
			SHIFT_CNT_COUNT			= 0,
		};

		const int		INCREASE_TERM;			// 증가 기준 시간

		int				m_iServerID;
		int				m_iTimeSerial;
		//int				m_arCount[MAX_SERIAL_TYPE];
		volatile LONG   m_arCount[MAX_SERIAL_TYPE];
		volatile LONG   m_iSyncCount;

		class Lock		m_cLock;
	};
} //namespace nsGSS 
#endif // __GUID_GENERATOR_H__
#ifndef _LOCALDATAMANAGER_H_
#define _LOCALDATAMANAGER_H_

#include <iostream>
#include <fstream>

///////////////////////////////////////////////////////////////////
//	특정 자료 구조를 로컬 상의 주소에 Binary 타입으로 기록
//
//	Save 모드의 경우 데이터의 중간 수정이 불가능 하며
//	파일의 끝에 데이터를 추가하는것만 가능하다.
//

namespace nsGSS
	{

	template <typename Obj>
	class CLocalDataManager
	{
	public:
		CLocalDataManager(void);
		~CLocalDataManager(void);

		enum eLocalDataOpenType
		{
			eDATATYPE_NONE		= 0x0001,
			eDATATYPE_WRITE				,
			eDATATYPE_READ				,
		};

		BOOL	InitFileInfo(char* szDirectory, char* szFileName);
		BOOL	OpenBinaryFile(eLocalDataOpenType eType);
		BOOL	CloseBinaryFile();

		INT		Write(std::vector<Obj>* pvObjList);
		INT		Read(std::vector<Obj>* pvObjList);

		void	MakeDirectorys(char* szFullPath);
	private:
		char				m_szFileName[_MAX_FNAME];
		char				m_szDirectory[_MAX_DIR];
		char				m_szFullPath[_MAX_DIR+_MAX_FNAME];
		eLocalDataOpenType	m_eOpenType;
		std::fstream		m_hFile;

	};

	//////////////////////////////////////////////////////////////////////////

	template <typename Obj>
	CLocalDataManager<Obj>::CLocalDataManager(void)
	:m_eOpenType(eDATATYPE_NONE)
	{
		ZeroMemory(m_szFileName, sizeof(m_szFileName));
		ZeroMemory(m_szDirectory, sizeof(m_szDirectory));
		ZeroMemory(m_szFullPath, sizeof(m_szFullPath));
	}

	template <typename Obj>
	CLocalDataManager<Obj>::~CLocalDataManager(void)
	{

	}

	template <typename Obj>
	void CLocalDataManager<Obj>::MakeDirectorys(char* szFullPath)
	{
		char temp[_MAX_DIR], *pChar;
		strcpy(temp, szFullPath);
		pChar = temp; 

		while((pChar = strchr(pChar, '\\'))) 
		{
			if(pChar > temp && *(pChar - 1) != ':') 
			{
				*pChar = '\0';
				CreateDirectory(temp, NULL);
				*pChar = '\\'; 
			}
			pChar++;
		}
	}

	template <typename Obj>
	BOOL CLocalDataManager<Obj>::InitFileInfo(char* szDirectory, char* szFileName)
	{
		// 인자가 NULL이 거나 파일이 열린경우는 셋팅 불가
		if( NULL == szDirectory || NULL == szFileName || true == m_hFile.is_open())
		{
			return FALSE;
		}

		strcpy(m_szDirectory, szDirectory);
		strcpy(m_szFileName, szFileName);
		sprintf(m_szFullPath, "%s\\%s", m_szDirectory, m_szFileName);

		MakeDirectorys(m_szFullPath);

		return TRUE;
	}


	template <typename Obj>
	BOOL CLocalDataManager<Obj>::OpenBinaryFile(eLocalDataOpenType eType)
	{
		// 파일이 이미 열린경우는 열기 불가
		if( eDATATYPE_NONE == eType || true == m_hFile.is_open())
		{
			return FALSE;
		}

		// ios::app 		파일 끝에 데이터를 덧붙인다. 데이터를 추가하는 것만 가능하다.
		// ios::ate	파일을 열자 마자 파일 끝으로 FP를 보낸다. FP를 임의 위치로 옮길 수 있다.

		m_eOpenType = eType;

		if( eDATATYPE_READ == m_eOpenType )
		{
			m_hFile.open(m_szFullPath, std::ios::in | std::ios::binary);
		}
		else if( eDATATYPE_WRITE == m_eOpenType )
		{
			m_hFile.open(m_szFullPath, std::ios::out | std::ios::binary | std::ios::app);
		}
		else
		{
			return FALSE;
		}

		if( false == m_hFile.is_open() )
		{
			m_eOpenType = eDATATYPE_NONE;

			return FALSE;
		}

		return TRUE;
	}

	template <typename Obj>
	BOOL CLocalDataManager<Obj>::CloseBinaryFile()
	{
		// 파일이 닫힌 경우는 닫기 불가
		if( false == m_hFile.is_open() )
		{
			return FALSE;
		}

		m_hFile.close();
		m_eOpenType	= eDATATYPE_NONE;

		return TRUE;
	}

	template <typename Obj>
	INT CLocalDataManager<Obj>::Write(std::vector<Obj>* pvObjList)
	{
		// 파일이 읽기 모드로 열린 경우 쓰기 불가
		if( eDATATYPE_READ == m_eOpenType || false == m_hFile.is_open())
		{
			return FALSE;
		}

		int iWriteCount = 0;
		std::vector<Obj>::iterator iter;
		for(iter = pvObjList->begin(); iter != pvObjList->end(); iter++)
		{
			m_hFile.write(reinterpret_cast<char *>(&(*iter)), sizeof(Obj));
			++iWriteCount;
		}

		return iWriteCount;
	}

	template <typename Obj>
	INT CLocalDataManager<Obj>::Read(std::vector<Obj>* pvObjList)
	{
		// 파일이 쓰기 모드로 열린 경우 읽기 불가
		if( eDATATYPE_WRITE == m_eOpenType || false == m_hFile.is_open())
		{
			return FALSE;
		}

		int iReadCount = 0;
		Obj	stTemp;
		while( false == m_hFile.eof() )
		{
			m_hFile.read(reinterpret_cast<char *>(&stTemp), sizeof(stTemp));
			if( 0 == m_hFile.gcount())
			{
				break;
			}

			pvObjList->push_back(stTemp);
			++iReadCount;
		}

		return iReadCount;
	}
}
#endif // #if !defined _LOCALDATAMANAGER_H_
#ifndef _MATCH_NODE
#define _MATCH_NODE

//#include "GOS.h"

namespace nsGSS
{

	//////////////////////////////////////////////////////////////////////////////
	/// 자식 노드의 구분은 비트값으로 이루어진다.
	//////////////////////////////////////////////////////////////////////////////
	// 
	class CMatchWordNode : public stdext::hash_map<size_t, CMatchWordNode*>
	{

	private:
		int m_iWordNumber; ///< 이 노드가 단어의 끝이 될 수 있는가? -1일 경우 아니다..


	public:
		/// 생성자
		CMatchWordNode() : m_iWordNumber(-1) 
		{

		}

		/// 소멸자
		~CMatchWordNode() 
		{ 
			/* 모든 자식 노드를 삭제 */
			// ToDo : Add Code to Delete All Child Node
		}

	public:
		/// 비트값을 이용해 해당하는 자식 노드를 찾는다.
		/// idx 찾고자 하는 자식 노드의 비트값
		/// CMatchWordNode* 해당하는 자식 노드가 존재할 경우 그 노드의 
		/// 포인터를 반환하고, 존재하지 않을 경우 NULL을 반환한다.
		CMatchWordNode* FindChild(size_t idx) const
		{
			const_iterator itr(find(idx));
			return itr != end() ? itr->second : NULL;
		}

		/// 해당하는 비트값의 자식 노드를 추가한다.
		/// idx 추가하고자 하는 자식 노드의 비트값
		/// CMatchWordNode* 새로 생성한 자식 노드의 포인터
		CMatchWordNode* AddChild(size_t idx)
		{
			// 해당하는 자식이 없을 경우, 새로운 노드를 생성해서 추가한다.
			iterator itr(find(idx));

			if ( itr == end() ) 
				itr = insert(value_type(idx, new CMatchWordNode)).first;

			return itr->second;
		}

		/// 단어의 끝 여부를 반환/설정
		const int   IsLeafNode()            { return m_iWordNumber; }
		void        SetLeafNode(int value)  { m_iWordNumber = value; }
	};
}
#endif  // _MATCH_NODE

#ifndef _MINIDUMP_H
#define _MINIDUMP_H

#ifdef WIN32

#include "GFrame.h"

namespace nsGSS
{

	class CMiniDump : public nsGSS::Singleton<CMiniDump>
	{
		 friend class nsGSS::Singleton<CMiniDump>;

	protected:
		CMiniDump();

	public:
		inline void SetFilePath( char *chFilePath ) 
		{ 
			strcpy( m_chTempFilePath, chFilePath );
			m_iTempFilePathLength = (int)strlen(m_chTempFilePath);

			if( m_chTempFilePath[m_iTempFilePathLength - 1] != '\\' )
			{
				strcat( m_chTempFilePath, "\\" );
			}
			m_iTempFilePathLength = (int)strlen(m_chTempFilePath);
		} 
		inline void SetFileName( char *szFileName ) 
		{ 
			strcpy( m_chTempFileName, szFileName ); 
			m_iTempFileNameLength = (int)strlen(m_chTempFileName);
		}
		inline void SetDumpType( MINIDUMP_TYPE eType )
		{
			m_eType = eType;
		}

		inline char *GetFileName()                  { return m_chTempFileName; }
		inline char *GetFilePath()                  { return m_chTempFilePath; } 
		inline int  GetPathLen()                    { return m_iTempFilePathLength; }
		inline int  GetFileLen()                    { return m_iTempFileNameLength; }
	
		BOOL        Begin(VOID);
		BOOL        End(VOID);

		int 		MakeDump(struct _EXCEPTION_POINTERS *exceptionInfo);

	protected:
		int m_iTempFileNameLength;
		int m_iTempFilePathLength;
		char m_chTempFileName[MAX_PATH];
		char m_chTempFilePath[MAX_PATH];

		MINIDUMP_TYPE m_eType;
	};
} // namespace nsGSS
#endif

#endif // !defined _MINIDUMP_H

//////////////////////////////////////////////////////////////////////
//
// DNetLog.h: interface for the DNetLog class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GSS_LOG_H__
#define __GSS_LOG_H__

#include "GFrame.h"

namespace nsGSS
{

	#define DEFAULT_LOG_PATH            _T("Log")		///<	기본 로그 저장 폴더명
	#define DEFAULT_LOG_DUPLICATE_PATH  _T("Bygone")	///<	기본 로그 백업 폴더명	
	#define DEFAULT_LOG_EXT             _T("txt")		///<	기본 로그 확장자명	

	#define NO_CONSOLE      false					
	#define NO_LOGFILE      false
	#define NO_LINEOVER     false


	#define MAX_LOG_BUFFER_SIZE 8192
	#define MAX_LOG_LINE        100000
	#define MAX_MSG_LEN         1024

	enum _LOG_LEVEL
	{
		///<	로그의 종류
		SYSERR_LEV          = 1,					///<	시스템 레벨에 에러
		ERROR_LEV           = 2,           			///<	로직상에 에러.
		INFO_LEV            = 3,           			///<	일반적인 정보.
		DEBUG_LEV           = 4,           			///<	디버그 할수있을 정도의 로그.
	};

	#define SYSERR_LEVEL_NAME	_T("SYSERR  : ")		///<	_LOG_LEVEL에 따른 Prefix들의 정의
	#define ERROR_LEVEL_NAME	_T("ERROR   : ")
	#define INFO_LEVEL_NAME 	_T("INFO    : ")
	#define DEBUG_LEVEL_NAME	_T("DEBUG   : ")
	#define UNKNOWN_LEVEL_NAME	_T("UNKNOWN : ")


	enum	_LOG_TYPE
	{
		///<	라이브러리쪽 에러인지( LIBLOG() 사용 ), 컨텐츠쪽 에러인지 구분하기 위한 값( LIB() 사용 )
		LIBRARY_TYPE		=	1,
		CONTENTS_TYPE		=	2,
	};

	#define	LIBRARY_TYPE_NAME	"LIBRARY  : "		///< _LOG_TYPE에 따른 Profix들의 정의	
	#define CONTENTS_TYPE_NAME	"CONTENTS : "
	#define UNKNOWN_TYPE_NAME	"UNKNOWN  : "

	//-------------------------------------------------------------------------------------
	//! 클래스로 지칭한다..
	//-------------------------------------------------------------------------------------
	enum _LOG_CLASS
	{
		WHOLE					= 0,    ///<	일반적인 전체 로그..
		
		//CHARACTER				= 1,    ///<	캐릭터에 관련된 로그..
		//LOGIN					= 2,    ///<	로그인에 관련된 로그..
		//CUSTOM					= 3,    ///<	이 영역부터 커스텀으로 한다..

		MAX_LOG_CLASS			= 10,   ///<	최대 로그 클래스 값
	};

	#define WHOLE_CLASS_NAME		_T("WHOLE")			///< 로그 클래스 종류에 따른 파일명 조합에 사용될 정의 
	#define CHARACTER_CLASS_NAME	_T("CHARACTER")
	#define LOGIN_CLASS_NAME		_T("LOGIN")
	#define CUSTOM_CLASS_NAME		_T("CUSTOM")
	#define UNKNOWN_CLASS_NAME		_T("UNKNOWN")

	#define GNetLOG			CGNetLog::getInstance()->LOG	///<	Contents쪽 로그 지원 함수
	#define GNetLIBLOG		CGNetLog::getInstance()->LIBLOG	///<	Library쪽 로그 지원 함수


	class CLogInfo
	{
	public:
		CLogInfo();
		~CLogInfo();

	public:
		__inline bool	IsEmpty()	{ return NULL == m_pFilePoint; }

		__inline void	SetFileActiveState( bool bActive )	{ m_bActiveWriteFile = bActive; }
		__inline bool	FileActiceState()	{ return m_bActiveWriteFile; }
		__inline void	SetConsoleActiveState( bool bActive )	{ m_bActiveWriteConsole = bActive; }
		__inline bool	ConsoleActiveState()	{ return m_bActiveWriteConsole; }
		
		__inline void	SetClassName( const TCHAR* szName )	{ _tcscpy_s( m_szClassName, _countof(m_szClassName), szName ); }
		__inline const TCHAR*	ClassName()	{ return m_szClassName; }
		__inline void	SetFileName( const TCHAR* szName )	{ _tcscpy_s( m_szFileName, _countof(m_szFileName), szName ); }
		__inline const TCHAR*	FileName()	{ return m_szFileName; }
		__inline void	SetLogClassName( const TCHAR* szName )	{ _tcscpy_s( m_szLogClassName, _countof(m_szLogClassName), szName ); }
		__inline const TCHAR*	LogClassName()	{ return m_szLogClassName; }

		__inline int	LineCount()	{ return m_iLineCount; }
		__inline int	FileCount()	{ return m_iFileCount; }
		__inline int	GetAndIncreaseFileCount()	{ return m_iFileCount++; }

		__inline LONG	FileCreateCheck()	{ return InterlockedCompareExchange( &m_iFileCreateCheck, 0, 1 ); }

	public:
		bool	CreateFileName();
		bool	OpenFPAndCloseOldFP( const TCHAR* szFileFullPath );
		bool	WriteToFile( const TCHAR* szLog );

	private:
		int		m_iFileCount;
		LONG	m_iLineCount;
		LONG	m_iFileCreateCheck;
		bool	m_bActiveWriteFile;
		bool	m_bActiveWriteConsole;

		TCHAR	m_szClassName[MAX_PATH];	// 파일 타입 (ex- WHOLE, LOGIN... )
		TCHAR	m_szFileName[MAX_PATH];		// 파일 이름 (ex- DNetLog_20100108125714.txt )
		TCHAR	m_szLogClassName[MAX_PATH];

		FILE	*m_pFilePoint;
		FILE	*m_pOldFilePoint;

	};

	class CGNetLog : public nsGSS::Singleton<CGNetLog>
	{
		/**
			Log로 사용할 모듈 기본적으로 콘솔쓰기와 파일쓰기로 구분하며..
			콘솔은 TRACE와 연결되어있다..
		*/
		
		friend class nsGSS::Singleton<CGNetLog>;

	protected:
		CGNetLog();
		~CGNetLog();

	public:        
		/**
			@사용법
			
			case 1)
			SetLogFile("TEST", _iLogClass ) => LOG( _iLogClass, INFO_LEV, "LogTest");

			case 2)
			iLogClass = SetLogFile("TEST", "UNA") => LOG( iLogClass, INFO_LEV, "LogTest");
			
			=> 두번째 인자값에 따라 사용법 다름 주의~!		
		*/

		int			SetFilePath( const TCHAR *szLogFolder );
		int         SetLogFile( TCHAR *szClassName, unsigned int iLogClass, bool bActiveFile = true, bool bActiveConsole = true );	///<	로그파일을 사용하기위해 기본 호출 함수
		int			SetLogFile( TCHAR *szClassName, TCHAR* szLogClass, bool bActiveFile = true, bool bActiveConsole = true );	
	    
		__inline void SetAllowLogLevel(int iLogLevel)                     { m_iAllowLogLevel = iLogLevel; }							///<	허용된 레벨 이하의 로그들만 들어온다..
		__inline int  GetAllowLogLevel()                                  { return m_iAllowLogLevel; }

		__inline bool IsValiedLogClass( unsigned int iLogClass ) {  return  MAX_LOG_CLASS > iLogClass; }

	public:
		int     LOG( unsigned int iLogClass, int iLogLevel ,const TCHAR *format, ... );		///<	contents 전용 
		int		LIBLOG( unsigned int iLogClass, int iLogLevel ,const TCHAR *format, ... );	///<	library 전용
		
	protected:
		int     LOG( TCHAR *szMsgBuf, unsigned int iLogClass, int iLogLevel, int iLogType );

		void    WriteLogPrefix(int iLogLevel, TCHAR *szBuf, TCHAR* szOrgLog , int iLogType);	

	protected:
		bool     CreateLogFile( unsigned int iLogClass );
	    
		/**
			Log 폴더 안의 .txt 파일 찾아서 백업 폴더로 이동 시켜주는 함수
		*/
		void    ProcessingDuplicateFromLastFile( const TCHAR* szClassName, const TCHAR* szLogClass );				///<	기존 파일명 체크해서 중복되는 파일을 백업 폴더로 이동
		void    ProcessingDuplicateFromSameFile( TCHAR *szKeepLogFile, unsigned int iLogClass);	///<	중복되는 파일명 변경
		void	MakeNextLogFileName( TCHAR *szKeepLogFile, unsigned int iLogClass );

	private:
		const TCHAR*	GetBaseLogClassName( int iLogClass );
		int		FindEmptyLogClass();

	protected:
		int     m_iAllowLogLevel;
		TCHAR	m_szPath[MAX_PATH];
		class CLogInfo	m_aLogInfo[MAX_LOG_CLASS];
	};

}// namespace nsGSS

#endif //__GSS_LOG_H__

#ifndef __GSS_RANDINT_H__
#define __GSS_RANDINT_H__

namespace nsGSS 
{
	
	#define MAX_DOUBLE 2147483648.0

	class CRandInt
	{
		// uniform distribution, assuming 32-bit long
		unsigned long randx;
	public:

		CRandInt( long s=0 );
		void   seed( long s );
		long   abs( long x );
		long   draw();
		double   fdraw();
		long   operator()();
	};
}

#endif //__GSS_RANDINT_H__
#ifndef __GSS_RUNNINGTIME_H__
#define __GSS_RUNNINGTIME_H__

#include "GFrame.h"

namespace nsGSS 
{

	class CRunningTime
	{
	public:
    
		enum _VIEW_
		{
			LOG         = 0x0001,
			SCREEN      = 0x0010,
			LOG_SCREEN  = 0x0011,
		};

		CRunningTime(unsigned int iIgnoreTime = 0, CRunningTime::_VIEW_ eView = CRunningTime::SCREEN, const TCHAR *szLogStr = _T("Default"));
		~CRunningTime(void);

	public:
		void                    ShowTime();

		__inline void           ReStart()     { gettimeofday(&m_stStart, NULL); m_iTime = 0; }
		__inline void           StopNow()     { gettimeofday(&m_stFinish, NULL); m_iTime = TIMER_MINUS(m_stFinish, m_stStart); }
		__inline unsigned int   GetTime()     { return m_iTime; }

	protected:
		CRunningTime::_VIEW_    m_eView;
		struct timeval          m_stStart, m_stFinish;

		unsigned int			m_iIgnoreTime;
		unsigned int            m_iTime;
		TCHAR                    m_szLogStr[BUFSIZ];
	};
} //namespace nsGSS 

#endif // #if !defined __RUNNINGTIME_H__
///////////////////////////////////////////////////////////////////////////////
// Header File Include.
#ifndef __GSS_COMMON_H__
#define __GSS_COMMON_H__

namespace nsGSS
{

	typedef unsigned char   byte;
	typedef unsigned char   uint8;
	typedef unsigned short  uint16;
	typedef unsigned long   uint32;
	typedef unsigned int    uint;
	typedef signed char     int8;
	typedef signed short    int16;
	typedef signed long     int32;


	//-------------------------------------------------
	// SHA1 타입
	//-------------------------------------------------
	#define SHA1_STR    1
	#define SHA1_HEX    2
	#define SHA1_B64    3

	class CSimpleCommonUtil 
	{
	public:
	#ifdef _GCC
		static int write_text(FILE *fd, char *buf);
	#endif
		static char *date_time_string(char *buf);
		static void gettimeofday_mini(struct timeval *pTime, struct timezone *pZone);
		static void time_diff(struct timeval *rslt, struct timeval *a, struct timeval *b);

		static int  parse_string_from_text(char *szDest, int iStartPoint, char *szBuf, char *szToken, int iBufLen);
		static int  skip_space_in_text(char *szResult, int iStartPoint, char *szBuf, int iBufLen);
		static int  clear_space_in_text(char *szResult, int iStartPoint, char *szBuf, int iBufLen);
		static int  clear_space_in_text_from_end_point(char *szBuf);
		static int  clear_front_space_in_text(char *szResult, char *szBuf, int iBufLen);
		static int  change_space_in_text(char *szResult, int iStartPoint, char *szBuf, int iBufLen, char cChangeChar);
		static int  to_upper_in_text(int iStartPoint, char *szBuf, int iBufLen);
		static int  to_upper_one_in_text(int iPoint, char *szBuf, int iBufLen);
		static int  use_only_alphabet_in_string(char *szBuf, int iBufLen);

		static int  filedb_cvs_fetch_row(char *szSrc, int iSrcLen, int iRowCount, char *szDest, int iDestBufLen);
		static int  filedb_get_field_name(char *szFieldName, char *szBuf);
		static int  filedb_get_field_data(char *szBuf, int iCount, char *szData);

		static int  safe_strcpy(char *szDest, const char *szSrc, int iMaxLen = 0);

		static SYSTEMTIME	convert_string_to_systemtime( std::string strDate );
		static time_t		convert_string_to_time_t( std::string strData );
		static std::string	convert_time_t_to_string( time_t tTime, std::string strFormat );
		static std::string	convert_systemtime_to_string( SYSTEMTIME stSystemTime, std::string strFormat );

	private:
		static BOOL	erase_format_in_string( std::string *strString, const std::string *stdForamt);

		//----------------------------------------------------------------------------------
		// MD5
		//----------------------------------------------------------------------------------
	public:
		struct  md5_context
		{
			int32   total[2];
			uint32  state[4];
			uint8   buffer[64];
		};

		static bool md5_file( const char *name, uint8 digest[16] );
		static bool md5_file( const char *name, uint8 digest[16], int iFrontSize );
		static void md5_mem( const char *str, uint32 length, uint8 digest[16] );

	protected:
		static void md5_process( md5_context *ctx, const uint8 data[64] );
		static void md5_starts( md5_context *ctx );
		static void md5_update( md5_context *ctx, const uint8 *input, uint32 length );
		static void md5_finish( md5_context *ctx, uint8 digest[16] );

		static uint16 READ_LE_UINT16(const void *ptr);
		static uint32 READ_LE_UINT32(const void *ptr);
		static void WRITE_LE_UINT16(void *ptr, uint16 value);
		static void WRITE_LE_UINT32(void *ptr, uint32 value);




		//----------------------------------------------------------------------------------
		// SHA1
		//----------------------------------------------------------------------------------
		static __inline void CoreSHA1(unsigned int *aX, int iaXLen, int iStrBitLen, unsigned int *aData)
		{
			unsigned int   w[80];

			/* append padding */
			aX[iStrBitLen >> 5] |= 0x80 << (24 - iStrBitLen % 32);
			aX[( ( ( iStrBitLen + 64 ) >> 9 ) << 4 ) + 15] = iStrBitLen;

			aData[0] =  1732584193;
			aData[1] = -271733879;
			aData[2] = -1732584194;
			aData[3] =  271733878;
			aData[4] = -1009589776;

			for(int i = 0; i < iaXLen /*x.length*/; i += 16)
			{
				int olda = aData[0];
				int oldb = aData[1];
				int oldc = aData[2];
				int oldd = aData[3];
				int olde = aData[4];

				for(int j = 0; j < 80; j++)
				{
					if(j < 16) 
						w[j] = aX[i + j];
					else 
						w[j] = RotateLeft(w[j-3] ^ w[j-8] ^ w[j-14] ^ w[j-16], 1);

					int t = SafeAdd(
						SafeAdd( RotateLeft(aData[0], 5), FTSha1(j, aData[1], aData[2], aData[3]) ), 
						SafeAdd( SafeAdd(aData[4], w[j]), KTSha1(j) )
						);
					aData[4] = aData[3];
					aData[3] = aData[2];
					aData[2] = RotateLeft(aData[1], 30);
					aData[1] = aData[0];
					aData[0] = t;
				}

				aData[0] = SafeAdd(aData[0], olda);
				aData[1] = SafeAdd(aData[1], oldb);
				aData[2] = SafeAdd(aData[2], oldc);
				aData[3] = SafeAdd(aData[3], oldd);
				aData[4] = SafeAdd(aData[4], olde);
			}
		}

		/*
		* Bitwise rotate a 32-bit number to the left.
		*/
		static __inline unsigned int RotateLeft(unsigned int iNum, int iCnt)
		{
			return (iNum << iCnt) | (iNum >> (32 - iCnt));
		}
		static __inline int SafeAdd( int x, int y)
		{
			int lsw = (x & 0xFFFF) + (y & 0xFFFF);
			int msw = (x >> 16) + (y >> 16) + (lsw >> 16);
			return (msw << 16) | (lsw & 0xFFFF);
		}

		/*
		* Perform the appropriate triplet combination function for the current
		* iteration
		*/
		static __inline int FTSha1( int t, int b, int c, int d)
		{
			if(t < 20) 
				return (b & c) | ((~b) & d);
			if(t < 40) 
				return b ^ c ^ d;
			if(t < 60) 
				return (b & c) | (b & d) | (c & d);

			return b ^ c ^ d;
		}
		/*
		* Determine the appropriate additive constant for the current iteration
		*/
		static __inline int KTSha1( int t)
		{
			return (t < 20) ?  1518500249 : (t < 40) ?  1859775393 :
				(t < 60) ? -1894007588 : -899497514;
		}  
		static __inline void Binb2Str(unsigned int *aBin, int iLen, std::string &str)
		{
			int mask = (1 << 8) - 1;

			for(int i = 0; i < iLen * 32; i += 8)
			{
				char szTemp[10];
				sprintf( szTemp, "%c", (aBin[i>>5] >> (24 - i%32)) & mask );
				str += szTemp;
			}
		}
		static __inline void Binb2B64(unsigned int *aBin, int iLen, std::string &str)
		{
			std::string tab = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        
			for( int i = 0; i < iLen * 4; i += 3)
			{
				int iTriplet = (((aBin[ i >> 2] >> 8 * ( 3 - i % 4 )) & 0xFF) << 16)
								| (((aBin[ (i + 1) >> 2] >> 8 * (3 - ( i + 1 ) % 4)) & 0xFF) << 8 )
								|  ((aBin[ (i + 2) >> 2] >> 8 * (3 - ( i + 2 ) % 4)) & 0xFF);
				for( int j = 0; j < 4; j++)
				{
					if(i * 8 + j * 6 > iLen * 32) 
						str += "";
					else 
						str += tab[ (iTriplet >> 6*(3-j)) & 0x3F ];
				}
			}
		}

		static __inline void Binb2Hex(unsigned int *aBin, int iLen, std::string &str)
		{
			bool bUpper = true;
			std::string hex_tab = bUpper ? "0123456789ABCDEF" : "0123456789abcdef";
        
			for( int i = 0; i < iLen * 4; i++)
			{
				char szTemp[10];
				sprintf(szTemp, "%c%c", hex_tab[ (aBin[i>>2] >> ((3 - i%4)*8+4)) & 0xF ], hex_tab[ (aBin[i>>2] >> ((3 - i%4)*8  )) & 0xF ] );

				str += szTemp;
			}
		}
    
		static __inline int Str2Binb(const char *szStr, unsigned int *aBin)
		{
			int iLen    = (int)strlen(szStr);
			int iMax    = iLen * 8;
			int mask    = (1 << 8) - 1;

			for(int i = 0; i < iMax ; i += 8)
				aBin[i >> 5] |= (szStr[i / 8] & mask) << (24 - i%32);

			return 1;
		}

	public:
		static __inline void GetSHA1( const char *szString, std::string &strReturn, int iType )
		{
			int iStrLen         = (int)strlen(szString);
			int iStrBitLen      = iStrLen * 8;
			int iASize          = ( ( ( iStrBitLen + 64 ) >> 9 ) << 4) + 15 + 1;

			unsigned int *aBin = NULL;
			unsigned int aData[5];

			// String에 대한 Array를 만든다..    
			aBin = new unsigned int[ iASize ];
			ZeroMemory( aBin, sizeof(unsigned int) * iASize );

			// 
			Str2Binb( szString, aBin );
			CoreSHA1( aBin, iASize, iStrBitLen, aData );

			switch( iType)
			{
				case SHA1_STR :
					Binb2Str( aData, 5, strReturn );
					break;
				case SHA1_HEX :
					Binb2Hex( aData, 5, strReturn );
					break;
				case SHA1_B64 :
					Binb2B64( aData, 5, strReturn );
					break;
			}
			delete [] aBin;
		}
	};
} // namespace nsGSS

#endif //__GSS_COMMON_H__



//////////////////////////////////////////////////////////////////////
//
// SimpleConsole.h: interface for the SimpleConsole class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GSS_SIMPLECONSOLE_H__
#define __GSS_SIMPLECONSOLE_H__

//#include "GFrame.h"
#include <richedit.h>
#include <iostream>

namespace nsGSS
{

	typedef struct tagTraceContext
	{
		int		iLogClass;
		TCHAR	szTraceStr[MAX_GSS_LOADSTRING * 3];

		struct tagTraceContext	*pNext;
		struct tagTraceContext	*pPrev;

		tagTraceContext()
		{
			iLogClass = 0;
			
			pNext = NULL;
			pPrev = NULL;
		}

	}TraceContext, *PTraceContext;

	template<typename T> class CSMemoryPoolLIB;

	class CSimpleConsole : public nsGSS::Singleton<CSimpleConsole>
	{
		friend class nsGSS::Singleton<CSimpleConsole>;

	public:
		CSimpleConsole(HWND hWnd = NULL);
		~CSimpleConsole(void);

		void    SetParentHandle(HWND hWnd);
		HWND    GetParentHandle();

	public:
		//------------------------------------------------------
		// 멤버함수..
		//------------------------------------------------------
		INT     NewConsole(const TCHAR* consoleTitle = _T("SimpleConsole"));

		INT     SetMsgType( UINT iLogClass );

		INT     Write( UINT iLogClass, const TCHAR *format, ...);
		INT     Write( const TCHAR *format, ... );

		//------------------------------------------------------
		// 멤버함수..(콘솔 설정)
		//------------------------------------------------------
		RECT	m_traceRect;
		INT     SetConsoleRect( PRECT pConRect );

	protected:
		INT     MakeConsole(const TCHAR* consoleTitle);
		INT     StartConsole();

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
		static DWORD   CALLBACK WndMsg(PVOID pParam);
		static LRESULT CALLBACK RichProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
	    

		INT     CreateRichEdit(HWND hWnd);
		INT     EnableRichEdit(bool bEnable);
		INT     DestroyRichEdit();

		INT     DeleteLine(int iCount = 1 );
		INT     WriteMsg(const TCHAR *szMsg);
		INT		ASyncTrace();

		//------------------------------------------------------
		// 멤버변수..
		//------------------------------------------------------
	protected:
		HWND        m_hParent;
		HWND        m_hMyHWnd;
		HINSTANCE   m_hMyInst;

		HMODULE     m_hRichMod;
		HWND        m_hRich;
	    
		LONG_PTR      m_fnRichCallFunc;

		WNDCLASS    m_WndClass;


		TCHAR        szClassName[MAX_PATH];
		// 여기에 타입별 색깔을 넣는다..

		//
		CSMemoryPoolLIB<TraceContext>*		m_pCTraceCtxPool;

		//
		class Lock	m_CLock;
	};

	//------------------------------------------------------
	// 콘솔 정의...
	//------------------------------------------------------
	#define _SCONSOLE           CSimpleConsole::getInstance()

	#ifdef WIN32
		#define GTRACE               _SCONSOLE->Write
	#else
		#define GTRACE               _tprintf("\n"); 
	#endif

} //namespace nsGSS

#endif //__GSS_SIMPLECONSOLE_H__



//////////////////////////////////////////////////////////////////////
//
// SimpleTimer.h: interface for the CSimpleTimer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GSS_SIMPLETIMER_H__
#define __GSS_SIMPLETIMER_H__

//#include "GOS.h"

namespace nsGSS
{

	#define SIMPLETIMER_RESOLUTION	50			// 단위: ms ( 50ms -> 0.05s )

	struct TimerCall
	{
		bool				bTimerTakeAction;					// 기본 FALSE , TRUE가 되면 동작한다.
		int					iTimerID;

		struct	timeval		sTimerInInterval;					// ms 단위로 움직인다..SIMPLETIMER_RESOLUTION 보다 커야된다.
		struct	timeval		sTimer;								// 현재 시간을 설정하고 값을 더해가며 현재 시간보다 커지면 콜이다!
			
		void				(*pTimerRollback)(void *, int);		// NULL 이면 빈 것임. 콜에서 시간을 잡아먹으면 안된다. 
																// Thread를 만들어서 호출하고 해제 해주는 식으로 가도 되지만
																// 양이 늘어날 경우에는 어떻게 할 것인가..
		void				*pTimerRollbackParam;
	};

	/* 
	현재는 CALL만을 지원한다.
	다른 것(event, thread creation)은 필요하면 만들자. 
	TIMER CALL을 Context Memory Pool을 이용해서 만들면 되겠다..
	*/
	class CSimpleTimer
	{
	public:
		CSimpleTimer(unsigned int nMaxCall = 100);
		~CSimpleTimer();

	public:
		int					InitTimerCall(int iTimerID);
		void				DestroyTimer();
		
		int					RegisterCall(int iIntervalms, void (*pRing)(void *, int), void *pParam, bool bAutoStart = false);
		int					UnRegisterCall(int iTimerID);

		int					StartCall(int iTimerID);
		int					StopCall(int iTimerID);

		__inline unsigned int GetMaxCall()			{ return m_nMaxCall; }
		__inline bool		GetExitCode()			{ return m_bTTHExit; }

		__inline void		Enter()					{ m_cTimerLock.Enter(); }
		__inline void		Leave()					{ m_cTimerLock.Leave(); }

	protected:
		void				StartSimpleTimer(void);

	public:
		unsigned int		m_nNRCall;
		unsigned int		m_nMaxCall;

		struct TimerCall	*m_aCall;
		struct TimerCall	*m_aCallTemp;

	protected:
		bool				m_bTTHExit;
		HANDLE				m_hTimerThread;
		class Lock			m_cTimerLock;
	};

}
#endif // __GSS_SIMPLETIMER_H__

//////////////////////////////////////////////////////////////////////
//
// SMemoryPoolLIB.h: interface for the CSMemoryPoolLIB class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GSS_SMEMORYPOOLLIB_H__
#define __GSS_SMEMORYPOOLLIB_H__

#include "GFrame.h"

namespace nsGSS
{

	// 등록하는 방법은 메모리 풀의 컨텍스트에서 data type, 위치, sizeof 값의 Pair로 간다...
	// 기본 Data Type을 아직 지원하지 않는다.. 초기화 문제때문에..
	#define MAX_ADD_MEMORY      100

	#define VALID_KEY(iIndex)     ( iIndex >= 0 && GetCreatedCount() > iIndex )

	//------------------------------------------------------------------------
	// 기본 컨텍스트를 가지도록 할까..
	//------------------------------------------------------------------------
	class CPoolContext
	{
	public:
		void ResetData() {}

	public:
		CPoolContext *pNext;
		CPoolContext *pPrev;
	};

	//------------------------------------------------------------------------
	// 특별하게 쓰는 일이 없는데...
	// 1. Child 기능을 제거할까...
	// 2. ExtraNum 기능을 제거할까..
	// 3. Attch / GetFree등을 수정???
	// 4. 
	//------------------------------------------------------------------------
	template <typename Obj> 
	class CSMemoryPoolLIB  
	{
	public:
		// 풀에서 데이타를 가져오는 설정..
		enum POOLTYPE{ __FIFO, __LIFO };

		CSMemoryPoolLIB(unsigned int iMaxNum = 0, POOLTYPE eType = __FIFO);
		virtual ~CSMemoryPoolLIB();

	protected:
		bool   					Create();
		bool                    CreateAdd();
		void					Destroy();

		void					MakeLinkedList(Obj *pArr, unsigned int iArrayCount);
		void                    MakeReSize(unsigned int iIndex);

		void					AttachAllocContext(Obj *pFrontCtx, Obj *pInsertCtx);				// 사용중인 메모리 리스트로 붙인다..
		void                    DetachAllocContext(Obj *pCtx);                                      // 사용중인 메모리 리스트에서 떼어낸다.
		void                    AttachFreeContext(Obj *pCtx);                                       // 프리 메모리 리스트로 붙인다..
		void                    DetachFreeContext(Obj *pCtx);                                       // 프리 메모리 리스트에서 떼어낸다..

		unsigned int            FindAddArrIndex(unsigned int iKeyIndex, unsigned int &iAddKeyIndex);

	public:
		void                    SetAddMemory(unsigned int iAddNum);
		void					InitializeObj(unsigned int iKeyIndex);		                        // 메모리 풀에 사용된 데이타를 초기화 한다...
																									// 단, Structure 기반이 아닐 경우 문제가 발생한다..
	public:
		//--------------------------------------------------------------------------------------------------
		// 사용하지 않는 메모리를 하나 얻어오거나 사용한 메모리를 해제한다..
		// 릴리즈 되지 않은 메모리를 해제하면 큰일난다..
		//--------------------------------------------------------------------------------------------------
		Obj*					GetFree(Obj* pSrcCtx = NULL);			                            // pSrcCtx가 있으면 바로 붙인다..
		void					Release(Obj* pCtx);					                                // 메모리를 반환한다..
		void                    ReleaseFree(Obj* pCtx);                                             // 락없이 메모리를 반환한다..
		Obj*					Direct(unsigned int iKeyIndex);                                     // 해당 Key에 대해서 메모리의 포인터를 얻어온다.

		//--------------------------------------------------------------------------------------------------
		// 사용중인 메모리의 첫번째 포인터를 얻는다..->Next를 통해서 사용.
		//--------------------------------------------------------------------------------------------------
		Obj*					GetAllocList();
		Obj*                    GetAllocNext(Obj *pCtx);
		Obj*                    GetAllocFinish();
		//--------------------------------------------------------------------------------------------------
		// 사용할려고 가져온 메모리를 Alloc 링크드 리스트에 붙인다..
		//--------------------------------------------------------------------------------------------------
		void                    Add(Obj *pInsertCtx);
		void                    AddAny(Obj *pFrontCtx, Obj *pInsertCtx);
		//--------------------------------------------------------------------------------------------------
		// 사용 리스트에서 삭제하고 내용만 복사한다..
		//--------------------------------------------------------------------------------------------------
		bool                    Get(Obj *pDestCtx);
		Obj*                    Get();

	protected:
		void                    AddHead(Obj *pInsertCtx);
		void                    AddTail(Obj *pInsertCtx);

		bool                    GetHead(Obj *pDestCtx);
		bool                    GetTail(Obj *pDestCtx);

		Obj*                    GetHead();
		Obj*                    GetTail();
	    
	public:
		//--------------------------------------------------------------------------------------------------
		// 큰의미가 없어서 롹을 쓰지 않는다..
		//--------------------------------------------------------------------------------------------------
		inline unsigned int     GetBaseCount()                          { return m_iBaseNum; }
		inline unsigned int     GetAddCount()                           { return m_iAddNum; }
		inline unsigned int		GetCreatedCount()				        { return m_iMaxNum; }
		inline unsigned int		GetUsedCount()					        { return m_iCurrentNum; }
		inline unsigned int     GetUsableCount()                        { return m_iMaxNum - m_iCurrentNum;  }
		
		inline unsigned int		GetTotalGetFreeCount()				    { return m_iTotalGetFreeCount; }
		inline unsigned int		GetTotalReleaseCount()					{ return m_iTotalReleaseCount; }

		inline void				IncrementUsedContext()					{ m_iTotalGetFreeCount++; m_iCurrentNum++; }
		inline void				DecrementUsedContext()					{ m_iTotalReleaseCount++; m_iCurrentNum--; }

	protected:
		//--------------------------------------------------------------------------------------------------
		//
		//--------------------------------------------------------------------------------------------------
		void					InitializeLock();
		void					DeInitializeLock();

		void					Enter();
		void					Leave();

	protected:
		//--------------------------------------------------------------------------------------------------
		//
		//--------------------------------------------------------------------------------------------------
		class Lock				m_cMemoryPoolLock;							// Lock
		volatile LONG           m_bMemoryPoolInUse;                         // Lock-Free
		
		//
		POOLTYPE                m_eType;

		// 메모리 추가기능 사용여부..
		bool                    m_bAddMemory;

		// 크기 관련 변수들..
		unsigned int			m_iBaseNum;									// 기본 메모리 풀 생성 값
		unsigned int			m_iMaxNum;									// 메모리풀 생성 최대값..
		unsigned int			m_iCurrentNum;								// 현재 할당된 값..
		unsigned int            m_iAddNum;                                  // 새로추가할때 추가되어야 할값..
	    
		// for LOG
		unsigned int            m_iTotalGetFreeCount;
		unsigned int            m_iTotalReleaseCount;
		
		// 저장소..
		Obj*					m_pCtxArr;									// 전체 생성된 메모리 Array
		Obj*                    m_pCtxAddArr[MAX_ADD_MEMORY];               // 새롭게 ADD 될때..
		unsigned int			m_aCtxArrEndIndex[MAX_ADD_MEMORY + 1];			// Add Memory 의 끝 인덱스


		// 
		Obj*					m_pFreeCtx;									// 사용 가능한 메모리 첫번째 링크..
		Obj*					m_pLastFreeCtx;								// 마지막 Free Link - 해제되는 놈을 여기 연결한다..

		Obj*					m_pAllocCtx;								// 이미 사용된 메모리 첫번째 링크..
		Obj*					m_pLastAllocCtx;							// 마지막 Alloc Link - 할당되는 놈을 여기 연결한다..
	};

	// SMemoryPoolLIB.cpp: implementation of the CSMemoryPoolLIB class.
	//
	//////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	template <typename Obj>
	CSMemoryPoolLIB<Obj>::CSMemoryPoolLIB(unsigned int iMaxNum /* = 0 */, POOLTYPE eType)
	:m_iBaseNum(iMaxNum), m_pCtxArr(NULL)
	{
		// 추가 할당 인덱스 설정..
		ZeroMemory( &m_aCtxArrEndIndex, (MAX_ADD_MEMORY + 1) * sizeof(unsigned int) );

		//
		m_eType                 = eType;

		// 추가 할당 사용여부
		m_bAddMemory            = false;

		// 사이즈
		m_iMaxNum			    = iMaxNum;
		m_iCurrentNum		    = 0;
		m_iAddNum               = 0;

		m_iTotalGetFreeCount    = 0;
		m_iTotalReleaseCount    = 0;

		// 링크드 리스드에 각 링크 초기화..
		m_pFreeCtx			    = NULL;
		m_pLastFreeCtx		    = NULL;

		m_pAllocCtx			    = NULL;
		m_pLastAllocCtx		    = NULL;

		// Add Array를 초기화한다..
		for( unsigned int iIndex = 0 ; iIndex < MAX_ADD_MEMORY ; iIndex++ )
		{
			m_pCtxAddArr[iIndex] = NULL;
			m_aCtxArrEndIndex[iIndex] = 0;
		}
		// 하나 더 추가한다..
		m_aCtxArrEndIndex[MAX_ADD_MEMORY] = 0;
	    
		// 설정된 정보대로 최초 생성..
		if( GetCreatedCount() > 0 )
		{
			Create();
		}
	}

	template <typename Obj>
	CSMemoryPoolLIB<Obj>::~CSMemoryPoolLIB()
	{
		if( GetCreatedCount() > 0 )
		{
			Destroy();
		}
	}

	template <typename Obj>
	bool CSMemoryPoolLIB<Obj>::Create()
	{
		InitializeLock();

		// 최초 설정 크기대로 생성한다...
		m_pCtxArr = new Obj[GetBaseCount()];
	    
		// 사이즈 및 인덱스 조절 저장..
		MakeReSize(0);
	    
		// 링크 정보 생성..
		MakeLinkedList(m_pCtxArr, GetBaseCount() );
		
		return true;
	}

	template <typename Obj>
	bool CSMemoryPoolLIB<Obj>::CreateAdd()
	{
		for( unsigned int iIndex = 0 ; iIndex < MAX_ADD_MEMORY ; iIndex++ )
		{
			if( NULL == m_pCtxAddArr[iIndex] )
			{
				m_pCtxAddArr[iIndex] = new Obj[GetAddCount()];

				if( m_pCtxAddArr[iIndex] != NULL )
				{
					MakeReSize( iIndex + 1);
					MakeLinkedList(m_pCtxAddArr[iIndex], m_iAddNum );

					return true;
				}

				return false;
			}
		}
	    
		return false;
	}


	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::MakeLinkedList(Obj *pArr, unsigned int iArrayCount)
	{
		// 루프를 돌면서 링크를 생성한다..
		for( unsigned int iIndex = 0 ; iIndex < iArrayCount ; iIndex++ )
		{
			// 메모리 개별 객체 내부에서 Critical Section을 쓸지는 미지수..
			// InitializeCriticalSection( &m_pCtxArr[loop].cs );

			if( iIndex < iArrayCount - 1)
			{
				pArr[iIndex].pNext = &(pArr[iIndex + 1]);
				pArr[iIndex].pPrev = NULL;
			}
			else
			{
				pArr[iIndex].pNext = NULL;
				pArr[iIndex].pPrev = NULL;
			}
		}

		if( NULL == m_pFreeCtx )
		{
			m_pFreeCtx				= &(pArr[0]);
			m_pLastFreeCtx			= &(pArr[iArrayCount - 1]);
		}
		else
		{
			m_pLastFreeCtx->pNext	= &(pArr[0]);
			m_pLastFreeCtx			= &(pArr[iArrayCount - 1]);
		}
	}

	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::MakeReSize(unsigned int iIndex)
	{
		// 
		m_iMaxNum = GetCreatedCount() + GetAddCount();
	    
		// LAST INDEX 설정
		m_aCtxArrEndIndex[iIndex] = GetCreatedCount();
	}

	template <typename Obj>
	unsigned int CSMemoryPoolLIB<Obj>::FindAddArrIndex(unsigned int iKeyIndex, unsigned int &iAddKeyIndex)
	{
		unsigned int iIndex = 0;

		for( ; iIndex < MAX_ADD_MEMORY + 1; iIndex++ )
		{		
			if( m_aCtxArrEndIndex[iIndex] > iKeyIndex )
			{
				if (iIndex > 0)
				{
					iAddKeyIndex = iKeyIndex - m_aCtxArrEndIndex[iIndex - 1];				
				}
				else
				{
					iAddKeyIndex = iKeyIndex;				
				}

				break;
			}		
		}

		return iIndex;
	}

	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::Destroy()
	{
		if( NULL != m_pCtxArr )
		{
			delete [] m_pCtxArr;
			m_pCtxArr = NULL;
		}

		for( unsigned int iIndex = 0 ; iIndex < MAX_ADD_MEMORY ; iIndex++ )
		{
			if( NULL != m_pCtxAddArr[iIndex] )
			{
				delete[] m_pCtxAddArr[iIndex];
				m_pCtxAddArr[iIndex] = NULL;
			}
		}

		m_pFreeCtx = NULL;

		DeInitializeLock();
	}

	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::SetAddMemory(unsigned int iAddNum)
	{
		Enter();

		m_iAddNum = iAddNum;
		if( m_iAddNum > 0 )
			m_bAddMemory = true;

		Leave();
	}

	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::InitializeObj(unsigned int iKeyIndex)
	{
		if( NULL != m_pCtxArr )
		{
			Enter();

			//m_pCtxArr[iKeyIndex].;

			Leave();
		}
	}

	template <typename Obj>
	Obj* CSMemoryPoolLIB<Obj>::Direct(unsigned int iKeyIndex)
	{
		Obj* pRet                   = NULL;
		unsigned int iAddKeyIndex   = 0, iIndex = 0;

		if( VALID_KEY(iKeyIndex) && NULL != m_pCtxArr)
		{	
			iIndex = FindAddArrIndex( iKeyIndex, iAddKeyIndex );

			if( iIndex == 0 )
				pRet = &(m_pCtxArr[iKeyIndex]);
			else
				pRet = &(m_pCtxAddArr[iIndex - 1][iAddKeyIndex]);
		}

		if( NULL == pRet )
		{
			// Error Case 메모리가 비어있거나...iKeyIndex가 잘못되었다..
		}

		return pRet;
	}

	template <typename Obj>
	Obj* CSMemoryPoolLIB<Obj>::GetFree(Obj *pSrcCtx /* = NULL */)
	{
		Obj* pRet = NULL;

		Enter();

		// Free 메모리가 없는데 추가 기능이 활성화 되어있다..
		if( m_pFreeCtx == NULL && m_bAddMemory == true )
		{
			CreateAdd();
		}

		// 
		if( NULL != m_pFreeCtx )
		{
			pRet				= m_pFreeCtx;

			DetachFreeContext( pRet );

			// 이 기능을 사용할때는 Obj안에 Initialize된 값이 없는 형태여야 한다.
			// 예) iKey 같은 경우 복사되어서 사라지면 이후에 문제가 된다...
			if( NULL != pSrcCtx )
			{
				memcpy( pRet, pSrcCtx, sizeof(Obj) );
				AddTail( pRet );
			}
		}
		Leave();

		return pRet;
	}


	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::Release(Obj* pCtx)
	{
		Enter();

		ReleaseFree(pCtx);

		Leave();
	}

	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::ReleaseFree(Obj* pCtx)
	{
		if( NULL != m_pCtxArr )
		{
			DetachAllocContext(pCtx);
			AttachFreeContext(pCtx);
		}
	}

	//--------------------------------------------------------------------------------------------------
	// 1. AttachFreeContext
	// 2. DetachFreeContext
	// 3. AttachAllocContext
	// 4. DetachAllocContext
	//--------------------------------------------------------------------------------------------------
	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::AttachFreeContext(Obj *pCtx)
	{
		if( NULL != m_pFreeCtx )	
		{
			m_pLastFreeCtx->pNext	= pCtx;
			m_pLastFreeCtx			= pCtx;
		}
		else
		{
			m_pFreeCtx				= pCtx;
			m_pLastFreeCtx			= m_pFreeCtx;
		}
	    
		DecrementUsedContext();
	}

	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::DetachFreeContext(Obj *pCtx)
	{
		m_pFreeCtx = static_cast<Obj *>( pCtx->pNext );

		pCtx->pNext = NULL;
		pCtx->pPrev = NULL;

		IncrementUsedContext();
	}


	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::AttachAllocContext(Obj *pFrontCtx, Obj *pInsertCtx) //, BOOL bFront /* = TRUE */ )
	{
		// 링크드 리스트 초기화..
		pInsertCtx->pPrev = NULL;
		pInsertCtx->pNext = NULL;

		// 
		if( NULL == m_pAllocCtx )
		{
			m_pAllocCtx = m_pLastAllocCtx = pInsertCtx;
		}
		else
		{
			if( NULL == pFrontCtx ) // 맨앞에
			{
				Obj *pTopCtx        = static_cast<Obj *>( m_pAllocCtx );

				pInsertCtx->pNext   = pTopCtx;
				pTopCtx->pPrev      = pInsertCtx;

				m_pAllocCtx         = pInsertCtx;
			}
			else
			{
				Obj *pRearCtx       = static_cast<Obj *>( pFrontCtx->pNext );

				pInsertCtx->pPrev   = pFrontCtx;
				pInsertCtx->pNext   = pRearCtx;

				pFrontCtx->pNext    = pInsertCtx;

				if( NULL != pRearCtx )
					pRearCtx->pPrev = pInsertCtx;
				else // 뒤가 없으면..
					m_pLastAllocCtx = pInsertCtx;
			}
		}
	}


	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::DetachAllocContext(Obj *pCtx)
	{
		if( NULL == m_pAllocCtx )
		{
			return;
		}
		else
		{// 제일 앞으로 보낸다..
			Obj *pPrevCtx = static_cast<Obj *>( pCtx->pPrev );
			Obj *pNextCtx = static_cast<Obj *>( pCtx->pNext );

			if( NULL != pPrevCtx )
				pPrevCtx->pNext = pNextCtx;
			if( NULL != pNextCtx )
				pNextCtx->pPrev = pPrevCtx;

			if( m_pAllocCtx == pCtx )
			{
				m_pAllocCtx = pNextCtx;
			}
			if( m_pLastAllocCtx == pCtx )
			{
				m_pLastAllocCtx = pPrevCtx;
			}

			pCtx->pNext = NULL;
			pCtx->pPrev = NULL;
		}
	}

	//--------------------------------------------------------------------------------------------------
	// 사용할려고 가져온 메모리를 Alloc 링크드 리스트에 붙인다..
	//--------------------------------------------------------------------------------------------------
	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::Add(Obj *pInsertCtx)
	{
		Enter();

		// 타입에 상관없이 일단 꼬리에 붙인다..
		AddTail( pInsertCtx );

		Leave();
	}

	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::AddAny(Obj *pFrontCtx, Obj *pInsertCtx)
	{
		//Enter();

		AttachAllocContext( pFrontCtx, pInsertCtx );

		//Leave();
	}

	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::AddHead(Obj *pInsertCtx)
	{
		AttachAllocContext( NULL, pInsertCtx );
	}

	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::AddTail(Obj *pInsertCtx)
	{
		AttachAllocContext( m_pLastAllocCtx, pInsertCtx );
	}

	//--------------------------------------------------------------------------------------------------
	// 사용 리스트에서 삭제하고 내용만 복사한다..
	//--------------------------------------------------------------------------------------------------
	template <typename Obj>
	bool CSMemoryPoolLIB<Obj>::Get(Obj *pDestCtx)
	{
		bool bRet = false;

		Enter();

		switch( m_eType )
		{
			case __FIFO :
				bRet = GetHead(pDestCtx);
			break;
			case __LIFO :
				bRet = GetTail(pDestCtx);
			break;
		}

		Leave();

		return bRet;
	}

	//--------------------------------------------------------------------------------------------------
	// 사용 리스트에서 삭제하지않고 포인터만 넘긴다..
	//--------------------------------------------------------------------------------------------------
	template <typename Obj>
	Obj* CSMemoryPoolLIB<Obj>::Get()
	{
		Obj *pDestCtx = NULL;

		Enter();

		switch( m_eType )
		{
			case __FIFO :
				pDestCtx = GetHead();
			break;
			case __LIFO :
				pDestCtx = GetTail();
			break;
		}

		Leave();

		return pDestCtx;
	}

	template <typename Obj>
	bool CSMemoryPoolLIB<Obj>::GetHead(Obj *pDestCtx)
	{
		if( m_pAllocCtx != NULL )
		{
			memcpy( pDestCtx, m_pAllocCtx, sizeof( Obj ) );
			ReleaseFree( m_pAllocCtx );

			return true;
		}
	    
		return false;
	}

	template <typename Obj>
	bool CSMemoryPoolLIB<Obj>::GetTail(Obj *pDestCtx)
	{
		if( m_pLastAllocCtx != NULL )
		{
			memcpy( pDestCtx, m_pLastAllocCtx, sizeof( Obj ) );
			ReleaseFree( m_pLastAllocCtx );

			return true;
		}
	    
		return false;
	}

	template <typename Obj>
	Obj* CSMemoryPoolLIB<Obj>::GetHead()
	{
		Obj *pRet = NULL;

		if( m_pAllocCtx != NULL )
		{
			pRet = m_pAllocCtx;
			DetachAllocContext( m_pAllocCtx );
		}
	    
		return pRet;
	}

	template <typename Obj>
	Obj* CSMemoryPoolLIB<Obj>::GetTail()
	{
		Obj *pRet = NULL;

		if( m_pLastAllocCtx != NULL )
		{
			pRet = m_pLastAllocCtx;
			DetachAllocContext( m_pLastAllocCtx );
		}
	    
		return pRet;
	}


	/*
	Usage

	Obj *pCtx = GetAllocList();
	while(pCtx != NULL )
	{
		pCtx 블라블라
		pCtx 블라블라
		.
		.
		if( 혹시 )
		{
			GetAllocFinish();
			break;
		}

		pCtx = GetAllocNext(pCtx);
	}
	*/
	template <typename Obj>
	Obj* CSMemoryPoolLIB<Obj>::GetAllocList()
	{
		Obj *pCtx;
	    
		switch( m_eType )
		{
			case __FIFO :
				pCtx = m_pAllocCtx;
			break;
			case __LIFO :
				pCtx = m_pLastAllocCtx;
			break;
		}

		if( pCtx != NULL )
			Enter();
	        
		return pCtx;
	}

	template <typename Obj>
	Obj* CSMemoryPoolLIB<Obj>::GetAllocNext(Obj *pFrontCtx)
	{
		Obj *pCtx = NULL;
	    
		if( pFrontCtx == NULL )
			return pCtx;

		switch( m_eType )
		{
			case __FIFO :
				pCtx = pFrontCtx->pNext;
			break;
			case __LIFO :
				pCtx = pFrontCtx->pPrev;
			break;
		}
	        
		if( pCtx == NULL )
			Leave();

		return pCtx;
	}

	template <typename Obj>
	Obj* CSMemoryPoolLIB<Obj>::GetAllocFinish()
	{
		Leave();

		return NULL;
	}


	//--------------------------------------------------------------------------------------------------
	// For Thread-Safe...
	//--------------------------------------------------------------------------------------------------
	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::InitializeLock()
	{
		//InitializeCriticalSection(&m_cs);	
		InterlockedExchange( &m_bMemoryPoolInUse, FALSE );
	}


	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::DeInitializeLock()
	{
		//DeleteCriticalSection(&m_cs);	
	}


	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::Enter()
	{
		// EnterCriticalSection(&m_cs);	
		//m_cMemoryPoolLock.Enter();

		// 스핀락에 대한 처리 방법에 하나..
		while( InterlockedExchange( &m_bMemoryPoolInUse, TRUE ) == TRUE )
		{
			// Sleep도 있고 쓰레드간 전환 방식도 있다..
			Sleep(0);
		}
	}


	template <typename Obj>
	void CSMemoryPoolLIB<Obj>::Leave()
	{
		//LeaveCriticalSection(&m_cs);
		//m_cMemoryPoolLock.Leave();

		InterlockedExchange( &m_bMemoryPoolInUse, FALSE );
	}

} //namespace nsGSS

#endif // !defined __GSS_SMEMORYPOOLLIB_H__

// StringMatch.h: interface for the CStringMatch class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _STRINGMATCH_H
#define _STRINGMATCH_H

//#include "GOS.h"

namespace nsGSS
{



	class CStringMatch
	{
		friend class CQueryMatch;

	private:
		struct IMPL;
		IMPL* m_pImpl; ///< 내부 데이터 PIMPL

	public:
		CStringMatch();         /// 생성자
		~CStringMatch();        /// 소멸자

	public:
		/// 특정 단어를 '*'로 치환한 문장을 리턴한다...일단 *는 임의
		const std::string Filter(const std::string& szOriginal); 

		/// 해당하는 문장이 특정 단어를 포함하고 있는지의 여부를 리턴한다.
		const int HasMatchWord(const std::string& szOriginal, int& iLen, int& iWordNumber);
		const int MultiMatchWord(int iNum, const std::string& szOriginal, int& iLen, int& iWordNumber);

		/// 단어를 추가한다.
		void AddWord(const std::string& szWord, int iWordNumber = 0);
	public:
		/// 해당하는 문장의 첫 바이트부터 특정 단어가 포함되어있는지 검사한다.
		const size_t Match(const std::string& szText, int& iWordNumber);

		/// 해당하는 글자가 특수 부호인지 검사한다.
		const bool IsPunctutation(char c);

	public:
		CMatchWordNode *GetCurrentMatchWordNode();

		inline void SetIndex(size_t index) { m_index = index; };
		inline void SetSize(size_t size) { m_size = size; };
		inline size_t GetIndex() { return m_index; };
		inline size_t GetSize() { return m_size; };

		size_t m_index;
		size_t m_size;
	};
} // namespace nsGSS

#endif // !defined _STRINGMATCH_H

// WinService.h: interface for the CWinService class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GSS_WIN_SERVICE_H__
#define __GSS_WIN_SERVICE_H__

#include "GFrame.h"

namespace nsGSS
{

	#define WIN_SERVICE	CWinService::getInstance()

	void ServiceMain( DWORD argc, LPTSTR *argv );
	void ServiceStop();

	class CWinService : public nsGSS::Singleton<CWinService>
	{
		 friend class nsGSS::Singleton<CWinService>;

	protected:
		CWinService();

	public:
		// 사용자 인터페이스
		bool		Install( LPTSTR szServiceName );
		bool		Uninstall( LPTSTR szServiceName );
		bool		Begin( LPTSTR szServiceName );

		// 내부 인터페이스
		void		RunMain( DWORD argc, LPTSTR *argv );
		void		CtrlHandlerCallback( DWORD dwState );

		const TCHAR	*GetFullPath()		{ return m_szFullPath; }
		const TCHAR	*GetFolderPath()	{ return m_szFolderPath; }
	private:
		bool		SetStatus( DWORD dwState, DWORD dwAccept = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE );

	private:
		SERVICE_STATUS			m_stServiceStatus;
		SERVICE_STATUS_HANDLE	m_hServiceHandle;
		TCHAR					m_szServiceName[MAX_PATH];
		TCHAR					m_szFullPath[MAX_PATH];
		TCHAR					m_szFolderPath[MAX_PATH];
	};
}//namespace nsGSS

#endif	//__GSS_WIN_SERVICE_H__
#ifndef __GSS_MSOCKET_H__
#define __GSS_MSOCKET_H__

namespace nsGSS
{
	#define LISTEN_SOCKET_CONTEXT_KEY 54321

	class CMSocket
	{	
	public:
		CMSocket(void);
		virtual ~CMSocket(void);

	public:	
		BOOL CreateSocket(INT socketType);				// 인자값에 따른 소켓 생성
		BOOL SetSocket(SOCKET _socket);					//
		SOCKET GetSocket() {return m_socket;}

		VOID CloseSocket(BOOL _closeSocket = TRUE);		// 소켓 종료
		INT HalfCloseSocket(INT _how);					// halfclose관련

		__inline BOOL IsActive() {return m_active;}		
		__inline VOID SetActive(BOOL _active) {m_active = _active;}	

		__inline LONG IsConnect() {return m_connect;}
		VOID SetConnect(LONG _connect);	

		VOID freeBuffers();

		/**
		--------------------------------------
		Functions for Socket Option                                          
		--------------------------------------
		*/
		INT	GetSocketOption(INT _option, PCHAR _pszOptVal, PINT _pOptLen, INT _level = SOL_SOCKET);		
		INT	SetSocketOption(INT _option, LPCSTR _pszOptVal, INT _optLen, INT _level = SOL_SOCKET);
		
		INT	SetSocketBuffer(INT _socketBufferSize);	
		INT	SetNonBlock();
		INT	SetBlock();
		INT	SetNoDelay();
		INT	SetDelay();
		INT	SetNoLinger();
		INT	SetLinger();
		INT	SetConditionalAcceptOn();
		INT	SetConditionalAcceptOff();
		INT	SetReUseAddr();
		INT	SetNoReUseAddr();
		INT	SetConnectTime(INT _timeSeconds);
		INT	SetSocketUpdateContext(SOCKET _listenSocket);

		/**
		--------------------------------------
		Functions for Read/Write Buffer                                      
		--------------------------------------
		*/
		INT SetReadWriteBuffer();	
		__inline INT GetMaxReadBufferSize() {return m_maxReadBufferSize;}
		__inline VOID SetMaxReadBufferSize(INT _size) {m_maxReadBufferSize = _size;}

		__inline INT GetMaxWriteBufferSize() {return m_maxWriteBufferSize;}
		__inline VOID SetMaxWriteBufferSize(INT _size)	{m_maxWriteBufferSize = _size;}
		
		__inline INT GetDataReadBuffer(PBYTE _pBuffer, INT _iLen) {return GNetCircularBuffer::GetDataBuffer(m_pReadBuffer, _pBuffer, _iLen);}
		__inline INT PutDataReadBuffer(PBYTE _pBuffer, INT _iLen) {return GNetCircularBuffer::PutDataBuffer(m_pReadBuffer, _pBuffer, _iLen);}
		__inline INT GetDataWriteBuffer(PBYTE _pBuffer, INT _iLen) {return GNetCircularBuffer::GetDataBuffer(m_pWriteBuffer, _pBuffer, _iLen);}
		__inline INT PutDataWriteBuffer(PBYTE _pBuffer, INT _iLen) {return GNetCircularBuffer::PutDataBuffer(m_pWriteBuffer, _pBuffer, _iLen);}
		
		__inline GNetCircularBuffer* GetReadBuffer() {return m_pReadBuffer;}
		__inline GNetCircularBuffer* GetWriteBuffer() {return m_pWriteBuffer;}		
		
	private:
		SOCKET m_socket;
		BOOL m_active;							// Default = INVALID_SOCKET 소켓값이 INVALID_VALUE와 같지 않으면 Active상태임.	
		volatile LONG m_connect;				// 실제 접속여부 (TCP : 실제로 FC_CONNECT 성공이 떨어지는 시점, UDP : 실제로 ReqConnect 패킷이 오는 시점)		

		/**
		--------------------------------------
		소켓에서 사용하는 버퍼
		--------------------------------------
		*/
		GNetCircularBuffer* m_pReadBuffer;
		GNetCircularBuffer*	m_pWriteBuffer;

		INT m_maxReadBufferSize;				// Maximum Read Buffer Size.
		INT	m_maxWriteBufferSize;				// Maximum Write Buffer Size.

	public:	
		SOCKADDR_IN m_socketAddr;

		CHAR m_userIPv4[16];					// Connected ip version 4 address.
		CHAR m_userIPv6[64];					// Connected ip version 6 address.       	
	};

} // namespace nsGSS
#endif //! __GSS_MSOCKET_H__


#ifndef __GSS_PACKET_INFO_H__
#define __GSS_PACKET_INFO_H__

namespace nsGSS
{
	//******************************************************************************
	// Structure of Main Packet.
	// Packet Header(1Byte) + Packet Info(struct PacketInfo) + Packet Data(nBytes) + Packet Tail(1Byte)
	// * 주의 *
	// 모든 구조체의 멤버는 크기를 지정하여야 한다. 이것은 32비트 시스템(XP이하)과 64비트 시스템(Vista이상)의 호환을 위한 것이다.
	//******************************************************************************
	//******************************************************************************
	// Main Packet Definition.
	//******************************************************************************
	#define PKTPHPTTYPE		unsigned char
	#define PKTSIZETYPE		unsigned short int		// Length of total packet.
	#define PKTCTRLTYPE		unsigned char
	#define PKTDATATYPE		unsigned char			// Type of each bytes of packet.
	#define PKTSEQUENCE		unsigned int

	#define PKTUNIQUEVALUETYPE		int				//  소켓 컨텍스트 인덱스( iKey ), Session CRandint 값 (iCid)		

	#define PKT_PH			0xF1
	#define PKT_PT			0xF2

	#define PKT_PH_LEN		1		// Packet Header Length
	#define PKT_PT_LEN		1		// Packet Tail Length
	#define PKT_PS_LEN		2		// Packet Size Length
	#define PKT_CT_LEN		1		// Each Length of Control Command

	#define PKT_STRING_TOKEN			0x00

	#define PKT_DIRECTION_C2M			0x01		// Client to Master Server.
	#define PKT_DIRECTION_C2G			0x02		// Client to Game Server.
	#define PKT_DIRECTION_M2C			0x10		// Master Server to Client.
	#define PKT_DIRECTION_G2C			0x20		// Game Server to Client.

	#define MAX_PACKET_BUFFER_SIZE		MAX_SINGLE_PACKET_SIZE*2

	// Structure for packet information. mapping to/from the packet.
	struct MainPacketInfo 
	{
		PKTSIZETYPE		Length;	// Entire Protocol Length. Include Protocol header and protocol tail.
		PKTCTRLTYPE		FC;		// First Control Command.
		PKTCTRLTYPE		SC;		// Second Control Command.			
	};

	struct MainPacketData {
		PKTSIZETYPE	DataLen;						// Length of data.
		PKTDATATYPE Data[MAX_PACKET_BUFFER_SIZE];	// Buffer for contain the data.
	};

	struct MainPacket {
		struct MainPacketInfo stInfo;	// Information part.
		struct MainPacketData stData;	// Data part.
	};

	// Structure for handle the each packet. not using for transmission or communication in network.
	// Using for make the linked list of process list of packets which has received from socket to process.
	struct MainPacketNode {
		struct MainPacketInfo stInfo;	// Information part.
		struct MainPacketData stData;	// Data part.

		struct SessionInfo *pSENDER;

		struct MainPacketNode *pNext;	// Next node.
		struct MainPacketNode *pPrev;	// Next node.
	};

	struct MainPacketList {
		struct MainPacketNode *pReadyHead;	// First head of the list.
		struct MainPacketNode *pReadyTail;	// Last tail of the list.
		struct MainPacketNode *pEmpty;
	};

	#define MAIN_PACKET_WRAPPER_FRONT_LENGTH    (PKTSIZETYPE)( sizeof(struct MainPacketInfo) + PKT_PH_LEN )

	///////////////////////////////////////////////////////////////////////////////
	// Structure for function pointer to prcess the each packet in linked list of prcess list.
	///////////////////////////////////////////////////////////////////////////////
	struct MainFunctionInfo {
		void (*fp)(struct MainPacketNode stPacket);
	};
	#define CMD(function_name) void (function_name)(struct MainPacketNode PN)

} //namespace nsGSS

#endif  // __GSS_PACKET_INFO_H__

#ifndef __GSS_STRUCTS_H__
#define __GSS_STRUCTS_H__

#include <mmsystem.h>

namespace nsGSS
{

	/**
	--------------------------------------
	핸들러로 사용할 인터페이스 클래스 각 로직 담당자들은 이를 상속받아 구현한다.
	핸들러가 NULL일 경우 기본적으로는 IO Buffer를 사용하는 방법으로 한다.
	--------------------------------------
	*/

	class INetworkHandler 
	{
	public:	
		virtual ~INetworkHandler() {}

		//! 워커 쓰레드에서 핸들러를 호출하는 방식에서 사용된다	
		virtual INT	OnFinishedReading(INT iThreadId, INT iKey, PBYTE pbyReadBuffer, INT iReadSize) = 0;	///< 데이타가 들어온 경우다...
		virtual INT	OnFinishedSending(INT iThreadId, INT iKey) = 0;
		virtual INT OnFinishedQuery(INT iThreadId, LPVOID pDB) = 0;
		virtual INT	OnAcceptClient(INT iKey)  = 0;
		virtual INT	OnCloseClient(INT iKey, INT _iCloseType=1)   = 0;
	};


	/**
		서버/클라이언트 제작시 세션관리에 기본이 될 소켓 컨텍스트의 기본 구조 
	*/
	struct SocketPersonalInfo
	{
		virtual VOID InitializePersonalInfo() = 0;
		// == 연산자 재정의를 할까..
		// virtual BOOL operator==(SocketPersonalInfo *pThis) = 0;
	};

	/** Example
	struct MySessionInfo : public SocketPersonalInfo
	{
		int a;

		void InitializePersonalInfo()
		{
			a = -1;
		}
		BOOL operator==(SocketPersonalInfo *pCmp)
		{
			if( memcmp( this, pCmp, sizeof(MySessionInfo) ) == 0 )
				return TRUE;
			else
				return FALSE;
		}
	};
	*/

	struct tagSocketContext : public CMSocket
	{
		Lock cSocketLock;    
		INT iKey;
	    
		BOOL bSendCompleted;       ///< Send가 완료된 상태인지 확인한다.	

		UCHAR byConnectionInfo[MAX_SINGLE_PACKET_SIZE];

		volatile LONG threadIndex;
		
		INT iExtraBufferSize;
		GNetCircularBuffer* pExtraBuffer;	

		IOContext recvCtx;
		IOContext sendCtx;
		IOContext acceptCtx;

		struct SocketPersonalInfo   *pSocketPersonalInfo;

		//! 링크드 리스트 위한 변수.
		struct tagSocketContext	    *pNext;
		struct tagSocketContext     *pPrev;
	    
		tagSocketContext() : iKey(-1), pSocketPersonalInfo(NULL), pExtraBuffer(NULL), threadIndex(INVALID_THREAD_INDEX)
		{
			Reset();
		};

		/**
			내부 함수
		*/
		__inline void SetSocketPersonalInfo( struct SocketPersonalInfo *pPersonalInfo ) { pSocketPersonalInfo = pPersonalInfo; }
		__inline struct SocketPersonalInfo *GetSocketPersonalInfo()						{ return pSocketPersonalInfo; }
		__inline void Lock()															{ cSocketLock.Enter(); }
		__inline void Unlock()															{ cSocketLock.Leave(); }
		__inline void SetExtraBufferSize(int iSize)                                     { iExtraBufferSize = iSize; }                              
		__inline void CreateIOBuffer()                                                  { 
																						  if( iExtraBufferSize == 0 )
																							iExtraBufferSize = MAX_BUF_SIZE * 2;
	                                                                                      
																						  pExtraBuffer = new GNetCircularBuffer( iExtraBufferSize * 2 ); 

																						  if( pExtraBuffer != NULL )
																							  pExtraBuffer->BufferActive();
																						}
		void Reset()
		{
			threadIndex = INVALID_THREAD_INDEX;
			
			recvCtx.iIOType	= OVERLAPPED_IO_TYPE_READ;
			sendCtx.iIOType	= OVERLAPPED_IO_TYPE_WRITE;
			acceptCtx.iIOType = OVERLAPPED_IO_TYPE_ACCEPT;

			ZeroMemory(&recvCtx.overlapped, sizeof(WSAOVERLAPPED));
			ZeroMemory(&sendCtx.overlapped, sizeof(WSAOVERLAPPED));
			ZeroMemory(&acceptCtx.overlapped, sizeof(WSAOVERLAPPED));
			
			ZeroMemory(m_userIPv4, sizeof(CHAR)*_countof(m_userIPv4));
			ZeroMemory(m_userIPv6, sizeof(CHAR)*_countof(m_userIPv6));		
			ZeroMemory( &m_socketAddr, sizeof( SOCKADDR_IN ) );
			
			pSocketPersonalInfo = NULL;		

			iExtraBufferSize    =	0;						
			
			if( pExtraBuffer )
			{
				pExtraBuffer->Clear();					
			}

			bSendCompleted = TRUE;		

			SetConnect(FALSE);
		}
	};
	typedef struct tagSocketContext SocketContext, *LPSocketContext;

	typedef struct _PacketControlBuffer
	{
	protected:
		UCHAR* szData;
		UCHAR szPacket[MAX_SINGLE_PACKET_SIZE * 4];//! 일단 넉넉하게 4096*5 사이즈로 사용
		PKTSIZETYPE uBufferSize;

		USHORT uPos;
		PKTCTRLTYPE uControl[2];

	public:	
		_PacketControlBuffer()
		{
			szData = NULL;
			uBufferSize	= 0;
		
			uPos = 1;
			uControl[0] = -1;
			uControl[1] = -1;		
		}
		~_PacketControlBuffer()
		{
		}

		__inline UCHAR* GetSendBuffer()	
		{ 
			return szPacket; 
		}
		__inline PKTSIZETYPE GetSendSize()		
		{ 
			return uBufferSize; 
		}

		__inline _PacketControlBuffer& operator<<(const PKTSIZETYPE iSendDataSize)
		{
			uBufferSize = iSendDataSize;

			return *this;
		}
		__inline _PacketControlBuffer& operator>>(PKTSIZETYPE& iSendDataSize)
		{
			iSendDataSize = uBufferSize;

			return *this;
		}

		__inline _PacketControlBuffer& operator<<(UCHAR* szSendData)
		{
			szData = szSendData;

			return *this;
		}
		__inline _PacketControlBuffer& operator>>(PUCHAR* szSendData)
		{
			*szSendData = szData;

			return *this;
		}	
		__inline _PacketControlBuffer& operator<<(const PKTCTRLTYPE iControl)
		{
			uControl[uPos++] = iControl;

			return *this;
		}
		__inline _PacketControlBuffer& operator>>(PKTCTRLTYPE& iControl)
		{
			iControl = uControl[--uPos];

			return *this;
		}
	}PacketControlBuffer, *PPacketControlBuffer;

} //namespace nsGSS

#endif // __GSS_STRUCTS_H__
#ifndef __GSS_PACKET_H__
#define __GSS_PACKET_H__

namespace nsGSS
{

	class CPacket
	{
	public:
		CPacket(void);
		~CPacket(void);

	public:
		static INT MakePacket(/* IN/OUT */ PPacketControlBuffer _pstControl);		

		static INT CountPacket(/* I N */ GNetCircularBuffer* pReadBuffer);
		static INT CountPacket(/* I N */ PBYTE pReadBuffer, int iLen);

		static INT ExtractPacket(/* OUT */ struct MainPacket *pExtractData,	/* I N */ GNetCircularBuffer* pReadBuffer);
		static INT ExtractPacket(/* OUT */ struct MainPacket *pExtractData, /* I N */ PBYTE pReadBuffer,/* I N */ INT iLen);
		
	public: // inline
		static __inline BOOL IS_PACKET_PH(PKTPHPTTYPE pkt)
		{
			if (pkt != PKT_PH)
			{
				return FALSE;
			}

			return TRUE;
		}

		static __inline INT ADD_PACKET_HEADER(PKTDATATYPE *Buffer)
		{
			Buffer[0] = PKT_PH;

			return PKT_PH_LEN;
		}
	};

} //namespace nsGSS

#endif  // __GSS_PACKET_H__



#ifndef __GSS_WINSOCKAPI_H__
#define __GSS_WINSOCKAPI_H__

namespace nsGSS
{
	class CWinsockAPI
	{
	public:
		CWinsockAPI(void);
		~CWinsockAPI(void);

		static INT BindSocket(SOCKET _socket, PSOCKADDR_IN _pSockAddr);
		static INT ListenSocket(SOCKET _socket, INT _maxConn = SOMAXCONN);

		static SOCKET AcceptSocket(SOCKET _socket, PSOCKADDR_IN _pPeer);

		static INT ConnectSocket(SOCKET _socket, PSOCKADDR_IN _pPeer);	
		static VOID CloseSocket(SOCKET _socket);

		static INT ReadTCPSocket(SOCKET _socket, PUCHAR _pbyBuffer, INT _len); // API recv() call
		static INT WriteTCPSocket(SOCKET _socket, PUCHAR _pbyBuffer, INT  _len);  // API send() call		
	};
} //namespace nsGSS

#endif//__GSS_WINSOCKAPI_H__

#ifndef __GSS_LIB_H__
#define __GSS_LIB_H__

namespace nsGSS
{
	#define		MAIN_SOCKET_CONTEXT_KEY		54321
	#define		DFL_PROTOCOL_TYPE			1
	#define		DFL_PORT_NUMBER				10105

	#define		DFL_PINGTHREAD_INTERVAL		50
	#define		DFL_SERVER_PINGREQ_TIME		1000

	/** 
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	@brief Server & Client 공통

	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	*/

	class CGNetLIB
	{
	public:
		CGNetLIB(void);
		virtual ~CGNetLIB(void);

		__inline LPSocketContext GetMainSocketPtr() {return	&m_mainSocket;}			
		
		INT GetBindAddress(PCHAR szAddress, USHORT portNumber, PSOCKADDR_IN pSa);

	protected:
		//BOOL InitDNetLIB();					///< WS2_32.DLL 사용
		//VOID DeinitDNetLIB();					///< WS2_32.DLL 반환		

		__inline VOID SetPortNum(USHORT _portNum) {m_portNum = _portNum;}
		__inline USHORT GetPortNum() {return m_portNum;}
		__inline VOID SetIP(ULONG _dwIP) {m_IP = _dwIP;}
		__inline ULONG GetIP() {return m_IP;}	

	protected:
		SocketContext m_mainSocket;						// Server:TCP-Listen Socket, UDP-사용 안함, Client:통신 소켓

	private:	
		ULONG m_IP;
		USHORT m_portNum;							    // 포트
	};

	/**
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	@brief Server 관련

	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	*/

	typedef	struct __stServerStartParam
	{	
		UINT uiServerMaxUser;
		UINT uiServerExtraUser;
		UINT uiExtraBufferSize;	
		UINT uiWorkerThreadCount;
		BOOL bUseAcceptEx;	

		__stServerStartParam(UINT _uiServerMaxUser = 100, UINT _uiServerExtraUser = 0, UINT _uiExtraBufferSize = MAX_BUF_SIZE * 2, UINT _uiWorkerThreadCount = 0, BOOL _bUseAcceptEx = FALSE)
		{
			uiServerMaxUser	= _uiServerMaxUser;
			uiServerExtraUser =	_uiServerExtraUser;
			uiExtraBufferSize =	_uiExtraBufferSize;
			uiWorkerThreadCount	= _uiWorkerThreadCount;
			bUseAcceptEx = _bUseAcceptEx;		
		};

	}stServerStartParam, *pstServerStartParam;

	class CBaseNetworkMgr;
	class CServer : public CGNetLIB
	{
	public:
		CServer();
		virtual	~CServer();

		virtual	VOID Init();				///<	초기화
		virtual	VOID Deinit();				///<			
					 
		virtual	VOID PreStart(INT _iSocketType, LPCTSTR _szIP, USHORT _port, INetworkHandler* _pNetHandler);	///< 소켓타입, 포트, 콜백핸들러설정	
		virtual	INT	 Start(CONST stServerStartParam* CONST _pstServerStartParam);			///< 시작
		virtual	VOID End();					///< 종료, 리소스 정리
					 
		virtual	INT	 Accept() = 0;			///< TCP만 Accept 
		virtual	VOID Close(INT _iKey) = 0;	///< 해당 소켓 종료 
					 
		virtual	INT	 Read(INT _iKey, PBYTE _pbyBuffer, INT _iLen, INT _iUDPParentKey=0) = 0;						///< _pbyBuffer에다 데이터 ReadData push
		virtual	INT	 Write(INT _iKey, PBYTE _pbyBuffer, INT _iLen) = 0;				///< send 함수 호출	
		virtual	INT	 WriteCompleteCheck( INT _iKey, PBYTE _pbyBuffer, INT _iLen) = 0;	///< sendComplete Check UDP + IOCP 에서만 의미있음 나머지는 Write() 호출과 같음.		

		INT	WriteFromWriteBuffer(INT _iKey);									///< 쓰기 버퍼에 담아져있는 데이터를 보내기
		INT	GetDataReadBuffer(INT _iKey, PBYTE _pBuffer, INT _iLen);	///< 해당 컨텍스트의 리드 버퍼에 있는 데이터 가져오기
		INT	PutDataWriteBuffer(INT _iKey, PBYTE _pBuffer, INT _iLen);	///< 해당 컨텍스트의 쓰기 버퍼에 데이터 넣기

		__inline INT GetSocketType() {return m_socketType;}	
		
		__inline INetworkHandler* GetCallBackHandler() {return m_pCallBackHandler; }
		
		__inline LPSocketContext GetSocketContext(INT _iKey) {return m_pSocketCtxPool->Direct(_iKey);}
		
		__inline UINT GetMaxUserCount()	{return m_pSocketCtxPool->GetCreatedCount();}
		__inline UINT GetCurrentUser() {return m_pSocketCtxPool->GetUsedCount();}

		__inline BOOL IsStart()	{return m_start;}	

		void SetHost(TCHAR* _host) {_tcscpy_s(m_hostIP, _countof(m_hostIP), _host);}
		TCHAR* GetHost() {return m_hostIP;}

	protected:	
		__inline VOID SetSocketType(INT _iSocketType) {m_socketType = _iSocketType;}						///< 소켓 타입설정
		__inline VOID SetCallBackHandler(INetworkHandler* _pHandler) {m_pCallBackHandler = _pHandler;}	///< 콜백핸들러 설정

		INT CreateContextPool(UINT _uiServerMaxUser, UINT _uiServerExtraUser, UINT _uiExtraBufferSize = MAX_BUF_SIZE * 2);	///< 컨텍스트 풀 만들기
		INT StartNetworkMgr(ULONG32 _dwIP, INT iPortNumber, USHORT _usWorkerThreadCount);					///< 소켓 타입별 해당 네트워크 매니저 시작

	protected:
		CSMemoryPoolLIB<SocketContext>* m_pSocketCtxPool;	// 세션풀
		INetworkHandler* m_pCallBackHandler;				// 외부 컨텐츠와 연결되는 콜백핸들러
		CBaseNetworkMgr* m_pNetworkMgr;						// 네트워크 핸들러( 소켓타입별 생성 )
		INT m_socketType;									// Socket(일반소켓), IOCP, WSAAsync, Event
		BOOL m_start;										// Server 시작여부TCHAR* GetHost() {return m_hostIP;}

	private:
		TCHAR m_hostIP[16];
	};

	class CTCPServer : public CServer
	{
	public:
		CTCPServer();
		virtual ~CTCPServer();		

		virtual	VOID Init();				//	초기화
		virtual	VOID Deinit();				//	

		virtual VOID PreStart(INT _iSocketType, LPCTSTR _szIP, USHORT _port, INetworkHandler* _pNetHandler);	///<	소켓타입, 포트, 콜백핸들러
		virtual	INT	Start(CONST stServerStartParam* CONST _pstServerStartParam);///< 
		virtual	VOID End();					///< 종료, 리소스 정리

		virtual	INT	Accept();
		virtual INT	Read(INT _iKey, PBYTE _pbyBuffer, INT _iLen, INT _iUDPParentKey = 0 );							///< _pbyBuffer에다 데이터 read
		virtual INT	Write(INT _iKey, PBYTE _pbyBuffer, INT _iLen);					// _pbyBuffer에 있는 데이터 _iLen길이만큼 전송
		virtual	INT	WriteCompleteCheck(INT _iKey, PBYTE _pbyBuffer, INT _iLen);	// 내부적으로 Write()호출

		virtual	VOID Close(INT _iKey);																				///< 해당 컨텍스트 종료
	};

	/**
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	Client 관련

	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	*/

	class IClientNetworkHandler
	{
	public:	
		virtual INT	OnFinishedConnect(INT _iRegistedIndex)  = 0;	
		virtual INT	OnFinishedReading(INT _iRegistedIndex, PBYTE pbyReadBuffer, INT _iSize) = 0;
		virtual INT	OnFinishedSending(INT _iRegistedIndex) = 0;		
		virtual INT	OnClose(INT _iRegistedIndex) = 0;
	};

	class CClient : public CGNetLIB
	{
		friend class CMultiClientMgr;

	public:
		CClient();
		virtual ~CClient();

		virtual BOOL PreStart(IClientNetworkHandler* _pCallback = NULL, INT iReadBufferSize = MAX_BUF_SIZE, INT iWriteBufferSize = MAX_BUF_SIZE , BOOL _bUseReadBuffer = TRUE, BOOL _bUseWriteBuffer = TRUE );	///< 메인 컨텍스트 버퍼 세팅
		virtual INT Start(LPCTSTR _szIPAddress, INT _iPortNumber) = 0;	// 해당 정보로 접속

		virtual INT Read(PBYTE _pbyBuffer, INT _iLen, PSOCKADDR_IN _pstAddr = NULL) = 0;												///< _pbyBuffer 에 최대 _iLen만큼 데이터 recv
		virtual INT	Write(PBYTE _pbyBuffer, INT _iLen) = 0;												///< _pbyBuffer에 있는 데이터 _iLen만큼 send
		
		virtual INT	WriteFromWriteBuffer() = 0;				///< 쓰기버퍼에 담아져 있는 데이터 전송 함수

		virtual VOID End();									///< 소켓 종료	

		virtual	VOID SendExtraQ() = 0;

		__inline INT PutDataReadBuffer(PBYTE _pBuffer, INT _iLen)		///< ReadBuffer에 데이터 입력
		{
			INT iLen =	m_mainSocket.PutDataReadBuffer(_pBuffer, _iLen);
			return iLen;
		}

		__inline INT GetDataReadBuffer(PBYTE _pBuffer, INT _iLen)		///< ReadBuffer에서 데이터 가져오기
		{	
			INT iLen =	m_mainSocket.GetDataReadBuffer(_pBuffer, _iLen);
			return iLen;
		}

		__inline INT PutDataWriteBuffer(PBYTE _pBuffer, INT _iLen)		///< WriteBuffer에 데이터 입력
		{
			INT iLen =	m_mainSocket.PutDataWriteBuffer(_pBuffer, _iLen);
			return iLen;
		}

		__inline INT GetDataWriteBuffer(PBYTE _pBuffer, INT _iLen)		///< WriteBuffer에서 데이터 가져오기
		{	
			INT iLen =	m_mainSocket.GetDataWriteBuffer(_pBuffer, _iLen);
			return iLen;
		}

		__inline VOID SetStart(BOOL _bStart)	///< 시작여부 세팅		
		{
			m_bStart = _bStart;
		}

		__inline BOOL IsStart()				///< 클라이언트 시작여부
		{
			return m_bStart;
		}		

		__inline PSOCKADDR_IN GetSendAddr()		///< 서버 정보
		{
			return &m_stSendAddr;
		}		

		__inline LONG IsConnect()
		{
			return GetMainSocketPtr()->IsConnect();
		}

		__inline void SetConnect(BOOL _bConn)
		{
			GetMainSocketPtr()->SetConnect(_bConn);
		}	

		__inline IClientNetworkHandler*	GetNetworkHandler()
		{
			return m_pCallback;
		}

	protected:
		virtual VOID Init();			///<초기화
		virtual VOID Deinit();		///<	

	protected:		
		SOCKADDR_IN	m_stSendAddr;		///< 서버 정보	

	private:	
		BOOL m_bStart;			///< 시작여부	
		IClientNetworkHandler*	m_pCallback;
	};

	class CTCPClient : public CClient
	{
	public:
		CTCPClient();
		virtual ~CTCPClient();

	public:
		virtual BOOL PreStart(IClientNetworkHandler* _pCallback = NULL, INT iReadBufferSize = MAX_BUF_SIZE, INT iWriteBufferSize=MAX_BUF_SIZE, BOOL _bUseReadBuffer = TRUE, BOOL _bUserWriteBuffer = TRUE);		///< 소켓 생성, readbuffer, writebuffer 버퍼 사이즈 설정

		virtual INT Start(LPCTSTR _szIPAddress, INT _iPortNumber);	///< 해당 정보로 서버에 접속
		virtual VOID End();																										///< 소켓 종료

		virtual INT Read(PUCHAR _pbyBuffer, INT _iLen, PSOCKADDR_IN _pstAddr = NULL );					///< _pbyBuffer 버퍼에 최대 _iLen 길이만큼 데이터 받기 준비 (sockaddr_in*  _pstAddr 는 UDP용)
		virtual INT Write(PUCHAR _pbyBuffer, INT _iLen);					///< _pbyBuffer 버퍼에 담겨진 데이터 _iLen만큼 전송하기	
		virtual INT	WriteFromWriteBuffer();				///< 쓰기버퍼에 담아져 있는 데이터 전송 함수

		virtual	VOID SendExtraQ();						///< ExtraBuffer에 있는 데이터 전송	

	private:
		virtual VOID Init();
		virtual VOID Deinit();
	};

	class CFactory
	{
	public:
		static CServer*	CreateServer() ///< 서버 객체 생성 함수
		{		
			return new CTCPServer();		
		}

		static CClient*	CreateClient() ///< 클라이언트 객체 생성 함수
		{		
			return new CTCPClient();	
		}
	};

	/**

	 MultiClient, MultiRelayAck 관련

	*/

	typedef struct __stEventSocket
	{
		WSAEVENT hEvent[WSA_MAXIMUM_WAIT_EVENTS];		//! WSAEventSelect에 등록될 이벤트 객체
		CClient* pClient[WSA_MAXIMUM_WAIT_EVENTS];		//! WSAEventSelect에 등록될 Client PTR 객체

		__stEventSocket()
		{			
			for (int i = 0; i < WSA_MAXIMUM_WAIT_EVENTS; i++)
			{
				pClient[i] = NULL;
				hEvent[i] = NULL;
			}
		}	
	}stEventSocket, *pstEventSocket;

	class CMultiClientMgr
	{	
		friend UINT WINAPI RunClientWSAEventSelect(VOID* _pMultiClient);	//! WSAEventSelect Thread Func		

	public:	
		CMultiClientMgr();
		virtual ~CMultiClientMgr();

		BOOL initDNetLIB();						///< WS2_32.DLL 사용
		VOID deinitDNetLIB();					///< WS2_32.DLL 반환

		BOOL Init(IClientNetworkHandler* _pCallbackHandler);	//! 초기화
		VOID Deinit();

		INT PreStartClient(); //! 객체 생성 함수
		BOOL StartClient(INT _iRegistedIndex,	//! 실제 접속 요청 함수	
						 LPCTSTR _szIP, 
						 USHORT _u4Port, 
						 INT _iReadBufferSize = MAX_BUF_SIZE, 
						 INT _iWriteBufferSize = MAX_BUF_SIZE, 
						 BOOL _bUseReadBuffer = TRUE,
						 BOOL _bUseWriteBuffer = TRUE);
		VOID EndClient(INT _iRegistedIndex);				//! 접속 종료
		VOID ClearClient(INT _iRegistedIndex);				//! 클라이언트 정보및 객체 삭제

		INT	Write(INT _iIndex, PKTDATATYPE* _pBuffer, PKTSIZETYPE _iBufferLen);		//!	전송함수

		BOOL IsConnect(INT _iRegistedIndex);				//! 서버 접속여부 확인
		CClient* GetClientPtr(INT _iRegistedIndex);			//! 
		
		BOOL IsActive(INT _registedIndex);	// Client 시작여부, 구동여부 
		__inline BOOL IsStart() {return m_bStart;}			//! MultiClientMgr 객체 작동여부

	private:
		VOID RunWSAEventSelect();							//! WSAEventSelect Thread에서 실제 호출하는 함수	

		INT GetFreeEventSocketIndex();						//! 사용가는한 이벤트(클라이언트) 인덱스
		INT GetFreeEventSocketCount();						//! 현재 남은 이벤트 인덱스 개수
		VOID SetFreeEventSocketIndex(INT _iIndex);			//! 이벤트(클라이언트) 인덱스 반환	

		__inline VOID SetStart(BOOL _bStart) {m_bStart = _bStart;}

	private:	
		IClientNetworkHandler* m_pCallbackHandler;			// 핸들러도 개별로 등록될수 있으면 좋겠지만 지금은 요구가 없으니 패스..

		BOOL m_bStart;								
		stEventSocket m_stEventSocket;
		
		std::deque<INT> m_dequeEnableSocketIndex;

		HANDLE m_hEventSelectThread;						// WSAEventSelect 기본 쓰레드 핸들러				

		Lock m_lock;
		SOCKET m_sockClose;
	};

} //namespace nsGSS
#endif//!__GSS_LIB_H_

#ifndef __GSS_WORK_INFO_H__
#define __GSS_WORK_INFO_H__

#pragma once

#include "IPHLPAPI.h"

namespace nsGSS
{
	class CDNetworkInfo
	{
	public:
		CDNetworkInfo(void);
		~CDNetworkInfo(void);

	public:
		BOOL GetMacAddress(char* _pMacAddr);

		DWORD	    GetInternetIP(char *szIP = NULL);
		void		GetPublicIP( std::vector<std::string>& vIPList );
		void		GetPrivateIP( std::vector<std::string>& vIPList );
		bool        IsPublic(const char *szIP);
		bool        IsPublic(unsigned long iIP);

		__inline bool	IsLoad()	{ return m_bLoad; }
		
	protected:	
		void		GetAdapterInfo();
		void		GetRoutingTableInfo();
		DWORD	    GetIP( int dwIndex, char *szIP );	

	private:
		bool m_bLoad;
		stdext::hash_map<int, IP_ADDRESS_STRING> m_hmAdapList;
		stdext::hash_map<int, string> m_hmMacAddrList;
		std::list<MIB_IPFORWARDROW> m_liRoutingInfo;	

		// Private과 Public을 구분하기 위해서 HOST로 쓸수없는 IP영역을 저장한다..
		// IMPL 패턴
		struct IMPL;
		IMPL   *m_pImpl;
	};

} //namespace nsGSS

#endif // __GSS_WORK_INFO_H__

#ifndef __GENERAL_DEFINE_H__
#define  __GENERAL_DEFINE_H__

enum
{
	MAX_IP_LEN				= 16,
	MAX_USERID_LEN			= 50,
	MAX_PASSWORD_LEN		= 32,	
	MAX_MACADDR_LEN			= 32,	
	MAX_TOKEN_LEN			= 256,	
	MAX_PATH_LEN			= 1024,	
	MAX_SESSIONTOKEN_LEN	= 512,
};

// 바꾸면 인증 안됨. (MATER_TOKEN => "memberID|MASTERKEY")
#define MASTER_KEY "AWESOME"
#define TOKEN_DELIMETER "|"

#define AUTH_CLIENT_INI_PATH "./authClientInfo.ini"

#endif //__GENERAL_DEFINE_H__//////////////////////////////////////////////////////////////////////////
//
// 패킷의 기본 정의
//
//////////////////////////////////////////////////////////////////////////

#ifndef __COMMON_PACKET_H__
#define __COMMON_PACKET_H__

#define PKT_PH			0xF1
#define PKT_PH_LEN		1		// Packet Header Length

enum eFC
{	
	// 이하 값은 절대 바꾸면 안됨!!
	// 앞에 값은, 기존에 쓰던 값으로 인해 중복될 우려가 있기에. 일단 0x01이 아닌 0x11부터 패킷을 사용한다.

	FC_CACHEAUTH = 0x11,	// CHACHE(S)와 AUTH(C) 통신
	FC_AUTHLAUNCHER,		// AUTH(S)와 LAUNCHER(C) 통신
	FC_AUTHGAME,			// AUTH(S)와 GAME(C) 통신		
	
	FC_AUTHWEB,				// web 서버(C)와.. - protocol buffer

	FC_MAX = 0xFF,	
};

enum eSC
{
	SC_MAX = 0xFF,
};

#endif //__COMMON_PACKET_H__

#ifndef __GAMESERVER_PACKET_H__
#define __GAMESERVER_PACKET_H__

//////////////////////////////////////////////////////////////////////////
//
// AuthServer(S)-GameServer(C) 통신시에 사용하는 패킷 정의
// FC : P_FC_AUTHGAME
//
//////////////////////////////////////////////////////////////////////////

enum 
{	
	SC_G2A_CheckAuth = 0x01,	// 인증값(토큰값) 확인요청
	SC_A2G_CheckAuthRT,		

	SC_G2A_LogoutUser,			// 게임 IN, OUT 여부(구분 가능할 경우만 사용)
	SC_A2G_LogoutUserRT,

	SC_A2G_KickUser,			// User Kick
	SC_G2A_KickUserRT,

	SC_A2G_Ping,
	SC_G2A_Pong,				// 핑퐁	(heart beat check용)
};

#pragma pack(1)
	
	// Token 값 확인
	typedef struct _P_G2A_CheckAuth
	{
		_P_G2A_CheckAuth()
		{			
			identity = -1;						
			ZeroMemory(token, _countof(token));
		}
		
		INT identity;		
		CHAR token[MAX_TOKEN_LEN];
	}P_G2A_CheckAuth,*LP_G2A_CheckAuth;

	typedef struct _P_A2G_CheckAuthRT
	{
		enum
		{			
			SUCC_AUTH = 1,		// 성공

			FAIL_GENERAL = 0,	// 로직 안탐 ( AuthServer 로직 에러)

			FAIL_MEMBERID = -1, // 인증 Flow안탄 이상한 유저
			FAIL_TOKEN = -2,		// 인증 토큰 값이 다름

			FAIL_DELAY = -3,		// 인증 시간 5초 이상 지연			
		};

		_P_A2G_CheckAuthRT()
		{
			ret = FAIL_GENERAL;
			identity = -1;
			memberId = 0;
			ZeroMemory(userId, _countof(userId));
		}		
		
		INT ret;
		INT identity;
		UINT memberId;
		CHAR userId[MAX_USERID_LEN];
	}P_A2G_CheckAuthRT, *LP_A2G_CheckAuthRT;

	typedef struct _P_G2A_LogoutUser
	{
		_P_G2A_LogoutUser()
		{
		}
		
		UINT memberId;		
	}P_G2A_LogoutUser, *LP_G2A_LogoutUser;

	typedef struct _P_A2G_LogoutUserRT
	{
		_P_A2G_LogoutUserRT()
		{
		}
		
		UINT memberId;		
		BOOL retType;
	}P_A2G_LogoutUserRT, *LP_A2G_LogoutUserRT;
	
	// Kick 유저 - 크파에서는 의미가 없음. 그러나 중복 로그인 시에 사용은 함!(크파는 자체 중복 로그인 처리함 )
	typedef struct _P_A2G_KickUser
	{
		enum
		{
			// reason;
			REASON_GENERAL = 0,
			REASON_DUPLICATION, // 중복 로그인
		};

		_P_A2G_KickUser()
		{
			memberId = 0;
			reason = REASON_GENERAL;
		}

		UINT memberId;
		INT reason;
	}P_A2G_KickUser, *LP_A2G_KickUser;

	typedef struct _P_G2A_KickUserRT
	{
		_P_G2A_KickUserRT()
		{
			memberId = 0;
			retType = FALSE;
		}

		UINT memberId;
		BOOL retType;
	}P_G2A_KickUserRT, *LP_G2A_KickUserRT;

	// heart beat Check
	typedef struct _P_A2G_Ping
	{
		//정의 없음
	}P_A2G_Ping, *LP_A2G_Ping;

	typedef struct _P_G2A_Pong
	{
		// 정의 없으
	}P_G2A_Pong, *LP_G2A_Pong;

#pragma pack()

#endif //__GAMESERVER_PACKET_H__

#ifndef __AUTH_NET_PROCESS_H__
#define __AUTH_NET_PROCESS_H__

#pragma once

namespace nsGSS
{	
	//#define MAX_AUTH_CLIENT_COUNT 4

#define MAX_LOGICTHREAD_COUNT 100
	
	class GsAuthNetProcess;

	struct stAuthThreadParam
	{
		GsAuthNetProcess* pNetProcess;
		INT threadNum;
	};

	class GsAuthClientNetworkHandler : public IClientNetworkHandler
	{
	public:
		GsAuthClientNetworkHandler(GsAuthNetProcess* _ptrNetProcess);
		virtual ~GsAuthClientNetworkHandler(){}

		virtual INT OnFinishedReading(INT _iRegistedIndex, PBYTE pbyReadBuffer, INT _iSize);
		virtual INT OnFinishedSending(INT _iRegistedIndex);
		virtual INT OnFinishedConnect(INT _iRegistedIndex);
		virtual INT OnClose(INT _iRegistedIndex);

	private:
		GsAuthNetProcess* m_ptrNetProcess;
	};	

	class GsAuthBaseClient;
	class GsAuthClientProc;

	class GsAuthNetProcess
	{
	public:
		GsAuthNetProcess(GsAuthBaseClient* _pAuthBaseClient);
		~GsAuthNetProcess(void);
		
		BOOL netInit(INT maxAuthClientCount, CHAR* _pHost, USHORT _pPort);
		BOOL netDeinit();
				
		BOOL reConnect();
		BOOL isConnected();
		
		INT& getRegistedIndex();

		CMultiClientMgr& getMcm() {return m_mcm;}

		VOID runLogicThread(INT _threadNum);
		const HANDLE& getCompletionPort() const {return m_completionPort;}

		VOID checkUserAuthTime();

		INT sendToAuth(PacketControlBuffer& _ctrlBuf, INT _registedIndex = -1);

	private:
		BOOL _connect(CHAR* _host, USHORT _port);
		VOID _disConnect();

	private:
		INT m_maxAuthClientCount;
		
		CHAR m_host[16];
		USHORT m_port;		

		GsAuthBaseClient* m_ptrAuthClient;
		GsAuthClientProc* m_pAuthClientProc;

		BOOL m_netStart;

		HANDLE m_completionPort;
		HANDLE m_hLogicThread[MAX_LOGICTHREAD_COUNT];
		stAuthThreadParam m_logicThreadParam[MAX_LOGICTHREAD_COUNT];

		INT m_registedIndex[WSA_MAXIMUM_WAIT_EVENTS];
		LONG m_sendCount;		

		CMultiClientMgr m_mcm;
		GsAuthClientNetworkHandler* m_pClientNetworkHandler;		
	};
} // nsGSS

#endif //__AUTH_NET_PROCESS_H__


#ifndef __AUTH_BASE_CLIENT_H__
#define __AUTH_BASE_CLIENT_H__

#pragma once

namespace nsGSS
{	
#ifdef __CREATING_DLL_
	#define EXPORT_OR_IMPORT __declspec(dllexport)
#elif __JUST_GENERAL_CLASS__
	#define EXPORT_OR_IMPORT
#else
	#define EXPORT_OR_IMPORT __declspec(dllimport)
#endif 


	#define dMAX_AUTH_WAITING_TIME 5 // 인증 서버로의 결과는 최대 client당 5초까지만 기다린다. 조정가능.
	
	typedef struct stRequestAuthInfo
	{
		void init()
		{
			memberId = 0;
			requestTime = 0;
			identity = -1;						
			bDel = FALSE;
		}

		UINT memberId;
		time_t requestTime;
		INT identity;
		BOOL bDel;

	}REQUESTAUTHINFO,*LPREQUESTAUTHINFO;
	

	class EXPORT_OR_IMPORT GsAuthBaseClient
	{		
		friend class GsAuthClientProc;
		friend unsigned int WINAPI RunAuthThread(VOID* _pAuthBaseClient);

	public:
		GsAuthBaseClient(void);
		virtual ~GsAuthBaseClient(void);		

		BOOL startClient(CHAR* _pHost, USHORT _port);
		BOOL reConnect();
		VOID endClient();

		BOOL isAlive();	 // AuthServer와의 접속 여부

		// @ sendFuntion, to AUTH Server 
		VOID sendAuthToken(CHAR* _pToken, INT _identity); // 토큰값 확인
		VOID sendLogoutUser(UINT _memberId);
		VOID sendKickUserResult(UINT _memberId, BOOL _retType);

		// @ recvFuncion, from AUTH Server		
		virtual VOID recvTokenResult(UINT _memberId, CHAR* _userId, INT _result, INT _identity) = 0; // 토큰 값 확인 결과
		virtual VOID recvKickUser(UINT _memberId, INT _reason) = 0;
		virtual VOID recvLogoutUserResult(UINT _memberId, BOOL _retType) = 0;

	protected:
		VOID getFilePath(CHAR* _filePath, INT _filePathLen);
		
	private:
		VOID sendPong(INT _registedIndex); // Auth 핑퐁 - 자체처리

		VOID threadFuncCheckAuthList();

	private:
		VOID init();
		VOID deinit();
		
		BOOL addCheckAuthList(UINT _memberId, INT _identity);
		BOOL delCheckAuthList(UINT _memberId);
		
	private:
		GsAuthNetProcess* m_pAuthNetProcess;		
		CAtlMap<UINT, stRequestAuthInfo>* m_pMAuthList; // usn, pool Key;

		Lock* m_pLock;		

		HANDLE m_hCheckAuthThread; 
		BOOL m_loop;

		UINT m_sendPongCount; 	
		BOOL m_bUseLog;	

		INT m_authClientCount;
	};		

} // nsGSS

#endif // __AUTH_BASE_CLIENT_H__

