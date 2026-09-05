#pragma once

class SpiritFuncWnd : public ioWnd
{
public:
	enum
	{
		ID_COMPOSE_TAB_BTN		= 1,
		ID_CONVERSION_TAB_BTN	= 2,

		ID_COMPOSE_MATERIAL_ICON = 11,
		ID_COMPOSE_SPECIAL_ICON = 12,
		ID_COMPOSE_RESULT_ICON = 13,
		ID_COMPOSE = 14,
		ID_COMPOSE_MINUS1 = 15,
		ID_COMPOSE_PLUS1 = 16,
		ID_COMPOSE_MINUS2 = 17,
		ID_COMPOSE_PLUS2 = 18,
		ID_COMPOSE_ROLLING = 19,

		ID_CONVERSION_SOURCE_ICON = 21,
		ID_CONVERSION_RESULT_ICON = 22,
		ID_CONVERSION_SELECT = 23,
		ID_CONVERSION = 24,
		ID_CONVERSION_MINUS = 25,
		ID_CONVERSION_PLUS = 26,
		ID_CONVERSION_ROLLING = 27,

		CONVERSION_MIN = 3,
	};
	enum
	{
		eCOMPOSE,
		eCONVERSION,
	};
	enum
	{
		eReady = 0,
		eNot,
		eSet,
		eSendPacket,
	};
	enum
	{
		BG_EFFECT_1_TIME = 200,
		BG_EFFECT_1_TIME_GAP = 50,
		BG_EFFECT_2_TIME = 600,
		BG_EFFECT_2_TIME_GAP = 100,
	};

private:
	int m_iTabState;
	int m_iFuncState;
	ioHashString m_szComposeStateMsg[4];
	ioHashString m_szComposeText;
	ioHashString m_szComposeAlarm[2];
	ioHashString m_szSendComposeError[2];
	ioHashString m_szSpiritCountText[2];
	ioHashString m_szNoSpiritAlarm;
	ioHashString m_szConversionStateMsg[4];
	ioHashString m_szConversionText;
	ioHashString m_szConversionAlarm;
	ioHashString m_szSendConversionError[2];

	ioHashString m_szComposeRollingSound;
	ioHashString m_szComposeResultSound;
	ioHashString m_szConversionRollingSound;
	ioHashString m_szConversionResultSound;

	ioUIRenderImage *m_pComposeBG;
	ioUIRenderImage *m_pConversionBG;
	ioUIRenderImage *m_pStripDark;
	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pBGEffect[2];

	ioUI3DEffectRender*	m_pEffect;

	int m_iSpecialSpiritCode;
	int m_iSpecialSpiritCount;

	int m_iSpiritCode;
	int m_iSpiritCount;
	int m_iCurSpiritCount;
	int m_iSpiritMinCount;
	int m_iSpiritMaxCount;
	int m_iConversionCode;

	int m_iProcessCount;
	DWORD m_dwProcessTime;
	DWORD m_dwProcessGapTime;
	DWORD m_dwProcessFirstGapTime;
	bool m_bFirstGap;

	DWORD m_dwRollingTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwTabChangeTime;

	int m_iComposeResultClassType;
	int m_iConversionResultSpiritCode;
	int m_iConversionResultCount;
	bool m_bConversionResultCritical;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	void SelectClickAction( int iCode, int iCount, bool bSoldier, bool bCompose );

	void OnSpiritComposeResult( int iClassType );
	void OnSpiritConversionResult( int iSpiritCode, int iCount, bool bCritical );

protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();
	virtual void OnProcess( float fTimePerSec );

	void SelectTab( DWORD dwID );
	void SendCompose();
	void SendConversion();

	void SetComposeResult();
	void SetConversionResult();

public:
	SpiritFuncWnd(void);
	virtual ~SpiritFuncWnd(void);
};

class SpiritComposeResultWnd : public ioWnd
{
public:
	enum
	{
		ID_3D_INFO = 100,
	};

private:
	CHARACTER        m_CharInfo;
	ioUI3DEffectRender*	m_pEffect;

	ioHashString m_szMortmain;
	ioHashString m_szDesc;
	ioHashString m_szName;
	DWORD m_dwShowTime;
	DWORD m_dwShowEndTime;

public:
	virtual void iwm_show();
	virtual void iwm_hide();

	void SetSoldierInfo( int iClassType );
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	SpiritComposeResultWnd(void);
	virtual ~SpiritComposeResultWnd(void);
};

class SpiritConversionResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ICON = 1,
	};

private:
	ioUI3DEffectRender*	m_pEffect;
	ioUIRenderImage *m_pNumText;

	DWORD m_dwShowTime;
	DWORD m_dwEffectStartTime;
	DWORD m_dwEffectTime;
	DWORD m_dwStartTime;

	ioHashString m_szName;
	bool m_bCritical;
	bool m_bEffect;
	int m_iCount;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

	virtual void OnProcess( float fTimePerSec );
	virtual void OnRenderAfterChild();

	void SetSpiritInfo( int iSpiritCode, int iCount, bool bCritical );

public:
	SpiritConversionResultWnd(void);
	virtual ~SpiritConversionResultWnd(void);
};