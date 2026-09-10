#pragma once

#include "../io3DEngine/ioButton.h"

class ioUIRenderImage;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PetIconBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pEmpty;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pEquipText;
	ioUIRenderImage *m_pRankImg;
	ioUIRenderFrame *m_pBaseFrm;
	ioUIRenderFrame *m_pEmptyFrm;
	ioUIRenderFrame *m_pSelectFrm;
	ioUIRenderFrame *m_pEquipFrm;

	float m_fScale;
	float m_fBackScale;

	int m_nPetCode;
	int m_nPetLevel;
	int m_nPetRank;
	int m_nSlotIndex;
	int m_nCurExp;

	int m_nMaxLevel;
	float m_fExpRate;
	
	ioHashString m_szPetRank;

	bool m_bDesc;
	bool m_bSelect;
	bool m_bEquip;
	bool m_bPreOver;
	
	DWORD m_dwCheckTime;
	DWORD m_dwChangeTime;

	bool m_bAddEquipText;
	
public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnDrawNormal( int nXPos, int nYPos );
	virtual void OnDrawOvered( int nXPos, int nYPos );
	virtual void OnDrawPushed( int nXPos, int nYPos );

protected:
	virtual void OnProcess( float fTimePerSec );

protected:
	void RenderBase( int nXPos, int nYPos );
	void RenderEquip( int nXPos, int nYPos );
	void RenderSelect( int nXPos, int nYPos );
	void RenderBack( int nXPos, int nYPos );
	void RenderIcon( int nXPos, int nYPos );
	void RenderDesc( int nXPos, int nYPos );
	void RenderEmpty( int nXPos, int nYPos );
	void RenderRankColor( int nXPos, int nYPos );
	void RenderEquipText( int nXPos, int nYPos );

protected:
	void ChangeEquipText();

public:
	//아이콘 설정
	bool SetIcon( int nPetCode, int nPetLevel, int nPetRank, float fScale, float fBackScale, bool bDesc );
	//아이콘 초기화
	void RemoveIcon();

public:
	//펫 정보 얻기
	int GetPetCode();
	int GetPetRank();
	int GetSlotIndex();

public:
	//펫 아이콘 개별 설정
	void SetSlotIndex( int nSlotIndex );
	void SetCurExp( int nCurExp );
	void SetSelect( bool bSelect );
	void SetEquip( bool bEquip );

public:
	PetIconBtn();
	virtual ~PetIconBtn();
};
