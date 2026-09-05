#ifndef __ioHackShield_h__
#define __ioHackShield_h__

#ifdef HACKSHIELD

// HShield.h _AHNHS_TRANS_BUFFER, ANTICPX_TRANS_BUFFER_MAX 동일해야 함. 컴파일 오류로 똑같은 struct 제작
#pragma pack(push) 
#pragma pack(8)
#define MAX_HACKSHIELD_PACKET	400
typedef struct tagHackShieldPacket
{
	unsigned char byBuffer[MAX_HACKSHIELD_PACKET];
	unsigned short nLength;
} HackShieldPacket;
#pragma pack(pop) 
//

#pragma comment(lib, "HackShield/HSUpChk.lib" )
#pragma comment(lib, "HackShield/HShield.lib" )

class ioHackShield
{
public:
	enum CallBackType
	{
		CBT_NONE       = 0,
		CBT_ERROR_EXIT = 1,
	};
protected:
	static ioHackShield *sg_Instance;

protected:
	char        m_szModuleFilePath[MAX_PATH];
	DWORD       m_dwStartErrorType;

	bool         m_bRunCallBack;
	CallBackType m_eCallBackType;
	ioHashString m_sErrorLog;
	long         m_iErrorCode;
	
public:
	static ioHackShield &GetInstance();
	static void ReleaseInstance();

public:
	bool Update();
	bool Start();
	void End();
	bool OnRecieveCheck( IN HackShieldPacket &rRecvBuf, OUT HackShieldPacket &rSendBuf );
	void ShowErrorDlg( const ioHashString &rsPrivateID );
	void SetUserID(const ioHashString &rsPrivateID);

	DWORD GetStartErrorType() const { return m_dwStartErrorType; }
	long GetErrorCode() const { return m_iErrorCode; }

	void SetRunCallBack(bool bRunCallBack) { m_bRunCallBack = bRunCallBack; }
	void SetCallBackType( CallBackType eCallBackType) { m_eCallBackType = eCallBackType; }
	void SetErrorLog( const char *szErrorLog ) { m_sErrorLog = szErrorLog; }
	void SetErrorCode(long iErrorCode) { m_iErrorCode = iErrorCode; }

	void Process();

protected:
	void SetModuleFilePath();

private: // Singleton class
	ioHackShield(void);
	virtual ~ioHackShield(void);
};

#define g_ioHackShield ioHackShield::GetInstance()

#endif // HACKSHIELD

#endif // __ioHackShield_h__