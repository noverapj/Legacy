#include "stdafx.h"

#include "PriateRulletWnd.h"
#include "GlobalWnd.h"

#include "../ioPriateRullet.h"
#include "../io3DEngine/ioUI3DEffectRender.h"

PriateNewStartButton::PriateNewStartButton()
{
	Clear();
}

PriateNewStartButton::~PriateNewStartButton()
{
}

void PriateNewStartButton::Clear()
{
	m_dwBlinkAnimationStart	= 0;
	m_nBlinkCount			= 0;
}

void PriateNewStartButton::iwm_show()
{
}

void PriateNewStartButton::iwm_hide()
{
}

void PriateNewStartButton::OnProcess( float fTimePerSec )
{
	ProcessBlinkAnimation();
}

void PriateNewStartButton::BlinkAnimationStart()
{
	m_dwBlinkAnimationStart	= FRAMEGETTIME();
	m_nBlinkCount			= 0;
}

void PriateNewStartButton::ProcessBlinkAnimation()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwBlinkAnimationStart && m_dwBlinkAnimationStart + 100 < dwCurTime )
	{
		m_nBlinkCount++;
		m_dwBlinkAnimationStart = dwCurTime;
	}
}
void PriateNewStartButton::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_nBlinkCount % 2 != 0 )
	{
		OnDrawOvered( iXPos, iYPos );
		PrintOverTitle( iXPos, iYPos );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SwordFrameButton::SwordFrameButton() : m_Index( 0 ), m_Count( 0 ), m_dwSAlphaAnimationStart( 0 ), m_fCurrAlphaRate( FLOAT1 )
{
	m_pChecked					= NULL;
	m_pSelectFrame				= NULL;
}

SwordFrameButton::~SwordFrameButton()
{
	SAFEDELETE( m_pChecked );
	SAFEDELETE( m_pSelectFrame );
}

void SwordFrameButton::iwm_show()
{
}

void SwordFrameButton::iwm_hide()
{
}

void SwordFrameButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Checked" )
	{
		SAFEDELETE( m_pChecked );
		m_pChecked = pImage;
	}
	else
	{
		ioRadioButton::AddRenderImage( szType, pImage );
	}
}

void SwordFrameButton::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "SelectFrame" )
	{
		SAFEDELETE( m_pSelectFrame );
		m_pSelectFrame = pFrame;
	}
	else
	{
		ioRadioButton::AddRenderFrame( szType, pFrame );
	}
}

void SwordFrameButton::SetSwordInfo( int Index )
{
	m_Index				= Index;
	m_Count				= 0;
	m_fCurrAlphaRate	= 1.0;
	ShowWnd();
}

void SwordFrameButton::StartAlphaAnimation()
{
	m_dwSAlphaAnimationStart = FRAMEGETTIME();
	m_fCurrAlphaRate		 = 0.0f;
}

void SwordFrameButton::PorcessAlphaAnimation()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwSAlphaAnimationStart )
	{
		if( m_dwSAlphaAnimationStart + 400 < dwCurTime )
		{
			m_fCurrAlphaRate			= FLOAT1;
			m_dwSAlphaAnimationStart	= 0;
		}
		else
		{
			m_fCurrAlphaRate = FLOAT1 - (((float)dwCurTime - m_dwSAlphaAnimationStart) / (float)400);
			m_fCurrAlphaRate = max( 0.0f, m_fCurrAlphaRate );
		}
	}
}

void SwordFrameButton::OnProcess( float fTimePerSec )
{
	ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
	if( pRullet )
	{
		m_Count = pRullet->GetSwordCount( m_Index );
		if( m_Count == 0 && !HasWndStyle( IWS_INACTIVE ) )
		{
			SetInActive();
		}
		else if( m_Count > 0 && HasWndStyle( IWS_INACTIVE ) )
		{
			SetActive();
		}
	}

	PorcessAlphaAnimation();
}

void SwordFrameButton::OnDrawPushed( int iXPos, int iYPos )
{
	ioRadioButton::OnDrawPushed( iXPos, iYPos );

	if( m_pChecked )
	{
		m_pChecked->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurrAlphaRate );
		m_pChecked->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_pSelectFrame )
	{
		m_pSelectFrame->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurrAlphaRate );
		m_pSelectFrame->Render( iXPos, iYPos, UI_RENDER_NORMAL );
	}
}

void SwordFrameButton::OnRender()
{
	ioRadioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( !HasWndStyle( IWS_INACTIVE ) )
	{
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + 39, iYPos + 103, FONT_SIZE_12, "%d개", m_Count );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PrivateRulletRewardButton::PrivateRulletRewardButton() :
	m_CurrMax( 0 ),
	m_CurrMin( 0 ),
	m_CurrHP( 0 ),
	m_dwStampScaleAnimationStart( 0 ),
	m_dwStampBlinkAnimationStart( 0 ),
	m_fScaleRate( 0.0f ),
	m_fCurrScaleRate( 0.0f ),
	m_dwSAlphaAnimationStart( 0 ),
	m_fCurrAlphaRate( FLOAT1 ),
	m_bReward( false ),
	m_bGray( false ) ,
	m_bHidden( false )
{
	m_pIcon		= NULL;
	m_pSubIcon = NULL;
	m_pStamp	= NULL;
	m_pSecret	= NULL;
}

PrivateRulletRewardButton::~PrivateRulletRewardButton()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pStamp );
	SAFEDELETE( m_pSecret );
}

void PrivateRulletRewardButton::iwm_show()
{

}

void PrivateRulletRewardButton::iwm_hide()
{
	m_dwSAlphaAnimationStart	 = 0;
	m_dwStampScaleAnimationStart = 0;
	m_dwStampBlinkAnimationStart = 0;
	m_nStampBlinkCount			 = 0;
	m_fCurrAlphaRate			 = FLOAT1;
	m_fCurrScaleRate			 = FLOAT1;
}

void PrivateRulletRewardButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "StampMark" )
	{
		SAFEDELETE( m_pStamp );
		m_pStamp = pImage;
	}
	else if( szType == "Secret" )
	{
		SAFEDELETE( m_pSecret );
		m_pSecret = pImage;
		if( m_pSecret )
			m_pSecret->SetScale( 0.62f, 0.62f );
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void PrivateRulletRewardButton::SetRewardInfo( int Index, int eRewardType )
{
	ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
	if( pRullet == NULL )
	{
		HideWnd();
		return;
	}

	m_dwStampBlinkAnimationStart	= 0;
	m_nStampBlinkCount				= 0;
	m_fCurrAlphaRate				= FLOAT1;
	m_dwSAlphaAnimationStart		= 0;
	m_dwStampScaleAnimationStart	= 0;

	PriateRulletReward* pInfo = NULL;
	switch( eRewardType )
	{
	case PriateRulletReward::RT_REWARD:
		{
			pInfo = pRullet->GetReward( Index );
			if( pInfo == NULL )
				break;

			m_CurrMax = pInfo->m_nMax;
			m_CurrMin = pInfo->m_nMin;
			m_bHidden = pInfo->m_bHidden;

			if( pRullet->GetCurrHP() <= m_CurrMax )
			{
				m_bGray				= true;
				m_bReward			= true;
				m_fCurrScaleRate	= FLOAT1;
			}
			else
			{
				m_bGray				= false;
				m_bReward			= false;
				m_fCurrScaleRate	= 0.0f;
			}
		}
		break;
	case PriateRulletReward::RT_BONUS_REWARD:
		{
			pInfo = pRullet->GetCurrBounsReward();
			if( pInfo == NULL )
				break;

			m_CurrMax = pInfo->m_nMax;
			m_CurrMin = pInfo->m_nMin;
			m_bHidden = pInfo->m_bHidden;

			if( pRullet->GetCurrHP() <= 0 && m_CurrMin <= pRullet->GetRulletStickedCount() && pRullet->GetRulletStickedCount() <= m_CurrMax )
			{
				m_bGray				= true;
				m_bReward			= true;
				m_bHidden			= false;
				m_fCurrScaleRate	= FLOAT1;
			}
			else
			{
				m_bGray				= false;
				m_bReward			= false;
				m_fCurrScaleRate	= 0.0f;
			}
		}
		break;
	default:
		{
			HideWnd();
			return;
		}
		break;
	}

	if( pInfo == NULL )
	{
		HideWnd();
		return;
	}

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pInfo->m_Icon );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pInfo->m_SubIcon );
	if( m_pIcon == NULL )
	{
		HideWnd();
		return;
	}

	if( m_pIcon )
		m_pIcon->SetScale( 0.62f, 0.62f );
	if( m_pSubIcon )
		m_pSubIcon->SetScale( 0.62f, 0.62f );

	m_Name = g_PresentMgr.GetPresentValue1Text( pInfo->m_eType, pInfo->m_nVaule1, pInfo->m_nVaule2 );
	ShowWnd();
}

bool PrivateRulletRewardButton::IsEnableStampAnimation( int nHP )
{
	if( !m_bReward && nHP <= m_CurrMax )
		return true;

	return false;
}

void PrivateRulletRewardButton::StampAnimationStart( int nRewardCount )
{
	if( 0 >= nRewardCount )
		return;

	m_fCurrScaleRate	= FLOAT1;
	m_bGray				= true;

	if( 1 < nRewardCount )
	{
		m_dwStampScaleAnimationStart	= FRAMEGETTIME();
		m_fScaleRate					= 0.3f;

		ProcessStampAnimation();
		
	}
	else
	{
		m_dwStampScaleAnimationStart	= FRAMEGETTIME();
		m_fScaleRate					= FLOAT1;

		ProcessStampAnimation();
	}
}

void PrivateRulletRewardButton::ProcessStampAnimation()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwStampScaleAnimationStart )
	{
		if( m_dwStampScaleAnimationStart + 200 < dwCurTime )
		{
			m_dwStampScaleAnimationStart	= 0;
			m_fCurrScaleRate				= FLOAT1;
			m_bReward						= true;
		}
		else
		{
			float fRate = FLOAT1 - ((float)(dwCurTime - m_dwStampScaleAnimationStart)) / (float)200;
			m_fCurrScaleRate = FLOAT1 + (m_fScaleRate * fRate);
		}
	}
}

bool PrivateRulletRewardButton::IsEnableStampBlinkAnimation()
{
	if( m_bReward )
		return true;

	return false;
}

void PrivateRulletRewardButton::StampBlinkAnimationStart()
{
	m_dwStampBlinkAnimationStart	= FRAMEGETTIME();
	m_nStampBlinkCount				= 0;
}

void PrivateRulletRewardButton::ProcessBlinkAnimation()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwStampBlinkAnimationStart && m_dwStampBlinkAnimationStart + 100 < dwCurTime )
	{
		m_nStampBlinkCount++;
		if( m_nStampBlinkCount < 10 )
		{	
			m_dwStampBlinkAnimationStart = dwCurTime;
		}
		else
		{
			m_dwStampBlinkAnimationStart	= 0;
			m_nStampBlinkCount				= 0;
		}
	}
}

void PrivateRulletRewardButton::StartAlphaAnimation()
{
	m_dwSAlphaAnimationStart	 = FRAMEGETTIME();
	m_fCurrAlphaRate			 = 0.0f;
	m_dwStampScaleAnimationStart = 0;
	m_fCurrScaleRate			 = FLOAT1;
	m_dwStampBlinkAnimationStart = 0;
	m_nStampBlinkCount			 = 0;
}

