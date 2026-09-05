#include "stdafx.h"

#include "ioWndBoundary.h"

namespace BoundaryUtil
{
	bool IsMouseOver( int XPos, int YPos, int Width, int Height, const POINT& Pos )
	{
		RECT rcDerived;
		rcDerived.left		= XPos;
		rcDerived.top		= YPos;
		rcDerived.right		= XPos + Width;
		rcDerived.bottom	= YPos + Height;

		if( PtInRect( &rcDerived, Pos ) )
			return true;

		return false;
	}

	bool IsDerivedShow( ioWnd* pWnd )
	{
		if( pWnd == NULL )
			return true;

		if( pWnd->IsShow() && IsDerivedShow( pWnd->GetParent() ) )
			return true;

		return false;
	}

	bool IsDerivedWnd( ioWnd* pWnd, ioWnd* pTargetWnd )
	{	
		if( pWnd == NULL || pTargetWnd == NULL )
			return false;

		if( pWnd == pTargetWnd )
			return true;

		return IsDerivedWnd( pWnd->GetParent(), pTargetWnd );
	}

	void OnBoundaryContextRender( ioRenderSystem *pSystem, ioHashStringVec& ContextList, int nContextMaxWidth )
	{
		ioWnd *pCurrWnd = g_GUIMgr.GetPreOverWnd();
		if( pCurrWnd == NULL )
			return;

		ioLSMouse* pMouse	= g_App.GetMouse();
		if( pMouse == NULL )
			return;

		char szUIText[MAX_PATH] = "";
		if( ContextList.empty() )
			return;

		ContextList.insert( ContextList.begin(), "[CTRL + C][클립보드로 정보를 복사합니다.] [ALT][절대좌표를 상대좌표로 변환합니다.]" );

		int iXPos = pMouse->GetMousePos().x;
		int iYPos = pMouse->GetMousePos().y;

		//바운더리 정보가 화면밖으로 넘어간다면 넘어간 만큼 위치 이동
		if( nContextMaxWidth + iXPos > Setting::Width() )
			iXPos -= nContextMaxWidth + iXPos - Setting::Width();

		if( (int)ContextList.size() * 20 + iYPos > Setting::Height() )
			iYPos -= (int)ContextList.size() * 20 + iYPos - Setting::Height();

		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor(TCT_DEFAULT_DARKGRAY );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		for( int i = 0; i < (int)ContextList.size(); ++i )
			g_FontMgr.PrintText( iXPos - 5, iYPos - 5 + i * 20, FONT_SIZE_13, ContextList[i].c_str() );

		g_UIRenderer.Render( pSystem );
	}

	void ExportContextClipBoard( const ioHashStringVec& ContextList )
	{
		std::string Context;
		for( int i = 0; i < (int)ContextList.size(); ++i )
		{
			Context += ContextList[i].c_str();
			Context += "\n";
		}

		HGLOBAL hmem = GlobalAlloc( GMEM_MOVEABLE, Context.length() + 1 );
		char *ptr = (char *)GlobalLock( hmem );
		memcpy( ptr, Context.c_str(), Context.length() );
		GlobalUnlock( hmem );

		if ( OpenClipboard( NULL ) )
		{
			EmptyClipboard();
			SetClipboardData( CF_TEXT, hmem );
			CloseClipboard();
		}
	}
}

//--------------------------------------------------------------------------------------------------------

WndBoundaryContainer::WndBoundaryContainer()
{
	
}

WndBoundaryContainer::~WndBoundaryContainer()
{
	for( int i = 0; i < (int)m_WndBoundaryList.size(); ++i )
	{			
		SAFEDELETE( m_WndBoundaryList[i] );
	}
}

void WndBoundaryContainer::AddBoundary( int InnerUniqueIndex, int eType, void* pTarget, ioWnd* pParentsWnd, const ioHashString& szType )
{
	ioWndBoundary* pBoundary = new ioWndBoundary;
	if( pBoundary )
	{
		pBoundary->SetImageType( szType );
		pBoundary->SetInnerUniqueIndex( InnerUniqueIndex );
		pBoundary->SetBindType( static_cast<ioWndBoundary::BindType>( eType ), pTarget, pParentsWnd );
		m_WndBoundaryList.push_back( pBoundary );
	}
}

ioWndBoundary* WndBoundaryContainer::GetBoundary( int InnerUniqueIndex )
{
	for( int i = 0; i < (int)m_WndBoundaryList.size(); ++i )
	{			
		ioWndBoundary* pBoundary = m_WndBoundaryList[i];
		if( pBoundary && pBoundary->GetInnerUniqueIndex() == InnerUniqueIndex )
			return pBoundary;
	}

	return NULL;
}

