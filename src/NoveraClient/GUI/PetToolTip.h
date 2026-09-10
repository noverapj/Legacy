#pragma once

class ioUIRenderImage;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PetToolTip : public ioWnd
{
protected:
	ioUIRenderImage *m_pPetIcon;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pBottomImg;

	int m_nGrowthStat[MAX_CHAR_GROWTH+MAX_ITEM_GROWTH];
	int m_nSelectPetCode;
	int m_nSelectPetLevel;
	int m_nSelectPetRank;
	int m_nManualIndex;

	int m_nMaxLevel;
	float m_fExpRate;

	ioHashString m_szPetName;
	ioHashString m_szPetIcon;
	
	ioComplexStringPrinter m_szDesc;

public:
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void RenderDesc( int nXPos, int nYPos );
	void RenderGrowth( int nXPos, int nYPos );
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

protected:
	void UpdateWndSize();

protected:
	void GetPetInfo();	

public:
	bool SetInfo();
	void SetData( int nPetCode, int nPetLevel, int nPetRank, int nCurExp );
	void Clear();

public:
	PetToolTip();
	virtual ~PetToolTip();
};