void PrivateRulletRewardButton::PorcessAlphaAnimation()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwSAlphaAnimationStart )
	{
		if( m_dwSAlphaAnimationStart + 400 < dwCurTime )
		{
			m_fCurrAlphaRate			= FLOAT1;
			m_dwSAlphaAnimationStart	= 0;
		}
		else
		{
			m_fCurrAlphaRate = FLOAT1- (((float)dwCurTime - m_dwSAlphaAnimationStart) / (float)400 );
			m_fCurrAlphaRate = max( 0.0f, m_fCurrAlphaRate );
		}
	}
}

void PrivateRulletRewardButton::OnProcess( float fTimePerSec )
{
	ProcessStampAnimation();
	ProcessBlinkAnimation();
	PorcessAlphaAnimation();
}

void PrivateRulletRewardButton::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pIcon )
	{
		if( 0 < m_dwSAlphaAnimationStart )
		{
			if( m_bHidden )
			{
				m_pSecret->SetAlpha( MAX_ALPHA_RATE );
				m_pSecret->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
			else
			{
				m_pIcon->SetAlpha( MAX_ALPHA_RATE );
				m_pIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );

				if( m_pSubIcon )
				{
					m_pSubIcon->SetAlpha( MAX_ALPHA_RATE );
					m_pSubIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );
				}
			}

			if( m_bGray )
			{
				m_pIcon->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurrAlphaRate );
				m_pIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_GRAY, TFO_BILINEAR );

				if( m_pSubIcon )
				{
					m_pSubIcon->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurrAlphaRate );
					m_pSubIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_GRAY, TFO_BILINEAR );
				}
			}
		}
		else
		{
			if( m_bGray )
			{
				m_pIcon->SetAlpha( MAX_ALPHA_RATE );
				m_pIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_GRAY, TFO_BILINEAR );

				if( m_pSubIcon )
				{
					m_pSubIcon->SetAlpha( MAX_ALPHA_RATE );
					m_pSubIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_GRAY, TFO_BILINEAR );
				}
			}
			else
			{
				if( m_bHidden )
				{
					m_pSecret->SetAlpha( MAX_ALPHA_RATE );
					m_pSecret->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );
				}
				else
				{
					m_pIcon->SetAlpha( MAX_ALPHA_RATE );
					m_pIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );

					if( m_pSubIcon )
					{
						m_pSubIcon->SetAlpha( MAX_ALPHA_RATE );
						m_pSubIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );
					}
				}
			}
		}
	}
	
	if( 0 < m_dwStampBlinkAnimationStart )
	{	
		if( m_pStamp )
		{
			m_pStamp->SetAlpha( MAX_ALPHA_RATE );
			m_pStamp->SetScale( FLOAT1, FLOAT1 );
			m_pStamp->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );
			if( m_nStampBlinkCount % 2 != 0 )
			{
				m_pStamp->SetAlpha( (float)MAX_ALPHA_RATE * 0.60f );
				m_pStamp->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_SCREEN, TFO_BILINEAR );
			}
		}
	}
	else if( 0 < m_dwSAlphaAnimationStart && m_bReward )
	{		
		m_pStamp->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurrAlphaRate );
		m_pStamp->SetScale( FLOAT1, FLOAT1 );
		m_pStamp->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( 0 < m_dwStampScaleAnimationStart || m_bReward )
	{
		if( m_pStamp )
		{
			m_pStamp->SetAlpha( MAX_ALPHA_RATE );
			m_pStamp->SetScale( m_fCurrScaleRate, m_fCurrScaleRate );
			m_pStamp->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	if( m_pIcon && IsOvered() )
	{
		m_pIcon->SetAlpha( MAX_ALPHA_RATE );
		m_pIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_SCREEN );

		if( m_pSubIcon )
		{
			m_pSubIcon->SetAlpha( MAX_ALPHA_RATE );
			m_pSubIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

OakHoleButton::OakHoleButton() : 
	m_Index( 0 ),
	m_bSticked( false ),
	m_StickedSwordCode( 0 ),
	m_AnimationType( AT_NONE ),
	m_dwAnimationTime( 0 ),
	m_eCritical( OCT_NONE ),
	m_dwSAlphaAnimationStart( 0 ),
	m_fCurrAlphaRate( FLOAT1 )
{
	m_OverSword				= NULL;

	m_GoldClickSword		= NULL;
	m_GoldClickMoveSword	= NULL;
	m_GoldStickedSword		= NULL;

	m_SilverClickSword		= NULL;
	m_SilverClickMoveSword	= NULL;
	m_SilverStickedSword	= NULL;
		
	m_WoodClickSword		= NULL;
	m_WoodClickMoveSword	= NULL;
	m_WoodStickedSword		= NULL;
}

OakHoleButton::~OakHoleButton()
{
	SAFEDELETE( m_OverSword );

	SAFEDELETE( m_GoldClickSword );
	SAFEDELETE( m_GoldClickMoveSword );
	SAFEDELETE( m_GoldStickedSword );

	SAFEDELETE( m_SilverClickSword );
	SAFEDELETE( m_SilverClickMoveSword );
	SAFEDELETE( m_SilverStickedSword );

	SAFEDELETE( m_WoodClickSword );
	SAFEDELETE( m_WoodClickMoveSword );
	SAFEDELETE( m_WoodStickedSword );
}

void OakHoleButton::iwm_show()
{

}

void OakHoleButton::iwm_hide()
{

}

void OakHoleButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "OverSword" )
	{
		SAFEDELETE( m_OverSword );
		m_OverSword = pImage;
	}
	else if( szType == "GoldClickSword" )
	{
		SAFEDELETE( m_GoldClickSword );
		m_GoldClickSword = pImage;
	}
	else if( szType =="GoldClickMoveSword" )
	{
		SAFEDELETE( m_GoldClickMoveSword );
		m_GoldClickMoveSword = pImage;
	}
	else if( szType == "GoldStickedSword" )
	{
		SAFEDELETE( m_GoldStickedSword );
		m_GoldStickedSword = pImage;
	}
	else if( szType == "SilverClickSword" )
	{
		SAFEDELETE( m_SilverClickSword );
		m_SilverClickSword = pImage;
	}
	else if( szType =="SilverClickMoveSword" )
	{
		SAFEDELETE( m_SilverClickMoveSword );
		m_SilverClickMoveSword = pImage;
	}
	else if( szType == "SilverStickedSword" )
	{
		SAFEDELETE( m_SilverStickedSword );
		m_SilverStickedSword = pImage;
	}
	else if( szType == "WoodClickSword" )
	{
		SAFEDELETE( m_WoodClickSword );
		m_WoodClickSword = pImage;
	}
	else if( szType =="WoodClickMoveSword" )
	{
		SAFEDELETE( m_WoodClickMoveSword );
		m_WoodClickMoveSword = pImage;
	}
	else if( szType == "WoodStickedSword" )
	{
		SAFEDELETE( m_WoodStickedSword );
		m_WoodStickedSword = pImage;
	}
	else if( szType == "WoodStickedSword" )
	{
		SAFEDELETE( m_WoodStickedSword );
		m_WoodStickedSword = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void OakHoleButton::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	m_StickedWoodSound			= xElement.GetStringAttribute_e( "StickedWoodSound" );
	m_StickedMetalSound			= xElement.GetStringAttribute_e( "StickedMetalSound" );
	m_StickedCriticalWoodSound	= xElement.GetStringAttribute_e( "StickedCriticalWoodSound" );
	m_StickedCriticalMetalSound	= xElement.GetStringAttribute_e( "StickedCriticalMetalSound" );
	m_DamageSound				= xElement.GetStringAttribute_e( "DamageSound" );
	m_CriticalDamageSound		= xElement.GetStringAttribute_e( "CriticalDamageSound" );
	m_EndDamageSound			= xElement.GetStringAttribute_e( "EndDamageSound" );
	m_nEffectXPos				= xElement.GetIntAttribute_e( "EffectXPos" );
	m_nEffectYPos				= xElement.GetIntAttribute_e( "EffectYPos" );
}

int OakHoleButton::GetEffectXPos()
{
	return m_nEffectXPos;
}

int OakHoleButton::GetEffectYPos()
{
	return m_nEffectYPos;
}

void OakHoleButton::SetOakHole( int Index )
{	
	m_Index = Index;
}

void OakHoleButton::AnimationStart( int eCritical )
{
	m_AnimationType		= AT_START;
	m_dwAnimationTime	= FRAMEGETTIME();
	m_eCritical			= OCT_NONE;
}

void OakHoleButton::UpdateStick()
{
	if( 0 < m_dwSAlphaAnimationStart )
		return;

	m_StickedSwordCode	= 0;
	m_bSticked			= false;
		
	ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
	if( pRullet )
	{
		DWORD dwCode = OakSwordTypeToOakCode( static_cast<OakSwordType>( pRullet->GetRulletStickedValue( m_Index )  ) );
		if( COMPARE( dwCode, ioEtcItem::EIT_ETC_OAK_WOOD_SWORD, ioEtcItem::EIT_ETC_OAK_GOLD_SWORD + 1 ) )
		{
			m_StickedSwordCode	= dwCode;
			m_bSticked			= true;
		}
	}
}

void OakHoleButton::StartAlphaAnimation()
{
	m_dwSAlphaAnimationStart = FRAMEGETTIME();
	m_fCurrAlphaRate		 = 0.0f;
}

void OakHoleButton::PorcessAlphaAnimation()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwSAlphaAnimationStart )
	{
		if( m_dwSAlphaAnimationStart + 400 < dwCurTime )
		{
			m_fCurrAlphaRate			= FLOAT1;
			m_dwSAlphaAnimationStart	= 0;
			UpdateStick();
		}
		else
		{
			m_fCurrAlphaRate = FLOAT1 -(((float)dwCurTime - m_dwSAlphaAnimationStart) / (float)400);
			m_fCurrAlphaRate = max( 0.0f, m_fCurrAlphaRate );
		}
	}
}

void OakHoleButton::ProcessStickAnimation()
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_AnimationType )
	{
	case AT_START:
		{
			if( 0 < m_dwAnimationTime && m_dwAnimationTime + 100 < dwCurTime )
			{
				m_dwAnimationTime	= dwCurTime;
				m_AnimationType		= AT_MOVE;
				m_bSticked			= false;
			}
			else
			{
				m_bSticked			= false;
			}
		}
		break;
	case AT_MOVE:
		{
			if( 0 < m_dwAnimationTime && m_dwAnimationTime + 100 < dwCurTime )
			{
				m_dwAnimationTime	= 0;
				m_AnimationType		= AT_NONE;
				m_bSticked			= true;
			}
			else
			{
				m_bSticked = false;

				int nHP = 0;
				ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
				if( pRullet )
					nHP = pRullet->GetCurrHP();

				switch( m_eCritical )
				{
				case OCT_HARF_CRITICAL:
				case OCT_FULL_CRITICAL:
					{
						if( m_StickedSwordCode == ioEtcItem::EIT_ETC_OAK_WOOD_SWORD )
							g_SoundMgr.PlaySound( m_StickedCriticalWoodSound );
						else
							g_SoundMgr.PlaySound( m_StickedCriticalMetalSound );

						if( nHP <= 0 )
							g_SoundMgr.PlaySound( m_EndDamageSound );
						else
							g_SoundMgr.PlaySound( m_CriticalDamageSound );
					}
					break;
				default:
					{
						if( m_StickedSwordCode == ioEtcItem::EIT_ETC_OAK_WOOD_SWORD )
							g_SoundMgr.PlaySound( m_StickedWoodSound );
						else
							g_SoundMgr.PlaySound( m_StickedMetalSound );

						if( nHP <= 0 )
							g_SoundMgr.PlaySound( m_EndDamageSound );
						else
							g_SoundMgr.PlaySound( m_DamageSound );
					}
					break;
				}
			}
		}
		break;
	}
}

