#pragma once

#include "../io3DEngine/ioOrientBox.h"
#include "../io3DEngine/ioEntityGroup.h"

class ioEffect;
class ioEntity;

class ioPetView : public ioEntityEvent
{
protected:
	ioEntityGroup *m_pGrp;	
	
public:
	void SetPosition( float fXPos, float fYPos, float fZPos );
	void SetPosition( const D3DXVECTOR3 &vPos );
	
	void SetPitch( float fPitch );
	void SetYaw( float fYaw );
	
	void RotateX( float fPitch );
	void RotateY( float fYaw );
	
	void SetScale( float fScale );
	void SetScale( const D3DXVECTOR3 &vScale );
	void SetScale( float fXScale, float fYScale, float fZScale );

public:
	void SetActionAni( const ioHashString &szAniName, float fFadeIn = FLOAT100, float fFadeOut = FLOAT100 );
	void SetLoopAni( const ioHashString &szAniName, float fFadeIn = 0.0f );

public:
	D3DXVECTOR3 GetWorldPosition() const;	
	float GetYaw();
	
public:
	void Update( float fTimePerSec );

public:
	ioEffect* AttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner );
	void EndEffect( const ioHashString &rkEffect, bool bGrace );

public:
	ioPetView( ioEntityGroup *pGrp );
	virtual ~ioPetView();
};

