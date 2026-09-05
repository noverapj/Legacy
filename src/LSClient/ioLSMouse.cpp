
#include "stdafx.h"

#include "ioLSMouse.h"
#include "GUI/ioSpecificDragItem.h"

ioLSMouse::ioLSMouse()
{
	m_pNormalCursor			= NULL;
	m_pToolTipCursor		= NULL;
	m_pHousingCameraCursor	= NULL;
	m_pCursor				= NULL;
	m_pDragImg				= NULL;
	m_pSubImg = NULL;
	m_pUI3DRender			= NULL;
	m_pUIChar				= NULL;
	m_pDragRenderFrame		= NULL;

	m_bMouseShowing			= false;
	m_bTraining				= false;
	m_dwToolTipWndID		= 0;
}

ioLSMouse::~ioLSMouse()
{
	ReleaseResource();
}

void ioLSMouse::InitResource()
{
	ReleaseResource();

	m_pNormalCursor			= g_UIImageSetMgr.CreateImage( "BaseImage004", "mouse_normal" );
	m_pToolTipCursor		= g_UIImageSetMgr.CreateImage( "BaseImage004", "mouse_info" );
	m_pHousingCameraCursor	=  g_UIImageSetMgr.CreateImage( "BaseImage039", "housing_mouse_icon" );
	m_pCursor				= m_pNormalCursor;

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "mouse_drag_cam" );
}

void ioLSMouse::ReleaseResource()
{
	m_pCursor = NULL;

	SAFEDELETE( m_pDragImg );
	SAFEDELETE( m_pSubImg );
	SAFEDELETE( m_pNormalCursor );
	SAFEDELETE( m_pToolTipCursor );
	SAFEDELETE( m_pHousingCameraCursor );
	SAFEDELETE( m_pUIChar );
	SAFEDELETE( m_pUI3DRender );
	SAFEDELETE( m_pDragRenderFrame );
}

