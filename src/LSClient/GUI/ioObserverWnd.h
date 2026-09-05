#pragma once

#include "ResultInfo.h"
#include "../io3DEngine/ioMovingWnd.h"

class ioUIRenderImage;
class ioPlayStage;

class ioObserverRedWnd : public ioMovingWnd
{
public:
	enum
	{
		MAX_TEAM_PLAYER = 8,
		DEFAULT_HEIGHT = 31,
		DEFAULT_WIDTH = 160,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

protected:
	ioPlayStage *m_pPlayStage;

	ioUIRenderImage *m_pGuildBack;

	ioUIRenderImage *m_pBlueBack;
	ioUIRenderImage *m_pRedBack;
	ioUIRenderImage *m_pBlueGauge;
	ioUIRenderImage *m_pRedGauge;
	ioUIRenderImage *m_pRedDead;
	ioUIRenderImage *m_pAddGauge;

	ioUIRenderImage *m_pRedKeyText[MAX_TEAM_PLAYER];
	ioUIRenderImage *m_pBlueKeyText[MAX_TEAM_PLAYER];

	ioUIRenderImage *m_pTargetMark;

	DWORD m_dwMarkChangeTime;

	bool m_bCheckMark;
	DWORD m_dwCheckTime;

	typedef std::vector< RoundInfo > RoundInfoList;
	RoundInfoList m_RoundInfoList;

	int m_iDefaultX;
	int m_iDefaultY;

	static bool m_bPosToDefualt;

public:
	void InitObserverWnd( ioPlayStage *pStage );
	void SetInfo( RoundInfo &rkInfo );

	void ClearInfo();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:	
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

	void OnRedRender( int iXPos, int iYPos, int iSlot, const RoundInfo &rkInfo );
	void OnOtherRender( int iXPos, int iYPos, int iSlot, const RoundInfo &rkInfo );

protected:
	bool IsDieChar( const ioHashString &szName );
	bool IsTarget( const ioHashString &szName );

	float GetCurHPRate( const ioHashString &szName );

	void ProcessCameraClick();

	void UpdateMainDefaultPos();
	void UpdateDefaultPos();
	void UpdateWndSize();
	void UpdateMark();

public:
	static void SetPosToDefualt(bool PosToDefualt) { m_bPosToDefualt = PosToDefualt; }

public:
	ioObserverRedWnd();
	virtual ~ioObserverRedWnd();
};
/////////////////////////////////////////////////////////////////////////////////////////////
class ioObserverBlueWnd : public ioMovingWnd
{
public:
	enum
	{
		MAX_TEAM_PLAYER = 8,
		DEFAULT_HEIGHT = 31,
		DEFAULT_WIDTH = 160,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

protected:
	ioPlayStage *m_pPlayStage;

	ioUIRenderImage *m_pGuildBack;

	ioUIRenderImage *m_pBlueBack;
	ioUIRenderImage *m_pRedBack;
	ioUIRenderImage *m_pBlueGauge;
	ioUIRenderImage *m_pRedGauge;

	ioUIRenderImage *m_pBlueDead;
	ioUIRenderImage *m_pAddGauge;

	ioUIRenderImage *m_pBlueNumKey;
	ioUIRenderImage *m_pRedNumKey;

	ioUIRenderImage *m_pTargetMark;

	DWORD m_dwMarkChangeTime;

	bool m_bCheckMark;
	DWORD m_dwCheckTime;

	typedef std::vector< RoundInfo > RoundInfoList;
	RoundInfoList m_RoundInfoList;

	int m_iDefaultX;
	int m_iDefaultY;

	static bool m_bPosToDefualt;

public:
	void InitObserverWnd( ioPlayStage *pStage );
	void SetInfo( RoundInfo &rkInfo );

	void ClearInfo();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

	void OnBlueRender( int iXPos, int iYPos, int iSlot, const RoundInfo &rkInfo );
	void OnOtherRender( int iXPos, int iYPos, int iSlot, const RoundInfo &rkInfo );

protected:
	bool IsDieChar( const ioHashString &szName );
	bool IsTarget( const ioHashString &szName );

	float GetCurHPRate( const ioHashString &szName );

	void ProcessCameraClick();

	void UpdateMainDefaultPos();
	void UpdateDefaultPos();
	void UpdateWndSize();
	void UpdateMark();

public:
	static void SetPosToDefualt(bool PosToDefualt) { m_bPosToDefualt = PosToDefualt; }

public:
	ioObserverBlueWnd();
	virtual ~ioObserverBlueWnd();
};
/////////////////////////////////////////////////////////////////////////////////////////////
class ioObserverInfoWnd : public ioWnd
{
private:
	ioUIRenderImage *m_pBack;
	IoString m_szDesc[4];

	bool m_bSpecialObserver;
	int m_iShowTime;
	int m_iBlinkTime;
	int m_iBlinkCount;
	DWORD m_dwShowStartTime;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	virtual void OnRender();
	void SetShow( bool bSpecial );

public:
	ioObserverInfoWnd();
	virtual ~ioObserverInfoWnd();
};