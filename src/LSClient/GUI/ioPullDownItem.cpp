#pragma once
#include "StdAfx.h"
#include "ioPullDownEventListener.h"
#include "ioPullDownItemFactory.h"
#include "ioPullDownList.h"
#include "ioPullDownItem.h"

#include <iterator>

ioPullDownItem::ioPullDownItem()
{
	m_iIndex		= 0;
	m_iHeight		= 0;
	m_iBlankWidth	= 0;
	m_iBlankHeight	= 0;

	m_pOverLine		= NULL;
	m_pDarkLine		= NULL;
	m_pLightLine	= NULL;

	m_bImageRender	= true;

	memset( &m_SelectRect, 0x00, sizeof( m_SelectRect ) );
}

ioPullDownItem::~ioPullDownItem()
{
	for( vPullDownInnerImage::iterator iter = m_vImageList.begin(); iter != m_vImageList.end(); ++iter )
	{
		PullDownInnerImage& rkImage = *iter;
		if( !rkImage.m_pImage )
			continue;

		if( rkImage.m_bManagedImageByItem )
		{
			SAFEDELETE( rkImage.m_pImage );			
		}
	}	
	m_vImageList.clear();
}

void ioPullDownItem::SetBlank( int iBlankWidth, int iBlankHeight )
{
	m_iBlankWidth	= iBlankWidth;
	m_iBlankHeight	= iBlankHeight;
}

void ioPullDownItem::ReSize()
{
	if( m_pOverLine )
	{
		m_pOverLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pOverLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pOverLine->SetSize( m_SelectRect.right - m_SelectRect.left, m_SelectRect.bottom - m_SelectRect.top );
	}

	if( m_pDarkLine )
	{
		m_pDarkLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pDarkLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pDarkLine->SetSize( m_SelectRect.right - m_SelectRect.left, m_SelectRect.bottom - m_SelectRect.top );
	}

	if( m_pLightLine )
	{
		m_pLightLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pLightLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pLightLine->SetSize( m_SelectRect.right - m_SelectRect.left, m_SelectRect.bottom - m_SelectRect.top );
	}	
}

void ioPullDownItem::AddImage( vPullDownInnerImage& rkList )
{
	m_vImageList = rkList;
}

void ioPullDownItem::AddImage( ioUIRenderImage* pImage, DWORD dwOverColor )
{
	if( !pImage )	
		return;

	PullDownInnerImage ImageSet;
	ImageSet.m_pImage				= pImage;
	ImageSet.m_dwOverColor			= dwOverColor;
	ImageSet.m_bManagedImageByItem	= false;
	m_vImageList.push_back( ImageSet );	
}

void ioPullDownItem::AddImage( const std::string& szImage, DWORD dwOverColor )
{

	PullDownInnerImage ImageSet;
	ImageSet.m_pImage				= g_UIImageSetMgr.CreateImageByFullName( szImage.c_str() );
	ImageSet.m_dwOverColor			= dwOverColor;
	ImageSet.m_bManagedImageByItem	= true;
	m_vImageList.push_back( ImageSet );
}

RECT ioPullDownItem::MakeImageRect( PullDownInnerImage& rkImage )
{	
	RECT rect;
	if( rkImage.m_pImage )
	{
		rect.left   = m_SelectRect.left + rkImage.m_iXPos;
		rect.top    = m_SelectRect.top  + rkImage.m_iYPos;
		rect.right  = rect.left + rkImage.m_pImage->GetWidth();
		rect.bottom = rect.top  + rkImage.m_pImage->GetHeight();
	}

	return rect;
}

int ioPullDownItem::OnSelectOver( const POINT& mousepos )
{
	if( ::PtInRect( &m_SelectRect, mousepos ) )
		return m_iIndex;

	return PullDownEvent::NONE_SELECT;
}

int ioPullDownItem::OnImageOver( const POINT& mousepos )
{
	vPullDownInnerImage::iterator iter = m_vImageList.begin();
	for( int iIdx = 0; iter != m_vImageList.end(); ++iter, ++iIdx )
	{
		PullDownInnerImage& rkImage = *iter;
		if( !rkImage.m_pImage )
			continue;

		RECT rect = MakeImageRect( rkImage );
		if( ::PtInRect( &rect, mousepos ) )
		{
			return iIdx;
		}
	}

	return PullDownEvent::NONE_SELECT;
}

