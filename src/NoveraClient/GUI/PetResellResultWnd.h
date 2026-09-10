#pragma once

class PetResellResultWnd : public ioWnd
{
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pBackEffect;
	
	int m_nPetCode;
	int m_nPetRank;
	int m_nPetLevel;

	ioComplexStringPrinter m_Title[2];
	ioComplexStringPrinter m_Desc[2];

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();
	virtual void iwm_show();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();
	
protected:
	void RenderIcon( int nXPos, int nYPos );
	void RenderDesc( int nXPos, int nYPos );

public:
	//∆Í »Æ¿Œ
	void SetResellResult( int nPetCode, int nPetRank, int nPetLevel );
	
public:
	PetResellResultWnd(void);
	virtual ~PetResellResultWnd(void);
};

