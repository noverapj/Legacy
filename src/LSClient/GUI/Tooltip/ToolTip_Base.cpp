#include <stdafx.h>

#include "ToolTip_Base.h"
#include "ToolTipManager.h"

ToolTip_Base::ToolTip_Base()
{
	m_pBackGround = NULL;
	m_pIcon = NULL;

	m_pTitle= NULL;
	m_pSubTitle = NULL;
	m_pComment = NULL;

	m_nFrameGap = 5;
	m_nTitleWidth  = -1;
	m_nSubTitleWidth = -1;
	m_nCommentWidth = 200;

	m_nBase_X = 0;
	m_nBase_Y = 0;

	m_nIconRenderPos_x = 0;
	m_nIconRenderPos_y = 0;

	m_eToolTipType = 0;

	m_bFixedPos = false;

	m_iFixedPosX = 0;
	m_iFixedPosY = 0;

	m_bReverse = false;

    m_kToolTipStringMap.clear();
}

ToolTip_Base::~ToolTip_Base()
{
	SAFEDELETE(m_pBackGround);
	SAFEDELETE(m_pIcon);

	SAFEDELETE(m_pTitle);
	SAFEDELETE(m_pSubTitle);
	SAFEDELETE(m_pComment);
}

void ToolTip_Base::iwm_create()
{

}

void ToolTip_Base::iwm_show()
{
	if(m_pBackGround)
	{
		m_pBackGround->SetShow(true);
	}
}

void ToolTip_Base::iwm_hide()
{
	m_bFixedPos = false;
	m_iFixedPosX = 0;
	m_iFixedPosY = 0;

	if(m_pBackGround)
	{
		m_pBackGround->SetShow(false);
	}

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pTitle);
	SAFEDELETE(m_pSubTitle);
	SAFEDELETE(m_pComment);

	g_pToolTipMgr->SetCurrentToolTip(NULL);

    m_kToolTipStringMap.clear();
}

void ToolTip_Base::OnShow()
{
	GoToTopWnd();
	CalculatorPos();
	ShowWnd();

	g_pToolTipMgr->SetCurrentToolTip(this);
}

void ToolTip_Base::OnOpen( bool bIsDirectOpen, int _x, int _y, const char* szIconName, const char* szTitle, const char* szSubTitle, const char* szComment )
{
	m_nBase_X = _x;
	m_nBase_Y = _y;

	if(szIconName)
	{
		SetIcon(szIconName);
	}

	if(szTitle)
	{
		SetTitle(szTitle);
	}

	if(szSubTitle)
	{
		SetSubTitle(szSubTitle);
	}

	if(szComment)
	{
		SetComment(szComment);
	}
}

void ToolTip_Base::SetTitleColor( DWORD dwTextColor, DWORD dwBkColor )
{
	_ENCSTR("Title", Title);
	IoUIText* pTitle = GetText(Title);
	if(pTitle)
		pTitle->SetAllColor( dwTextColor, dwBkColor );
}

void ToolTip_Base::SetSubTitleColor( DWORD dwTextColor, DWORD dwBkColor )
{
	_ENCSTR("SubTitle", SubTitle);
	IoUIText* pSubTitle = GetText(SubTitle);
	if( pSubTitle)
		pSubTitle->SetAllColor( dwTextColor, dwBkColor );
}

void ToolTip_Base::SetCommentColor( DWORD dwTextColor, DWORD dwBkColor )
{
	_ENCSTR("Comment", Comment);
	IoUIText* pComment = GetText(Comment);
	if(pComment)
		pComment->SetAllColor( dwTextColor, dwBkColor );
}

void ToolTip_Base::SetIcon( const char* szIconName )
{
	SAFEDELETE(m_pIcon);
	if(szIconName && strlen(szIconName) > 0)
	{
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	}
}

void ToolTip_Base::SetTitle( const char* szTitle)
{
	if(szTitle && strlen(szTitle) > 0)
	{
		_ENCSTR("Title", Title);
		IoUIText* pTitle = GetText(Title);
		if(pTitle)
		{
			pTitle->SetText(szTitle, "");
			SAFEDELETE(m_pTitle);
			m_pTitle = new IoString;
			m_pTitle->SetByUIText(pTitle, 0, 0, m_nTitleWidth);
            m_kToolTipStringMap[TTCT_TITLE] = m_pTitle;
		}
	}
}

void ToolTip_Base::SetSubTitle( const char* szSubTitle)
{
	if(szSubTitle && strlen(szSubTitle) > 0)
	{
		_ENCSTR("SubTitle", Title);
		IoUIText* pSubTitle = GetText(Title);
		if(pSubTitle)
		{
			pSubTitle->SetText(szSubTitle, "");
			SAFEDELETE(m_pSubTitle);
			m_pSubTitle= new IoString;
			m_pSubTitle->SetByUIText(pSubTitle, 0, 0, m_nSubTitleWidth);
            m_kToolTipStringMap[TTCT_SUBTITLE] = m_pSubTitle;
		}
	}
}