void OakHoleButton::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
	
	UpdateStick();
	PorcessAlphaAnimation();
	ProcessStickAnimation();
}

void OakHoleButton::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );
}

void OakHoleButton::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );
	OnRenderOver( iXPos, iYPos );
}

void OakHoleButton::OnRenderOver( int iXPos, int iYPos )
{
	if( m_bSticked )
	{
		switch( m_StickedSwordCode )
		{
		case ioEtcItem::EIT_ETC_OAK_WOOD_SWORD:
			if( m_WoodStickedSword )
				m_WoodStickedSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			break;
		case ioEtcItem::EIT_ETC_OAK_SILVER_SWORD:
			if( m_SilverStickedSword )
				m_SilverStickedSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			break;
		case ioEtcItem::EIT_ETC_OAK_GOLD_SWORD:
			if( m_GoldStickedSword )
				m_GoldStickedSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			break;
		}
	}
	else
	{
		if( m_OverSword && m_AnimationType == AT_NONE )
			m_OverSword->Render( iXPos, iYPos, UI_RENDER_ADD, TFO_BILINEAR );
	}
}

void OakHoleButton::OnRenderPushAnimation( int iXPos, int iYPos )
{
	switch( m_StickedSwordCode )
	{
	case ioEtcItem::EIT_ETC_OAK_WOOD_SWORD:
		{
			switch( m_AnimationType )
			{
			case AT_START:
				if( m_WoodClickSword )
					m_WoodClickSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
				break;
			case AT_MOVE:
				if( m_WoodClickMoveSword )
					m_WoodClickMoveSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
				break;
			}
		}
		break;
	case ioEtcItem::EIT_ETC_OAK_SILVER_SWORD:
		{
			switch( m_AnimationType )
			{
			case AT_START:
				if( m_SilverClickSword )
					m_SilverClickSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
				break;
			case AT_MOVE:
				if( m_SilverClickMoveSword )
					m_SilverClickMoveSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
				break;
			}
		}
		break;
	case ioEtcItem::EIT_ETC_OAK_GOLD_SWORD:
		{
			switch( m_AnimationType )
			{
			case AT_START:
				if( m_GoldClickSword )
					m_GoldClickSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
				break;
			case AT_MOVE:
				if( m_GoldClickMoveSword )
					m_GoldClickMoveSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
				break;
			}
		}
		break;
	}
}

void OakHoleButton::OnStickedRender( int iXPos, int iYPos )
{
	if( m_bSticked )
	{
		switch( m_StickedSwordCode )
		{
		case ioEtcItem::EIT_ETC_OAK_WOOD_SWORD:
			if( m_WoodStickedSword )
			{
				m_WoodStickedSword->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurrAlphaRate );
				m_WoodStickedSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
			break;
		case ioEtcItem::EIT_ETC_OAK_SILVER_SWORD:
			if( m_SilverStickedSword )
			{
				m_SilverStickedSword->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurrAlphaRate );
				m_SilverStickedSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
			break;
		case ioEtcItem::EIT_ETC_OAK_GOLD_SWORD:
			if( m_GoldStickedSword )
			{
				m_GoldStickedSword->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurrAlphaRate );
				m_GoldStickedSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
			break;
		}
	}
}

void OakHoleButton::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	switch( m_AnimationType )
	{
	case AT_START:
	case AT_MOVE:
		OnRenderPushAnimation( iXPos, iYPos );
		break;
	default:
		OnStickedRender( iXPos, iYPos );
		break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PriateRulletWnd::PriateRulletWnd() : 
	m_RulletState( RS_START ),
	m_DevelopKState( DK_NONE ),
	m_bDevelopKBack( false ),
	m_StickedSwordCode( 0 ),
	m_dwPreStartTime( 0 ),
	m_dwPreEndStartTime( 0 ),
	m_dwEndStartTime( 0 ),
	m_dwDevelopKBackTime( 0 ),
	m_CurrHP( 0 ),
	m_CurrFaceIndex( 0 ),
	m_bDevelopKShake( false ),
	m_MonkeyFaceTime( 0 ),
	m_TreeBlinkTime( 0 ),
	m_LeftBombBlinkTime( 0 ),
	m_RightBombBlinkTime( 0 ),
	m_fCurrTreeAlphaRate( 0.0f ),
	m_fLeftCurrBombAlphaRate( 0.0f ),
	m_fRightCurrBombAlphaRate( 0.0f ),
	m_nSwordUseCount( 0 ),
	m_dwSemiCriticalEffectStart( 0 ),
	m_dwCriticalEffectStart( 0 )
{
	m_OakImageCap			= NULL;
	m_OakImageBody			= NULL;
	m_DevelopK				= NULL;
	m_DevelopKBack			= NULL;
	m_DevelopKBackToolTip	= NULL;
	m_DevelopKBackAnimation	= NULL;
	m_RewardMap				= NULL;
	m_SwordBorder			= NULL;
	m_BubbleRight			= NULL;
	m_BubbleLeft			= NULL;
	m_SwordBorderTitle		= NULL;
	m_DevelopKFlyStart		= NULL;
	m_DevelopKFlyEnd		= NULL;

	m_CurrDevelopKFace		= NULL;
	m_DevelopKWounded		= NULL;
	m_DevelopKWoundedBack	= NULL;
	m_DevelopKProvoke		= NULL;
	m_DevelopKProvokeBack	= NULL;
	m_MonKeyFace			= NULL;
	m_ShipTreeLight			= NULL;
	m_ShipTreeInvLight		= NULL;
	m_BombLeftFire			= NULL;
	m_BombLeftInvFire		= NULL;
	m_BombRightFire			= NULL;
	m_BombRightInvFire		= NULL;

	for( int i = 0; i < EFFECT_MAX; ++i )
		m_FireWorkEffect[i] = NULL;

	m_pSemiCriticalEffect		= NULL;
	m_pCriticalEffect			= NULL;
}

PriateRulletWnd::~PriateRulletWnd()
{
	SAFEDELETE( m_OakImageCap );
	SAFEDELETE( m_OakImageBody );
	SAFEDELETE( m_DevelopK );
	SAFEDELETE( m_DevelopKBack );
	SAFEDELETE( m_DevelopKBackToolTip );
	SAFEDELETE( m_DevelopKBackAnimation );
	SAFEDELETE( m_RewardMap );
	SAFEDELETE( m_SwordBorder );
	SAFEDELETE( m_BubbleRight );
	SAFEDELETE( m_BubbleLeft );
	SAFEDELETE( m_SwordBorderTitle );
	SAFEDELETE( m_CurrDevelopKFace );
	SAFEDELETE( m_DevelopKFlyStart );
	SAFEDELETE( m_DevelopKFlyEnd );
	SAFEDELETE( m_DevelopKWounded );
	SAFEDELETE( m_DevelopKWoundedBack );
	SAFEDELETE( m_DevelopKProvoke );
	SAFEDELETE( m_DevelopKProvokeBack );
	SAFEDELETE( m_MonKeyFace );
	SAFEDELETE( m_ShipTreeLight );
	SAFEDELETE( m_ShipTreeInvLight );
	SAFEDELETE( m_BombLeftFire );
	SAFEDELETE( m_BombLeftInvFire );
	SAFEDELETE( m_BombRightFire );
	SAFEDELETE( m_BombRightInvFire );

	for( int i = 0; i < EFFECT_MAX; ++i )
		SAFEDELETE( m_FireWorkEffect[i] );
}

void PriateRulletWnd::iwm_show()
{
	m_FaceState					= FS_NORMAL;

	m_bDevelopKBack				= false;
	m_dwDevelopKBackTime		= 0;
	m_CurrHP					= 0;
	m_CurrFaceIndex				= 0;

	m_dwFaceAnimationStartTime	= 0;
	m_dwFaceAnimationTime		= 0;
	m_nFaceAnimationIndex		= 0;

	DWORD dwCurTime				= FRAMEGETTIME();
	m_MonkeyFaceTime			= dwCurTime;
	m_TreeBlinkTime				= dwCurTime;
	m_LeftBombBlinkTime			= dwCurTime;
	m_RightBombBlinkTime		= dwCurTime;
	m_fLeftCurrBombAlphaRate	= 0.0f;
	m_fRightCurrBombAlphaRate	= 0.0f;
	m_nSwordUseCount			= 0;
	m_dwSemiCriticalEffectStart	= 0;
	m_dwCriticalEffectStart		= 0;

	m_pSemiCriticalEffect		= GetEffect("SemiCriticalEffect");
	m_pCriticalEffect			= GetEffect("CriticalEffect");

	//이미 HP가 0이면 End상태로만 변경
	ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
	if( pRullet && pRullet->GetCurrHP() <= 0 )
	{
		SetState( RS_PRE_END, false );
	}
	else
	{
		SetState( m_RulletState );
	}
}

void PriateRulletWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "OakCap" )
	{
		SAFEDELETE( m_OakImageCap );
		m_OakImageCap = pImage;
	}
	else if( szType == "OakBody" )
	{
		SAFEDELETE( m_OakImageBody );
		m_OakImageBody = pImage;
	}
	else if( szType == "DevelopK" )
	{
		SAFEDELETE( m_DevelopK );
		m_DevelopK = pImage;
	}
	else if( szType == "DevelopKBack" )
	{
		SAFEDELETE( m_DevelopKBack );
		m_DevelopKBack = pImage;
	}
	else if( szType == "DevelopKBackAnimation" )
	{
		SAFEDELETE( m_DevelopKBackAnimation );
		m_DevelopKBackAnimation = pImage;
	}
	else if( szType == "RewardMap" )
	{
		SAFEDELETE( m_RewardMap );
		m_RewardMap = pImage;
	}
	else if( szType == "SwordBorderTitle" )
	{
		SAFEDELETE( m_SwordBorderTitle );
		m_SwordBorderTitle = pImage;
	}
	else if( szType == "DevelopKFlyStart" )
	{
		SAFEDELETE( m_DevelopKFlyStart );
		m_DevelopKFlyStart = pImage;
	}
	else if( szType == "DevelopKFlyEnd" )
	{
		SAFEDELETE( m_DevelopKFlyEnd );
		m_DevelopKFlyEnd = pImage;
	}
	else if( szType == "DevelopKWounded" )
	{
		SAFEDELETE( m_DevelopKWounded );
		m_DevelopKWounded = pImage;
	}
	else if( szType == "DevelopKWoundedBack" )
	{
		SAFEDELETE( m_DevelopKWoundedBack );
		m_DevelopKWoundedBack = pImage;
		if( m_DevelopKWoundedBack )
			m_DevelopKWoundedBack->SetScale( 0.65f, 0.65f );
	}
	else if( szType == "DevelopKProvoke" )
	{
		SAFEDELETE( m_DevelopKProvoke );
		m_DevelopKProvoke = pImage;
	}
	else if( szType == "DevelopKProvokeBack" )
	{
		SAFEDELETE( m_DevelopKProvokeBack );
		m_DevelopKProvokeBack = pImage;

		if( m_DevelopKProvokeBack )
			m_DevelopKProvokeBack->SetScale( 0.65f, 0.65f );
	}
	else if( szType == "DevelopKBackToolTip" )
	{
		SAFEDELETE( m_DevelopKBackToolTip );
		m_DevelopKBackToolTip = pImage;
	}
	else if( szType == "FireWorkEffect01" )
	{
		SAFEDELETE( m_FireWorkEffect[0] );
		m_FireWorkEffect[0] = pImage;
	}
	else if( szType == "FireWorkEffect02" )
	{
		SAFEDELETE( m_FireWorkEffect[1] );
		m_FireWorkEffect[1] = pImage;
	}
	else if( szType == "FireWorkEffect03" )
	{
		SAFEDELETE( m_FireWorkEffect[2] );
		m_FireWorkEffect[2] = pImage;
	}
	else if( szType == "FireWorkEffect04" )
	{
		SAFEDELETE( m_FireWorkEffect[3] );
		m_FireWorkEffect[3] = pImage;
	}
	else if( szType == "MonKeyFace" )
	{
		SAFEDELETE( m_MonKeyFace );
		m_MonKeyFace = pImage;
	}
	else if( szType == "ShipTreeLight" )
	{
		SAFEDELETE( m_ShipTreeLight );
		m_ShipTreeLight = pImage;
	}
	else if( szType == "ShipTreeInvLight" )
	{
		SAFEDELETE( m_ShipTreeInvLight );
		m_ShipTreeInvLight = pImage;
	}
	else if( szType == "BombLeftFire" )
	{
		SAFEDELETE( m_BombLeftFire );
		m_BombLeftFire = pImage;
	}
	else if( szType == "BombLeftInvFire" )
	{
		SAFEDELETE( m_BombLeftInvFire );
		m_BombLeftInvFire = pImage;
	}
	else if( szType == "BombRightFire" )
	{
		SAFEDELETE( m_BombRightFire );
		m_BombRightFire = pImage;
	}
	else if( szType == "BombRightInvFire" )
	{
		SAFEDELETE( m_BombRightInvFire );
		m_BombRightInvFire = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PriateRulletWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "SwordBorder" )
	{
		SAFEDELETE( m_SwordBorder );
		m_SwordBorder = pFrame;
	}
	else if( szType == "BubbleRight" )
	{
		SAFEDELETE( m_BubbleRight );
		m_BubbleRight = pFrame;
	}
	else if( szType == "BubbleLeft" )
	{
		SAFEDELETE( m_BubbleLeft );
		m_BubbleLeft = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void PriateRulletWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );

	m_ProvocationSound	= xElement.GetStringAttribute_e( "ProvocationSound" );
	m_RewardSound		= xElement.GetStringAttribute_e( "RewardWoodSound" );
	m_DevelopKSound		= xElement.GetStringAttribute_e( "DevelopKSound" );
}

