#pragma once

class ioUIRenderImage;
class ioPetView;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PetViewWnd : public ioWnd
{
public:
	enum 
	{
		ID_CHAR_CONTROL_WND     = 1,
	};

protected:
	ioUI3DRender*	m_pUI3DRender;
	ioPetView*		m_pPetView;
	ioPetView*		m_pEffectDummy;

	ioUIRenderImage *m_p3DBack;
	ioUIRenderImage *m_pShadow;

	float         m_fRotateYaw;
	float         m_fPreYaw;
	float         m_fXMLRotateYaw;
	float         m_fXMLShadowScale;

	D3DXVECTOR3 m_vCharPos;
	D3DXVECTOR3 m_vDummyPos;
	D3DXVECTOR3 m_vCameraPos;

	bool          m_bLeftRotateOn;
	bool          m_bRightRotateOn;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void RotateY( float fYaw );

public:
	void InitRotate();
	void CreatePet( int nPetCode, int nPetRank );
	void DestroyPet();

public:
	void SetPetScale( int nPetCode, int nPetRank );
	void SetLoopAni( ioHashString szPetViewAni );

public:
	void CreateEffectDummy();
	void DummyAttachEffect( ioHashString szPetEffect );
	void DummyEndEffect( ioHashString szPetEffect );


public:
	PetViewWnd();
	virtual ~PetViewWnd();
};
