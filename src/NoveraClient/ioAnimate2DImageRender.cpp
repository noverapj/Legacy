

#include "stdafx.h"

#include "ioAnimate2DImageRender.h"

ioAnimate2DImageRender::ioAnimate2DImageRender()
{
	m_FrameType = FT_DEFAULT;
	m_dwFrameTime = 100;
	m_iCurFrame = 0;

	m_bRightSideMove = true;
	m_bShow = false;
}

ioAnimate2DImageRender::~ioAnimate2DImageRender()
{
	ClearResource();
}

void ioAnimate2DImageRender::ClearResource()
{
	AnimateImageList::iterator iter;
	for( iter=m_vImageList.begin() ; iter!=m_vImageList.end() ; ++iter )
	{
		delete *iter;
	}
	m_vImageList.clear();
}

void ioAnimate2DImageRender::SetFrameTime( DWORD dwFrameTime )
{
	m_dwFrameTime = dwFrameTime;
}

void ioAnimate2DImageRender::SetAnimateType( FrameType eType )
{
	m_FrameType = eType;
}

void ioAnimate2DImageRender::AddImage( const ioHashString &szImageSet,
									   const ioHashString &szImage )
{
	ioUIRenderImage *pNewImg = g_UIImageSetMgr.CreateImage( szImageSet, szImage );
	if( pNewImg )
	{
		m_vImageList.push_back( pNewImg );
	}
}

void ioAnimate2DImageRender::AddImage( const ioHashString &szFileName )
{
	ioUIRenderImage *pNewImg = g_UIImageSetMgr.CreateImageByFile( szFileName );
	if( pNewImg )
	{
		m_vImageList.push_back( pNewImg );
	}
}

void ioAnimate2DImageRender::AddImageByFullName( const ioHashString &szFullName )
{
	ioUIRenderImage *pNewImg = g_UIImageSetMgr.CreateImageByFullName( szFullName );
	if( pNewImg )
	{
		m_vImageList.push_back( pNewImg );
	}
}

// Left, Center, Right
void ioAnimate2DImageRender::SetHorzAlign( const ioHashString &szAlign )
{
	AnimateImageList::iterator iter;
	for( iter=m_vImageList.begin() ; iter!=m_vImageList.end() ; ++iter )
	{
		(*iter)->SetHorzFormatting( szAlign );
	}
}

// Top, Center, Bottom
void ioAnimate2DImageRender::SetVertAlign( const ioHashString &szAlign )
{
	AnimateImageList::iterator iter;
	for( iter=m_vImageList.begin() ; iter!=m_vImageList.end() ; ++iter )
	{
		(*iter)->SetVertFormatting( szAlign );
	}
}

void ioAnimate2DImageRender::ShowRender()
{
	if( !m_bShow )
	{
		m_FrameTimer.InitTimer();
		m_iCurFrame = 0;
		m_bRightSideMove = true;
		m_bShow = true;
	}
}

void ioAnimate2DImageRender::HideRender()
{
	m_bShow = false;
}

void ioAnimate2DImageRender::Update()
{
	if( m_vImageList.empty() )	return;
	if( !m_FrameTimer.IsTimePast( m_dwFrameTime ) )
		return;	

	int iMaxFrame = m_vImageList.size();

	switch( m_FrameType )
	{
	case FT_DEFAULT:
		m_iCurFrame = ( m_iCurFrame + 1 ) % iMaxFrame;
		break;
	case FT_ZIGZAG:
		if( m_bRightSideMove )
		{
			m_iCurFrame++;

			if( m_iCurFrame >= iMaxFrame )
			{
				m_iCurFrame = iMaxFrame - 2;
				m_bRightSideMove = false;
			}
		}
		else
		{
			m_iCurFrame--;

			if( m_iCurFrame < 0 )
			{
				m_iCurFrame = 1;
				m_bRightSideMove = true;
			}
		}
		break;
	}
}

void ioAnimate2DImageRender::Render( int xPos, int yPos )
{
	if( !IsShow() )	return;

	int iFrameCount = m_vImageList.size();
	if( COMPARE( m_iCurFrame, 0, iFrameCount ) )
	{
		if( m_vImageList[m_iCurFrame] )
		{
			m_vImageList[m_iCurFrame]->Render( xPos, yPos );
		}
	}
}