void PriateRulletWnd::SetState( RulletState eRulletState, bool bAnimation /*= true */ )
{
	m_RulletState = eRulletState;

	switch( m_RulletState )
	{
	case RS_START:
		{
			HidePlayState();
			HidePlayOakPart();
			HideHelpState();
			HidePreEndState();
			HideEndState();

			ShowStartState();
		}
		break;
	case RS_PRE:
		{
			HideStartState();
			HideHelpState();
			HidePreEndState();
			HideEndState();

			ShowPreState();
		}
		break;
	case RS_PLAY:
		{
			HideStartState();
			HideHelpState();
			HidePreEndState();
			HideEndState();

			ShowPlayState();
		}
		break;
	case RS_HELP:
		{
			HideStartState();
			HidePlayState();
			HidePreEndState();
			HideEndState();

			ShowHelpState();
		}
		break;
	case RS_PRE_END:
		{
			HideStartState();
			HideHelpState();
			HidePlayState();
			HideEndState();
			ShowPreEndState( bAnimation );
		}
		break;
	case RS_END:
		{
			HideStartState();
			HidePlayState();
			HidePreEndState();

			ShowEndState( bAnimation );
		}
		break;
	}
}

void PriateRulletWnd::ShowStartState()
{
	ShowChildWnd( ID_GAME_RULE_TITLE );
	ShowChildWnd( ID_GAME_RULE_RICH_LABEL );
	ShowChildWnd( ID_START );
}

void PriateRulletWnd::HideStartState()
{
	HideChildWnd( ID_GAME_RULE_TITLE );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL );
	HideChildWnd( ID_START );
}

void PriateRulletWnd::ShowPreState()
{
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON10; ++dwID )
	{	
		PrivateRulletRewardButton* pButton = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( dwID ) );
		if( pButton )
			pButton->SetRewardInfo( dwID - ID_REWARD_BUTTON01, PriateRulletReward::RT_REWARD );
	}

	for( DWORD dwID = ID_WOOD_SWORD_RADIO; dwID <= ID_GOLD_SWORD_RADIO; ++dwID )
	{
		SwordFrameButton* pButton = dynamic_cast<SwordFrameButton*>( FindChildWnd( dwID ) );
		if( pButton )
		{
			int Index = dwID - ID_WOOD_SWORD_RADIO;
			pButton->SetSwordInfo( Index );
		}
	}

	PrivateRulletRewardButton* pButton = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( ID_BONUS_REWARD_BUTTON ) );
	if( pButton )
		pButton->SetRewardInfo( 0, PriateRulletReward::RT_BONUS_REWARD );

	ShowChildWnd( ID_HELP_BUTTON );
	ShowChildWnd( ID_RESET_BUTTON );

	m_dwPreStartTime		= FRAMEGETTIME();
	m_bDevelopKBack			= false;
	m_dwDevelopKBackTime	= 0;
}

void PriateRulletWnd::ShowPlayState()
{
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON10; ++dwID )
	{	
		PrivateRulletRewardButton* pButton = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( dwID ) );
		if( pButton )
			pButton->SetRewardInfo( dwID - ID_REWARD_BUTTON01, PriateRulletReward::RT_REWARD );
	}

	for( DWORD dwID = ID_WOOD_SWORD_RADIO; dwID <= ID_GOLD_SWORD_RADIO; ++dwID )
	{
		SwordFrameButton* pButton = dynamic_cast<SwordFrameButton*>( FindChildWnd( dwID ) );
		if( pButton )
		{
			int Index = dwID - ID_WOOD_SWORD_RADIO;
			pButton->SetSwordInfo( Index );
		}
	}
	CheckOakHole();

	PrivateRulletRewardButton* pButton = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( ID_BONUS_REWARD_BUTTON ) );
	if( pButton )
		pButton->SetRewardInfo( 0, PriateRulletReward::RT_BONUS_REWARD );

	ShowChildWnd( ID_HELP_BUTTON );
	ShowChildWnd( ID_RESET_BUTTON );	

	ShowChildWnd( ID_ROTATE_LEFT );
	ShowChildWnd( ID_ROTATE_RIGHT );

	if( m_bDevelopKBack )
	{
		HideChildWnd( ID_RIGHT_BUBBLE );
		ShowChildWnd( ID_LEFT_BUBBLE );
	}
	else
	{
		ShowChildWnd( ID_RIGHT_BUBBLE );
		HideChildWnd( ID_LEFT_BUBBLE );
	}

	InitDevelopKAnimation();

	m_FaceState					= FS_PROVOKE;
	m_dwFaceAnimationStartTime	= FRAMEGETTIME();
	g_SoundMgr.PlaySound( m_ProvocationSound );
}

void PriateRulletWnd::HidePlayState()
{

	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON10; ++dwID )
	{
		HideChildWnd( dwID );
	}
	HideChildWnd( ID_BONUS_REWARD_BUTTON );

	HideChildWnd( ID_WOOD_SWORD_RADIO );
	HideChildWnd( ID_SILVER_SWORD_RADIO );
	HideChildWnd( ID_GOLD_SWORD_RADIO );

	HideChildWnd( ID_HELP_BUTTON );
	HideChildWnd( ID_RESET_BUTTON );

	HideChildWnd( ID_RIGHT_BUBBLE );
	HideChildWnd( ID_LEFT_BUBBLE );
}

void PriateRulletWnd::HidePlayOakPart()
{
	for( DWORD dwID = ID_OAK_HOLE_BUTTON01; dwID <= ID_OAK_HOLE_BUTTON20; ++dwID )
	{
		HideChildWnd( dwID );
	}

	HideChildWnd( ID_ROTATE_LEFT );
	HideChildWnd( ID_ROTATE_RIGHT );
}

void PriateRulletWnd::ShowHelpState()
{
	ShowChildWnd( ID_GAME_RULE_TITLE );
	ShowChildWnd( ID_GAME_RULE_RICH_LABEL );
	ShowChildWnd( ID_RETURN_PLAY_BUTTON );
}

void PriateRulletWnd::HideHelpState()
{
	HideChildWnd( ID_GAME_RULE_TITLE );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL );
	HideChildWnd( ID_RETURN_PLAY_BUTTON );
}

void PriateRulletWnd::ShowPreEndState( bool bAnimation )
{
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON10; ++dwID )
	{	
		PrivateRulletRewardButton* pButton = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( dwID ) );
		if( pButton )
			pButton->SetRewardInfo( dwID - ID_REWARD_BUTTON01, PriateRulletReward::RT_REWARD );
	}

	PrivateRulletRewardButton* pButton = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( ID_BONUS_REWARD_BUTTON ) );
	if( pButton )
		pButton->SetRewardInfo( 0, PriateRulletReward::RT_BONUS_REWARD );
	

	for( DWORD dwID = ID_WOOD_SWORD_RADIO; dwID <= ID_GOLD_SWORD_RADIO; ++dwID )
	{
		SwordFrameButton* pButton = dynamic_cast<SwordFrameButton*>( FindChildWnd( dwID ) );
		if( pButton )
		{
			int Index = dwID - ID_WOOD_SWORD_RADIO;
			pButton->SetSwordInfo( Index );
		}
	}

	PriateNewStartButton* pNewStartButton = dynamic_cast<PriateNewStartButton*>( FindChildWnd( ID_NEW_START ) );
	if( pNewStartButton )
	{
		pNewStartButton->Clear();
		pNewStartButton->BlinkAnimationStart();
		pNewStartButton->ShowWnd();
	}

	InitDevelopKAnimation();

	if( bAnimation )
		m_dwPreEndStartTime = FRAMEGETTIME();
}

void PriateRulletWnd::HidePreEndState()
{
	HideChildWnd( ID_NEW_START );
}

