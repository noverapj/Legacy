
#include <stdafx.h>

#include "ExcavationShovelWnd.h"
#include "../ioExcavationManager.h"

ExcavationShovelWnd::ExcavationShovelWnd()
{
	m_pIcon = NULL;
	m_pIconBack = NULL;
}

ExcavationShovelWnd::~ExcavationShovelWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void ExcavationShovelWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_Text[TEXT_CUR_ITEM]       = xElement.GetStringAttribute_e( "TextCurrentCount" );
	m_Text[TEXT_CHARGE_SHOVEL]   = xElement.GetStringAttribute_e( "TextShovelCharge" );
	m_Text[TEXT_BUY_SHOVEL] = xElement.GetStringAttribute_e( "TextBuyChargeItem" );
	m_Text[TEXT_IMPOSSIBLE_CHARGE] = xElement.GetStringAttribute_e( "TextImpossibleCharge" );
}

void ExcavationShovelWnd::iwm_show()
{
	UpdateUI();

	SAFEDELETE( m_pIcon );

	ioHashString szIconName;
	if( g_ExcavationMgr.GetUseIconName( szIconName ) )
	{
		m_pIcon     = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	}

	if( !m_pIconBack )
	{
		m_pIconBack	= g_UIImageSetMgr.CreateImageByFullName( "BaseImage002#iconeffect_4" );
	}
}

void ExcavationShovelWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if(!pWnd) return;

    DWORD dwID = pWnd->GetID();

    switch (cmd)
    {
    case IOBN_BTNUP:
        {
            switch (dwID)
            {
            case ID_EXIT_BTN:
            case ID_CLOSE_BTN:
                if( cmd == IOBN_BTNUP )
                {
                    HideWnd();
                }
                break;
            case ID_CHARGE_BTN:
                if( cmd == IOBN_BTNUP )
                {
					g_ExcavationMgr.SendRechargeShovel();
                }
                break;
			case ID_BUY_BTN:
				if( cmd == IOBN_BTNUP )
				{
					ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_NEW_EXCAVATING_SHOVEL );
					if( pItem )
					{
						NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
						if( pNewShopWnd )
						{
							pNewShopWnd->ShowWnd();
							pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, ioEtcItem::EIT_ETC_NEW_EXCAVATING_SHOVEL, 0 );
							HideWnd();
						}
					}

					HideWnd();
				}
				break;
            }
        }
        break;
    }
}

bool ExcavationShovelWnd::iwm_spacebar()
{
	if( g_App.IsMouseBusy() )
	{
		return false;
	}

	ioWnd *pChildWnd = FindChildWnd( ID_CHARGE_BTN );

	if( pChildWnd )
	{
		if( pChildWnd->IsShow() )
		{
			ioWnd *pWnd = FindChildWnd( ID_CHARGE_BTN );
			if( pWnd )
			{
				if( g_ExcavationMgr.GetShovelCount() < g_ExcavationMgr.GetMaxShovelCount() )
				{
					g_ExcavationMgr.SendRechargeShovel();
				}
				return true;
			}
		}
	}

	return false;
}

void ExcavationShovelWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	/*m_sDesc1.OnRender( iXPos + 205, iYPos + 191 );
	m_sDesc2.OnRender( iXPos + 205, iYPos + 215 );
	m_sDesc3.OnRender( iXPos + 205, iYPos + 239 );*/

	if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos + 178, iYPos + 98, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	if( m_pIcon )
	{
		m_pIcon->Render( iXPos + 178, iYPos + 98, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( 58, 58, 58 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetVertAlign( TVA_TOP );
	if( g_ExcavationMgr.GetShovelCount() >= g_ExcavationMgr.GetMaxShovelCount() )
	{
		g_FontMgr.PrintText( iXPos + 177, iYPos + 172, FONT_SIZE_14, m_Text[TEXT_CUR_ITEM].c_str(), m_iShovelCnt );
		g_FontMgr.PrintText( iXPos + 177, iYPos + 194, FONT_SIZE_14, m_Text[TEXT_IMPOSSIBLE_CHARGE].c_str() );
	}
	else if( m_iShovelCnt <= 0 )
	{
		g_FontMgr.PrintText( iXPos + 177, iYPos + 172, FONT_SIZE_14, m_Text[TEXT_CUR_ITEM].c_str(), m_iShovelCnt );
		g_FontMgr.PrintText( iXPos + 177, iYPos + 194, FONT_SIZE_14, m_Text[TEXT_BUY_SHOVEL].c_str() );
	}
	else
	{
		g_FontMgr.PrintText( iXPos + 177, iYPos + 172, FONT_SIZE_14, m_Text[TEXT_CUR_ITEM].c_str(), m_iShovelCnt );
		g_FontMgr.PrintText( iXPos + 177, iYPos + 194, FONT_SIZE_14, m_Text[TEXT_CHARGE_SHOVEL].c_str() );
	}
}

void ExcavationShovelWnd::UpdateUI()
{
	m_iShovelCnt = 0;
	
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem )
	{
		m_iShovelCnt = pUserEtcItem->GetUse( ioEtcItem::EIT_ETC_NEW_EXCAVATING_SHOVEL );
	}

	if( g_ExcavationMgr.GetShovelCount() >= g_ExcavationMgr.GetMaxShovelCount() )
	{
		ShowChildWnd( ID_CHARGE_BTN );
		HideChildWnd( ID_BUY_BTN );
		SetChildInActive( ID_CHARGE_BTN );
	}
	else if( m_iShovelCnt <= 0 )
	{
		HideChildWnd( ID_CHARGE_BTN );
		ShowChildWnd( ID_BUY_BTN );
	}
	else
	{
		ShowChildWnd( ID_CHARGE_BTN );
		HideChildWnd( ID_BUY_BTN );
		SetChildActive( ID_CHARGE_BTN );
	}
	/*m_sDesc1.Set( 0, 0, m_Text[TEXT_DESC].c_str(), FONT_SIZE_14, -1, TS_NORMAL, TCT_DEFAULT_WHITE, TCT_DEFAULT_BLACK, TAT_CENTER, TVA_CENTER );

	char szBuf[MAX_PATH] = "", szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( (INT64)iCnt, szConvertNum, MAX_PATH );
	wsprintf( szBuf, m_Text[TEXT_CUR_ITEM].c_str(), szConvertNum );
	m_sDesc2.Set( 0, 0, szBuf, FONT_SIZE_14, -1, TS_NORMAL, TCT_DEFAULT_WHITE, TCT_DEFAULT_BLACK, TAT_CENTER, TVA_CENTER );

	if( g_ExcavationMgr.GetShovelCount() >= g_ExcavationMgr.GetMaxShovelCount() )
	{
		ShowChildWnd( ID_CHARGE_BTN );
		HideChildWnd( ID_BUY_BTN );
		SetChildInActive( ID_CHARGE_BTN );

		m_sDesc3.Set( 0, 0, m_Text[TEXT_MAX_SHOVEL].c_str(), FONT_SIZE_14, -1, TS_NORMAL, TCT_DEFAULT_RED2, TCT_DEFAULT_BLACK, TAT_CENTER, TVA_CENTER );
	}
	else if( iCnt <= 0 )
	{
		HideChildWnd( ID_CHARGE_BTN );
		ShowChildWnd( ID_BUY_BTN );

		m_sDesc3.Set( 0, 0, m_Text[TEXT_BUY_SHOVEL].c_str(), FONT_SIZE_14, -1, TS_NORMAL, TCT_DEFAULT_RED2, TCT_DEFAULT_BLACK, TAT_CENTER, TVA_CENTER );
	}
	else
	{
		ShowChildWnd( ID_CHARGE_BTN );
		HideChildWnd( ID_BUY_BTN );
		SetChildActive( ID_CHARGE_BTN );

		m_sDesc3.Set( 0, 0, m_Text[TEXT_ADD_SHOVEL].c_str(), FONT_SIZE_14, -1, TS_NORMAL, TCT_DEFAULT_WHITE, TCT_DEFAULT_BLACK, TAT_CENTER, TVA_CENTER );
	}*/
}

bool ExcavationShovelWnd::IsInWndRect( const POINT &vPos )
{
	RECT rcDerived;
	rcDerived.left	= GetDerivedPosX();
	rcDerived.top   = GetDerivedPosY() - 23;
	rcDerived.right	= rcDerived.left + GetWidth();
	rcDerived.bottom= rcDerived.top + GetHeight() + 23;

	if( PtInRect( &rcDerived, vPos ) )
		return true;

	return false;
}