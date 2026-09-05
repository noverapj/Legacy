#pragma once

class ioUIRenderImage;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PetResellWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT			= 1,
		ID_SELL			= 2,
		ID_CANCEL		= 3,
		ID_SELL_EDIT	= 4,

		ID_PET_ICON		= 11,
	};
		
protected:
	ioUIRenderImage* m_pSellMark;
	ioUIRenderFrame *m_pBottomFrm;

	int m_nPetCode;
	int m_nPetLevel;
	int m_nPetRank;
	int m_nPetSlotIndex;

	ioHashString m_szPetName;

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pSellEdit;
	
public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );


protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();

protected:
	void RenderDesc( int nXPos, int nYPos );
	void RenderManual( int nXPos, int nYPos );

public:
	void SetIcon( int nPetCode, int nPetLevel, int nPetRank, int nPetSlotIndex, float fScale, float fBackScale );

public:
	PetResellWnd();
	virtual ~PetResellWnd();
};