void PriateRulletWnd::ShowEndState( bool bAnimation )
{
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON10; ++dwID )
	{	
		PrivateRulletRewardButton* pButton = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( dwID ) );
		if( pButton )
		{	
			if( bAnimation )
			{
				pButton->ShowWnd();
				pButton->StartAlphaAnimation();
			}
			else
			{
				pButton->SetRewardInfo( dwID - ID_REWARD_BUTTON01, PriateRulletReward::RT_REWARD );
			}
			
		}
	}

	PrivateRulletRewardButton* pButton = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( ID_BONUS_REWARD_BUTTON ) );
	if( pButton )
	{
		if( bAnimation )
		{
			pButton->ShowWnd();
			pButton->StartAlphaAnimation();
		}
		else
		{
			pButton->SetRewardInfo( 0, PriateRulletReward::RT_BONUS_REWARD );
		}
	}

	for( DWORD dwID = ID_OAK_HOLE_BUTTON01; dwID <= ID_OAK_HOLE_BUTTON20; ++dwID )
	{
		OakHoleButton* pButton = dynamic_cast<OakHoleButton*>( FindChildWnd( dwID ) );
		if( pButton )
		{
			DWORD dwIndex = dwID - ID_OAK_HOLE_BUTTON01;
			pButton->SetOakHole( m_bDevelopKBack ? dwIndex + ioPriateRullet::RULLET_BACK : dwIndex );
			pButton->ShowWnd();
			if( bAnimation )
			{
				pButton->StartAlphaAnimation();
			}
		}
	}

	for( DWORD dwID = ID_WOOD_SWORD_RADIO; dwID <= ID_GOLD_SWORD_RADIO; ++dwID )
	{
		SwordFrameButton* pButton = dynamic_cast<SwordFrameButton*>( FindChildWnd( dwID ) );
		if( pButton )
		{
			int Index = dwID - ID_WOOD_SWORD_RADIO;
			pButton->SetSwordInfo( Index );
			if( bAnimation )
			{
				pButton->StartAlphaAnimation();
			}
		}
	}

	ShowChildWnd( ID_HELP_BUTTON );
	ShowChildWnd( ID_RESET_BUTTON );

	if( bAnimation )
		m_dwEndStartTime = FRAMEGETTIME();
}

void PriateRulletWnd::HideEndState()
{
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON10; ++dwID )
	{
		HideChildWnd( dwID );
	}
	HideChildWnd( ID_BONUS_REWARD_BUTTON );

	HideChildWnd( ID_WOOD_SWORD_RADIO );
	HideChildWnd( ID_SILVER_SWORD_RADIO );
	HideChildWnd( ID_GOLD_SWORD_RADIO );
	HideChildWnd( ID_HELP_BUTTON );
	HideChildWnd( ID_RESET_BUTTON );
}

void PriateRulletWnd::SetStickType( DWORD dwID )
{
	if( ID_WOOD_SWORD_RADIO == dwID )
		m_StickedSwordCode = ioEtcItem::EIT_ETC_OAK_WOOD_SWORD;
	else if( ID_SILVER_SWORD_RADIO == dwID )
		m_StickedSwordCode = ioEtcItem::EIT_ETC_OAK_SILVER_SWORD;
	else if( ID_GOLD_SWORD_RADIO == dwID )
		m_StickedSwordCode = ioEtcItem::EIT_ETC_OAK_GOLD_SWORD;
	else
		m_StickedSwordCode = 0;
}

void PriateRulletWnd::CheckSwordButton( DWORD dwID )
{	
	CheckRadioButton( ID_WOOD_SWORD_RADIO, ID_GOLD_SWORD_RADIO, dwID );
	SetStickType( dwID );
}

void PriateRulletWnd::CheckOakHole()
{
	for( DWORD dwID = ID_OAK_HOLE_BUTTON01; dwID <= ID_OAK_HOLE_BUTTON20; ++dwID )
	{
		OakHoleButton* pButton = dynamic_cast<OakHoleButton*>( FindChildWnd( dwID ) );
		if( pButton )
		{
			DWORD dwIndex = dwID - ID_OAK_HOLE_BUTTON01;
			pButton->SetOakHole( m_bDevelopKBack ? dwIndex + ioPriateRullet::RULLET_BACK : dwIndex );
			pButton->ShowWnd();
		}
	}
}

void PriateRulletWnd::OnSwordUse( int Index, bool bCritical, bool bProvoke, int nCurrHP, int nRewardCount, int eCritical )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwID = ID_OAK_HOLE_BUTTON01 + (m_bDevelopKBack ? Index - ioPriateRullet::RULLET_BACK : Index);
	OakHoleButton* pButton = dynamic_cast<OakHoleButton*>( FindChildWnd( dwID ) );
	if( pButton )
	{
		pButton->AnimationStart( eCritical );

		switch( eCritical )
		{
		case OCT_FULL_CRITICAL:
			if( m_pCriticalEffect )
			{
				m_pCriticalEffect->SetPosX( pButton->GetXPos() - m_pCriticalEffect->GetWidth()/2 + pButton->GetEffectXPos() );
				m_pCriticalEffect->SetPosY( pButton->GetYPos() - m_pCriticalEffect->GetHeight()/2 + pButton->GetEffectYPos() );
				m_dwCriticalEffectStart = dwCurTime;
			}
			break;
		default:
			if( m_pSemiCriticalEffect )
			{
				m_pSemiCriticalEffect->SetPosX( pButton->GetXPos() - m_pSemiCriticalEffect->GetWidth()/2 + pButton->GetEffectXPos() );
				m_pSemiCriticalEffect->SetPosY( pButton->GetYPos() - m_pSemiCriticalEffect->GetHeight()/2 + pButton->GetEffectYPos() );
				m_dwSemiCriticalEffectStart = dwCurTime;
			}
			break;
		}
	}

	if( bProvoke )
	{
		m_FaceState					= FS_PROVOKE;
		m_dwFaceAnimationStartTime	= dwCurTime;
		g_SoundMgr.PlaySound( m_ProvocationSound );
	}
	else
	{
		m_FaceState					= FS_WOUNDED;
		m_dwFaceAnimationStartTime	= dwCurTime;
	}
	m_dwPreEndCheckStartTime = dwCurTime;

	bool bReward = false;
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON10; ++dwID )
	{
		PrivateRulletRewardButton* pButton = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( dwID ) );
		if( pButton && pButton->IsEnableStampAnimation( nCurrHP ) )
		{
			bReward = true;
			pButton->StampAnimationStart( nRewardCount );
		}
	}
	
	PrivateRulletRewardButton* pBonus = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( ID_BONUS_REWARD_BUTTON ) );
	if( pBonus )
	{
		//HP가 0 이하면 획득 애니메이션을 처리, 0보다 크면 보너스 보상 정보 갱신
		if( nCurrHP <= 0 )
		{
			bReward = true;
			pBonus->StampAnimationStart( nRewardCount + 1 );
		}
		else
		{
			pBonus->SetRewardInfo( 0, PriateRulletReward::RT_BONUS_REWARD );
		}
	}

	if( bReward )
		g_SoundMgr.PlaySound( m_RewardSound );

	ioUserEtcItem* pETC = g_MyInfo.GetUserEtcItem();
	if( pETC )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pETC->GetEtcItem( m_StickedSwordCode, kSlot ) )
		{
			if( 0 >= kSlot.m_iValue1 )
				CheckSwordButton( 0 );
		}
		else
		{	
			CheckSwordButton( 0 );
		}
	}
}

void PriateRulletWnd::iwm_hide()
{
	if( m_pSemiCriticalEffect )
		m_pSemiCriticalEffect->Stop();

	if( m_pCriticalEffect )
		m_pCriticalEffect->Stop();
}

void PriateRulletWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	//드럼통 버튼
	if( COMPARE( dwID, ID_OAK_HOLE_BUTTON01, ID_OAK_HOLE_BUTTON20 + 1 ) )
	{
		if( cmd == IOBN_BTNUP )
		{
			ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
			if( pRullet )
			{
				int nCount = 0;
				for( DWORD ID = ID_WOOD_SWORD_RADIO; ID <= ID_GOLD_SWORD_RADIO; ++ID )
				{
						int Index = ID - ID_WOOD_SWORD_RADIO;
						nCount += pRullet->GetSwordCount( Index );
				}
				
				if( nCount <= 0 )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "보유한 칼이 없습니다." );
					return;
				}

				if( m_RulletState == RS_PLAY && m_dwEndStartTime == 0 )
				{
					if( m_StickedSwordCode == 0 )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "칼을 선택해서 오크통 구멍에# 꼽아주세요." );
					}
					else
					{	
						DWORD dwIndex = dwID - ID_OAK_HOLE_BUTTON01;
						if( !pRullet->SendRulletUse( m_bDevelopKBack ? dwIndex + ioPriateRullet::RULLET_BACK : dwIndex, m_StickedSwordCode ) )
						{
							g_GUIMgr.SetMsgBox( MB_OK, NULL, "이미 칼이 꽃혀 있습니다." );
						}
					}
				}
			}
		}
		else if( cmd == IOWN_OVERED )
		{
			OakHoleButton* pButton = dynamic_cast<OakHoleButton*>( FindChildWnd( dwID ) );
			if( pButton && !pButton->IsSticked() )
				StartDevelopKShakeAnimation();
		}
	}
	//보상버튼
	else if( COMPARE( dwID, ID_REWARD_BUTTON01, ID_REWARD_BUTTON10 + 1 ) )
	{
		if( cmd == IOWN_OVERED )
		{
			PrivateRulletRewardButton* pButton = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( dwID ) );
			{
				ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
				kPrinter[0].SetTextStyle( TS_NORMAL );
				kPrinter[0].SetBkColor( 0, 0, 0 );
				kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
				kPrinter[0].AddTextPiece( FONT_SIZE_12, "%s", pButton->GetName().c_str() );

				GlobalTextTooltip *pTooltip = dynamic_cast<GlobalTextTooltip*>( FindChildWnd( ID_GLOBAL_TOOLTIP ) );
				if( pTooltip )
					pTooltip->SetTooltipText( kPrinter );
			}
		}
		else if( cmd == IOWN_LEAVED )
		{
			GlobalTextTooltip *pTooltip = dynamic_cast<GlobalTextTooltip*>( FindChildWnd( ID_GLOBAL_TOOLTIP ) );
			if( pTooltip )
				pTooltip->HideWnd();
		}
	}
	else if( dwID == ID_BONUS_REWARD_BUTTON )
	{
		if( cmd == IOWN_OVERED )
		{
			ShowChildWnd( ID_BONUS_REWARD_TOOLTIP );
		}
		else if( cmd == IOWN_LEAVED )
		{
			HideChildWnd( ID_BONUS_REWARD_TOOLTIP );
		}
	}
	else
	{
		switch( dwID )
		{
		case ID_EXIT:
			if( cmd == IOBN_BTNUP )
			{
				HideWnd();
			}
			break;
		case ID_START:
			if( cmd == IOBN_BTNUP )
			{
				SetState( RS_PRE );
			}
			break;
		case ID_WOOD_SWORD_RADIO:
		case ID_SILVER_SWORD_RADIO:
		case ID_GOLD_SWORD_RADIO:
			if( cmd == IOBN_BTNUP )
			{	
				CheckSwordButton( dwID );
			}
			break;
		case ID_HELP_BUTTON:
			if( cmd == IOBN_BTNUP && m_RulletState == RS_PLAY && m_dwEndStartTime == 0 )
			{
				SetState( RS_HELP );
			}
			break;
		case ID_RETURN_PLAY_BUTTON:
			if( cmd == IOBN_BTNUP )
			{
				SetState( RS_PLAY );
			}
			break;
		case ID_ROTATE_LEFT:
		case ID_ROTATE_RIGHT:
			if( cmd == IOBN_BTNUP && m_RulletState == RS_PLAY && m_dwEndStartTime == 0 )
			{
				m_dwDevelopKBackTime = FRAMEGETTIME();
			}
			break;
		case ID_RESET_BUTTON:
		case ID_NEW_START:
			if( cmd == IOBN_BTNUP && (m_RulletState != RS_END || m_dwEndStartTime == 0 ) )
			{
				g_GUIMgr.ShowWnd( PRIATE_RULLET_RESET_POPUP );
			}
			break;
		}
	}
}

void PriateRulletWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioWnd::OnProcess( fTimePerSec );

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_RulletState )
	{
	case RS_PRE:
		if( m_dwPreStartTime + 200 <= dwCurTime )
		{
			g_SoundMgr.PlaySound( m_DevelopKSound );
			SetState(  RS_PLAY );
		}
		break;
	case RS_HELP:
	case RS_PLAY:
		{
			ProcessOakRotateAnimation();
			ProcessDevelopKShakeAnimation();

			switch( m_FaceState )
			{
			case FS_NORMAL:
				ProcessDevelopKFaceCheck();
				break;
			case FS_PROVOKE:
				ProcessDevelopKProvokeFaceCheck();
				break;
			case FS_WOUNDED:
				ProcessDevelopKWoundedFaceCheck();
				break;
			}
			
			ProcessRulletStateCheck();
		}
	case RS_PRE_END:
		{
			ProcessPreEndState();
		}
		break;
	case RS_END:
		{
			ProcessEndState();
		}
		break;
	}

	ProcessBackGround();

}

void PriateRulletWnd::ProcessOakRotateAnimation()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwDevelopKBackTime && m_dwDevelopKBackTime + 100 < dwCurTime )
	{
		m_bDevelopKBack			= !m_bDevelopKBack;
		m_dwDevelopKBackTime	= 0;
		ProcessDevelopKFaceCheck( true );

		CheckOakHole();

		if( m_bDevelopKBack )
		{
			HideChildWnd( ID_RIGHT_BUBBLE );
			ShowChildWnd( ID_LEFT_BUBBLE );
		}
		else
		{
			ShowChildWnd( ID_RIGHT_BUBBLE );
			HideChildWnd( ID_LEFT_BUBBLE );
		}
	}
}

void PriateRulletWnd::InitDevelopKAnimation()
{
	m_DevelopKState					= DK_NONE;
	m_dwDevelopAnimationStartTime	= 0;
	m_dwDevelopAnimationTime		= 0;
	m_nAnimationX					= 0;
	m_bDevelopKShake				= false;
}

void PriateRulletWnd::StartDevelopKShakeAnimation()
{
	if( !m_bDevelopKShake )
	{
		m_DevelopKState					= DK_RIGHT;
		m_dwDevelopAnimationTime		= FRAMEGETTIME();
		m_dwDevelopAnimationStartTime	= m_dwDevelopAnimationTime;
		m_nAnimationX					= 0;
		m_bDevelopKShake				= true;
	}
}

void PriateRulletWnd::ProcessDevelopKShakeAnimation()
{
	if( !m_bDevelopKShake )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_RulletState == RS_PLAY || m_RulletState == RS_HELP )
	{
		switch( m_DevelopKState )
		{
		case DK_RIGHT:
			{
				if( m_dwDevelopAnimationTime + 25 < dwCurTime )
				{
					m_dwDevelopAnimationTime = dwCurTime;
					m_nAnimationX += 1;
				}
				if( m_nAnimationX >= SHAKE_MAX )
				{
					m_DevelopKState				= DK_LEFT;
					m_dwDevelopAnimationTime	= dwCurTime;
				}
			}
			break;
		case DK_LEFT:
			{
				if( m_dwDevelopAnimationTime + 25 < dwCurTime )
				{
					m_dwDevelopAnimationTime = dwCurTime;
					m_nAnimationX -= 1;
				}
				if( m_nAnimationX <= -SHAKE_MAX )
				{
					m_DevelopKState				= DK_RIGHT;
					m_dwDevelopAnimationTime	= dwCurTime;
					
					if( m_dwDevelopAnimationStartTime + 900 < dwCurTime )
						InitDevelopKAnimation();
				}
			}
			break;
		}
	}
}

void PriateRulletWnd::ProcessDevelopKFaceCheck( bool bMustChange /*= false */ )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//HP가 달라졌다면 표정 교체
	ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
	if( pRullet && ( pRullet->GetCurrHP() != m_CurrHP || bMustChange) )
	{
		//현재 페이스를 제거
		for( int i = 0; i < (int)m_CurrFaceList.size(); ++i )
		{
			SAFEDELETE( m_CurrFaceList[i] );
		}
		m_CurrFaceList.clear();

		m_CurrDevelopKFace	= NULL;

		//HP 갱신
		m_CurrHP = pRullet->GetCurrHP();

		//HP에 맞는 FACE를 가져옴
		ioHashStringVec FaceList;
		pRullet->GetCurrFaceName( FaceList );

		m_nFaceAnimationIndex = 0;

		//FACE 이미지 생성
		for( int i = 0; i < (int)FaceList.size(); ++i )
		{
			ioUIRenderImage* pImage = g_UIImageSetMgr.CreateImageByFullName( FaceList[i] );
			if( pImage )
				m_CurrFaceList.push_back( pImage );
		}

		if( !m_CurrFaceList.empty() )
		{
			m_CurrDevelopKFace			= m_CurrFaceList[0];
			if( m_CurrDevelopKFace )
			{
				if( m_bDevelopKBack )
					m_CurrDevelopKFace->SetScale( 0.65f, 0.65f );
				else
					m_CurrDevelopKFace->SetScale( FLOAT1, FLOAT1 );
			}

			m_dwFaceAnimationTime		= pRullet->GetCurrFaceTime();
			m_dwFaceAnimationStartTime	= dwCurTime;
		}
	}

	//애니메이션 처리
	if( !m_CurrFaceList.empty() )
	{
		if( 0 < m_dwFaceAnimationStartTime && m_dwFaceAnimationStartTime + m_dwFaceAnimationTime < dwCurTime )
		{
			m_nFaceAnimationIndex++;
			if( !COMPARE( m_nFaceAnimationIndex, 0, (int)m_CurrFaceList.size() ) )
				m_nFaceAnimationIndex = 0;

			m_CurrDevelopKFace = m_CurrFaceList[m_nFaceAnimationIndex];
			if( m_CurrDevelopKFace )
			{
				if( m_bDevelopKBack )
					m_CurrDevelopKFace->SetScale( 0.65f, 0.65f );
				else
					m_CurrDevelopKFace->SetScale( FLOAT1, FLOAT1 );
			}
			m_dwFaceAnimationStartTime = dwCurTime;
		}
	}
}

void PriateRulletWnd::ProcessDevelopKProvokeFaceCheck()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFaceAnimationStartTime + 300 < dwCurTime )
	{
		m_FaceState					= FS_NORMAL;
		m_dwFaceAnimationStartTime = dwCurTime;
	}
}

void PriateRulletWnd::ProcessDevelopKWoundedFaceCheck()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFaceAnimationStartTime + 300 < dwCurTime )
	{
		m_FaceState					= FS_NORMAL;
		m_dwFaceAnimationStartTime	= dwCurTime;
	}
}

void PriateRulletWnd::ProcessRulletStateCheck()
{
	//HP가 달라졌다면 표정 교체
	ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
	if( pRullet && pRullet->GetCurrHP() <= 0 && m_RulletState != RS_PRE_END )
	{
		SetState( RS_PRE_END );
	}
}

void PriateRulletWnd::ProcessPreEndState()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwPreEndStartTime && m_dwPreEndStartTime + 900 < dwCurTime )
	{
		m_dwPreEndStartTime = 0;
		for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON10; ++dwID )
		{	
			PrivateRulletRewardButton* pButton = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( dwID ) );
			if( pButton && pButton->IsEnableStampBlinkAnimation() )
				pButton->StampBlinkAnimationStart();
		}

		PrivateRulletRewardButton* pButton = dynamic_cast<PrivateRulletRewardButton*>( FindChildWnd( ID_BONUS_REWARD_BUTTON ) );
		if( pButton  && pButton->IsEnableStampBlinkAnimation() )
			pButton->StampBlinkAnimationStart();

		PriateNewStartButton* pNewStartButton = dynamic_cast<PriateNewStartButton*>( FindChildWnd( ID_NEW_START ) );
		if( pNewStartButton )
			pNewStartButton->BlinkAnimationStart();
	}
}

void PriateRulletWnd::ProcessEndState()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwEndStartTime && m_dwEndStartTime + 400 < dwCurTime )
	{
		m_dwEndStartTime = 0;
		CheckSwordButton( 0 );
		SetState( RS_PRE );
	}
}

void PriateRulletWnd::ProcessBackGround()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_MonkeyFaceTime + 3000 < dwCurTime )
	{
		m_MonkeyFaceTime = dwCurTime;
	}

	if( 0 < m_TreeBlinkTime )
	{
		if( dwCurTime <= m_TreeBlinkTime + 300 )
		{
			m_fCurrTreeAlphaRate	= (((float)dwCurTime - m_TreeBlinkTime) / (float)300);
			m_fCurrTreeAlphaRate	= max( 0.0f, m_fCurrTreeAlphaRate );
		}
		else if( dwCurTime <= m_TreeBlinkTime + 600 )
		{
			m_fCurrTreeAlphaRate	= FLOAT1 - (((float)dwCurTime - m_TreeBlinkTime) / (float)300);
			m_fCurrTreeAlphaRate	= max( 0.0f, m_fCurrTreeAlphaRate );
		}
		else
		{
			m_TreeBlinkTime			= dwCurTime;
			m_fCurrTreeAlphaRate	= 0.0f;
		}
	}

	if( 0 < m_LeftBombBlinkTime )
	{
		if( dwCurTime <= m_LeftBombBlinkTime + 300 )
		{
			m_fLeftCurrBombAlphaRate	= (((float)dwCurTime - m_LeftBombBlinkTime) / (float)300);
			m_fLeftCurrBombAlphaRate	= max( 0.0f, m_fLeftCurrBombAlphaRate );
		}
		else if( dwCurTime <= m_LeftBombBlinkTime + 600 )
		{
			m_fLeftCurrBombAlphaRate	= FLOAT1 - (((float)dwCurTime - m_LeftBombBlinkTime) / (float)300);
			m_fLeftCurrBombAlphaRate	= max( 0.0f, m_fLeftCurrBombAlphaRate );
		}
		else
		{
			m_LeftBombBlinkTime			= dwCurTime;
			m_fLeftCurrBombAlphaRate	= 0.0f;
		}
	}

	if( 0 < m_RightBombBlinkTime )
	{
		if( dwCurTime <= m_RightBombBlinkTime + 300 )
		{
			m_fRightCurrBombAlphaRate	= (((float)dwCurTime - m_RightBombBlinkTime) / (float)300);
			m_fRightCurrBombAlphaRate	= max( 0.0f, m_fRightCurrBombAlphaRate );
		}
		else if( dwCurTime <= m_RightBombBlinkTime + 600 )
		{
			m_fRightCurrBombAlphaRate	= FLOAT1 - (((float)dwCurTime - m_RightBombBlinkTime) / (float)300);
			m_fRightCurrBombAlphaRate	= max( 0.0f, m_fRightCurrBombAlphaRate );
		}
		else
		{
			m_RightBombBlinkTime		= dwCurTime;
			m_fRightCurrBombAlphaRate	= 0.0f;
		}
	}

	ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
	if( pRullet )
	{
		m_nSwordUseCount = pRullet->GetRulletStickedCount();
	}

	if( 0 < m_dwSemiCriticalEffectStart && m_dwSemiCriticalEffectStart + 150 < dwCurTime )
	{
		if( m_pSemiCriticalEffect )
			m_pSemiCriticalEffect->Start();

		m_dwSemiCriticalEffectStart = 0;
	}

	if( 0 < m_dwCriticalEffectStart && m_dwCriticalEffectStart + 150 < dwCurTime )
	{
		if( m_pCriticalEffect )
			m_pCriticalEffect->Start();

		m_dwCriticalEffectStart = 0;
	}
}

void PriateRulletWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_MonkeyFaceTime && m_MonkeyFaceTime + 100 <= dwCurTime && dwCurTime <= m_MonkeyFaceTime + 200 )
	{
		if( m_MonKeyFace )
			m_MonKeyFace->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_ShipTreeLight )
	{
		m_ShipTreeLight->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurrTreeAlphaRate );
		m_ShipTreeLight->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_ShipTreeInvLight )
	{
		m_ShipTreeInvLight->SetAlpha( (float)MAX_ALPHA_RATE * (1.0f - m_fCurrTreeAlphaRate ) );
		m_ShipTreeInvLight->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_BombLeftFire )
	{
		m_BombLeftFire->SetAlpha( (float)MAX_ALPHA_RATE * m_fLeftCurrBombAlphaRate );
		m_BombLeftFire->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_BombLeftInvFire )
	{
		m_BombLeftInvFire->SetAlpha( (float)MAX_ALPHA_RATE * (1.0f - m_fLeftCurrBombAlphaRate ) );
		m_BombLeftInvFire->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_BombRightFire )
	{
		m_BombRightFire->SetAlpha( (float)MAX_ALPHA_RATE * m_fRightCurrBombAlphaRate );
		m_BombRightFire->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_BombRightInvFire )
	{
		m_BombRightInvFire->SetAlpha( (float)MAX_ALPHA_RATE * (1.0f - m_fRightCurrBombAlphaRate ) );
		m_BombRightInvFire->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	switch( m_RulletState )
	{
	case RS_START:
		StartRender( iXPos, iYPos );
		break;
	case RS_PRE:
		PreRender( iXPos, iYPos );
		break;
	case RS_PLAY:
		PlayRender( iXPos, iYPos );
		break;
	case RS_END:
	case RS_PRE_END:
		EndPreRender( iXPos, iYPos );
		break;
	case RS_HELP:
		DevelopKRender( iXPos, iYPos );
		break;
	}
}

void PriateRulletWnd::StartRender( int iXPos, int iYPos )
{
	if( m_OakImageCap )
		m_OakImageCap->Render( iXPos, iYPos );

	if( m_OakImageBody )
		m_OakImageBody->Render( iXPos, iYPos );
}

void PriateRulletWnd::PreRender( int iXPos, int iYPos )
{
	if( m_OakImageCap )
		m_OakImageCap->Render( iXPos, iYPos );

	if( m_dwPreStartTime + 100 < FRAMEGETTIME() )
	{
		if( m_DevelopKFlyEnd )
			m_DevelopKFlyEnd->Render( iXPos + 121, iYPos + 42 );
	}
	else
	{
		if( m_DevelopKFlyStart )
			m_DevelopKFlyStart->Render( iXPos + 121, iYPos + 42 );
	}

	if( m_OakImageBody )
		m_OakImageBody->Render( iXPos, iYPos );

	if( m_RewardMap )
		m_RewardMap->Render( iXPos, iYPos );

	if( m_SwordBorder )
		m_SwordBorder->Render( iXPos, iYPos );

	if( m_SwordBorderTitle )
		m_SwordBorderTitle->Render( iXPos, iYPos );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_2X );
	g_FontMgr.SetBkColor( 0xFF512F0F );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 568, iYPos + 247, FONT_SIZE_13, "%d개 사용", m_nSwordUseCount );
}

void PriateRulletWnd::PlayRender( int iXPos, int iYPos )
{
	DevelopKRender( iXPos, iYPos );

	if( m_RewardMap )
		m_RewardMap->Render( iXPos, iYPos );

	if( m_SwordBorder )
		m_SwordBorder->Render( iXPos, iYPos );

	if( m_SwordBorderTitle )
		m_SwordBorderTitle->Render( iXPos, iYPos );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_2X );
	g_FontMgr.SetBkColor( 0xFF512F0F );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 568, iYPos + 247, FONT_SIZE_13, "%d개 사용", m_nSwordUseCount );
}

void PriateRulletWnd::DevelopKRender( int iXPos, int iYPos )
{
	if( m_OakImageCap )
		m_OakImageCap->Render( iXPos, iYPos );

	if( m_bDevelopKBack )
	{
		if( m_DevelopKBack )
			m_DevelopKBack->Render( iXPos + m_nAnimationX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( m_DevelopKBackToolTip )
			m_DevelopKBackToolTip->Render( iXPos + 279, iYPos + 55, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else
	{
		if( m_DevelopK )
			m_DevelopK->Render( iXPos + m_nAnimationX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );	
	}

	switch( m_FaceState )
	{
	case FS_NORMAL:
		if( m_bDevelopKBack )
		{
			if( m_CurrDevelopKFace )
				m_CurrDevelopKFace->Render( iXPos + 321, iYPos + 115, UI_RENDER_NORMAL, TFO_BILINEAR );

			if( m_BubbleLeft )
				m_BubbleLeft->Render( iXPos, iYPos );
		}
		else
		{
			if( m_CurrDevelopKFace )
				m_CurrDevelopKFace->Render( iXPos + m_nAnimationX + 180, iYPos + 156, UI_RENDER_NORMAL, TFO_BILINEAR );

			if( m_BubbleRight )
				m_BubbleRight->Render( iXPos, iYPos );
		}
		break;
	case FS_PROVOKE:
		if( m_bDevelopKBack )
		{
			if( m_DevelopKProvokeBack )
				m_DevelopKProvokeBack->Render( iXPos + 321, iYPos + 115, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			if( m_DevelopKProvoke )
				m_DevelopKProvoke->Render( iXPos + m_nAnimationX + 180, iYPos + 156, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		break;
	case FS_WOUNDED:
		if( m_bDevelopKBack )
		{
			if( m_DevelopKWoundedBack )
				m_DevelopKWoundedBack->Render( iXPos + 321, iYPos + 115, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			if( m_DevelopKWounded )
				m_DevelopKWounded->Render( iXPos + m_nAnimationX + 180, iYPos + 156, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		break;
	}

	if( m_OakImageBody )
		m_OakImageBody->Render( iXPos, iYPos );

	if( 0 < m_dwDevelopKBackTime && m_DevelopKBackAnimation )
		m_DevelopKBackAnimation->Render( iXPos, iYPos );
}

void PriateRulletWnd::EndPreRender( int iXPos, int iYPos )
{
	if( m_OakImageCap )
		m_OakImageCap->Render( iXPos, iYPos );

	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwPreEndStartTime && m_dwPreEndStartTime + 100 > dwCurTime )
	{
		if( m_DevelopKFlyEnd )
			m_DevelopKFlyEnd->Render( iXPos + 121, iYPos + 42 );
	}
	else if( dwCurTime > m_dwPreEndStartTime + 100 && m_dwPreEndStartTime + 200 > dwCurTime )
	{
		if( m_DevelopKFlyStart )
			m_DevelopKFlyStart->Render( iXPos + 121, iYPos + 42 );
	}
	else if( dwCurTime > m_dwPreEndStartTime + 300 && m_dwPreEndStartTime + 500 > dwCurTime )
	{
		if( m_FireWorkEffect[0] )
			m_FireWorkEffect[0]->Render( iXPos + 234, iYPos + 60, UI_RENDER_SCREEN );
	}
	else if( dwCurTime > m_dwPreEndStartTime + 600 && m_dwPreEndStartTime + 700 > dwCurTime )
	{
		if( m_FireWorkEffect[1] )
			m_FireWorkEffect[1]->Render( iXPos + 234, iYPos + 60, UI_RENDER_SCREEN );
	}
	else if( dwCurTime > m_dwPreEndStartTime + 700 && m_dwPreEndStartTime + 800 > dwCurTime )
	{
		if( m_FireWorkEffect[2] )
			m_FireWorkEffect[2]->Render( iXPos + 234, iYPos + 60, UI_RENDER_SCREEN );
	}
	else if( dwCurTime > m_dwPreEndStartTime + 800 && m_dwPreEndStartTime + 900 > dwCurTime )
	{
		if( m_FireWorkEffect[3] )
			m_FireWorkEffect[3]->Render( iXPos + 234, iYPos + 60, UI_RENDER_SCREEN );
	}

	if( m_OakImageBody )
		m_OakImageBody->Render( iXPos, iYPos );

	if( m_RewardMap )
		m_RewardMap->Render( iXPos, iYPos );

	if( m_SwordBorder )
		m_SwordBorder->Render( iXPos, iYPos );

	if( m_SwordBorderTitle )
		m_SwordBorderTitle->Render( iXPos, iYPos );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_OUTLINE_2X );
	g_FontMgr.SetBkColor( 0xFF512F0F );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 568, iYPos + 247, FONT_SIZE_13, "%d개 사용", m_nSwordUseCount );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PrivateRulletBonusRewardTooltip::PrivateRulletBonusRewardTooltip() : m_bHidden( false )
{
	m_pIcon		= NULL;
	m_pSubIcon = NULL;
	m_pIconBack	= NULL;
	m_pNoneIcon	= NULL;
	m_pSecret	= NULL;
}

PrivateRulletBonusRewardTooltip::~PrivateRulletBonusRewardTooltip()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pNoneIcon );
	SAFEDELETE( m_pSecret );

	DestroyAllReward();
}

void PrivateRulletBonusRewardTooltip::DestroyAllReward()
{
	for( int i = 0; i < (int)m_RewardSetList.size(); ++i )
	{
		SAFEDELETE( m_RewardSetList[i].pImage );
		SAFEDELETE( m_RewardSetList[i].pSubImage );
	}
	m_RewardSetList.clear();
}

void PrivateRulletBonusRewardTooltip::iwm_show()
{
	m_nStickedCount = -1;
}

void PrivateRulletBonusRewardTooltip::iwm_hide()
{
}

void PrivateRulletBonusRewardTooltip::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
		if( m_pIconBack )
			m_pIconBack->SetScale( 0.56f, 0.56f );
	}
	else if( szType == "NoneIcon" )
	{
		SAFEDELETE( m_pNoneIcon );
		m_pNoneIcon = pImage;
		if( m_pNoneIcon )
			m_pNoneIcon->SetScale( 0.56f, 0.56f );
	}
	else if( szType == "Secret" )
	{
		SAFEDELETE( m_pSecret );
		m_pSecret = pImage;
		if( m_pSecret )
			m_pSecret->SetScale( 0.56f, 0.56f );
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PrivateRulletBonusRewardTooltip::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessPosition();
	ProcessOnMainReward();
	ProcessOnAllReward();
}

void PrivateRulletBonusRewardTooltip::ProcessPosition()
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse )
	{
		int iOffSetXY = 8;
		int iXPos = pMouse->GetMousePos().x;
		int iYPos = pMouse->GetMousePos().y;

		if( iXPos + iOffSetXY + GetWidth() > Setting::Width() )
			iXPos -= ( GetWidth() + iOffSetXY );
		else 
			iXPos += iOffSetXY;

		if( iYPos + iOffSetXY + GetHeight() > Setting::Height() - 45 )
			iYPos -= ( GetHeight() + iOffSetXY );
		else 
			iYPos += iOffSetXY;

		if( GetParent() )
		{
			iXPos -= GetParent()->GetDerivedPosX();
			iYPos -= GetParent()->GetDerivedPosY();
		}

		SetWndPos( iXPos, iYPos );
		pMouse->SetToolTipCursor();
	}
}

void PrivateRulletBonusRewardTooltip::ProcessOnMainReward()
{
	ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
	if( pRullet == NULL )
		return;

	PriateRulletReward* pInfo = pRullet->GetCurrBounsReward();
	if( pInfo == NULL )
		return;

	int nStickCount = pRullet->GetRulletStickedCount();
	ioHashString szNewIconName = g_PresentMgr.GetPresentValue1Text( pInfo->m_eType, pInfo->m_nVaule1, pInfo->m_nVaule2 );
	if( m_RewardName != szNewIconName )
	{
		SAFEDELETE( m_pIcon );
		SAFEDELETE( m_pSubIcon );
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pInfo->m_Icon );
		m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pInfo->m_SubIcon );
		if( m_pIcon )
			m_pIcon->SetScale( 0.56f, 0.56f );
		if( m_pSubIcon )
			m_pSubIcon->SetScale( 0.56f, 0.56f );
		m_RewardName = szNewIconName;
	}

	if( pInfo->m_bHidden && nStickCount <= pInfo->m_nMax )
		m_bHidden = true;
	else
		m_bHidden = false;
}

