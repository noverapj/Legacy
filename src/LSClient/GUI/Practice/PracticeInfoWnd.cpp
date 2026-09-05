#include <stdafx.h>
#include "PracticeInfoWnd.h"
#include "../../GameStage/Mode/Practice/IoPracticeManager.h"


PracticeInfoWnd::PracticeInfoWnd()
{
	m_pStarLine = NULL;
	m_pStarYellow = NULL;
	m_pStarGray = NULL;
	m_pTimeSec = NULL;
	m_pTimeSmall = NULL;
	m_pTimeClear = NULL;
	m_pTimeEa = NULL;
	m_pTimeClear2 = NULL;
	m_iStar = 0;
	m_iRenderTextType = 0;
//	InitShowAniState();
}

PracticeInfoWnd::~PracticeInfoWnd()
{
	SAFEDELETE( m_pStarLine );
	SAFEDELETE( m_pStarYellow );
	SAFEDELETE( m_pStarGray );
	SAFEDELETE( m_pTimeSec );
	SAFEDELETE( m_pTimeSmall );
	SAFEDELETE( m_pTimeClear );
	SAFEDELETE( m_pTimeEa );
	SAFEDELETE( m_pTimeClear2 );
}

void PracticeInfoWnd::iwm_hide()
{
	if( !m_szNoRecordSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szNoRecordSound, 0 );
}

void PracticeInfoWnd::iwm_create()
{
	m_iX = GetXPos();
	m_iY = GetYPos();

	if( COMPARE( m_iX, 0, g_App.GetCurWidth() ) && 
		COMPARE( m_iY, 0, g_App.GetCurHeight() ) )
	{
		SetWndPos( m_iX, m_iY );
	}
}

void PracticeInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Line")
	{
		SAFEDELETE( m_pStarLine );
		m_pStarLine = pImage;
	}
	else if( szType == "StarYellow")
	{
		SAFEDELETE( m_pStarYellow );
		m_pStarYellow = pImage;
	}
	else if( szType == "StarGray")
	{
		SAFEDELETE( m_pStarGray );
		m_pStarGray = pImage;
	}
	else if( szType == "TimeSec")
	{
		SAFEDELETE( m_pTimeSec );
		m_pTimeSec = pImage;
	}
	else if( szType == "TimeSmall")
	{
		SAFEDELETE( m_pTimeSmall );
		m_pTimeSmall = pImage;
	}
	else if( szType == "TimeClear")
	{
		SAFEDELETE( m_pTimeClear );
		m_pTimeClear = pImage;
	}
	else if( szType == "TimeEa")
	{
		SAFEDELETE( m_pTimeEa );
		m_pTimeEa = pImage;
	}
	else if( szType == "TimeClear2")
	{
		SAFEDELETE( m_pTimeClear2 );
		m_pTimeClear2 = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PracticeInfoWnd::ParseExtraInfo( ioXMLElement &xElement )
{

	m_szNoRecordSound = xElement.GetStringAttribute("NoRecordSound");
	m_szRank = xElement.GetStringAttribute_e("TextInfo1");
	m_szName = xElement.GetStringAttribute_e("TextInfo2");
}

void PracticeInfoWnd::SetPracticeInfo( int iPracticeIndex )
{
	m_iRenderTextType = iPracticeIndex;
}

void PracticeInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

//	UpdateShowAni();
	ioWnd::OnProcess( fTimePerSec );
}

void PracticeInfoWnd::OnRender()
{

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if(m_iRenderTextType > 0)
	{
		RenderStar( iXPos, iYPos );
		RenderSecond( iXPos, iYPos );
		RenderRanker( iXPos, iYPos );

		m_pStarLine->Render( iXPos, iYPos+19, UI_RENDER_NORMAL );
		m_pStarLine->Render( iXPos, iYPos+57, UI_RENDER_ADD_ALPHA );//알파 70으로
		m_pStarLine->Render( iXPos, iYPos+95, UI_RENDER_ADD_ALPHA );
		m_pStarLine->Render( iXPos, iYPos+133, UI_RENDER_NORMAL );
	}

}


void PracticeInfoWnd::iwm_show()
{
	int iIndex = g_PracticeMgr.GetPreIndex();
	m_iGradeA = g_PracticeMgr.GetPracticeGradeA(iIndex);
	m_iGradeB = g_PracticeMgr.GetPracticeGradeB(iIndex);
	m_iGradeC = g_PracticeMgr.GetPracticeGradeC(iIndex);

	for(int i = 0; i < 3; ++i)
	{
		m_szRanker[i] = g_PracticeMgr.GetRankerInfo(i).szName;
		m_szTime[i] = g_PracticeMgr.GetRankerInfo(i).szTime;
		if( m_szRanker[i].IsEmpty() )
			m_szRanker[i] = m_szName;
	}

}

