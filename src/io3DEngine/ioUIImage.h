

#ifndef _ioUIImage_h_
#define _ioUIImage_h_

#include "ioUIRenderElement.h"

class ioUIImageSet;

class __EX ioUIImage
{
private:
	ioUIImageSet *m_pCreator;
	ioHashString m_Name;

	mutable int m_iRefCount;

	RECT	m_rcArea;		// Texture안에서의 위치
	int		m_iOffsetX;
	int		m_iOffsetY;

	mutable D3DXVECTOR4 m_TexUV;	// Texture안에서의 UV
	mutable bool m_bTexUVInited;

public:
	void SetAreaRect( const RECT &rcRect );
	void SetOffsetX( int iOffX ) { m_iOffsetX = iOffX; }
	void SetOffsetY( int iOffY ) { m_iOffsetY = iOffY; }
	void SetOffsetXY( int iX, int iY ) { m_iOffsetX = iX, m_iOffsetY = iY; }

public:
	inline const ioHashString& GetName() const { return m_Name; }
	inline int GetRefCount() const { return m_iRefCount; }

	inline RECT  GetAreaRect() const { return m_rcArea; }

	inline int GetX() const { return m_rcArea.left; }
	inline int GetY() const { return m_rcArea.top; }
	inline float GetWidth() const { return (float)( m_rcArea.right - m_rcArea.left ); }
	inline float GetHeight() const { return (float)( m_rcArea.bottom - m_rcArea.top ); }

	inline int GetOffsetX() const { return m_iOffsetX; }
	inline int GetOffsetY() const { return m_iOffsetY; }

	const D3DXVECTOR4& GetTexUV() const;

public:
	bool FillFileImage( const char *szFileName,
						const int iFileWidth,
						const int iFileHeight,
						OUT D3DXIMAGE_INFO *pSrcInfo = NULL ) const;

	IDirect3DTexture9* GetD3DTexture() const;
	IDirect3DTexture9* GetGrayTexture() const;
	IDirect3DTexture9* GetGrayLightTexture() const;
	
public:
	void NotifyAttached() const;
	void NotifyDetached() const;

	void ForceTouch() const;

public:
	ioUIImage( ioUIImageSet *pImageSet, const ioHashString &rkName, const RECT &rcArea );
	~ioUIImage();
};

#endif
