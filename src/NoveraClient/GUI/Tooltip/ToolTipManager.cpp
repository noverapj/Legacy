#include <stdafx.h>

#include "ToolTipManager.h"

template<> IoToolTipManager* Singleton< IoToolTipManager >::ms_Singleton = 0;

IoToolTipManager::IoToolTipManager()
{
	m_pCurrentTip = NULL;
}

IoToolTipManager::~IoToolTipManager()
{
	m_pCurrentTip = NULL;
}

ToolTip_Base* IoToolTipManager::GetToolTip( eToolTip_Type eType )
{
	std::map<eToolTip_Type, ToolTip_Base*>::iterator it = m_mapTooltip.find(eType);
	if(it == m_mapTooltip.end())
	{
		switch(eType)
		{
		case eToolTip_Base:
			{
				ToolTip_Base* pWnd = (ToolTip_Base*)g_GUIMgr.AddWnd( "XML/ToolTip/ToolTip_Base.xml" );
				if(pWnd)
				{
					m_mapTooltip.insert(std::pair<eToolTip_Type, ToolTip_Base*>(eType, pWnd));
				}
				return pWnd;
			}
		}
	}
	return (ToolTip_Base*)it->second;
}

bool IoToolTipManager::OpenToolTip( eToolTip_Type eType, const char* szIconName, const char* szTitle, const char* szSubTitle, const char* szComment, bool bFixedPos /*= false */, int iFixedPosX /*= 0*/, int iFixedPosY /*= 0*/, bool bReverse /*=false*/ )
{
	ToolTip_Base* pTip = GetToolTip(eType);
	if(!pTip) return false;

	switch(eType)
	{
	case eToolTip_Base:
		pTip->SetIcon(szIconName);
		pTip->SetTitle(szTitle);
		pTip->SetSubTitle(szSubTitle);
		pTip->SetComment(szComment);
		pTip->OnShow();
		pTip->SetFixed(bFixedPos);
		pTip->SetFixedPos(iFixedPosX, iFixedPosY);
		pTip->SetReverse( bReverse );
		return true;
	}
	return false;
}

bool IoToolTipManager::OpenToolTip(eToolTip_Type eType, ToolTipData& kData)
{
    if (OpenToolTip(eType
                  , kData.GetIconName()
                  , kData.GetString(TTCT_TITLE)
                  , kData.GetString(TTCT_SUBTITLE)
                  , kData.GetString(TTCT_COMMENT)
                  , kData.GetFixedPos()
                  , kData.GetFixedPosX()
                  , kData.GetFixedPosY()))
    {
        auto pTip = GetToolTip(eType);
        if (pTip)
        {
            pTip->SetStringSize(TTCT_TITLE, kData.GetSize(TTCT_TITLE));
            pTip->SetStringSize(TTCT_SUBTITLE, kData.GetSize(TTCT_SUBTITLE));
            pTip->SetStringSize(TTCT_COMMENT, kData.GetSize(TTCT_COMMENT));
        }
        return true;
    }

    return false;
}

void IoToolTipManager::SetCurrentToolTip( ioWnd* pWnd )
{
	m_pCurrentTip = pWnd;
}

void IoToolTipManager::Update()
{
	if(m_pCurrentTip)
	{
		g_App.GetMouse()->SetToolTipCursor();
	}
}

void IoToolTipManager::CloseToolTip()
{
	if(m_pCurrentTip)
		m_pCurrentTip->HideWnd();

	m_pCurrentTip = NULL;
}

void IoToolTipManager::Release()
{
	m_mapTooltip.clear();
}

void IoToolTipManager::InitToolTipMgr()
{
	g_GUIMgr.RegistToolTipID(TOOLTIP_BASE);
}

void IoToolTipManager::SetTitleColor( eToolTip_Type eType, DWORD dwTextColor, DWORD dwBkColor )
{
	ToolTip_Base* pTip = GetToolTip(eType);
	if(!pTip) return;

	pTip->SetTitleColor( dwTextColor, dwBkColor );
}

void IoToolTipManager::SetSubTitleColor( eToolTip_Type eType, DWORD dwTextColor, DWORD dwBkColor )
{
	ToolTip_Base* pTip = GetToolTip(eType);
	if(!pTip) return;

	pTip->SetSubTitleColor( dwTextColor, dwBkColor );
}

void IoToolTipManager::SetCommentColor( eToolTip_Type eType, DWORD dwTextColor, DWORD dwBkColor )
{
	ToolTip_Base* pTip = GetToolTip(eType);
	if(!pTip) return;

	pTip->SetCommentColor( dwTextColor, dwBkColor );
}