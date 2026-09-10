#pragma once

#include "../io3DEngine/ioRenderable.h"

class ioScreenBlind : public ioRenderable
{
protected:
	ioMaterial *m_pMaterial;
	DWORD m_dwTFactor;

public:
	void InitResource();
	void SetAlphaRate( int iAlpha );
	void SetColor( int iRed, int iGreen, int iBlue );
	void SetTextureFactor( DWORD dwFactor );

public:
	virtual ioMaterial* GetMaterial() const;
	virtual float GetSquaredViewDepth( const ioCamera *pCamera );
	virtual void GetWorldTransform( D3DXMATRIX *pMatWorld ) const;

	virtual void GetRenderOperation( ioRenderOperation &rkRenderOp );

public:
	ioScreenBlind();
	virtual ~ioScreenBlind();
};

