#pragma once

#include "../io3DEngine/ioButton.h"

class ioUIRenderImage;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class NoviceGradeBuffBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconEffect;
	ioUIRenderImage *m_pIconAddEffect;
	float m_fScale;
		
	DWORD m_dwCheckTime;
	DWORD m_dwChangeTime;
	int m_nCurAlpha;
	int m_nAddAlpha;
	bool m_bIsAddAlpha;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void iwm_show();
	
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	
protected:
	void UpdateEffect();

public:
	NoviceGradeBuffBtn();
	virtual ~NoviceGradeBuffBtn();
};