void ioLSMouse::RenderDragImage()
{
	if( m_MouseState == MS_DROP_ENABLE )
	{
		if( m_pDragImg )
		{
			m_pDragImg->Render( m_ptMousePos.x, m_ptMousePos.y, UI_RENDER_NORMAL, TFO_BILINEAR );
			m_pDragImg->Render( m_ptMousePos.x, m_ptMousePos.y, UI_RENDER_SCREEN, TFO_BILINEAR );
			m_pDragImg->Render( m_ptMousePos.x, m_ptMousePos.y, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
		if( m_pSubImg )
		{
			m_pSubImg->Render( m_ptMousePos.x, m_ptMousePos.y, UI_RENDER_NORMAL, TFO_BILINEAR );
			m_pSubImg->Render( m_ptMousePos.x, m_ptMousePos.y, UI_RENDER_SCREEN, TFO_BILINEAR );
			m_pSubImg->Render( m_ptMousePos.x, m_ptMousePos.y, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
	}
	else
	{
		if( m_pDragImg )
		{
			m_pDragImg->Render( m_ptMousePos.x, m_ptMousePos.y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		if( m_pSubImg )
		{
			m_pSubImg->Render( m_ptMousePos.x, m_ptMousePos.y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}

void ioLSMouse::RenderDragUIFrame()
{
	if( m_MouseState == MS_DROP_ENABLE )
	{
		if( m_pDragRenderFrame )
		{
			m_pDragRenderFrame->SetAlpha( (float)MAX_ALPHA_RATE * 0.75f );
			m_pDragRenderFrame->Render( m_ptMousePos.x, m_ptMousePos.y, UI_RENDER_NORMAL );
		}
	}
	else
	{
		if( m_pDragRenderFrame )
		{
			m_pDragRenderFrame->SetAlpha( (float)MAX_ALPHA_RATE * 0.75f );
			m_pDragRenderFrame->Render( m_ptMousePos.x, m_ptMousePos.y, UI_RENDER_NORMAL );
		}
	}
}

void ioLSMouse::RenderDragCharacter()
{
	if( m_pUIChar && m_pUI3DRender )
	{
		D3DXMATRIX matUI = m_pUI3DRender->MakeUIProjection( m_ptMousePos.x - 400, m_ptMousePos.y - 230 );
		m_pUI3DRender->Render( &matUI, true );
	}
}

void ioLSMouse::Render()
{
	m_bMouseShowing = true;

	if( IsDragDropping() )
	{
		RenderDragImage();
		RenderDragCharacter();	
		RenderDragUIFrame();
	}

	if( m_pCursor )
	{
		m_pCursor->Render( m_ptMousePos.x, m_ptMousePos.y );
		m_pCursor = m_pNormalCursor;
	}
}

void ioLSMouse::SetToolTipID( const ioHashString &szToolTipID, int iType )
{
	m_szToolTipID  = szToolTipID;
	m_iToolTipType = iType;
}

void ioLSMouse::SetToolTipCursor()
{
	m_pCursor = m_pToolTipCursor;
}

void ioLSMouse::SetHousingCameraCursor()
{
	m_pCursor = m_pHousingCameraCursor;
}

void ioLSMouse::ClearMouseShowingState()
{
	m_bMouseShowing = false;
}

bool ioLSMouse::IsDragDropping() const
{
	if( m_MouseState == MS_DROP_ENABLE || m_MouseState== MS_DROP_DISABLE )
		return true;

	return false;
}

bool ioLSMouse::IsMouseBtnAction() const
{
	if( IsLBtnDown() || IsLBtnPress() ||
		IsRBtnDown() || IsRBtnPress() ||
		IsLBtnDown() || IsLBtnPress() )
	{
		return true;
	}

	return false;

}

void ioLSMouse::SetDragDropStart( ioDragItem *pDragItem, const POINT &ptOffset )
{
	if( IsBusy() )	return;

	ioMouse::SetDragDropStart( pDragItem, ptOffset );

	// 이미지 드래그
	ioInvenDragItem *pInven = ToInvenDragItem( pDragItem );
	if( pInven )
	{
		SAFEDELETE( m_pUIChar );
		SAFEDELETE( m_pDragImg );
		SAFEDELETE( m_pSubImg );

		m_pDragImg = g_UIImageSetMgr.CreateImageByFullName( pDragItem->GetDragImageName() );
		if( m_pDragImg )
		{
			m_pDragImg->SetScale( 0.75f );		// 보통 75%의 사이즈로 아이템을 드래깅한다
			m_pDragImg->SetPosition( pInven->GetIconOffsetX(), pInven->GetIconOffsetY() );
		}

		m_pSubImg = g_UIImageSetMgr.CreateImageByFullName( pDragItem->GetDragSubImageName() );
		if( m_pSubImg )
		{
			m_pSubImg->SetScale( 0.75f );		// 보통 75%의 사이즈로 아이템을 드래깅한다
			m_pSubImg->SetPosition( pInven->GetIconOffsetX(), pInven->GetIconOffsetY() );
		}
		return; // 주의!!
	}

	// 용병 드래그
	ioSoldierDragItem *pSoldier = ToSoldierDragItem( pDragItem );
	if( pSoldier )
	{
		D3DXVECTOR3 vCharPos( 0.0f, 0.0f, 800.0f );

		ioCreateChar *pChar = m_pUI3DRender->CreateChar( pSoldier->GetCharInfo(), "DragChar" );
		pChar->SetPosition( vCharPos );
		pChar->SetYaw( FLOAT10 );

		m_pUI3DRender->SetLookAt( vCharPos );

		SAFEDELETE( m_pDragImg );
		SAFEDELETE( m_pUIChar );
		m_pUIChar = pChar;

		// Equip Item..
		// 코스튬 관련 (용병 드래그 시)
		for( int i=0 ; i< MAX_INVENTORY ; i++ )
		{
			DWORD dwCostumeMaleCustom = pSoldier->GetCharInfo().m_EquipCostume[i].m_Costume_Male_Custom;
			DWORD dwCostumeFemaleCustom = pSoldier->GetCharInfo().m_EquipCostume[i].m_Costume_Female_Custom;

			const ITEM_DATA &rkItem = pSoldier->GetEquipInfo( i );
			if( rkItem.m_item_code > 0 )
			{
				if ( pSoldier->GetCharInfo().m_EquipCostume[i].m_CostumeCode <= 0 )
				{
					dwCostumeMaleCustom = rkItem.m_item_male_custom;
					dwCostumeFemaleCustom = rkItem.m_item_female_custom;
				}
				m_pUIChar->EquipItem( rkItem.m_item_code, dwCostumeMaleCustom, dwCostumeFemaleCustom, pSoldier->GetCharInfo().m_EquipCostume[i].m_CostumeCode );
			}
		}

		ioHashString szModelAnimation = m_pUI3DRender->GetModelAnimation( 0, m_pUIChar->IsMale() );
		if( !szModelAnimation.IsEmpty() )
		{
			m_pUIChar->SetLoopAni( szModelAnimation );
			m_pUIChar->Update( 0.001f );
		}
		return; // 주의!!
	}

	//상점 내용병 버튼 드래그
	ioShopSoldierDragItem *pShopSoldier = ToShopSoldierDragItem( pDragItem );
	if( pShopSoldier )
	{
		SAFEDELETE( m_pUIChar );
		SAFEDELETE( m_pDragImg );
		SAFEDELETE( m_pSubImg );

		m_pDragImg = g_UIImageSetMgr.CreateImageByFullName( pShopSoldier->GetDragImageName() );
		if( m_pDragImg )
		{
			m_pDragImg->SetPosition( pShopSoldier->GetIconOffsetX(), pShopSoldier->GetIconOffsetY() );
			m_pDragImg->SetScale( SHOP_MY_SOLDIER_BTN_SCALE );
		}
		m_pSubImg = g_UIImageSetMgr.CreateImageByFullName( pShopSoldier->GetDragSubImageName() );
		if( m_pSubImg )
		{
			m_pSubImg->SetPosition( pShopSoldier->GetIconOffsetX(), pShopSoldier->GetIconOffsetY() );
			m_pSubImg->SetScale( SHOP_MY_SOLDIER_BTN_SCALE );
		}
		return; // 주의!!
	}

	ioUnAllocateButtonDragItem *pUnAllocateItem = ToUnAllocateButtonDragItem( pDragItem );
	if( pUnAllocateItem )
	{
		SAFEDELETE( m_pDragRenderFrame );
		if( pUnAllocateItem->GetRenderFrame() )
		{
			m_pDragRenderFrame = dynamic_cast<ioUIRenderFrame*>( pUnAllocateItem->GetRenderFrame()->Clone() );
			m_pDragRenderFrame->SetPosition( -m_pDragRenderFrame->GetWidth()/2, -m_pDragRenderFrame->GetHeight()/2 );
		}

		return; // 주의!!
	}

	ioAllocatedButtonDragItem *pAllocatedItem = ToAllocatedButtonDragItem( pDragItem );
	if( pAllocatedItem )
	{
		SAFEDELETE( m_pDragRenderFrame );
		if( pAllocatedItem->GetRenderFrame() )
		{
			m_pDragRenderFrame = dynamic_cast<ioUIRenderFrame*>( pAllocatedItem->GetRenderFrame()->Clone() );
			m_pDragRenderFrame->SetPosition( -m_pDragRenderFrame->GetWidth()/2, -m_pDragRenderFrame->GetHeight()/2 );
		}
		
		return; // 주의!!
	}
}

void ioLSMouse::GetToolTipID( OUT ioHashString &rsToolTipID, OUT int &riType )
{
	rsToolTipID = m_szToolTipID;
	riType      = m_iToolTipType;
}

void ioLSMouse::SetPacketID( DWORD dwPacketID )
{
	if ( dwPacketID != CTPK_JOIN_BATTLEROOM_LIST && dwPacketID != CTPK_SERVER_LOBBY_CHAT && dwPacketID != CTPK_CAMP_DATA_SYNC )
		m_dwPacketID = dwPacketID;
}