void WndBoundaryContainer::CollectBoundaryContext( int& nContextMaxWidth, ioHashStringVec& ContextList )
{	
	for( auto iter = m_WndBoundaryList.rbegin(); iter != m_WndBoundaryList.rend(); ++iter )
	{
		ioWndBoundary* pBoundary = *iter;
		if( pBoundary )
		{
			pBoundary->UpdateBind();
			if( pBoundary->IsShow() )
			{
				ioHashStringVec ContextGroup = pBoundary->GetContext();
				if( !ContextGroup.empty() )
				{
					for( auto iter = ContextGroup.begin(); iter != ContextGroup.end(); ++iter )
					{
						const ioHashString& Context = *iter;
						if( !Context.IsEmpty() )
						{
							ContextList.push_back( Context );

							//바운더리 정보 텍스트 크기값을 구한다(화면밖으로 넘어가는 것을 방지하기 위함)
							int nCurrContextWidth = g_FontMgr.GetTextWidth( Context.c_str(), FONT_SIZE_13 );
							if( nContextMaxWidth < nCurrContextWidth )
								nContextMaxWidth = nCurrContextWidth;
						}
					}
				}
			}
		}
	}
}

void WndBoundaryContainer::RenderBoundary()
{
	for( auto iter = m_WndBoundaryList.rbegin(); iter != m_WndBoundaryList.rend(); ++iter )
	{
		ioWndBoundary* pBoundary = *iter;
		if( pBoundary && pBoundary->IsShow() )
			pBoundary->OnBoundaryRender( &RenderSystem() );
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioWndBoundary::ioWndBoundary() : m_pBindTarget( NULL ), m_pParentWnd( NULL ), m_bShow( false ), m_dwInnerUniqueIndex( 0 )
{

}

ioWndBoundary::~ioWndBoundary()
{
}

void ioWndBoundary::SetBindType( BindType eType, void* pTarget, ioWnd* pParentWnd )
{
	m_eBindType		= eType;
	m_pBindTarget	= pTarget;
	m_pParentWnd	= pParentWnd;

	UpdateBind();
}

bool ioWndBoundary::IsShow()
{
	return m_bShow;
}

void ioWndBoundary::SetInnerUniqueIndex( DWORD InnerUniqueIndex )
{
	m_dwInnerUniqueIndex = InnerUniqueIndex;
}
DWORD ioWndBoundary::GetInnerUniqueIndex() const
{
	return m_dwInnerUniqueIndex;
}

void ioWndBoundary::SetImageType( const ioHashString& szType )
{
	m_ImageType = szType;
}

const ioHashString& ioWndBoundary::GetImageType() const
{
	return m_ImageType;
}

const ioHashStringVec& ioWndBoundary::GetContext() const
{
	return m_Context;
}

void ioWndBoundary::UpdateBind()
{
	ioWnd *pCurrWnd = g_GUIMgr.GetPreOverWnd();
	if( pCurrWnd == NULL )
		return;
	
	ioLSMouse* pMouse = g_App.GetMouse();
	if( pMouse == NULL )
		return;

	m_bShow		= false;
	m_Context.clear();

	if( m_pBindTarget )
	{
		switch( m_eBindType )
		{
		case BT_WND:
			{
				UpdateBindByWnd( pCurrWnd, pMouse->GetMousePos() );
			}
			break;
		case BT_UIFRAME:
			{
				UpdateBindByUIFrame( pCurrWnd, pMouse->GetMousePos() );
			}
			break;
		case BT_UIIMAGE:
			{
				UpdateBindByUIImage( pCurrWnd, pMouse->GetMousePos() );
			}
			break;
		}
	}
}

void ioWndBoundary::UIAlignToString( ioWnd::UIPosAlign Align, ioHashString& szAlign )
{
	switch( Align )
	{
	case ioWnd::UI_LEFT_ALIGN:
		szAlign = "Left";
		break;
	case ioWnd::UI_RIGHT_ALIGN:
		szAlign = "Right";
		break;
	case ioWnd::UI_CENTER_ALIGN:
		szAlign = "Center";
		break;
	case ioWnd::UI_RATE_ALIGN:
		szAlign = "Left";
		break;
	case ioWnd::UI_TOP_ALIGN:
		szAlign = "Top";
		break;
	case ioWnd::UI_BOTTOM_ALIGN:
		szAlign = "Bottom";
		break;
	default:
		szAlign = "None";
		break;
	}
}

void ioWndBoundary::UpdateBindByWnd( ioWnd *pCurrWnd, const POINT& Pos )
{
	ioWnd* pWnd = reinterpret_cast<ioWnd*>( m_pBindTarget );
	if( pWnd )
	{
		int XPos	= pWnd->GetDerivedPosX();
		int YPos	= pWnd->GetDerivedPosY();
		int Width	= pWnd->GetWidth();
		int Height	= pWnd->GetHeight();

		if( BoundaryUtil::IsDerivedShow( pWnd ) && BoundaryUtil::IsDerivedWnd( pWnd, pCurrWnd ) && BoundaryUtil::IsMouseOver( XPos, YPos, Width, Height, Pos ) )
		{
			m_bShow = true;
			m_Color = 0xff00ff00;
			UpdateBoundary( pWnd->GetDerivedPosX(), pWnd->GetDerivedPosY(), pWnd->GetWidth(), pWnd->GetHeight() );

			char szContext[MAX_PATH] = "";
			ioHashString szSrc, szHorz, szVert;

			ioWnd* pParentWnd = m_pParentWnd;
			while ( pParentWnd )
			{
				szSrc = "[PARENT WINDOW][NAME : %s, ID : %d][X : %d, Y : %d, WIDTH : %d, HEIGHT : %d][XALIGN : %s, YALIGN : %s]";
				
				UIAlignToString( static_cast<ioWnd::UIPosAlign>( pParentWnd->GetHorzAlign() ), szHorz );
				UIAlignToString( static_cast<ioWnd::UIPosAlign>( pParentWnd->GetVertAlign() ), szVert );
				sprintf( szContext, szSrc.c_str(), pParentWnd->GetClassNameEX(),
												   pParentWnd->GetID(),
												   ( g_Input.IsKeyDown( KEY_LEFTALT ) || g_Input.IsKeyDown( KEY_RIGHTALT ) ) ? pParentWnd->GetXPos() : pParentWnd->GetDerivedPosX(),
												   ( g_Input.IsKeyDown( KEY_LEFTALT ) || g_Input.IsKeyDown( KEY_RIGHTALT ) ) ? pParentWnd->GetYPos() : pParentWnd->GetDerivedPosY(),
												   pParentWnd->GetWidth(),
												   pParentWnd->GetHeight(),
												   szHorz.c_str(),
												   szVert.c_str() );

				m_Context.insert( m_Context.begin(), szContext );
				if( pParentWnd->GetParent() && pParentWnd->GetParent() != g_GUIMgr.GetDeskTop() )
					pParentWnd = pParentWnd->GetParent();
				else
					pParentWnd = NULL;
			}

			UIAlignToString( static_cast<ioWnd::UIPosAlign>( pWnd->GetHorzAlign() ), szHorz );
			UIAlignToString( static_cast<ioWnd::UIPosAlign>( pWnd->GetVertAlign() ), szVert );
			szSrc = "[WINDOW][NAME : %s, ID : %d][X : %d, Y : %d, WIDTH : %d, HEIGHT : %d][XALIGN : %s, YALIGN : %s]";
			sprintf( szContext, szSrc.c_str(), pWnd->GetClassNameEX(), 
											   pWnd->GetID(),
											   ( g_Input.IsKeyDown( KEY_LEFTALT ) || g_Input.IsKeyDown( KEY_RIGHTALT ) ) ? pWnd->GetXPos() : pWnd->GetDerivedPosX(),
											   ( g_Input.IsKeyDown( KEY_LEFTALT ) || g_Input.IsKeyDown( KEY_RIGHTALT ) ) ? pWnd->GetYPos() : pWnd->GetDerivedPosY(),
											   Width,
											   Height,
											   szHorz.c_str(),
											   szVert.c_str() );
			m_Context.push_back( szContext );
		}
	}
}

void ioWndBoundary::UpdateBindByUIFrame( ioWnd *pCurrWnd, const POINT& Pos )
{
	if( BoundaryUtil::IsDerivedShow( m_pParentWnd ) && BoundaryUtil::IsDerivedWnd( m_pParentWnd, pCurrWnd ) )
	{
		ioUIRenderFrame* pFrame = reinterpret_cast<ioUIRenderFrame*>( m_pBindTarget );
		if( pFrame )
		{
			//프로그램상에서 위치 변환하여 렌더링하는 경우가 있기 때문에 최종적으로 렌더링된 위치로 좌표를 계산
			int XPos	= pFrame->GetRenderedPos().x - m_pParentWnd->GetDerivedPosX();
			int YPos	= pFrame->GetRenderedPos().y - m_pParentWnd->GetDerivedPosY();
			int Width	= pFrame->GetWidth();
			int Height	= pFrame->GetHeight();

			//렌더링 된적이 없던 정보는 그리지 않음
			if( XPos < 0 && YPos < 0 )
				return;

			if( BoundaryUtil::IsMouseOver( pFrame->GetRenderedPos().x, pFrame->GetRenderedPos().y, Width, Height, Pos ) )
			{
				m_bShow	= true;
				m_Color	= 0xffff0000;
				UpdateBoundary( pFrame->GetRenderedPos().x, pFrame->GetRenderedPos().y, Width, Height );

				char szContext[MAX_PATH] = "";
				ioHashString szSrc = "[UIFRAME][NAME : %s, TYPE : %s][X : %d, Y : %d, WIDTH : %d, HEIGHT : %d]";
				if( g_Input.IsKeyDown( KEY_LEFTALT ) || g_Input.IsKeyDown( KEY_RIGHTALT ) )
				{
					sprintf( szContext, szSrc.c_str(), pFrame->GetName().c_str(), m_ImageType.c_str(), XPos, YPos, Width, Height );
				 }
				 else
				{
					sprintf( szContext, szSrc.c_str(), pFrame->GetName().c_str(),
													   m_ImageType.c_str(),
													   m_pParentWnd->GetDefaultPosX() + pFrame->GetPosX(),
													   m_pParentWnd->GetDefaultPosY() + pFrame->GetPosY(),
													   Width,
													   Height );
				 }

				m_Context.push_back( szContext );
			}
		}
	}
}

void ioWndBoundary::UpdateBindByUIImage( ioWnd *pCurrWnd, const POINT& Pos )
{
	if( BoundaryUtil::IsDerivedShow( m_pParentWnd ) && BoundaryUtil::IsDerivedWnd( m_pParentWnd, pCurrWnd ) )
	{
		ioUIRenderImage* pImage = reinterpret_cast<ioUIRenderImage*>( m_pBindTarget );
		if( pImage )
		{
			//프로그램상에서 위치 변환하여 렌더링하는 경우가 있기 때문에 최종적으로 렌더링된 위치로 좌표를 계산
			int XPos	= pImage->GetRenderedPos().x - m_pParentWnd->GetDerivedPosX();
			int YPos	= pImage->GetRenderedPos().y - m_pParentWnd->GetDerivedPosY();
			int Width	= pImage->GetWidth();
			int Height	= pImage->GetHeight();

			//렌더링 된적이 없던 정보는 그리지 않음
			if( XPos < 0 && YPos < 0 )
				return;

			if( BoundaryUtil::IsMouseOver( pImage->GetRenderedPos().x, pImage->GetRenderedPos().y, Width, Height, Pos ) )
			{
				m_bShow	= true;
				m_Color	= 0xff0000ff;
				UpdateBoundary( pImage->GetRenderedPos().x, pImage->GetRenderedPos().y, Width, Height );

				char szContext[MAX_PATH] = "";

				ioHashString szSrc = "[UIIMAGE][NAME : %s, TYPE : %s][X : %d, Y : %d, WIDTH : %d, HEIGHT : %d][XALIGN : %s, YALIGN : %s]";
				if( g_Input.IsKeyDown( KEY_LEFTALT ) || g_Input.IsKeyDown( KEY_RIGHTALT ) )
				{
					sprintf( szContext, szSrc.c_str(), pImage->GetImageName().c_str(),
													   m_ImageType.c_str(),
													   XPos,
													   YPos,
													   Width,
													   Height,
													   pImage->GetHorzFormatting().c_str(),
													   pImage->GetVertFormatting().c_str() );
				}
				else
				{
					sprintf( szContext, szSrc.c_str(), pImage->GetImageName().c_str(),
													   m_ImageType.c_str(),
													   m_pParentWnd->GetDerivedPosX() + pImage->GetPosX(),
													   m_pParentWnd->GetDerivedPosY() + pImage->GetPosY(),
													   Width,
													   Height,
													   pImage->GetHorzFormatting().c_str(),
													   pImage->GetVertFormatting().c_str() );
				}

				m_Context.push_back( szContext );
			}
		}
	}
}

void ioWndBoundary::UpdateBoundary( int XPos, int YPos, int Width, int Height )
{	
	FillVtxRHWTexRectStrip( m_Boundary, XPos, YPos, Width, Height );
}

void ioWndBoundary::OnBoundaryRender( ioRenderSystem *pSystem )
{
	pSystem->SetFixedFVF( D3DFVF_XYZRHW );
	pSystem->DisableShader();
	pSystem->SetLightEnable( false );
	pSystem->SetDepthFunction( D3DCMP_LESSEQUAL, false, false );
	pSystem->SetCullingMode( CM_NO );
	pSystem->SetAlphaTest( D3DCMP_GREATER, 0 );	

	DWORD eMode;
	pSystem->GetDevice()->GetRenderState( D3DRS_FILLMODE, &eMode );
	pSystem->GetDevice()->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	pSystem->SetTextureFactor( m_Color );
	pSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ZERO );
	pSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_DIFFUSE );
	pSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_DIFFUSE );
	pSystem->DisableTextureUnitsFrom( 1 );
	pSystem->ClearStreamDesc();
	pSystem->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_Boundary, sizeof(ioVtxRHWTex) );
	pSystem->DisableTextureUnitsFrom( 0 );

	pSystem->GetDevice()->SetRenderState( D3DRS_FILLMODE, eMode );
}