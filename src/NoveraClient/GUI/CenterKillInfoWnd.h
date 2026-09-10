#pragma once

#include "../ioModeUIController.h"

class ioINILoader;
class ioXMLElement;
class ioUIRenderImage;
class ioComplexStringPrinter;

enum CenterKillAniType
{
	CKAT_NONE,
	CKAT_WAIT,
	CKAT_WAIT_LOOP,
	CKAT_FADE_IN,
	CKAT_LOOP,
	CKAT_LOOP_ADD,
	CKAT_FADE_OUT,
};

class CenterKillInfoWnd : public ioWnd
{
protected:
	typedef struct tagKillCountInfo
	{
		ioHashString szKillComboImgNameRed;
		ioHashString szKillComboImgNameRedKill;
		ioHashString szKillComboImgNameBlue;
		ioHashString szKillComboImgNameBlueKill;
		ioHashString szKillComboSoundName;
	}KillCountInfo;
	typedef std::map< int, KillCountInfo > KillCountInfoMap;
	KillCountInfoMap m_KillCountInfoMap;

protected:
	typedef struct tagReserveCenterKillInfo
	{
		//е╦ют
		CenterKillType KillType;

		TeamType KillCharTeam;
		ioHashString szKillCharName;
		DWORD dwKillCharItemCode;
		bool bKillerMale;

		TeamType DieCharTeam;
		ioHashString szDieCharName;

		int iKillCount;

	public:
		bool IsEmpty()
		{
			if ( KillType == CKT_NONE && ( KillCharTeam == TEAM_NONE && DieCharTeam == TEAM_NONE ) &&
				( szKillCharName.IsEmpty() && szDieCharName.IsEmpty() ) &&
				( dwKillCharItemCode == 0 && iKillCount == 0 ) )
					return true;

			return false;
		}

		void Init()
		{
			KillType = CKT_NONE;
			KillCharTeam = DieCharTeam = TEAM_NONE;
			dwKillCharItemCode = iKillCount = 0;
			szKillCharName.Clear();
			szDieCharName.Clear();
			bKillerMale = false;
		}

	}ReserveCenterKillInfo;
	ReserveCenterKillInfo m_ReserveCenterKillInfo;

protected:
	//-----------------Common-------------------
	int						m_iKillComboMaxCount;
	ioHashString			m_szCurCenterInfoSound;
	CenterKillType			m_CurCenterKillType;

	//------------------Line--------------------
	CenterKillAniType		m_LineAniType;
	ioComplexStringPrinter	m_KillInfoPrinter;
	ioUIRenderImage			*m_pCurLine;
	ioUIRenderImage			*m_pSoldierIcon;
	ioUIRenderImage			*m_pSoldierSubIcon;
	float					m_fCurLineAlpha;
	float					m_fCurLineAlphaSpeed;
	DWORD					m_dwCurLineLoopTime;
	DWORD					m_dwCurLineFadeOutTime;
	DWORD					m_dwLineAniStartTime;

	//Data
	ioUIRenderImage			*m_pIconBG;
	ioUIRenderImage			*m_pKillRedLine;
	ioUIRenderImage			*m_pKillBlueLine;
	ioUIRenderImage			*m_pNoneItemKillerIcon;
	DWORD					m_dwOneKillLineLoopTime;
	DWORD					m_dwOneKillLineFadeOutTime;
	DWORD					m_dwOtherKillLineLoopTime;
	DWORD					m_dwOtherKillLineFadeOutTime;

	//----------------One Kill------------------
	CenterKillAniType		m_OneKillAniType;
	ioUIRenderImage			*m_pCurOneKillImg;

	float					m_fCurOneKillAlpha;
	float					m_fCurOneKillAlphaSpeed;
	float					m_fCurOneKillScale;
	float					m_fCurOneKillScaleSpeed;
	DWORD					m_dwOneKillAniStartTime;
	
	//Data
	DWORD					m_dwOneKillFadeInTime;
	DWORD					m_dwOneKillLoopTime;
	DWORD					m_dwOneKillFadeOutTime;
	float					m_fOneKillStartAlpha;
	float					m_fOneKillStartScale;

	ioHashString			m_szOwnerKillSound;
	ioHashString			m_szOwnerDieSound;
	ioHashString			m_szTeamCharKillSound;
	ioHashString			m_szTeamCharDieSound;
	ioUIRenderImage			*m_pOwnerKillRedImg;
	ioUIRenderImage			*m_pOwnerKillBlueImg;
	ioUIRenderImage			*m_pOwnerDieRedImg;
	ioUIRenderImage			*m_pOwnerDieBlueImg;
	ioUIRenderImage			*m_pTeamCharKillRedImg;
	ioUIRenderImage			*m_pTeamCharKillBlueImg;
	ioUIRenderImage			*m_pTeamCharDieRedImg;
	ioUIRenderImage			*m_pTeamCharDieBlueImg;

