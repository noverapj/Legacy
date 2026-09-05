#pragma once

class ManualMgr;

class PetEggResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ROLLING	= 1,
		ID_GO		= 2,
		ID_USE		= 3,
	};

	enum
	{
		ROLLING_BTN  = 1,
		RESULT_BTN	 = 2,

		MAX_EGG = 100,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pEffect;
	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pFinalBackEffect;

	ManualMgr* m_pManual;

	ioHashStringVec m_vIconList[MAX_EGG];
	
	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vEndEfffect;
	
	bool m_bResult;
	bool m_bFinalResult;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTime;
	DWORD m_dwRollingTotalTime;

	int m_nPetEggArray;
	int m_iCurIconArray;
	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;

	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	ioHashString m_szRollingSound;
	ioHashString m_szResultSound;

	DWORD m_dwEtcItemType;

	int m_nPetCode;
	int m_nPetRank;
	int m_nPetSlotIndex;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();
	virtual void iwm_show();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	//펫 확인
	void ShowRandomBox( DWORD dwEtcItemType );

protected:
	//버튼 재배치
	void ReBatchButton( int iType );
	//아이콘 롤링
	void RollingIcon( float fTimePerSec );
	//이펙트 롤링
	void RollingEffect( float fTimePerSec );

public:
	//롤링 확인
	bool IsRolling();
	//결과 설정
	void OnResultInfo( int nPetCode, int nPetRank, int nPetSlotIndex );
	
protected:
	int GetPetEggArray();

public:
	PetEggResultWnd(void);
	virtual ~PetEggResultWnd(void);
};