void ToolTip_Base::SetComment( const char* szComment)
{
	if(szComment && strlen(szComment) > 0)
	{
		_ENCSTR("Comment", Title);
		IoUIText* pComment = GetText(Title);
		if(pComment)
		{
			pComment->SetText(szComment, "");
			SAFEDELETE(m_pComment);
			m_pComment= new IoString;
			m_pComment->SetByUIText(pComment, 0, 0, m_nCommentWidth);
            m_kToolTipStringMap[TTCT_COMMENT] = m_pComment;
		}
	}
}

void ToolTip_Base::CalculatorPos()
{
	int nIcon_x = 0;
	int nIcon_y = 0;
	int nIconSize_x = 0;
	int nIconSize_y = 0;

	int nTitleSize_x = 0;
	int nTitleSize_y = 0;

	int nSubTitleSize_x = 0;
	int nSubTitleSize_y = 0;

	int nCommentSize_x = 0;
	int nCommentSize_y = 0;

	int nTotal_x = 0;
	int nTotal_y = m_nFrameGap + m_nBase_Y;

	if(m_pIcon)
	{
		nIconSize_x = m_pIcon->GetSize().x;
		nIconSize_y = m_pIcon->GetSize().y;		

		nTotal_x = nSubTitleSize_x;
	}

	if(m_pTitle)
	{
		g_FontMgr.SetTextStyle( m_pTitle->GetStyle() );
		nTitleSize_x = m_pTitle->GetMaxLineWidth();
		nTitleSize_y = m_pTitle->GetMaxHeight();

		if(nTotal_x < nTitleSize_x)
			nTotal_x = nTitleSize_x;
	}

	if(m_pSubTitle)
	{
		g_FontMgr.SetTextStyle( m_pSubTitle->GetStyle() );
		nSubTitleSize_x = m_pSubTitle->GetMaxLineWidth();
		nSubTitleSize_y = m_pSubTitle->GetMaxHeight();

		if(nTotal_x < nSubTitleSize_x)
			nTotal_x = nSubTitleSize_x;
	}

	if(m_pComment)
	{
		g_FontMgr.SetTextStyle( m_pComment->GetStyle() );
		nCommentSize_x = m_pComment->GetMaxLineWidth();
		nCommentSize_y = m_pComment->GetMaxHeight();

		if(nTotal_x < nCommentSize_x)
			nTotal_x = nCommentSize_x;
	}

	if(m_pIcon)
	{
		int _TotalXx_helf = nTotal_x/2;
		int _IconSize_xHelf = nIconSize_x/2;

		m_nIconRenderPos_x = m_nBase_X + (_TotalXx_helf - _IconSize_xHelf) + m_pIcon->GetOffsetX() + m_nFrameGap;
		m_nIconRenderPos_y = m_nBase_Y + m_nFrameGap + m_pIcon->GetOffsetY();

		nTotal_y += nIconSize_y + m_nItemGap;
	}
	
	if(m_pTitle)
	{
		int _TotalXx_helf = nTotal_x/2;
		int _TitleX_helf = nTitleSize_x/2;

		m_pTitle->Set_x(m_nBase_X + (_TotalXx_helf - _TitleX_helf));
		m_pTitle->Set_y(nTotal_y);
		m_pTitle->Parsing();

		nTotal_y += nTitleSize_y + m_nItemGap;
	}

	if(m_pSubTitle)
	{
		m_pSubTitle->Set_x(m_nBase_X + m_nFrameGap);
		m_pSubTitle->Set_y(nTotal_y);
		m_pSubTitle->Parsing();

		nTotal_y += nSubTitleSize_y + m_nItemGap;
	}

	if(m_pComment)
	{
		m_pComment->Set_x(m_nBase_X + m_nFrameGap);
		m_pComment->Set_y(nTotal_y);
		m_pComment->Parsing();

		nTotal_y += nCommentSize_y;
	}
	

	if(m_pBackGround)
	{
		m_pBackGround->SetSize( nTotal_x + (m_nFrameGap*2), (nTotal_y + (m_nFrameGap)) - m_nBase_Y );
	}
}

void ToolTip_Base::ParseExtraInfo( ioXMLElement &xElement )
{
	m_nFrameGap = xElement.GetIntAttribute_e("FrameGap");
	m_nItemGap = xElement.GetIntAttribute_e("ItemGap");
	m_nTitleWidth = xElement.GetIntAttribute_e("TitleWidth");
	m_nSubTitleWidth = xElement.GetIntAttribute_e("SubTitleWidth");
	m_nCommentWidth = xElement.GetIntAttribute_e("CommentWidth");
}

void ToolTip_Base::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "Back" )
	{
		SAFEDELETE( m_pBackGround );
		m_pBackGround = pFrame;
		m_pBackGround->SetShow(false);
	}
	else
	{
		ioWnd::AddRenderFrame(szType, pFrame);
	}	
}

