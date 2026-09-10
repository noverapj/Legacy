#pragma once

#include "../io3DEngine/ioTimer.h"

class ioAnimate2DImageRender
{
public:
	enum FrameType
	{
		FT_DEFAULT,
		FT_ZIGZAG,
	};

private:
	typedef std::vector< ioUIRenderImage* > AnimateImageList;
	AnimateImageList m_vImageList;

	FrameType m_FrameType;
	ioTimer m_FrameTimer;
	DWORD m_dwFrameTime;
	int m_iCurFrame;

	bool m_bRightSideMove;
	bool m_bShow;

private:
	void ClearResource();

public:
	void SetFrameTime( DWORD dwFrameTime );
	void SetAnimateType( FrameType eType );

	void AddImage( const ioHashString &szImageSet, const ioHashString &szImage );
	void AddImage( const ioHashString &szFileName );
	void AddImageByFullName( const ioHashString &szFullName );

	// Left, Center, Right
	void SetHorzAlign( const ioHashString &szAlign );

	// Top, Center, Bottom
	void SetVertAlign( const ioHashString &szAlign );

public:
	void ShowRender();
	void HideRender();

	void Update();
	void Render( int xPos, int yPos );

public:
	inline bool IsShow() const { return m_bShow; }

public:
	ioAnimate2DImageRender();
	~ioAnimate2DImageRender();
};
