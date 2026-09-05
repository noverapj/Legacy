#pragma once
#include "stdafx.h"

class ioBaseChar;
class ioUIRenderImage;

class ioShuffleCharStateUI
{
protected:
	bool			   m_bShuffleUIRender;
	D3DXVECTOR3		   m_vShuffleUIHeadPos;

	ioUIRenderImage* m_pStarBg;
	ioUIRenderImage* m_pStarNum;
	
protected:
	int m_iXPos;
	int m_iYPos;
		
	int m_iStarXOffset;	

protected:
	int m_iStarCount;

public:
	void Load();

public:
	void UpdateShuffleModeUI( bool bSystemVisible, ioBaseChar* pOwner, ioBaseChar *pCameraTarget );
	void CheckOutScreen( ioUIRenderImage* pImage, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vCamTargetPos, int &iXPos, int &iYPos, float fScale, bool bVisible );

	void RenderShuffleModeUI( ioBaseChar* pOwner );
	
public:
	inline void SetStarCount( int iStarCount ){ m_iStarCount = iStarCount; }

public:
	ioShuffleCharStateUI();
	~ioShuffleCharStateUI();
};