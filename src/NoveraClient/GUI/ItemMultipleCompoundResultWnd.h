#pragma once

#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioButton.h"

#include "../ioComplexStringPrinter.h"
#include "ioMessageBox.h"

class ioUIRenderImage;
class ioXMLElement;
class ManualMgr;

////////////////////////////////////////////////////////////////////////////////
class ItemMultipleCompoundResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ROLLING	= 1,
		ID_USE		= 2,
		ID_GO		= 3,
	};

	enum
	{
		ROLLING_BTN = 1,
		RESULT_BTN	= 2,
	};

	// Icon
protected:
	ioHashStringVec m_vWeaponIcon;
	ioHashStringVec m_vArmorIcon;
	ioHashStringVec m_vHelmetIcon;
	ioHashStringVec m_vCloakIcon;
	ioHashStringVec m_vUniqueIcon;

	ioHashStringVec m_vIconList;

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pEffect;

	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pFinalBackEffect;

	ManualMgr* m_pManual;

	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;

	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vEndEfffect;

	int m_iEtcType;
	int m_iItemCode;
	int m_iReinforce;
	ioHashString m_ItemName;

	bool m_bFinalResult;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTime;
	DWORD m_dwRollingTotalTime;

	int m_iCurIconArray;
	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;


	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	ioHashString m_szRollingSound;
	ioHashString m_szResultSound;

protected:
	int m_nGradeType;
	float m_fGradeScale;

	IntVec m_vWeaponGrade;
	IntVec m_vArmorGrade;
	IntVec m_vHelmetGrade;
	IntVec m_vCloakGrade;
	IntVec m_vUniqueGrade;
	IntVec m_vGradeList;

public:
	void SetInfoAndShow( int iEtcType, int iItemCode, int iReinforce );

protected:
	void ReBatchButton( int iType );

	void LoadIconList();
	void GetCurIconList();

	void RollingIcon( float fTimePerSec );
	void RollingEffect( float fTimePerSec );

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	ItemMultipleCompoundResultWnd();
	virtual ~ItemMultipleCompoundResultWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////

class UniqueItemMultipleCompoundResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ROLLING	= 1,
		ID_USE		= 2,
		ID_GO		= 3,
	};

	enum
	{
		ROLLING_BTN = 1,
		RESULT_BTN	= 2,
	};

	// Icon
protected:
	ioHashStringVec m_vWeaponIcon;
	ioHashStringVec m_vArmorIcon;
	ioHashStringVec m_vHelmetIcon;
	ioHashStringVec m_vCloakIcon;
	ioHashStringVec m_vUniqueIcon;

	ioHashStringVec m_vIconList;

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pEffect;

	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pFinalBackEffect;

	ManualMgr* m_pManual;

	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;

	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vEndEfffect;

	int m_iEtcType;
	int m_iItemCode;
	int m_iReinforce;
	ioHashString m_ItemName;

	bool m_bFinalResult;

	float m_fIconScale;
	DWORD m_dwIconScaleTime;
	DWORD m_dwRollingTime;
	DWORD m_dwRollingTotalTime;

	int m_iCurIconArray;
	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwScaleStartTime;


	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	ioHashString m_szRollingSound;
	ioHashString m_szResultSound;

protected:
	int m_nGradeType;
	float m_fGradeScale;

	IntVec m_vWeaponGrade;
	IntVec m_vArmorGrade;
	IntVec m_vHelmetGrade;
	IntVec m_vCloakGrade;
	IntVec m_vUniqueGrade;
	IntVec m_vGradeList;

	ioUI3DEffectRender*	m_pLoopEffect;
	ioUI3DEffectRender*	m_pResultEffect;

public:
	void SetInfoAndShow( int iEtcType, int iItemCode, int iReinforce );

protected:
	void ReBatchButton( int iType );

	void LoadIconList();
	void GetCurIconList();

	void RollingIcon( float fTimePerSec );
	void RollingEffect( float fTimePerSec );

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );
	virtual void iwm_show();
public:
	UniqueItemMultipleCompoundResultWnd();
	virtual ~UniqueItemMultipleCompoundResultWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////