void ioPullDownItem::OnRender( int iOverIdx, int iOverImageIdx )
{
	if( m_iIndex == iOverIdx && PullDownEvent::NONE_SELECT == iOverImageIdx )
	{
		if( m_pOverLine )
			m_pOverLine->Render( m_SelectRect.left, m_SelectRect.top );
	}
	else
	{
		if( m_iIndex % 2 == 0 )
		{
			if( m_pDarkLine )
				m_pDarkLine->Render( m_SelectRect.left, m_SelectRect.top );
		}
		else
		{
			if( m_pLightLine )
				m_pLightLine->Render( m_SelectRect.left, m_SelectRect.top );
		}
	}
}

void ioPullDownItem::OnImageListRender( int iOverIdx, int iOverImageIdx )
{	
	if( IsEnableIamgeRender() )
	{
		vPullDownInnerImage::iterator iter = m_vImageList.begin();
		for( int Idx = 0; iter != m_vImageList.end(); ++iter, ++Idx )
		{
			PullDownInnerImage& rkImage = *iter;
			if( !rkImage.m_pImage )
				continue;

			if( m_iIndex == iOverIdx && iOverImageIdx == Idx )
				rkImage.m_pImage->SetColor( rkImage.m_dwOverColor );
			else
				rkImage.m_pImage->SetColor( rkImage.m_dwColor );

			rkImage.m_pImage->Render( m_SelectRect.left + rkImage.m_iXPos, m_SelectRect.top + rkImage.m_iYPos, rkImage.m_eRenderType );
		}
	}
}

//-----------------------------------------------------------------------------------------------------------------------------

void SinglePullDownItem::OnRender( int iOverIdx, int iOverImageIdx )
{
	ioPullDownItem::OnRender( iOverIdx, iOverImageIdx );

	g_FontMgr.SetAlignType( TAT_LEFT );	
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_iIndex == iOverIdx && PullDownEvent::NONE_SELECT == iOverImageIdx )
		g_FontMgr.SetTextColor( m_dwTitleOverColor );
	else
		g_FontMgr.SetTextColor( m_dwTitleColor );

	int iHeight = m_SelectRect.bottom - m_SelectRect.top;
	int iYPos = m_SelectRect.top + ( iHeight / 2 ) - static_cast<int>( g_PullDownFactory.GetFontTextPixel( FONT_SIZE_12 ) * FLOAT05 );
	g_FontMgr.PrintTextWidthCut( m_SelectRect.left + 3, iYPos, FONT_SIZE_12, m_SelectRect.right - m_SelectRect.left, m_szTitle.c_str() );
}

//-----------------------------------------------------------------------------------------------------------------------------

void DoublePullDownItem::OnRender( int iOverIdx, int iOverImageIdx )
{
	ioPullDownItem::OnRender( iOverIdx, iOverImageIdx );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_iIndex == iOverIdx && PullDownEvent::NONE_SELECT == iOverImageIdx )	
		g_FontMgr.SetTextColor( m_dwTitleOverColor );	
	else	
		g_FontMgr.SetTextColor( m_dwTitleColor );
	
	int iFontPixel	= static_cast<int>( g_PullDownFactory.GetFontTextPixel( FONT_SIZE_12 ) );
	int iHeight		= m_SelectRect.bottom - m_SelectRect.top;
	int iYPos		= m_SelectRect.top + ( iHeight / 2 );

	g_FontMgr.PrintTextWidthCut( m_SelectRect.left + 3, iYPos - iFontPixel - 2, FONT_SIZE_12, m_SelectRect.right - m_SelectRect.left, m_szTitle.c_str() );

	if( m_iIndex == iOverIdx && PullDownEvent::NONE_SELECT == iOverImageIdx )	
		g_FontMgr.SetTextColor( m_dwTitleSubOverColor );
	else	
		g_FontMgr.SetTextColor( m_dwTitleSubColor );
	
	g_FontMgr.PrintTextWidthCut( m_SelectRect.left + 3, iYPos, FONT_SIZE_12, m_SelectRect.right - m_SelectRect.left, m_szSubTitle.c_str() );
}