void ToolTip_Base::OnRender()
{
	GoToTopWnd();

	if( m_bFixedPos )
	{
		m_nBase_X = m_iFixedPosX;
		m_nBase_Y = m_iFixedPosY;
	}
	else
	{
		m_nBase_X = g_App.GetMouse()->GetMousePos().x;
		m_nBase_Y = g_App.GetMouse()->GetMousePos().y;
	}

	CalculatorPos();

	if(m_pBackGround)
	{
		int _x = m_nBase_X + m_pBackGround->GetSize().x;
		if(m_nBase_X + m_pBackGround->GetSize().x > g_App.GetCurWidth())
		{
			if( !m_bReverse )
				MoveLeftWnd();
		}
		else if( m_bReverse )
		{
			MoveLeftWnd();
		}


		int _y = m_nBase_Y + m_pBackGround->GetSize().y;
		int iGapY = (m_nBase_Y + m_pBackGround->GetSize().y) - g_App.GetCurHeight();
		if( iGapY > 0 )
		{
			MoveUpWnd( iGapY );
		}
	}

	if(m_pBackGround)
	{
		m_pBackGround->Render(m_nBase_X, m_nBase_Y);
	}

	if(m_pIcon)
	{
		m_pIcon->Render(m_nIconRenderPos_x, m_nIconRenderPos_y);
	}

	if(m_pTitle)
	{
		m_pTitle->OnRender();
	}

	if(m_pSubTitle)
	{
		m_pSubTitle->OnRender();
	}

	if(m_pComment)
	{
		m_pComment->OnRender();
	}
}

void ToolTip_Base::MoveLeftWnd()
{
	int _x = 0;

	if(m_pBackGround)
	{
		_x = m_pBackGround->GetSize().x;
	}

	m_nBase_X -= _x;
	m_nIconRenderPos_x -= _x;
	
	if(m_pTitle)
	{
		m_pTitle->Set_x(m_pTitle->Get_x() - _x);
		m_pTitle->Parsing();
	}

	if(m_pSubTitle)
	{
		m_pSubTitle->Set_x(m_pSubTitle->Get_x() - _x);
		m_pSubTitle->Parsing();
	}

	if(m_pComment)
	{
		m_pComment->Set_x(m_pComment->Get_x() - _x);
		m_pComment->Parsing();
	}
}

void ToolTip_Base::MoveUpWnd()
{
	int _y = 0;

	if(m_pBackGround)
	{
		_y = m_pBackGround->GetSize().y;
	}

	m_nBase_Y -= _y;
	m_nIconRenderPos_y -= _y;

	if(m_pTitle)
	{
		m_pTitle->Set_y(m_pTitle->Get_y() - _y);
		m_pTitle->Parsing();
	}

	if(m_pSubTitle)
	{
		m_pSubTitle->Set_y(m_pSubTitle->Get_y() - _y);
		m_pSubTitle->Parsing();
	}

	if(m_pComment)
	{
		m_pComment->Set_y(m_pComment->Get_y() - _y);
		m_pComment->Parsing();
	}
}

void ToolTip_Base::MoveUpWnd( int iGapY )
{
	int _y = 0;

	if(m_pBackGround)
	{
		_y = m_pBackGround->GetSize().y;
	}

	//m_nBase_Y -= _y;
	//m_nIconRenderPos_y -= _y;

	m_nBase_Y -= iGapY;
	m_nIconRenderPos_y -= iGapY;

	if(m_pTitle)
	{
		//m_pTitle->Set_y(m_pTitle->Get_y() - _y);
		m_pTitle->Set_y(m_pTitle->Get_y() - iGapY);
		m_pTitle->Parsing();
	}

	if(m_pSubTitle)
	{
		//m_pSubTitle->Set_y(m_pSubTitle->Get_y() - _y);
		m_pSubTitle->Set_y(m_pSubTitle->Get_y() - iGapY);
		m_pSubTitle->Parsing();
	}

	if(m_pComment)
	{
		//m_pComment->Set_y(m_pComment->Get_y() - _y);
		m_pComment->Set_y(m_pComment->Get_y() - iGapY);
		m_pComment->Parsing();
	}
}

void ToolTip_Base::SetFixedPos( int iPosX, int iPosY )
{
	m_iFixedPosX = iPosX;
	m_iFixedPosY = iPosY;
}

IoString* ToolTip_Base::GetString(TOOLTIP_CONTENTS_TYPE eType)
{
    auto iter = m_kToolTipStringMap.find(eType);
    if (iter != m_kToolTipStringMap.end())
    {
        return iter->second;
    }

    return NULL;
}

void ToolTip_Base::SetStringSize(TOOLTIP_CONTENTS_TYPE eType, float fSize)
{
    auto pString = GetString(eType);
    if (pString)
    {
        pString->SetTextSize(fSize);
    }
}