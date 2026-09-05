

#ifndef _ioUIRenderElement_h_
#define _ioUIRenderElement_h_

#include "io3DCommon.h"

class __EX ioUIRenderElement
{
public:
	enum UIElementType
	{
		UIE_FRAME,
		UIE_IMAGE
	};

protected:
	bool m_bShow;
	RECT m_RenderRect;	// 부모 윈도우기준의 좌표
	DWORD m_dwColor;

	float m_fXScaleRate;
	float m_fYScaleRate;

public:
	// fOffX, fOffY : 부모윈도우의 전체 화면에서의 위치
	virtual void Render( float fOffX,
						 float fOffY,
						 UIRenderType eRenderType = UI_RENDER_NORMAL,
						 TextureFilterOption eOption = TFO_NEAREST,
						 bool bNoMagicAlign = false,
						 UIRenderType eGrayRenderType = UI_RENDER_NORMAL ) = 0;

	virtual ioUIRenderElement* Clone() const = 0;
	virtual UIElementType GetType() const = 0;

public:
	virtual void SetPosition( int iXPos, int iYPos );
	virtual void SetSize( int iWidth, int iHeight );
	virtual void SetRenderRect( const RECT &rcRect );
	const RECT& GetRenderRect() const { return m_RenderRect; }

	virtual void SetScale( float fRate );
	virtual void SetScale( float fXScale, float fYScale );
	virtual void SetXScale( float fRate );
	virtual void SetYScale( float fRate );

	DWORD GetColor() const { return m_dwColor; }
	virtual void SetColor( DWORD dwColor );
	virtual void SetColor( int iRed, int iGreen, int iBlue );
	virtual void SetAlpha( BYTE iAlpha );
	virtual void SetAlphaWithColor( BYTE iAlpha );

public:
	inline int GetPosX() const { return m_RenderRect.left; }
	inline int GetPosY() const { return m_RenderRect.top; }
	inline int GetWidth() const { return m_RenderRect.right - m_RenderRect.left; }
	inline int GetHeight() const { return m_RenderRect.bottom - m_RenderRect.top; }

	D3DXVECTOR2 GetSize() const;

	virtual D3DXVECTOR2 GetScale() const { return D3DXVECTOR2( m_fXScaleRate, m_fYScaleRate ); }
	virtual BYTE GetAlpha() const { return m_dwColor >> 24; }

	void SetRenderType( UIRenderType eRenderType ) { m_eRenderType = eRenderType; }
	UIRenderType GetRenderType() const { return m_eRenderType; }
	void SetTextureFilterOption( TextureFilterOption eTextureFilterOption ) { m_eTextureFilterOption = eTextureFilterOption; }
	TextureFilterOption GetTextureFilterOption() const { return m_eTextureFilterOption; }
	void SetGrayRenderType( UIRenderType eGrayRenderType ) { m_eGrayRenderType = eGrayRenderType; }
	UIRenderType GetGrayRenderType() const { return m_eGrayRenderType; }
	UIRenderType m_eRenderType;
	TextureFilterOption m_eTextureFilterOption;
	UIRenderType m_eGrayRenderType;
		
	void SetName( const char* name ) { if ( name ) m_name = name; }
	const std::string& GetName() const { return m_name; }
	std::string m_name;

	bool		IsShow(){return m_bShow;};
	void		SetShow(bool bShow){m_bShow = bShow;};

public:
	ioUIRenderElement();
	ioUIRenderElement( const ioUIRenderElement &rhs );
	virtual ~ioUIRenderElement();
};

#endif