void PracticeInfoWnd::RenderStar( int iXPos, int iYPos )
{

		m_pStarGray->Render( iXPos-4, iYPos+29 );		
		m_pStarGray->Render( iXPos+19, iYPos+29 );
		m_pStarGray->Render( iXPos+39, iYPos+29 );

		m_pStarGray->Render( iXPos+6, iYPos+66 );
		m_pStarGray->Render( iXPos+26, iYPos+66 );

		m_pStarGray->Render( iXPos+16, iYPos+104 );

		if(m_iStar > 2)
		{
			m_pStarYellow->Render( iXPos-4, iYPos+29 );		
			m_pStarYellow->Render( iXPos+19, iYPos+29 );
			m_pStarYellow->Render( iXPos+39, iYPos+29 );
		}
		if(m_iStar > 1)
		{
			m_pStarYellow->Render( iXPos+6, iYPos+66 );
			m_pStarYellow->Render( iXPos+26, iYPos+66 );
		}
		if(m_iStar > 0)
			m_pStarYellow->Render( iXPos+16, iYPos+104 );

		
}

void PracticeInfoWnd::RenderSecond( int iXPos, int iYPos )
{

	m_pTimeSmall->RenderNum( iXPos+65, iYPos+30, m_iGradeA/10  );
	m_pTimeSmall->RenderNum( iXPos+75, iYPos+30, m_iGradeA%10 );
	
	m_pTimeSmall->RenderNum( iXPos+56, iYPos+67, m_iGradeB/10 );
	m_pTimeSmall->RenderNum( iXPos+66, iYPos+67, m_iGradeB%10 );
	
	if(m_iGradeC/100 != 0)
		m_pTimeSmall->RenderNum( iXPos+40, iYPos+106, m_iGradeC/100  );
	m_pTimeSmall->RenderNum( iXPos+50, iYPos+106, (m_iGradeC/10)%10 );
	m_pTimeSmall->RenderNum( iXPos+60, iYPos+106, m_iGradeC%10 );

	if(m_iRenderTextType == 2)
	{
		m_pTimeSec->Render( iXPos+85, iYPos+30, UI_RENDER_NORMAL );
		m_pTimeClear->Render( iXPos+107, iYPos+33, UI_RENDER_NORMAL );
		m_pTimeSec->Render( iXPos+76, iYPos+67, UI_RENDER_NORMAL );
		m_pTimeClear->Render( iXPos+98, iYPos+70, UI_RENDER_NORMAL );
		m_pTimeSec->Render( iXPos+69, iYPos+105, UI_RENDER_NORMAL );
		m_pTimeClear->Render( iXPos+90, iYPos+108, UI_RENDER_NORMAL );
	}
	else if(m_iRenderTextType == 1)
	{
		if(m_pTimeClear2 && m_pTimeEa)
		{
			m_pTimeEa->Render( iXPos+85, iYPos+30, UI_RENDER_NORMAL );
			m_pTimeClear2->Render( iXPos+107, iYPos+33, UI_RENDER_NORMAL );
			m_pTimeEa->Render( iXPos+76, iYPos+67, UI_RENDER_NORMAL );
			m_pTimeClear2->Render( iXPos+98, iYPos+70, UI_RENDER_NORMAL );
			m_pTimeEa->Render( iXPos+69, iYPos+105, UI_RENDER_NORMAL );
			m_pTimeClear2->Render( iXPos+90, iYPos+108, UI_RENDER_NORMAL );
		}
	}
	
}

void PracticeInfoWnd::RenderRanker( int iXPos, int iYPos )
{
	IoString str;
	char szBuf[MAX_PATH];
	
	for(int i = 0; i < 3; ++i)
	{
		wsprintf( szBuf, m_szRank.c_str(), i+1 );
		str.Set(0, 0, szBuf, FONT_SIZE_13, 30, TS_OUTLINE_2X, TCT_DEFAULT_WHITE, 0, TAT_LEFT, TVA_TOP, 0);
		str.OnRender( iXPos + 16, iYPos + 183+i*20 );
		str.Set(0, 0, m_szRanker[i].c_str(), FONT_SIZE_13, 300, TS_OUTLINE_2X, TCT_DEFAULT_WHITE, 0, TAT_LEFT, TVA_TOP, 0);
		str.OnRender( iXPos + 102, iYPos + 183+i*20 );
		str.Set(0, 0, m_szTime[i].c_str(), FONT_SIZE_13, 80, TS_OUTLINE_2X,0x0199FE , 0, TAT_LEFT, TVA_TOP, 0);
		str.OnRender( iXPos + 42, iYPos + 183+i*20 );
	}
}

void PracticeInfoWnd::SetStar( int iStar )
{
	if( m_iStar == iStar )
		return;

	if( m_iStar > iStar )
	{
		g_SoundMgr.PlaySound( m_szNoRecordSound );
	}

	m_iStar = iStar;
}
