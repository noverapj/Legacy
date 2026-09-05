#pragma once
#include "StdAfx.h"
#include "ioPullDownEventListener.h"
#include "ioPullDownItemFactory.h"
#include "ioPullDownList.h"
#include "ioPullDownItem.h"

template<> ioPullDownItemFactory* Singleton< ioPullDownItemFactory >::ms_Singleton = 0;

ioPullDownItemFactory& ioPullDownItemFactory::GetSingleton()
{
	return Singleton< ioPullDownItemFactory >::GetSingleton();
}

ioPullDownItemFactory::ioPullDownItemFactory()
{
}

ioPullDownItemFactory::~ioPullDownItemFactory()
{
}

void ioPullDownItemFactory::Layout( ioPullDownList* pList, PullDownStyle eStyle )
{
	if( !pList )
	{
		LOG.PrintTimeAndLog( 0, "%s - pList == NULL!!", __FUNCTION__ );
		return;
	}
		
	int iParentX	= 0;
	int iParentY	= 0;

	if( pList->GetParent() )
	{
		iParentX = pList->GetParent()->GetDerivedPosX();
		iParentY = pList->GetParent()->GetDerivedPosY();
	}

	int iCurrXPos = iParentX + pList->GetDefaultPosX();
	int iCurrYPos = iParentY + pList->GetDefaultPosY();
	
	switch( eStyle )
	{
	case PDS_DOWN:
		{			
			//풀다운아이템 레이아웃
			int i = 0;
			for( int iPos = pList->m_iCurPos; iPos < pList->m_iCurPos + pList->m_iMaxPageList; ++iPos, ++i )
			{
				ioPullDownItem* pPullDown = pList->GetPullDown( iPos );
				if( !pPullDown )
					continue;

				if( i == 0 )
					iCurrYPos = PullDownItemLayout( pList, pPullDown, eStyle, iCurrXPos, iCurrYPos, pList->m_iBlankWidth, pList->m_iBlankHeight );
				else
					iCurrYPos = PullDownItemLayout( pList, pPullDown, eStyle, iCurrXPos, iCurrYPos, pList->m_iBlankWidth, 0 );
			}

			//풀다운 위치/크기 지정
			pList->SetSize( pList->GetWidth(), iCurrYPos + pList->m_iBlankHeight - pList->GetDerivedPosY() );
		}
		break;
	case PDS_UP:
		{			
			//풀다운아이템 레이아웃
			int i		= 0;
			int iMax	= min( pList->m_iMaxPageList-1, pList->GetPullDownCount()-1 );

			for( int iPos = pList->m_iCurPos + iMax; iPos >= pList->m_iCurPos; --iPos, ++i )
			{
				ioPullDownItem* pPullDown = pList->GetPullDown( iPos );
				if( !pPullDown )
					continue;

				if( i == 0 )
					iCurrYPos = PullDownItemLayout( pList, pPullDown, eStyle, iCurrXPos, iCurrYPos, pList->m_iBlankWidth, pList->m_iBlankHeight );
				else
					iCurrYPos = PullDownItemLayout( pList, pPullDown, eStyle, iCurrXPos, iCurrYPos, pList->m_iBlankWidth, 0 );
			}

			//풀다운 위치/크기 지정
			int iHeight = iParentY + pList->GetDefaultPosY() - iCurrYPos;
			pList->SetWndPos( pList->GetDefaultPosX(), pList->GetDefaultPosY() - pList->m_iBlankHeight - iHeight );
			pList->SetSize( pList->GetWidth(), iHeight + pList->m_iBlankHeight );
		}
		break;
	default:
		{
			LOG.PrintTimeAndLog(0, "%s - Unkown style : %d", __FUNCTION__, static_cast<int>( eStyle ) );
		}
		break;
	}
}

int ioPullDownItemFactory::PullDownItemLayout( ioPullDownList* pList, ioPullDownItem* pPullDown, PullDownStyle eStyle, int iCurrXPos, int iCurrYPos, int BlankWidth, int iBlankHeight )
{
	//좌/우/상/하단의 빈 공간 영역 지정	
	pPullDown->SetBlank( BlankWidth, iBlankHeight );

	//풀다운 오버/라인 이미지 지정
	pPullDown->m_pOverLine		= pList->m_pOverLine;
	pPullDown->m_pDarkLine		= pList->m_pDarkLine;
	pPullDown->m_pLightLine		= pList->m_pLightLine;

	//풀다운 이미지 등록
	pPullDown->AddImage( pList->m_vImageList );

	//풀다운 크기 지정
	int iHeight = 0;
	if( 0 < pPullDown->m_iHeight )
		iHeight = pPullDown->m_iHeight;	//코드상이나 부모 윈도우에서 풀다운 아이템의 높이를 결정한 경우
	else
		iHeight = pList->m_iListHeight;	//XML을 통해서 풀다운 아이템의 높이를 결정한 경우

	//풀다운 아이템 레이아웃 설정
	return SetSelectLayout( pPullDown, eStyle, iCurrXPos, iCurrYPos, pList->GetWidth() - pList->GetScrollBarCutWidth(), iHeight );
}

int ioPullDownItemFactory::SetSelectLayout( ioPullDownItem* pPullDown, PullDownStyle eStyle, int iXPos, int iYPos, int iWidth, int iHeight )
{
	if( !pPullDown )
	{
		LOG.PrintTimeAndLog( 0, "%s - pPullDown == NULL!!", __FUNCTION__ );
		return 0;
	}

	switch( eStyle )
	{
	case PDS_DOWN:
		{
			pPullDown->m_SelectRect.left	= iXPos + pPullDown->m_iBlankWidth;
			pPullDown->m_SelectRect.top		= iYPos + pPullDown->m_iBlankHeight;
			pPullDown->m_SelectRect.right	= iXPos + iWidth - pPullDown->m_iBlankWidth;
			pPullDown->m_SelectRect.bottom	= pPullDown->m_SelectRect.top + iHeight;

			return pPullDown->m_SelectRect.bottom;
		}
		break;
	case PDS_UP:
		{
			pPullDown->m_SelectRect.left	= iXPos + pPullDown->m_iBlankWidth;
			pPullDown->m_SelectRect.top		= iYPos - iHeight - pPullDown->m_iBlankHeight;
			pPullDown->m_SelectRect.right	= iXPos + iWidth - pPullDown->m_iBlankWidth;
			pPullDown->m_SelectRect.bottom	= pPullDown->m_SelectRect.top + iHeight;

			return pPullDown->m_SelectRect.top;
		}
		break;
	default:
		{
			LOG.PrintTimeAndLog(0, "%s - Unkown style : %d", __FUNCTION__, static_cast<int>( eStyle ) );
		}
		break;
	}

	return 0;
}