	//------------------Other Kill--------------------
	CenterKillAniType		m_OtherImgAniType;
	CenterKillAniType		m_KillImgAniType;
	ioUIRenderImage			*m_pCurOtherImg;
	ioUIRenderImage			*m_pCurKillImg;

	DWORD					m_dwOtherImgStartTime;
	float					m_fCurOtherImgAlpha;
	float					m_fCurOtherImgAlphaSpeed;
	float					m_fCurOtherImgScale;
	float					m_fCurOtherImgScaleSpeed;
	DWORD					m_dwOtherImgTwinkleStartTime;
	bool					m_bOtherImgTwinkle;

	DWORD					m_dwKillImgStartTime;
	float					m_fCurKillImgAlpha;
	float					m_fCurKillImgAlphaSpeed;
	float					m_fCurKillImgScale;
	float					m_fCurKillImgScaleSpeed;
	DWORD					m_dwKillImgTwinkleStartTime;
	bool					m_bKillImgTwinkle;

	float					m_fCurTwinkleAlhpa;

	//Data
	DWORD					m_dwOtherImgFadeInTime;
	DWORD					m_dwOtherImgLoopAddTime;
	DWORD					m_dwOtherImgLoopTime;
	DWORD					m_dwOtherImgTwinkleTime;
	DWORD					m_dwOtherImgFadeOutTime;
	float					m_fOtherImgStartAlpha;
	float					m_fOtherImgStartScale;

	DWORD					m_dwKillImgWaitTime;
	DWORD					m_dwKillImgFadeInTime;
	DWORD					m_dwKillImgLoopTime;
	DWORD					m_dwKillImgLoopAddTime;
	DWORD					m_dwKillImgFadeOutTime;
	float					m_fKillImgStartAlpha;
	float					m_fKillImgStartScale;
	DWORD					m_dwKillImgTwinkleTime;

	float					m_fTwinkleBlueAlpha;
	float					m_fTwinkleRedAlpha;

	//Sound
	DWORD					m_dwSoundWaitTime;
	DWORD					m_dwSoundStartTime;
	
public:
	void ClearCurInfoSound() { m_szCurCenterInfoSound.Clear(); }

public:
	void SetOtherKillCenterUI( const int& iKillCount, const TeamType& eKillerTeam, const ioHashString &szKillerName,
							   const TeamType& eDieCharTeam, const ioHashString &szDieCharName, DWORD dwKillCharItemCode, bool bKillerMale );

	void SetOneKillCenterUI( const CenterKillType& KillType, const int& iKillCount, const TeamType& eKillerTeam, const ioHashString &szKillerName,
							 const TeamType& eDieCharTeam, const ioHashString &szDieCharName, DWORD dwKillCharItemCode, bool bKillerMale );

protected:
	void ClearData();

	void SetReserveInfo( const CenterKillType& KillType, const int& iKillCount, const TeamType& eKillerTeam, const ioHashString &szKillerName, 
						 const TeamType& eDieCharTeam, const ioHashString &szDieCharName, DWORD dwKillCharItemCode, bool bKillerMale );

	void SetStartOneKillCenterInfo( const CenterKillType& KillType, const int& iKillCount, const TeamType& eKillerTeam, const ioHashString &szKillerName,
		const TeamType& eDieCharTeam, const ioHashString &szDieCharName, DWORD dwKillCharItemCode, bool bKillerMale );

	void SetStartOtherKillCenterInfo( const int& iKillCount, const TeamType& eKillerTeam, const ioHashString &szKillerName,
		const TeamType& eDieCharTeam, const ioHashString &szDieCharName, DWORD dwKillCharItemCode, bool bKillerMale );

	void SetLineCenterAniInfo( const TeamType& eKillerTeam, const ioHashString &szKillerName, DWORD dwKillCharItemCode, DWORD dwLoopTime, DWORD dwFadeOutTime, bool bKillerMale );
	void SetOneKillAniInfo( const CenterKillType& KillType, const TeamType& eKillerTeam );
	void SetOtherKillAniInfo( const int& iKillCount, const TeamType& eKillerTeam, const ioHashString &szKillerName );

	void SetRserveInfoStart();

	//Process
	void OnLineProcess( float fTimePerSec );
	void OnOneKillProcess( float fTimePerSec );
	void OnOtherKillProcess( float fTimePerSec );

	//Render
	void OnLineRender( int iXPos, int iYPos );
	void OnOneKillRender( int iXPos, int iYPos );
	void OnOtherKillRender( int iXPos, int iYPos );

public:
	virtual void iwm_hide();
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	CenterKillInfoWnd(void);
	virtual ~CenterKillInfoWnd(void);
};

