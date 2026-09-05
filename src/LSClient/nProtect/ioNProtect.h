#ifndef __ioNProtect_h__
#define __ioNProtect_h__

#ifdef NPROTECT

#include "NPGameLib.h"

#pragma comment(lib, "nProtect/NPGameLib.lib" )

#ifdef NPROTECT_CSAUTH3
#pragma pack(1) 
typedef struct tagNProtectPacket
{
	enum 
	{
		 MAX_PACKET_BUF = 4096,
	};

	DWORD m_dwStructSize;
	BYTE  m_byNProtectPacket[MAX_PACKET_BUF];

	tagNProtectPacket()
	{
		m_dwStructSize = 0;
		ZeroMemory( m_byNProtectPacket, sizeof( m_byNProtectPacket ) );
	}
}NProtectPacket;
#pragma pack()
#endif // NPROTECT_CSAUTH3

class ioNProtect
{
public:
	enum CallBackType
	{
		CBT_NONE       = 0,
		CBT_ERROR_EXIT = 1,
		CBT_ERROR_LOG  = 2,
		CBT_ERROR_LOG2  = 3, // 에러로그 구분용
		CBT_SEND_CHECK = 4,
	};

	enum 
	{
		MAX_CHECK_CNT = 9, 
	};
protected:
	static ioNProtect *sg_Instance;

protected:
	DWORD       m_dwStartErrorType;
	int         m_iCheckCnt;

	bool        m_bRunCallBack;
	CallBackType m_eCallBackType;
	ioHashString m_sErrorLog;
	bool		m_bMessageBox;

#ifdef NPROTECT_CSAUTH3
	NProtectPacket m_kCheckData;
#else
	GG_AUTH_DATA m_kCheckData;
#endif
	
public:
	static ioNProtect &GetInstance();
	static void ReleaseInstance();

public:
	bool Start();
	void GetStartErrorMsg( OUT char *szError, IN int iSize );
	void SetHwnd( HWND hWnd );
	void Check( bool bDirectCheck );
	void SendPrivateID( const char *szPrivateID );
#ifdef NPROTECT_CSAUTH3
	void OnRecieveCheck( BYTE *pPacket, DWORD dwPacketSize );
#else
	void OnRecieveCheck( GG_AUTH_DATA &rkAuthData );
#endif
	LPBYTE GetHackInfo( OUT DWORD *dwSize );

	DWORD GetStartErrorType() const { return m_dwStartErrorType; }

	void SetRunCallBack(bool bRunCallBack) { m_bRunCallBack = bRunCallBack; }
	void SetCallBackType( CallBackType eCallBackType) { m_eCallBackType = eCallBackType; }
	void SetErrorLog( const char *szErrorLog ) { m_sErrorLog = szErrorLog; }
#ifdef NPROTECT_CSAUTH3
	void SetCheckData( const NProtectPacket &rkCheckData );
#else
	void SetCheckData( const GG_AUTH_DATA &rkCheckData) { m_kCheckData = rkCheckData; }
#endif
	void SetD3dDevice(IDirect3DDevice9 * pD3dDevice);

	void Process();

	void SetMessageBox(bool val) { m_bMessageBox = val; }
	bool GetMessageBox() const { return m_bMessageBox; }

private: // Singleton class
	ioNProtect(void);
	virtual ~ioNProtect(void);
};

#define g_ioNProtect ioNProtect::GetInstance()

#endif // NPTROTECT

#endif // __ioNProtect_h__