void PrivateRulletBonusRewardTooltip::ProcessOnAllReward()
{
	ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
	if( pRullet == NULL )
		return;

	if( m_nStickedCount == pRullet->GetRulletStickedCount() )
		return;

	m_nStickedCount = pRullet->GetRulletStickedCount();
	DestroyAllReward();
	
	char szBuf[MAX_PATH];
	for( int i = 0; i < REWARD_MAX; ++i )
	{
		PriateRulletReward* pReward = pRullet->GetBounsReward( i );
		if( pReward )
		{
			RewardSet kSet;
			kSet.pImage = g_UIImageSetMgr.CreateImageByFullName( pReward->m_Icon );
			kSet.pSubImage = g_UIImageSetMgr.CreateImageByFullName( pReward->m_SubIcon );
			if( kSet.pImage )
				kSet.pImage->SetScale( 0.56f, 0.56f );
			if( kSet.pSubImage )
				kSet.pSubImage->SetScale( 0.56f, 0.56f );

			wsprintf_e( szBuf, "%d ~ %d개", pReward->m_nMin, pReward->m_nMax );
			kSet.szName		= szBuf;
			kSet.m_nMax		= pReward->m_nMax;
			kSet.m_bHidden	= pReward->m_bHidden;

			m_RewardSetList.push_back( kSet );
		}
	}
}

void PrivateRulletBonusRewardTooltip::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 34, iYPos + 30, UI_RENDER_MULTIPLY );

	
	if( m_bHidden )
	{
		if( m_pSecret )
			m_pSecret->Render( iXPos + 34, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else
	{
		if( m_pIcon )
			m_pIcon->Render( iXPos + 34, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_pSubIcon )
			m_pSubIcon->Render( iXPos + 34, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	
	for( int i = 0; i < REWARD_MAX; ++i )
	{
		int nWidth	= i % 3;
		int nHeight	= i / 3;

		if( COMPARE( i, 0, (int)m_RewardSetList.size() ) )
		{
			if( m_pIconBack )
				m_pIconBack->Render( iXPos + 38 + nWidth * 60, iYPos + 118 + nHeight * 79, UI_RENDER_MULTIPLY );

			if( m_RewardSetList[i].pImage )
			{				
				if( m_RewardSetList[i].m_nMax < m_nStickedCount )
				{
					m_RewardSetList[i].pImage->Render( iXPos + 38 + nWidth * 60, iYPos + 118 + nHeight * 79, UI_RENDER_GRAY, TFO_BILINEAR );
					if( m_RewardSetList[i].pSubImage )
						m_RewardSetList[i].pSubImage->Render( iXPos + 38 + nWidth * 60, iYPos + 118 + nHeight * 79, UI_RENDER_GRAY, TFO_BILINEAR );
				}
				else
				{
					if( m_RewardSetList[i].m_bHidden )
						m_pSecret->Render( iXPos + 38 + nWidth * 60, iYPos + 118 + nHeight * 79, UI_RENDER_NORMAL, TFO_BILINEAR );
					else
					{
						m_RewardSetList[i].pImage->Render( iXPos + 38 + nWidth * 60, iYPos + 118 + nHeight * 79, UI_RENDER_NORMAL, TFO_BILINEAR );
						if( m_RewardSetList[i].pSubImage )
							m_RewardSetList[i].pSubImage->Render( iXPos + 38 + nWidth * 60, iYPos + 118 + nHeight * 79, UI_RENDER_NORMAL, TFO_BILINEAR );
					}
				}				
			}

			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 38+ nWidth * 60, iYPos + 147 + nHeight * 79, FONT_SIZE_12, "%s", m_RewardSetList[i].szName.c_str() );
		}
		else
		{
			if( m_pNoneIcon )
				m_pNoneIcon->Render( iXPos + 38 + nWidth * 60, iYPos + 118 + nHeight * 79, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			g_FontMgr.PrintText( iXPos + 38 + nWidth * 60, iYPos + 147 + nHeight * 79, FONT_SIZE_12, "-" );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DevelopKBubble::DevelopKBubble() : m_CurrHP( 0 ), m_CurrBubbleIndex( 0 ), m_CurrBubbleTime( 0 ), m_StartBubbleTime( 0 )
{
	m_pBubbleTail	= NULL;
}

DevelopKBubble::~DevelopKBubble()
{
	SAFEDELETE( m_pBubbleTail );
}

void DevelopKBubble::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BubbleTail" )
	{
		SAFEDELETE( m_pBubbleTail );
		m_pBubbleTail = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}	

void DevelopKBubble::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );

	m_bReverseTail = xElement.GetBoolAttribute_e( "ReverseTail" );
}

void DevelopKBubble::iwm_show()
{
}

void DevelopKBubble::iwm_hide()
{
}

void DevelopKBubble::ProcessDevelopKBubble( bool bMustChange /*= false */ )
{
	if( !IsShow() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	//HP가 달라졌다면 표정 교체
	ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
	if( pRullet && ( pRullet->GetCurrHP() != m_CurrHP || bMustChange ) )
	{
		//HP 갱신
		m_CurrHP = pRullet->GetCurrHP();

		//말풍선 생성
		m_CurrBubbleList.clear();
		m_CurrBubbleIndex = 0;

		pRullet->GetCurrBubble( m_CurrBubbleList, m_CurrBubbleTime );

		ProcessBubbleMessage();
		m_StartBubbleTime = dwCurTime;
	}

	if( m_StartBubbleTime + m_CurrBubbleTime < dwCurTime )
	{
		m_CurrBubbleIndex++;
		if( !COMPARE( m_CurrBubbleIndex, 0, (int)m_CurrBubbleList.size() ) )
			m_CurrBubbleIndex = 0;
		
		m_StartBubbleTime = dwCurTime;
		ProcessBubbleMessage();
	}
}

void DevelopKBubble::ProcessBubbleMessage()
{
	m_CurrBubble.clear();

	int iMinWidth	= 0;
	int iMinHeight	= 0;
	if( COMPARE( m_CurrBubbleIndex, 0, (int)m_CurrBubbleList.size() ) )
	{
		m_CurrBubble = ioStringConverter::Split( m_CurrBubbleList[m_CurrBubbleIndex].c_str(), "#" );

		StringVector::iterator iter;
		for( iter = m_CurrBubble.begin() ; iter!=m_CurrBubble.end() ; ++iter )
		{
			int iCurLineWidth = g_FontMgr.GetTextWidth( iter->c_str(), TS_NORMAL, FONT_SIZE_13  );
			if( iMinWidth < iCurLineWidth )
				iMinWidth = iCurLineWidth;
		}
	}
	iMinHeight = 18 * m_CurrBubble.size();

	SetSize( min( 120, iMinWidth ) + 20, iMinHeight + 22 );

	int XPos = GetDefaultPosX();
	int YPos = GetDefaultPosY();
	SetWndPos( XPos, YPos - iMinHeight );
}

void DevelopKBubble::OnProcess( float fTimePerSec )
{
	ProcessDevelopKBubble();
}

void DevelopKBubble::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + GetWidth() / 2;
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.SetAlignType( TAT_CENTER );
		
	for( int i = 0; i < (int)m_CurrBubble.size(); ++i )
	{
		if( !m_CurrBubble[i].empty() )
		{
			g_FontMgr.PrintTextWidthCut( iXPos + 1, iYPos + 13 + 18 * i, FONT_SIZE_13, 120, m_CurrBubble[i].c_str() );
		}
	}

	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_bReverseTail )
	{
		if( m_pBubbleTail )
		{
			m_pBubbleTail->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
			m_pBubbleTail->Render( iXPos + 49, iYPos + GetHeight() - 2 );
		}
	}
	else
	{
		if( m_pBubbleTail )
		{
			m_pBubbleTail->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
			m_pBubbleTail->Render( iXPos + 30, iYPos + GetHeight() - 2, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PrivateRulletResetPopup::PrivateRulletResetPopup()
{
	m_pIconBack = NULL;
	m_pIcon		= NULL;
}

PrivateRulletResetPopup::~PrivateRulletResetPopup()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

void PrivateRulletResetPopup::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_SEND:
		if( cmd == IOBN_BTNUP )
		{
			ioPriateRullet* pRullet = g_MyInfo.GetPriateRullet();
			if( pRullet )
			{	
				pRullet->SendRulletReset();
			}
			HideWnd();
		}
		break;
	}
}

bool PrivateRulletResetPopup::iwm_spacebar()
{
	iwm_command( FindChildWnd( ID_SEND ), IOBN_BTNUP, 0 );
	return true;
}

void PrivateRulletResetPopup::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else 
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PrivateRulletResetPopup::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos + 151, iYPos + 98 );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, "사용된 검은 다시 회수되지 않습니다." );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 162, FONT_SIZE_13, "초기화 이후 다시 진행 가능합니다." );
}