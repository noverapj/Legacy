#include "stdafx.h"

#include "PriateRulletRenewalWnd.h"
#include "GlobalWnd.h"

#include "../ioPriateRullet.h"
#include "../ioPriateRenewalRullet.h"
#include "../io3DEngine/ioUI3DEffectRender.h"

RenewalPriateNewStartButton::RenewalPriateNewStartButton()
{
	Clear();
}

RenewalPriateNewStartButton::~RenewalPriateNewStartButton()
{
}

void RenewalPriateNewStartButton::Clear()
{
	m_dwBlinkAnimationStart	= 0;
	m_nBlinkCount			= 0;
}

void RenewalPriateNewStartButton::iwm_show()
{
}

void RenewalPriateNewStartButton::iwm_hide()
{
}

void RenewalPriateNewStartButton::OnProcess( float fTimePerSec )
{
	ProcessBlinkAnimation();
}

void RenewalPriateNewStartButton::BlinkAnimationStart()
{
	m_dwBlinkAnimationStart	= FRAMEGETTIME();
	m_nBlinkCount			= 0;
}

void RenewalPriateNewStartButton::ProcessBlinkAnimation()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwBlinkAnimationStart && m_dwBlinkAnimationStart + 100 < dwCurTime )
	{
		m_nBlinkCount++;
		m_dwBlinkAnimationStart = dwCurTime;
	}
}
void RenewalPriateNewStartButton::OnRender()
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

RenewalPrivateRulletRewardButton::RenewalPrivateRulletRewardButton() :
	m_CurrMax( 0 ),
	m_CurrMin( 0 ),
	m_CurrHP( 0 ),
	m_dwStampBlinkAnimationStart( 0 ),
	m_fScaleRate( 0.0f ),
	m_fCurrScaleRate( 0.0f ),
	m_dwSAlphaAnimationStart( 0 ),
	m_fCurrAlphaRate( FLOAT1 ),
	m_bReward( false ),
	m_bGray( false ),
	m_dwFailEffectStart( 0 ),
	m_dwRollingEffectStart( 0 ),
	m_dwRollingIconStart( 0 ),
	m_dwRewardEffectStart( 0 ),
	m_bAccept( false ),
	m_iRollingEffectIndex( 0 ),
	m_iRollingIconIndex( 0 )
{
	m_pAcceptIcon		= NULL;
	m_pSubIcon = NULL;
	m_pSecret			= NULL;
	for ( int i=0 ; i<FAIL_EFFECT_MAX ; ++i )
		m_FailEffect[i] = NULL;

	for ( int i=0 ; i<ROLLING_EFFECT_MAX ; ++i )
		m_RollingEffect[i] = NULL;

	for ( int i=0 ; i<REWARD_EFFECT_MAX ; ++i )
		m_RewardEffect[i] = NULL;
}

RenewalPrivateRulletRewardButton::~RenewalPrivateRulletRewardButton()
{
	SAFEDELETE( m_pAcceptIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pSecret );
	for ( int i=0 ; i<FAIL_EFFECT_MAX ; ++i )
		SAFEDELETE( m_FailEffect[i] );

	for ( int i=0 ; i<ROLLING_EFFECT_MAX ; ++i )
		SAFEDELETE( m_RollingEffect[i] );

	for ( int i=0 ; i<REWARD_EFFECT_MAX ; ++i )
		SAFEDELETE( m_RewardEffect[i] );
}

void RenewalPrivateRulletRewardButton::iwm_show()
{

}

void RenewalPrivateRulletRewardButton::iwm_hide()
{
	m_dwSAlphaAnimationStart	 = 0;
	m_dwStampBlinkAnimationStart = 0;
	m_nStampBlinkCount			 = 0;
	m_fCurrAlphaRate			 = FLOAT1;
	m_fCurrScaleRate			 = FLOAT1;
}

void RenewalPrivateRulletRewardButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Secret" )
	{
		SAFEDELETE( m_pSecret );
		m_pSecret = pImage;
		if( m_pSecret )
			m_pSecret->SetScale( 0.6f, 0.6f );
	}
	else if( szType == "Effect_Fail01" )
	{
		SAFEDELETE( m_FailEffect[0] );
		m_FailEffect[0] = pImage;
		m_FailEffect[0]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Fail02" )
	{
		SAFEDELETE( m_FailEffect[1] );
		m_FailEffect[1] = pImage;
		m_FailEffect[1]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Fail03" )
	{
		SAFEDELETE( m_FailEffect[2] );
		m_FailEffect[2] = pImage;
		m_FailEffect[2]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Fail04" )
	{
		SAFEDELETE( m_FailEffect[3] );
		m_FailEffect[3] = pImage;
		m_FailEffect[3]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Fail05" )
	{
		SAFEDELETE( m_FailEffect[4] );
		m_FailEffect[4] = pImage;
		m_FailEffect[4]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Fail06" )
	{
		SAFEDELETE( m_FailEffect[5] );
		m_FailEffect[5] = pImage;
		m_FailEffect[5]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Fail07" )
	{
		SAFEDELETE( m_FailEffect[6] );
		m_FailEffect[6] = pImage;
		m_FailEffect[6]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Fail08" )
	{
		SAFEDELETE( m_FailEffect[7] );
		m_FailEffect[7] = pImage;
		m_FailEffect[7]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Fail09" )
	{
		SAFEDELETE( m_FailEffect[8] );
		m_FailEffect[8] = pImage;
		m_FailEffect[8]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Fail10" )
	{
		SAFEDELETE( m_FailEffect[9] );
		m_FailEffect[9] = pImage;
		m_FailEffect[9]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Rolling01" )
	{
		SAFEDELETE( m_RollingEffect[0] );
		m_RollingEffect[0] = pImage;
		m_RollingEffect[0]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Rolling02" )
	{
		SAFEDELETE( m_RollingEffect[1] );
		m_RollingEffect[1] = pImage;
		m_RollingEffect[1]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Rolling03" )
	{
		SAFEDELETE( m_RollingEffect[2] );
		m_RollingEffect[2] = pImage;
		m_RollingEffect[2]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Rolling04" )
	{
		SAFEDELETE( m_RollingEffect[3] );
		m_RollingEffect[3] = pImage;
		m_RollingEffect[3]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Rolling05" )
	{
		SAFEDELETE( m_RollingEffect[4] );
		m_RollingEffect[4] = pImage;
		m_RollingEffect[4]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Rolling06" )
	{
		SAFEDELETE( m_RollingEffect[5] );
		m_RollingEffect[5] = pImage;
		m_RollingEffect[5]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Rolling07" )
	{
		SAFEDELETE( m_RollingEffect[6] );
		m_RollingEffect[6] = pImage;
		m_RollingEffect[6]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Rolling08" )
	{
		SAFEDELETE( m_RollingEffect[7] );
		m_RollingEffect[7] = pImage;
		m_RollingEffect[7]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Rolling09" )
	{
		SAFEDELETE( m_RollingEffect[8] );
		m_RollingEffect[8] = pImage;
		m_RollingEffect[8]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Reward01" )
	{
		SAFEDELETE( m_RewardEffect[0] );
		m_RewardEffect[0] = pImage;
		m_RewardEffect[0]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Reward02" )
	{
		SAFEDELETE( m_RewardEffect[1] );
		m_RewardEffect[1] = pImage;
		m_RewardEffect[1]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Reward03" )
	{
		SAFEDELETE( m_RewardEffect[2] );
		m_RewardEffect[2] = pImage;
		m_RewardEffect[2]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Reward04" )
	{
		SAFEDELETE( m_RewardEffect[3] );
		m_RewardEffect[3] = pImage;
		m_RewardEffect[3]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Reward05" )
	{
		SAFEDELETE( m_RewardEffect[4] );
		m_RewardEffect[4] = pImage;
		m_RewardEffect[4]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Reward06" )
	{
		SAFEDELETE( m_RewardEffect[5] );
		m_RewardEffect[5] = pImage;
		m_RewardEffect[5]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Reward07" )
	{
		SAFEDELETE( m_RewardEffect[6] );
		m_RewardEffect[6] = pImage;
		m_RewardEffect[6]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Reward08" )
	{
		SAFEDELETE( m_RewardEffect[7] );
		m_RewardEffect[7] = pImage;
		m_RewardEffect[7]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Reward09" )
	{
		SAFEDELETE( m_RewardEffect[8] );
		m_RewardEffect[8] = pImage;
		m_RewardEffect[8]->SetScale( 0.75f );
	}
	else if( szType == "Effect_Reward10" )
	{
		SAFEDELETE( m_RewardEffect[9] );
		m_RewardEffect[9] = pImage;
		m_RewardEffect[9]->SetScale( 0.75f );
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void RenewalPrivateRulletRewardButton::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );
	m_iEffectXPos	= xElement.GetIntAttribute_e( "EffectXPos" );
	m_iEffectYPos	= xElement.GetIntAttribute_e( "EffectYPos" );
}

void RenewalPrivateRulletRewardButton::SetRewardInfo( int Index )
{
	ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
	if( pRullet == NULL )
	{
		HideWnd();
		return;
	}

	m_dwStampBlinkAnimationStart	= 0;
	m_nStampBlinkCount				= 0;
	m_fCurrAlphaRate				= FLOAT1;
	m_dwSAlphaAnimationStart		= 0;
	m_dwRewardEffectStart			= 0;
	m_dwRollingEffectStart			= 0;
	m_dwRollingIconStart			= 0;
	m_dwFailEffectStart				= 0;

	m_iRollingEffectIndex			= 0;
	m_iRollingIconIndex				= 0;
	m_iRewardEffectIndex			= 0;

	SAFEDELETE( m_pAcceptIcon );
	SAFEDELETE( m_pSubIcon );
	for ( int i=0 ; i<(int)m_vRollingIcon.size() ; ++i )
		SAFEDELETE(m_vRollingIcon[i]);
	m_vRollingIcon.clear();
	for ( int i=0 ; i<(int)m_vSubIcon.size() ; ++i )
		SAFEDELETE(m_vSubIcon[i]);
	m_vSubIcon.clear();

	PriateRenewalRewardInfo* pInfo = NULL;
	pInfo = pRullet->GetReward( Index );
	if( pInfo == NULL )
		return;

	m_CurrMax = pInfo->m_iMax;
	m_bAccept = false;
	if( pRullet->GetCurEnableRewardTurnNumber() >= m_CurrMax )
	{
		//보상 획득 가능 상태
		m_bGray				= false;

		m_bReward			= true;
		m_fCurrScaleRate	= FLOAT1;
	}
	else
	{
		m_bGray				= true;
		m_bReward			= false;
		m_fCurrScaleRate	= 0.0f;
	}
		

	if( pInfo == NULL )
	{
		HideWnd();
		return;
	}
	/*
	SAFEDELETE( m_pIcon );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pInfo->m_Icon );
	if( m_pIcon == NULL )
	{
		HideWnd();
		return;
	}

	if( m_pIcon )
		m_pIcon->SetScale( 0.62f, 0.62f );

	m_Name = g_PresentMgr.GetPresentValue1Text( pInfo->m_eType, pInfo->m_nVaule1, pInfo->m_nVaule2 );
	*/
	ShowWnd();
}

bool RenewalPrivateRulletRewardButton::IsEnableStampAnimation( int iValue )
{
	if( !m_bReward && iValue >= m_CurrMax )
		return true;

	return false;
}

void RenewalPrivateRulletRewardButton::StampAnimationStart( int nRewardCount )
{
	if( 0 >= nRewardCount )
		return;

	m_fCurrScaleRate	= FLOAT1;
	m_bGray				= false;
	m_bReward			= true;
}

void RenewalPrivateRulletRewardButton::StampBlinkAnimationStart()
{
	m_dwStampBlinkAnimationStart	= FRAMEGETTIME();
	m_nStampBlinkCount				= 0;
}

void RenewalPrivateRulletRewardButton::ProcessBlinkAnimation()
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

bool RenewalPrivateRulletRewardButton::IsEnableStampBlinkAnimation()
{
	if( m_bReward )
		return true;

	return false;
}

void RenewalPrivateRulletRewardButton::StartAlphaAnimation()
{
	m_dwSAlphaAnimationStart	 = FRAMEGETTIME();
	m_fCurrAlphaRate			 = 0.0f;
	m_fCurrScaleRate			 = FLOAT1;
	m_dwStampBlinkAnimationStart = 0;
	m_nStampBlinkCount			 = 0;
}

void RenewalPrivateRulletRewardButton::PorcessAlphaAnimation()
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

void RenewalPrivateRulletRewardButton::OnProcess( float fTimePerSec )
{
	ProcessBlinkAnimation();
	PorcessAlphaAnimation();
	ProcessFailEffectAnimation();
	ProcessSuccessEffectAnimation();
	ProcessRollingAnimation();
}

void RenewalPrivateRulletRewardButton::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_bGray )
	{
		if ( m_pSecret )
		{
			m_pSecret->SetAlpha( MAX_ALPHA_RATE );
			m_pSecret->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_GRAY, TFO_BILINEAR );
		}
	}
	else 
	{
		if ( m_bAccept )
		{
			if ( m_bAccept && m_pAcceptIcon )
			{
				m_pAcceptIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );
				if( m_pSubIcon )
					m_pSubIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
		else
		{
			if( m_pSecret )
			{
				m_pSecret->SetAlpha( MAX_ALPHA_RATE );
				m_pSecret->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}		
	}

	if ( m_dwFailEffectStart > 0 && m_FailEffect[m_iFailIndex] )
		m_FailEffect[m_iFailIndex]->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_SCREEN );

	if ( m_dwRollingIconStart > 0 && m_vRollingIcon[m_iRollingIconIndex] )
		m_vRollingIcon[m_iRollingIconIndex]->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );
	if ( m_dwRollingIconStart > 0 && m_vSubIcon[m_iRollingIconIndex] )
		m_vSubIcon[m_iRollingIconIndex]->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL, TFO_BILINEAR );

	if ( m_dwRollingEffectStart > 0 && m_RollingEffect[m_iRollingEffectIndex] )
		m_RollingEffect[m_iRollingEffectIndex]->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_SCREEN );

	if ( m_dwRewardEffectStart > 0 && m_RewardEffect[m_iRewardEffectIndex] )
		m_RewardEffect[m_iRewardEffectIndex]->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_SCREEN );

	

	
	/*
	//리뉴얼
	if ( IsOvered() )
	{
		if ( m_bHidden )
		{
			if ( m_pSecret )
			{
				m_pSecret->SetAlpha( MAX_ALPHA_RATE );
				m_pSecret->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_GRAY, TFO_BILINEAR );
			}
		}
		else
		{
			if ( m_pIcon )
			{
				m_pIcon->SetAlpha( MAX_ALPHA_RATE );
				m_pIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_SCREEN );
			}
		}
	}
	*/
}

void RenewalPrivateRulletRewardButton::FailAnimationStart()
{
	if ( !m_bReward )
		return;

	m_bGray				= true;
	m_bReward			= false;
	m_dwFailEffectStart = FRAMEGETTIME();
	m_iFailIndex = 0;
}

void RenewalPrivateRulletRewardButton::ProcessFailEffectAnimation()
{
	if ( m_dwFailEffectStart > 0 && m_dwFailEffectStart + 100 < FRAMEGETTIME() )
	{
		m_dwFailEffectStart = FRAMEGETTIME();
		m_iFailIndex++;
		if ( m_iFailIndex >= FAIL_EFFECT_MAX  )
		{
			m_dwFailEffectStart = 0;
			m_iFailIndex = 0;
		}
	}
}

void RenewalPrivateRulletRewardButton::SuccessAnimationStart( int iIndex, int iRewardIndex )
{
	if ( !m_bReward )
		return;

	ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
	if( pRullet == NULL )
	{
		HideWnd();
		return;
	}

	PriateRenewalRewardInfo* pInfo = NULL;
	pInfo = pRullet->GetReward( iIndex );
	if( pInfo == NULL )
	{
		HideWnd();
		return;
	}

	
	if ( iRewardIndex < 0 || iRewardIndex >= (int)pInfo->m_PresentDataList.size() )
	{
		HideWnd();
		return;
	}

	SAFEDELETE( m_pAcceptIcon );
	SAFEDELETE( m_pSubIcon );
	m_pAcceptIcon = g_UIImageSetMgr.CreateImageByFullName( pInfo->m_PresentDataList[iRewardIndex].m_Icon );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pInfo->m_PresentDataList[iRewardIndex].m_SubIcon );
	if ( !m_pAcceptIcon )
	{
		HideWnd();
		return;
	}

	m_pAcceptIcon->SetScale( 0.6f );
	if( m_pSubIcon )
		m_pSubIcon->SetScale( 0.6f );
	m_bAccept = true;
		
	m_Name = pInfo->m_PresentDataList[iRewardIndex].m_Name;

	m_dwRewardEffectStart = FRAMEGETTIME();
	m_iRewardEffectIndex = 0;

	m_dwRollingEffectStart = 0;
	m_iRollingEffectIndex = 0;
	m_dwRollingIconStart = 0;
	m_iRollingIconIndex = 0;
	for ( int i=0 ; i<(int)m_vRollingIcon.size() ; ++i )
		SAFEDELETE(m_vRollingIcon[i]);
	m_vRollingIcon.clear();

	for ( int i=0 ; i<(int)m_vSubIcon.size() ; ++i )
		SAFEDELETE(m_vSubIcon[i]);
	m_vSubIcon.clear();
}

void RenewalPrivateRulletRewardButton::ProcessSuccessEffectAnimation()
{
	if ( m_dwRewardEffectStart > 0 && m_dwRewardEffectStart + 100 < FRAMEGETTIME() )
	{
		m_dwRewardEffectStart = FRAMEGETTIME();
		m_iRewardEffectIndex++;
		if ( m_iRewardEffectIndex >= REWARD_EFFECT_MAX  )
		{
			m_dwRewardEffectStart = 0;
			m_iRewardEffectIndex = 0;
		}
		else if ( m_iRewardEffectIndex < 5 )
		{
			if( m_pAcceptIcon )
				m_pAcceptIcon->SetScale( FLOAT05 );
			if( m_pSubIcon )
				m_pSubIcon->SetScale( FLOAT05 );
		}
		else
		{
			if( m_pAcceptIcon )
				m_pAcceptIcon->SetScale( 0.6f );
			if( m_pSubIcon )
				m_pSubIcon->SetScale( 0.6f );
		}
	}
}

int RenewalPrivateRulletRewardButton::GetEffectYPos()
{
	return m_iEffectXPos;
}

int RenewalPrivateRulletRewardButton::GetEffectXPos()
{
	return m_iEffectYPos;
}

void RenewalPrivateRulletRewardButton::RollingAnimationStart( int iIndex )
{
	ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
	if( pRullet == NULL )
	{
		HideWnd();
		return;
	}

	if ( !m_bReward )
		return;

	PriateRenewalRewardInfo* pInfo = NULL;
	pInfo = pRullet->GetReward( iIndex );
	if( pInfo == NULL )
		return;

	for ( int i=0; i <(int)pInfo->m_PresentDataList.size() ; ++i )
	{
		ioUIRenderImage* pRollingIcon = g_UIImageSetMgr.CreateImageByFullName( pInfo->m_PresentDataList[i].m_Icon );
		ioUIRenderImage* pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pInfo->m_PresentDataList[i].m_SubIcon );
		if ( pRollingIcon )
		{
			pRollingIcon->SetScale( 0.6f, 0.6f );
			m_vRollingIcon.push_back( pRollingIcon );

			if( pSubIcon )
				pSubIcon->SetScale( 0.6f, 0.6f );
			m_vSubIcon.push_back( pSubIcon );
		}
	}
	
	if ( m_vRollingIcon.empty() )
	{
		HideWnd();
		return;
	}

	m_dwRollingEffectStart = m_dwRollingIconStart = FRAMEGETTIME();
	m_iRollingIconIndex = m_iRollingEffectIndex = 0;
}

void RenewalPrivateRulletRewardButton::ProcessRollingAnimation()
{
	if ( m_dwRollingEffectStart > 0 && m_dwRollingEffectStart + 100 < FRAMEGETTIME() )
	{
		m_dwRollingEffectStart = FRAMEGETTIME();
		m_iRollingEffectIndex++;
		if ( m_iRollingEffectIndex >= ROLLING_EFFECT_MAX  )
			m_iRollingEffectIndex = 0;
	}

	if ( m_dwRollingIconStart > 0 && m_dwRollingIconStart + 200 < FRAMEGETTIME() )
	{
		m_dwRollingIconStart = FRAMEGETTIME();
		m_iRollingIconIndex++;
		if ( m_iRollingIconIndex >= (int)m_vRollingIcon.size() )
			m_iRollingIconIndex = 0;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RenewalOakHoleButton::RenewalOakHoleButton() : 
	m_Index( 0 ),
	m_bSticked( false ),
	m_AnimationType( AT_NONE ),
	m_dwAnimationTime( 0 ),
	m_eCritical( OCT_NONE ),
	m_dwSAlphaAnimationStart( 0 ),
	m_fCurrAlphaRate( FLOAT1 )
{
	m_OverSword				= NULL;

	m_WoodClickSword		= NULL;
	m_WoodClickMoveSword	= NULL;
	m_WoodStickedSword		= NULL;
}

RenewalOakHoleButton::~RenewalOakHoleButton()
{
	SAFEDELETE( m_OverSword );

	SAFEDELETE( m_WoodClickSword );
	SAFEDELETE( m_WoodClickMoveSword );
	SAFEDELETE( m_WoodStickedSword );
}

void RenewalOakHoleButton::iwm_show()
{

}

void RenewalOakHoleButton::iwm_hide()
{

}

void RenewalOakHoleButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "OverSword" )
	{
		SAFEDELETE( m_OverSword );
		m_OverSword = pImage;
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
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void RenewalOakHoleButton::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	m_StickedWoodSound			= xElement.GetStringAttribute_e( "StickedWoodSound" );
	m_DamageSound				= xElement.GetStringAttribute_e( "DamageSound" );
	m_EndDamageSound			= xElement.GetStringAttribute_e( "EndDamageSound" );
	m_nEffectXPos				= xElement.GetIntAttribute_e( "EffectXPos" );
	m_nEffectYPos				= xElement.GetIntAttribute_e( "EffectYPos" );
}

int RenewalOakHoleButton::GetEffectXPos()
{
	return m_nEffectXPos;
}

int RenewalOakHoleButton::GetEffectYPos()
{
	return m_nEffectYPos;
}

void RenewalOakHoleButton::SetOakHole( int Index )
{	
	m_Index = Index;
}

void RenewalOakHoleButton::AnimationStart()
{
	m_AnimationType		= AT_START;
	m_dwAnimationTime	= FRAMEGETTIME();
	m_eCritical			= OCT_NONE;
}

void RenewalOakHoleButton::StartAlphaAnimation()
{
	m_dwSAlphaAnimationStart = FRAMEGETTIME();
	m_fCurrAlphaRate		 = 0.0f;
}

void RenewalOakHoleButton::PorcessAlphaAnimation()
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
			m_fCurrAlphaRate = FLOAT1 -(((float)dwCurTime - m_dwSAlphaAnimationStart) / (float)400);
			m_fCurrAlphaRate = max( 0.0f, m_fCurrAlphaRate );
		}
	}
}

void RenewalOakHoleButton::ProcessStickAnimation()
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
				int nHP = 0;
				ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
				if( pRullet )
					nHP = pRullet->GetCurEnableRewardTurnNumber();


				g_SoundMgr.PlaySound( m_StickedWoodSound );
				if( nHP <= 0 )
					g_SoundMgr.PlaySound( m_EndDamageSound );
				else
					g_SoundMgr.PlaySound( m_DamageSound );
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
		}
		break;
	}
}

void RenewalOakHoleButton::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
	
	UpdateStick();
	PorcessAlphaAnimation();
	ProcessStickAnimation();
}

void RenewalOakHoleButton::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );
}

void RenewalOakHoleButton::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );
	OnRenderOver( iXPos, iYPos );
}

void RenewalOakHoleButton::OnRenderOver( int iXPos, int iYPos )
{
	if( m_bSticked )
	{
		if( m_WoodStickedSword )
			m_WoodStickedSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else
	{
		if( m_OverSword && m_AnimationType == AT_NONE )
			m_OverSword->Render( iXPos, iYPos, UI_RENDER_ADD, TFO_BILINEAR );
	}
}

void RenewalOakHoleButton::OnRenderPushAnimation( int iXPos, int iYPos )
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

void RenewalOakHoleButton::OnStickedRender( int iXPos, int iYPos )
{
	if( m_bSticked )
	{
		if( m_WoodStickedSword )
		{
			m_WoodStickedSword->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurrAlphaRate );
			m_WoodStickedSword->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}

void RenewalOakHoleButton::OnRender()
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

void RenewalOakHoleButton::UpdateStick()
{

	if( 0 < m_dwSAlphaAnimationStart )
		return;

	if ( m_AnimationType != AT_NONE )
		return;

	m_bSticked			= false;

	ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
	if( pRullet && pRullet->GetRulletStickedValue( m_Index ) && !m_bSticked )
		m_bSticked			= true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PriateRulletRenewalWnd::PriateRulletRenewalWnd() : 
	m_RulletState( RS_START ),
	m_DevelopKState( DK_NONE ),
	m_StickedSwordCode( 0 ),
	m_dwPreStartTime( 0 ),
	m_dwPreEndStartTime( 0 ),
	m_dwEndStartTime( 0 ),
	m_CurrState( -1 ),
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
	m_dwCriticalEffectStart( 0 ),
	m_iStickedSwordCount( 0 ),
	m_dwClearStampLoopTime( 0 ),
	m_dwClearStampStartTime( 0 ),
	m_dwEnableRewardEffectStart( 0 ),
	m_dwFailStartTime( 0 ),
	m_iCurPage( 0 )
{
	m_OakImageCap			= NULL;
	m_OakImageBody			= NULL;
	m_DevelopK				= NULL;
	m_RewardMap				= NULL;
	m_FailMap				= NULL;
	m_SwordBar				= NULL;
	m_BubbleRight			= NULL;
	m_BubbleLeft			= NULL;
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

	m_pCriticalEffect		= NULL;
	m_pEnableRewardEffect	= NULL;

	m_WoodSword				= NULL;
	m_ClearStamp			= NULL;
	m_pFailAcceptIcon		= NULL;
	m_pSubIcon		= NULL;
}

PriateRulletRenewalWnd::~PriateRulletRenewalWnd()
{
	SAFEDELETE( m_OakImageCap );
	SAFEDELETE( m_OakImageBody );
	SAFEDELETE( m_DevelopK );
	SAFEDELETE( m_RewardMap );
	SAFEDELETE( m_FailMap );
	SAFEDELETE( m_SwordBar );
	SAFEDELETE( m_BubbleRight );
	SAFEDELETE( m_BubbleLeft );
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
	SAFEDELETE( m_WoodSword );
	SAFEDELETE( m_ClearStamp );
	SAFEDELETE( m_pFailAcceptIcon );
	SAFEDELETE( m_pSubIcon );

	for( int i = 0; i < EFFECT_MAX; ++i )
		SAFEDELETE( m_FireWorkEffect[i] );
}

void PriateRulletRenewalWnd::iwm_show()
{
	m_FaceState					= FS_NORMAL;

	m_CurrState					= -1;
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
	m_dwCriticalEffectStart		= 0;
	m_dwEnableRewardEffectStart = 0;

	m_pCriticalEffect			= GetEffect("CriticalEffect");
	m_pEnableRewardEffect		= GetEffect("RewardEffect");
	m_iStickedSwordCount = 0;
	m_dwFailStartTime = 0;

	ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
	if ( pRullet )
	{
		//단검은 하나로 고정
		m_StickedSwordCode = pRullet->GetSwordCode();
	}
	
	if( pRullet && pRullet->GetCurEnableRewardTurnNumber() > 0 )
		SetState( RS_PLAY );
	else
		SetState( m_RulletState );
}

void PriateRulletRenewalWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else if( szType == "RewardMap" )
	{
		SAFEDELETE( m_RewardMap );
		m_RewardMap = pImage;
	}
	else if( szType == "FailMap" )
	{
		SAFEDELETE( m_FailMap );
		m_FailMap = pImage;
	}
	else if( szType == "SwordBar" )
	{
		SAFEDELETE( m_SwordBar );
		m_SwordBar = pImage;
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
	//리뉴얼
	else if( szType == "WoodSword" )
	{
		SAFEDELETE( m_WoodSword );
		m_WoodSword = pImage;
		if ( m_WoodSword )
			m_WoodSword->SetScale( 0.69f );
	}
	else if ( szType == "ClearStamp")
	{
		SAFEDELETE( m_ClearStamp );
		m_ClearStamp = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PriateRulletRenewalWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BubbleRight" )
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

void PriateRulletRenewalWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );

	m_ProvocationSound		= xElement.GetStringAttribute_e( "ProvocationSound" );
	m_RewardSound			= xElement.GetStringAttribute_e( "RewardSound" );
	m_DevelopKSound			= xElement.GetStringAttribute_e( "DevelopKSound" );
	m_FailSound				= xElement.GetStringAttribute_e( "FailSound" );
	m_RollingSound			= xElement.GetStringAttribute_e( "RollingSound" );
	m_ClearStampSound		= xElement.GetStringAttribute_e( "ClearStampSound" );
	m_fClearStampScaleSpeed = xElement.GetFloatAttribute_e( "ClearStampScaleSpeed" );
	m_dwClearStampLoopTime	= (DWORD)xElement.GetIntAttribute_e( "ClearStampLoopTime" );
	m_iMaxPage				= xElement.GetIntAttribute_e( "HelpMaxPage" );
}

void PriateRulletRenewalWnd::SetState( RulletState eRulletState, bool bAnimation /*= true */ )
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
			HideRollingState();
			HideFailState();

			ShowStartState();
		}
		break;
	case RS_PRE:
		{
			HideStartState();
			HideHelpState();
			HidePreEndState();
			HideEndState();
			HideRollingState();
			HideRollingEndState();
			HideFailState();

			ShowPreState();
		}
		break;
	case RS_PLAY:
		{
			HideStartState();
			HideHelpState();
			HidePreEndState();
			HideEndState();
			HideRollingState();
			HideRollingEndState();
			HideFailState();

			ShowPlayState();
		}
		break;
	case RS_HELP:
		{
			HideStartState();
			HidePlayState();
			HidePreEndState();
			HideEndState();
			HideRollingState();
			HideRollingEndState();
			HideFailState();

			ShowHelpState();
		}
		break;
	case RS_PRE_END:
		{
			HideStartState();
			HideHelpState();
			HidePlayState();
			HideEndState();
			HideRollingState();
			HideRollingEndState();
			HideFailState();
			ShowPreEndState( bAnimation );
		}
		break;
	case RS_ROLLING:
		{
			HideStartState();
			HideHelpState();
			HideFailState();

			ShowRollingState();
		}
		break;
	case RS_ROLLING_END:
		{
			HideStartState();
			HideHelpState();
			HideFailState();

			ShowRollingEndState();
		}
		break;
	case RS_END:
		{
			HideStartState();
			HidePlayState();
			HidePreEndState();
			HideRollingState();
			HideRollingEndState();
			HideFailState();

			ShowEndState( bAnimation );
		}
		break;
	case RS_FAIL:
		{
			ShowFailState( bAnimation );
		}
		break;
	}

	UpdateSwordCount();
}

void PriateRulletRenewalWnd::ShowStartState()
{
	ShowChildWnd( ID_GAME_RULE_TITLE );
	ShowChildWnd( ID_GAME_RULE_SUB_TITLE );
	ShowChildWnd( ID_GAME_RULE_RICH_LABEL1 );
	ShowChildWnd( ID_GAME_RULE_RICH_LABEL2 );
	ShowChildWnd( ID_GAME_RULE_RICH_LABEL3 );
	ShowChildWnd( ID_START );
}

void PriateRulletRenewalWnd::HideStartState()
{
	HideChildWnd( ID_GAME_RULE_TITLE );
	HideChildWnd( ID_GAME_RULE_SUB_TITLE );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL1 );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL2 );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL3 );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL4 );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL5 );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL6 );
	HideChildWnd( ID_START );
}

void PriateRulletRenewalWnd::ShowPreState()
{
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON11; ++dwID )
	{	
		RenewalPrivateRulletRewardButton* pButton = dynamic_cast<RenewalPrivateRulletRewardButton*>( FindChildWnd( dwID ) );
		if( pButton )
			pButton->SetRewardInfo( dwID - ID_REWARD_BUTTON01 );
	}

	ShowChildWnd( ID_HELP_BUTTON );
	ShowChildWnd( ID_ACCEPT_REWARD );

	m_dwPreStartTime		= FRAMEGETTIME();
}

void PriateRulletRenewalWnd::ShowPlayState()
{
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON11; ++dwID )
	{	
		RenewalPrivateRulletRewardButton* pButton = dynamic_cast<RenewalPrivateRulletRewardButton*>( FindChildWnd( dwID ) );
		if( pButton )
			pButton->SetRewardInfo( dwID - ID_REWARD_BUTTON01 );
	}

	CheckOakHole();

	ShowChildWnd( ID_HELP_BUTTON );
	ShowChildWnd( ID_ACCEPT_REWARD );

	
	ShowChildWnd( ID_RIGHT_BUBBLE );

	InitDevelopKAnimation();

	m_FaceState					= FS_PROVOKE;
	m_dwFaceAnimationStartTime	= FRAMEGETTIME();
	g_SoundMgr.PlaySound( m_ProvocationSound );
}

void PriateRulletRenewalWnd::HidePlayState()
{
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON11; ++dwID )
	{
		HideChildWnd( dwID );
	}

	HideChildWnd( ID_HELP_BUTTON );
	HideChildWnd( ID_ACCEPT_REWARD );

	HideChildWnd( ID_RIGHT_BUBBLE );
}

void PriateRulletRenewalWnd::HidePlayOakPart()
{
	for( DWORD dwID = ID_OAK_HOLE_BUTTON01; dwID <= ID_OAK_HOLE_BUTTON12; ++dwID )
	{
		HideChildWnd( dwID );
	}
}

void PriateRulletRenewalWnd::ShowHelpState()
{
	ShowChildWnd( ID_GAME_RULE_TITLE );
	ShowChildWnd( ID_GAME_RULE_SUB_TITLE );
	ShowChildWnd( ID_GAME_RULE_RICH_LABEL1 );
	ShowChildWnd( ID_GAME_RULE_RICH_LABEL2 );
	ShowChildWnd( ID_GAME_RULE_RICH_LABEL3 );
	ShowChildWnd( ID_RETURN_PLAY_BUTTON );
	//ShowChildWnd( ID_HELP_PRE_BTN );
	ShowChildWnd( ID_HELP_NEXT_BTN );
}

void PriateRulletRenewalWnd::HideHelpState()
{
	HideChildWnd( ID_GAME_RULE_TITLE );
	HideChildWnd( ID_GAME_RULE_SUB_TITLE );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL1 );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL2 );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL3 );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL4 );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL5 );
	HideChildWnd( ID_GAME_RULE_RICH_LABEL6 );
	HideChildWnd( ID_RETURN_PLAY_BUTTON );
	HideChildWnd( ID_HELP_PRE_BTN );
	HideChildWnd( ID_HELP_NEXT_BTN );
}

void PriateRulletRenewalWnd::ShowPreEndState( bool bAnimation )
{
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON11; ++dwID )
	{	
		RenewalPrivateRulletRewardButton* pButton = dynamic_cast<RenewalPrivateRulletRewardButton*>( FindChildWnd( dwID ) );
		if( pButton )
			pButton->SetRewardInfo( dwID - ID_REWARD_BUTTON01 );
	}

	RenewalPriateNewStartButton* pNewStartButton = dynamic_cast<RenewalPriateNewStartButton*>( FindChildWnd( ID_NEW_START ) );
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

void PriateRulletRenewalWnd::HidePreEndState()
{
	HideChildWnd( ID_NEW_START );
}

void PriateRulletRenewalWnd::ShowEndState( bool bAnimation )
{
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON11; ++dwID )
	{	
		RenewalPrivateRulletRewardButton* pButton = dynamic_cast<RenewalPrivateRulletRewardButton*>( FindChildWnd( dwID ) );
		if( pButton )
		{	
			if( bAnimation )
			{
				pButton->ShowWnd();
				pButton->StartAlphaAnimation();
			}
			else
			{
				pButton->SetRewardInfo( dwID - ID_REWARD_BUTTON01 );
			}
			
		}
	}

	for( DWORD dwID = ID_OAK_HOLE_BUTTON01; dwID <= ID_OAK_HOLE_BUTTON12; ++dwID )
	{
		RenewalOakHoleButton* pButton = dynamic_cast<RenewalOakHoleButton*>( FindChildWnd( dwID ) );
		if( pButton )
		{
			DWORD dwIndex = dwID - ID_OAK_HOLE_BUTTON01;
			pButton->SetOakHole( dwIndex );
			pButton->ShowWnd();
			if( bAnimation )
			{
				pButton->StartAlphaAnimation();
			}
		}
	}

	ShowChildWnd( ID_HELP_BUTTON );
	ShowChildWnd( ID_ACCEPT_REWARD );

	if( bAnimation )
		m_dwEndStartTime = FRAMEGETTIME();
}

void PriateRulletRenewalWnd::HideEndState()
{
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON11; ++dwID )
	{
		HideChildWnd( dwID );
	}
	HideChildWnd( ID_HELP_BUTTON );
	HideChildWnd( ID_ACCEPT_REWARD );
}

void PriateRulletRenewalWnd::ShowRollingState()
{
	/*
	ioPriateRenewalRullet *pRullet = g_MyInfo.GetPriateRenewalRullet();
	if ( !pRullet )
		return;
		*/
	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON11; ++dwID )
	{	
		RenewalPrivateRulletRewardButton* pButton = dynamic_cast<RenewalPrivateRulletRewardButton*>( FindChildWnd( dwID ) );
		if( pButton )
			pButton->RollingAnimationStart( dwID - ID_REWARD_BUTTON01 );
	}

	HideChildWnd( ID_ACCEPT_REWARD );
	ShowChildWnd( ID_CHECKING );
	g_SoundMgr.PlaySound( m_RollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
	ioWnd *pButton = FindChildWnd(ID_CHECKING);
	//if ( pButton )
//		pButton->SetInActive();
}

void PriateRulletRenewalWnd::HideRollingState()
{
	HideChildWnd( ID_CHECKING );
}

void PriateRulletRenewalWnd::ShowRollingEndState()
{
	HideChildWnd( ID_ACCEPT_REWARD );
	HideChildWnd( ID_HELP_BUTTON );
	HideChildWnd( ID_ACCEPT_REWARD );
	
	RenewalPriateNewStartButton* pNewStartButton = dynamic_cast<RenewalPriateNewStartButton*>( FindChildWnd( ID_NEW_START ) );
	if( pNewStartButton )
	{
		pNewStartButton->Clear();
		pNewStartButton->BlinkAnimationStart();
		pNewStartButton->ShowWnd();
	}

	m_dwClearStampStartTime = FRAMEGETTIME();
	m_fCurClearStampScale = 3.0f;
	if ( m_ClearStamp )
		m_ClearStamp->SetScale( m_fCurClearStampScale );
	g_SoundMgr.StopSound( m_RollingSound, 0 );
}

void PriateRulletRenewalWnd::HideRollingEndState()
{

}

void PriateRulletRenewalWnd::CheckOakHole()
{
	for( DWORD dwID = ID_OAK_HOLE_BUTTON01; dwID <= ID_OAK_HOLE_BUTTON12; ++dwID )
	{
		RenewalOakHoleButton* pButton = dynamic_cast<RenewalOakHoleButton*>( FindChildWnd( dwID ) );
		if( pButton )
		{
			DWORD dwIndex = dwID - ID_OAK_HOLE_BUTTON01;
			pButton->SetOakHole( dwIndex );
			pButton->ShowWnd();
		}
	}
}

void PriateRulletRenewalWnd::OnSuccessSwordUse( int iIndex, int iRewardCount )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwID = ID_OAK_HOLE_BUTTON01 + iIndex;
	RenewalOakHoleButton* pButton = dynamic_cast<RenewalOakHoleButton*>( FindChildWnd( dwID ) );
	if( pButton )
		pButton->AnimationStart();

	m_FaceState					= FS_WOUNDED;
	m_dwFaceAnimationStartTime	= dwCurTime;
	m_dwPreEndCheckStartTime = dwCurTime;

	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON11; ++dwID )
	{
		RenewalPrivateRulletRewardButton* pButton = dynamic_cast<RenewalPrivateRulletRewardButton*>( FindChildWnd( dwID ) );
		if( pButton && pButton->IsEnableStampAnimation( iRewardCount ) )
		{
			if ( m_pEnableRewardEffect )
			{
				m_pEnableRewardEffect->SetSize( 200, 200 );
				m_pEnableRewardEffect->SetPosX( pButton->GetXPos() - m_pEnableRewardEffect->GetWidth()/2 + pButton->GetEffectXPos() );
				m_pEnableRewardEffect->SetPosY( pButton->GetYPos() - m_pEnableRewardEffect->GetHeight()/2 + pButton->GetEffectYPos() );
			}
			
			m_dwEnableRewardEffectStart = dwCurTime;
			pButton->StampAnimationStart( iRewardCount );
		}
	}

	g_SoundMgr.PlaySound( m_RewardSound );
}

void PriateRulletRenewalWnd::OnFailSwordUse( int iIndex, int iRewardIndex, int iRewardCount )
{
	ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
	if( !pRullet )
		return;

	if ( iRewardCount -1 < 0 || iRewardCount -1 >= ioPriateRenewalRullet::MAX_RENEWAL_RULLET_COUNT )
		return;

	DWORD dwID = ID_OAK_HOLE_BUTTON01 + iIndex;
	RenewalOakHoleButton* pButton = dynamic_cast<RenewalOakHoleButton*>( FindChildWnd( dwID ) );
	if( pButton )
		pButton->AnimationStart();

	for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON11; ++dwID )
	{
		RenewalPrivateRulletRewardButton* pButton = dynamic_cast<RenewalPrivateRulletRewardButton*>( FindChildWnd( dwID ) );
		if( pButton )
		{
			pButton->FailAnimationStart();
		}
	}

	if ( m_pEnableRewardEffect )
	{
		enum { EFFECT_X_OFFSET = 569, EFFECT_Y_OFFSET = 316 };
		m_pEnableRewardEffect->SetSize( 250, 250 );
		m_pEnableRewardEffect->SetPosX( EFFECT_X_OFFSET - m_pEnableRewardEffect->GetWidth()/2 );
		m_pEnableRewardEffect->SetPosY( EFFECT_Y_OFFSET - m_pEnableRewardEffect->GetHeight()/2 );
		m_pEnableRewardEffect->Stop();
	}
	
	SAFEDELETE( m_pFailAcceptIcon );
	SAFEDELETE( m_pSubIcon );

	PriateRenewalRewardInfo* pInfo = NULL;
	pInfo = pRullet->GetReward( iRewardCount - 1 );
	if( pInfo == NULL )
		return;

	SetState( RS_FAIL );
	g_SoundMgr.PlaySound( m_FailSound );

	if ( iRewardIndex < 0 || iRewardIndex >= (int)pInfo->m_PresentDataList.size() )
		return;

	m_pFailAcceptIcon = g_UIImageSetMgr.CreateImageByFullName( pInfo->m_PresentDataList[iRewardIndex].m_Icon );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pInfo->m_PresentDataList[iRewardIndex].m_SubIcon );
	if ( !m_pFailAcceptIcon )
		return;
	m_pFailAcceptIcon->SetScale( 0.85f );
	if( m_pSubIcon )
		m_pSubIcon->SetScale( 0.85f );

	m_szFailAcceptIconName = pInfo->m_PresentDataList[iRewardIndex].m_Name;
}

void PriateRulletRenewalWnd::iwm_hide()
{
	if( m_pCriticalEffect )
		m_pCriticalEffect->Stop();
	if ( m_pEnableRewardEffect )
		m_pEnableRewardEffect->Stop();

	ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
	if ( pRullet )
	{
		if( m_RulletState == RS_ROLLING_END )
		{
			m_RulletState = RS_END;
			m_vRewardList.clear();
			m_dwRollingEndTime = 0;
		}
		else if ( m_RulletState == RS_ROLLING && m_dwRollingEndTime > 0 )
		{
			m_RulletState = RS_END;
			m_vRewardList.clear();
			m_dwRollingEndTime = 0;
		}
		else if ( m_RulletState == RS_FAIL )
		{
			m_RulletState = RS_END;
			SAFEDELETE( m_pFailAcceptIcon );
			SAFEDELETE( m_pSubIcon );
			m_dwFailStartTime = 0;
		}
	}
}

void PriateRulletRenewalWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	//드럼통 버튼
	if( COMPARE( dwID, ID_OAK_HOLE_BUTTON01, ID_OAK_HOLE_BUTTON12 + 1 ) )
	{
		if( cmd == IOBN_BTNUP )
		{
			ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
			if( pRullet )
			{
				int nCount = pRullet->GetSwordCount();

				if( nCount <= 0 )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "보유한 칼이 없습니다." );
					return;
				}

				if ( pRullet->GetCurEnableRewardTurnNumber() >= pRullet->GetMaxEnableRewardTurnNumber() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "모든 보상 받기에 성공하셨습니다.# 보상받기 버튼을 눌러주세요." );
					return;
				}

				if( m_RulletState == RS_PLAY )
				{
					DWORD dwIndex = dwID - ID_OAK_HOLE_BUTTON01;
					if( !pRullet->SendRenewalRulletUse( dwIndex ) )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "이미 칼이 꽃혀 있습니다." );
					}
				}
			}
		}
		else if( cmd == IOWN_OVERED )
		{
			RenewalOakHoleButton* pButton = dynamic_cast<RenewalOakHoleButton*>( FindChildWnd( dwID ) );
			if( pButton && !pButton->IsSticked() )
				StartDevelopKShakeAnimation();
		}
	}
	//보상버튼
	else if( COMPARE( dwID, ID_REWARD_BUTTON01, ID_REWARD_BUTTON11 + 1 ) )
	{
		if( cmd == IOWN_OVERED )
			SetToolTipText( dwID );
		else if( cmd == IOWN_LEAVED )
		{
			PriateTextTooltip *pTooltip = dynamic_cast<PriateTextTooltip*>( FindChildWnd( ID_GLOBAL_TOOLTIP ) );
			if( pTooltip && pTooltip->IsShow() )
				pTooltip->HideWnd();
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
		case ID_NEW_START:
			if ( cmd == IOBN_BTNUP )
			{
				if (m_RulletState != RS_END || m_dwEndStartTime == 0 )
				{
					ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
					if ( pRullet )
					{
						pRullet->SetValue( 0 );
						pRullet->ClearRullet();
						SetState( RS_END );
					}
				}
				else if ( m_RulletState == RS_FAIL && m_dwFailStartTime == 0 )
				{
					ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
					if ( pRullet )
					{
						pRullet->SetValue( 0 );
						pRullet->ClearRullet();
						SetState( RS_END );
					}
				}
			}
			break;
		case ID_ACCEPT_REWARD:
			if( cmd == IOBN_BTNUP )
			{
				if( m_RulletState == RS_PLAY && m_dwEndStartTime == 0 )
				{
					ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
					if( pRullet && pRullet->SendRewardRequest() )
						SetState( RS_ROLLING );
				}
			}
			break;
		case ID_HELP_PRE_BTN:
			if( cmd == IOBN_BTNUP )
			{
				if ( m_iCurPage > 0 )
				{
					m_iCurPage--;
					UpdateHelpInfo();
				}
			}
			break;
		case ID_HELP_NEXT_BTN:
			if( cmd == IOBN_BTNUP )
			{
				if ( m_iCurPage < m_iMaxPage - 1 )
				{
					m_iCurPage++;
					UpdateHelpInfo();
				}
			}
			break;
		}
	}
}

void PriateRulletRenewalWnd::OnProcess( float fTimePerSec )
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
	case RS_ROLLING:
		ProcessRollingState();
		break;
	case RS_ROLLING_END:
		ProcessRollingEndState( fTimePerSec );
		break;
	}

	ProcessBackGround();
	UpdateSwordCount();
	ProcessFailIconTooltip();
}

void PriateRulletRenewalWnd::InitDevelopKAnimation()
{
	m_DevelopKState					= DK_NONE;
	m_dwDevelopAnimationStartTime	= 0;
	m_dwDevelopAnimationTime		= 0;
	m_nAnimationX					= 0;
	m_bDevelopKShake				= false;
}

void PriateRulletRenewalWnd::StartDevelopKShakeAnimation()
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

void PriateRulletRenewalWnd::ProcessDevelopKShakeAnimation()
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

void PriateRulletRenewalWnd::ProcessDevelopKFaceCheck( bool bMustChange /*= false */ )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//보상 가능 횟수가 달라졌다면 표정 교체
	ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
	if( pRullet && ( pRullet->GetCurEnableRewardTurnNumber() != m_CurrState || bMustChange) )
	{
		//현재 페이스를 제거
		for( int i = 0; i < (int)m_CurrFaceList.size(); ++i )
		{
			SAFEDELETE( m_CurrFaceList[i] );
		}
		m_CurrFaceList.clear();

		m_CurrDevelopKFace	= NULL;

		m_CurrState = pRullet->GetCurEnableRewardTurnNumber();

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
				m_CurrDevelopKFace->SetScale( FLOAT1, FLOAT1 );

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
				m_CurrDevelopKFace->SetScale( FLOAT1, FLOAT1 );
			m_dwFaceAnimationStartTime = dwCurTime;
		}
	}
}

void PriateRulletRenewalWnd::ProcessDevelopKProvokeFaceCheck()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFaceAnimationStartTime + 300 < dwCurTime )
	{
		m_FaceState					= FS_NORMAL;
		m_dwFaceAnimationStartTime = dwCurTime;
	}
}

void PriateRulletRenewalWnd::ProcessDevelopKWoundedFaceCheck()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFaceAnimationStartTime + 300 < dwCurTime )
	{
		m_FaceState					= FS_NORMAL;
		m_dwFaceAnimationStartTime	= dwCurTime;
	}
}

void PriateRulletRenewalWnd::ProcessPreEndState()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwPreEndStartTime && m_dwPreEndStartTime + 900 < dwCurTime )
	{
		m_dwPreEndStartTime = 0;
		for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON11; ++dwID )
		{	
			RenewalPrivateRulletRewardButton* pButton = dynamic_cast<RenewalPrivateRulletRewardButton*>( FindChildWnd( dwID ) );
			if( pButton && pButton->IsEnableStampBlinkAnimation() )
				pButton->StampBlinkAnimationStart();
		}

		RenewalPriateNewStartButton* pNewStartButton = dynamic_cast<RenewalPriateNewStartButton*>( FindChildWnd( ID_NEW_START ) );
		if( pNewStartButton )
			pNewStartButton->BlinkAnimationStart();
	}
}

void PriateRulletRenewalWnd::ProcessEndState()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwEndStartTime && m_dwEndStartTime + 400 < dwCurTime )
	{
		m_dwEndStartTime = 0;
		SetState( RS_PRE );
	}
}

void PriateRulletRenewalWnd::ProcessRollingState()
{
	if ( m_dwRollingEndTime > 0 && m_dwRollingEndTime + 3000 < FRAMEGETTIME() )
	{
		m_dwRollingEndTime = 0;
		ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
		if ( pRullet )
			ShowRollingEnd();
	}
}

void PriateRulletRenewalWnd::ProcessBackGround()
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

	ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
	if( pRullet )
	{
		m_nSwordUseCount = pRullet->GetRulletStickedCount();
	}

	if( 0 < m_dwCriticalEffectStart && m_dwCriticalEffectStart + 150 < dwCurTime )
	{
		if( m_pCriticalEffect )
			m_pCriticalEffect->Start();

		m_dwCriticalEffectStart = 0;
	}
	if( 0 < m_dwEnableRewardEffectStart && m_dwEnableRewardEffectStart < dwCurTime )
	{
		if( m_pEnableRewardEffect )
			m_pEnableRewardEffect->Start();

		m_dwEnableRewardEffectStart = 0;
	}
	
}

void PriateRulletRenewalWnd::OnRender()
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
		m_ShipTreeInvLight->SetAlpha( (float)MAX_ALPHA_RATE * (FLOAT1 - m_fCurrTreeAlphaRate ) );
		m_ShipTreeInvLight->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_BombLeftFire )
	{
		m_BombLeftFire->SetAlpha( (float)MAX_ALPHA_RATE * m_fLeftCurrBombAlphaRate );
		m_BombLeftFire->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_BombLeftInvFire )
	{
		m_BombLeftInvFire->SetAlpha( (float)MAX_ALPHA_RATE * (FLOAT1 - m_fLeftCurrBombAlphaRate ) );
		m_BombLeftInvFire->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_BombRightFire )
	{
		m_BombRightFire->SetAlpha( (float)MAX_ALPHA_RATE * m_fRightCurrBombAlphaRate );
		m_BombRightFire->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_BombRightInvFire )
	{
		m_BombRightInvFire->SetAlpha( (float)MAX_ALPHA_RATE * (FLOAT1 - m_fRightCurrBombAlphaRate ) );
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
	case RS_ROLLING:
		RollingRender( iXPos, iYPos );
		break;
	case RS_ROLLING_END:
		RollingEndRender( iXPos, iYPos );
		break;
	case RS_FAIL:
		FailRender( iXPos, iYPos );
		break;
	}
}

void PriateRulletRenewalWnd::StartRender( int iXPos, int iYPos )
{
	if( m_OakImageCap )
		m_OakImageCap->Render( iXPos, iYPos );

	if( m_OakImageBody )
		m_OakImageBody->Render( iXPos, iYPos );
}

void PriateRulletRenewalWnd::PreRender( int iXPos, int iYPos )
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

	if ( m_SwordBar )
		m_SwordBar->Render( iXPos, iYPos );

	if( m_WoodSword )
		m_WoodSword->Render( iXPos, iYPos );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_2X );
	g_FontMgr.SetBkColor( 0xFF2D1C0E );
	g_FontMgr.SetTextColor( 0xFFFCB12E);
	g_FontMgr.PrintText( iXPos + 494, iYPos + 91, FONT_SIZE_18, "%d개", m_iStickedSwordCount );

	ioComplexStringPrinter	szText;
	szText.SetTextStyle( TS_NORMAL );
	szText.SetTextColor( 0xFF520404 );
	szText.AddTextPiece( FONT_SIZE_12, "※보상받기 클릭 후 " );
	szText.SetTextColor( 0xFF7F0303 );
	szText.AddTextPiece( FONT_SIZE_12, "랜덤으로 보상 획득" );
	szText.PrintFullText( iXPos + 566, iYPos + 358, TAT_CENTER );
}

void PriateRulletRenewalWnd::PlayRender( int iXPos, int iYPos )
{
	DevelopKRender( iXPos, iYPos );

	if( m_RewardMap )
		m_RewardMap->Render( iXPos, iYPos );

	if ( m_SwordBar )
		m_SwordBar->Render( iXPos, iYPos );

	if( m_WoodSword )
		m_WoodSword->Render( iXPos, iYPos );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_2X );
	g_FontMgr.SetBkColor( 0xFF2D1C0E );
	g_FontMgr.SetTextColor( 0xFFFCB12E);
	g_FontMgr.PrintText( iXPos + 494, iYPos + 91, FONT_SIZE_18, "%d개", m_iStickedSwordCount );

	ioComplexStringPrinter	szText;
	szText.SetTextStyle( TS_NORMAL );
	szText.SetTextColor( 0xFF520404 );
	szText.AddTextPiece( FONT_SIZE_12, "※보상받기 클릭 후 " );
	szText.SetTextColor( 0xFF7F0303 );
	szText.AddTextPiece( FONT_SIZE_12, "랜덤으로 보상 획득" );
	szText.PrintFullText( iXPos + 566, iYPos + 358, TAT_CENTER );
}

void PriateRulletRenewalWnd::DevelopKRender( int iXPos, int iYPos )
{
	if( m_OakImageCap )
		m_OakImageCap->Render( iXPos, iYPos );

	if( m_DevelopK )
		m_DevelopK->Render( iXPos + m_nAnimationX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );	

	switch( m_FaceState )
	{
	case FS_NORMAL:
		{
			if( m_CurrDevelopKFace )
				m_CurrDevelopKFace->Render( iXPos + m_nAnimationX + 180, iYPos + 156, UI_RENDER_NORMAL, TFO_BILINEAR );

			if( m_BubbleRight )
				m_BubbleRight->Render( iXPos, iYPos );
		}
		break;
	case FS_PROVOKE:
		{
			if( m_DevelopKProvoke )
				m_DevelopKProvoke->Render( iXPos + m_nAnimationX + 180, iYPos + 156, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		break;
	case FS_WOUNDED:
		{
			if( m_DevelopKWounded )
				m_DevelopKWounded->Render( iXPos + m_nAnimationX + 180, iYPos + 156, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		break;
	}

	if( m_OakImageBody )
		m_OakImageBody->Render( iXPos, iYPos );
}

void PriateRulletRenewalWnd::EndPreRender( int iXPos, int iYPos )
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

	if ( m_SwordBar )
		m_SwordBar->Render( iXPos, iYPos );

	if( m_WoodSword )
		m_WoodSword->Render( iXPos, iYPos );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_2X );
	g_FontMgr.SetBkColor( 0xFF2D1C0E );
	g_FontMgr.SetTextColor( 0xFFFCB12E);
	g_FontMgr.PrintText( iXPos + 494, iYPos + 91, FONT_SIZE_18, "%d개", m_iStickedSwordCount );

	ioComplexStringPrinter	szText;
	szText.SetTextStyle( TS_NORMAL );
	szText.SetTextColor( 0xFF520404 );
	szText.AddTextPiece( FONT_SIZE_12, "※보상받기 클릭 후 " );
	szText.SetTextColor( 0xFF7F0303 );
	szText.AddTextPiece( FONT_SIZE_12, "랜덤으로 보상 획득" );
	szText.PrintFullText( iXPos + 566, iYPos + 358, TAT_CENTER );
}

void PriateRulletRenewalWnd::UpdateSwordCount()
{
	m_iStickedSwordCount = 0;
	ioUserEtcItem* pETC = g_MyInfo.GetUserEtcItem();
	if( pETC )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pETC->GetEtcItem( m_StickedSwordCode, kSlot ) )
			m_iStickedSwordCount = kSlot.m_iValue1;
	}
}

void PriateRulletRenewalWnd::RollingRender( int iXPos, int iYPos )
{
	DevelopKRender( iXPos, iYPos );

	if( m_RewardMap )
		m_RewardMap->Render( iXPos, iYPos );

	if ( m_SwordBar )
		m_SwordBar->Render( iXPos, iYPos );

	if( m_WoodSword )
		m_WoodSword->Render( iXPos, iYPos );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_2X );
	g_FontMgr.SetBkColor( 0xFF2D1C0E );
	g_FontMgr.SetTextColor( 0xFFFCB12E);
	g_FontMgr.PrintText( iXPos + 494, iYPos + 91, FONT_SIZE_18, "%d개", m_iStickedSwordCount );

	ioComplexStringPrinter	szText;
	szText.SetTextStyle( TS_NORMAL );
	szText.SetTextColor( 0xFF520404 );
	szText.AddTextPiece( FONT_SIZE_12, "※보상받기 클릭 후 " );
	szText.SetTextColor( 0xFF7F0303 );
	szText.AddTextPiece( FONT_SIZE_12, "랜덤으로 보상 획득" );
	szText.PrintFullText( iXPos + 566, iYPos + 358, TAT_CENTER );
}

void PriateRulletRenewalWnd::RollingEndRender( int iXPos, int iYPos )
{
	DevelopKRender( iXPos, iYPos );

	if( m_RewardMap )
		m_RewardMap->Render( iXPos, iYPos );

	if ( m_SwordBar )
		m_SwordBar->Render( iXPos, iYPos );

	if( m_WoodSword )
		m_WoodSword->Render( iXPos, iYPos );

	if( m_ClearStamp )
		m_ClearStamp->Render( iXPos, iYPos );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_2X );
	g_FontMgr.SetBkColor( 0xFF2D1C0E );
	g_FontMgr.SetTextColor( 0xFFFCB12E);
	g_FontMgr.PrintText( iXPos + 494, iYPos + 91, FONT_SIZE_18, "%d개", m_iStickedSwordCount );

	ioComplexStringPrinter	szText;
	szText.SetTextStyle( TS_NORMAL );
	szText.SetTextColor( 0xFF520404 );
	szText.AddTextPiece( FONT_SIZE_12, "※보상받기 클릭 후 " );
	szText.SetTextColor( 0xFF7F0303 );
	szText.AddTextPiece( FONT_SIZE_12, "랜덤으로 보상 획득" );
	szText.PrintFullText( iXPos + 566, iYPos + 358, TAT_CENTER );
}

void PriateRulletRenewalWnd::ProcessRollingEndState( float fTimePerSec )
{
	DWORD dwCurTIme = FRAMEGETTIME();
	if ( m_dwClearStampStartTime > 0 && m_dwClearStampStartTime + m_dwClearStampLoopTime > dwCurTIme )
	{
		m_fCurClearStampScale -= m_fClearStampScaleSpeed * fTimePerSec;
		m_fCurClearStampScale = max( FLOAT1, m_fCurClearStampScale );
	}
	else if ( m_dwClearStampStartTime > 0 )
	{
		m_dwClearStampStartTime = 0;
		m_fCurClearStampScale = FLOAT1;
		g_SoundMgr.PlaySound( m_ClearStampSound );
	}
	else
		m_fCurClearStampScale = FLOAT1;

	if ( m_ClearStamp )
		m_ClearStamp->SetScale( m_fCurClearStampScale );
}

void PriateRulletRenewalWnd::ShowRollingEnd()
{
	if ( !COMPARE( m_vRewardList.size(), 1, ioPriateRenewalRullet::MAX_RENEWAL_RULLET_COUNT ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Reward Data reward count error : %d ", __FUNCTION__, m_vRewardList.size() );
		HideWnd();
		return;
	}

	for ( int i=0 ; i<(int)m_vRewardList.size() ; ++i )
	{
		RenewalPrivateRulletRewardButton* pButton = dynamic_cast<RenewalPrivateRulletRewardButton*>( FindChildWnd( ID_REWARD_BUTTON01+i ) );
		if( pButton )
			pButton->SuccessAnimationStart( i, m_vRewardList[i] );
	}

	SetState( RS_ROLLING_END );
}

void PriateRulletRenewalWnd::StartRollingEndTime( const IntVec& vRewardList )
{
	m_vRewardList = vRewardList;
	m_dwRollingEndTime = FRAMEGETTIME();
}

void PriateRulletRenewalWnd::SetToolTipText( DWORD dwID )
{
	RenewalPrivateRulletRewardButton* pButton = dynamic_cast<RenewalPrivateRulletRewardButton*>( FindChildWnd( dwID ) );
	if ( !pButton )
		return;

	ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
	if ( !pRullet )
		return;

	PriateTextTooltip *pTooltip = dynamic_cast<PriateTextTooltip*>( FindChildWnd( ID_GLOBAL_TOOLTIP ) );
	if ( !pTooltip || pTooltip->IsShow() )
		return;

	if ( !pButton->IsAccept() )
	{
		PriateRenewalRewardInfo* pInfo = NULL;
		pInfo = pRullet->GetReward( dwID - ID_REWARD_BUTTON01 );
		if( pInfo == NULL )
			return;

		int iSize = pInfo->m_PresentDataList.size();
		ioComplexStringPrinter kPrinter[TEXT_TOOLTIP_LINE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, "%s", "보상목록" );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, "%s", "아래 아이템중 랜덤으로 지급" );
		for ( int i =  0 ; i< iSize ; ++i )
		{
			if ( i+2 >= TEXT_TOOLTIP_LINE )
				break;

			kPrinter[i+2].SetTextStyle( TS_NORMAL );
			kPrinter[i+2].SetBkColor( 0, 0, 0 );
			kPrinter[i+2].SetTextColor( TCT_DEFAULT_GREEN );
			kPrinter[i+2].AddTextPiece( FONT_SIZE_12, "- %s", pInfo->m_PresentDataList[i].m_Name );

		}

		pTooltip->SetTooltipText( kPrinter );
	}
	else
	{
		ioComplexStringPrinter kPrinter[TEXT_TOOLTIP_LINE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, "%s", pButton->GetName() );

		pTooltip->SetTooltipText( kPrinter );
	}
}

void PriateRulletRenewalWnd::ShowFailState( bool bAnimation )
{
	RenewalPriateNewStartButton* pNewStartButton = dynamic_cast<RenewalPriateNewStartButton*>( FindChildWnd( ID_NEW_START ) );
	if( pNewStartButton )
	{
		pNewStartButton->Clear();
		pNewStartButton->BlinkAnimationStart();
		pNewStartButton->ShowWnd();
	}

	HideChildWnd( ID_RIGHT_BUBBLE );
	InitDevelopKAnimation();
	if( bAnimation )
		m_dwFailStartTime = FRAMEGETTIME();
}

void PriateRulletRenewalWnd::HideFailState()
{
	m_dwFailStartTime = 0;
	HideChildWnd( ID_NEW_START );
	
	SAFEDELETE( m_pFailAcceptIcon );
	SAFEDELETE( m_pSubIcon );

	if ( m_pEnableRewardEffect )
		m_pEnableRewardEffect->Stop();
	m_szFailAcceptIconName.Clear();
}

void PriateRulletRenewalWnd::FailRender( int iXPos, int iYPos )
{
	if( m_OakImageCap )
		m_OakImageCap->Render( iXPos, iYPos );

	if( m_OakImageBody )
		m_OakImageBody->Render( iXPos, iYPos );

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwFailStartTime && m_dwFailStartTime + 1200 > dwCurTime )
	{
		if( 0 < m_dwFailStartTime && m_dwFailStartTime + 100 > dwCurTime )
		{
			if( m_DevelopKFlyEnd )
				m_DevelopKFlyEnd->Render( iXPos + 121, iYPos + 42 );
		}
		else if( dwCurTime > m_dwFailStartTime + 100 && m_dwFailStartTime + 200 > dwCurTime )
		{
			if( m_DevelopKFlyStart )
				m_DevelopKFlyStart->Render( iXPos + 121, iYPos + 42 );
		}
		else if( dwCurTime > m_dwFailStartTime + 300 && m_dwFailStartTime + 500 > dwCurTime )
		{
			if( m_FireWorkEffect[0] )
				m_FireWorkEffect[0]->Render( iXPos + 234, iYPos + 60, UI_RENDER_SCREEN );
		}
		else if( dwCurTime > m_dwFailStartTime + 600 && m_dwFailStartTime + 700 > dwCurTime )
		{
			if( m_FireWorkEffect[1] )
				m_FireWorkEffect[1]->Render( iXPos + 234, iYPos + 60, UI_RENDER_SCREEN );
		}
		else if( dwCurTime > m_dwFailStartTime + 700 && m_dwFailStartTime + 800 > dwCurTime )
		{
			if( m_FireWorkEffect[2] )
				m_FireWorkEffect[2]->Render( iXPos + 234, iYPos + 60, UI_RENDER_SCREEN );
		}
		else if( dwCurTime > m_dwFailStartTime + 800 && m_dwFailStartTime + 900 > dwCurTime )
		{
			if( m_FireWorkEffect[3] )
				m_FireWorkEffect[3]->Render( iXPos + 234, iYPos + 60, UI_RENDER_SCREEN );
		}

		if( m_RewardMap )
			m_RewardMap->Render( iXPos, iYPos );

		if ( m_SwordBar )
			m_SwordBar->Render( iXPos, iYPos );

		if( m_WoodSword )
			m_WoodSword->Render( iXPos, iYPos );

		ioComplexStringPrinter	szText;
		szText.SetTextStyle( TS_NORMAL );
		szText.SetTextColor( 0xFF520404 );
		szText.AddTextPiece( FONT_SIZE_12, "※보상받기 클릭 후 " );
		szText.SetTextColor( 0xFF7F0303 );
		szText.AddTextPiece( FONT_SIZE_12, "랜덤으로 보상 획득" );
		szText.PrintFullText( iXPos + 566, iYPos + 358, TAT_CENTER );
	}
	else if ( m_dwFailStartTime && m_dwFailStartTime + 1250 > dwCurTime )
	{
		if ( !m_pEnableRewardEffect->IsLive() )
		{
			m_pEnableRewardEffect->Start();
			for( DWORD dwID = ID_REWARD_BUTTON01; dwID <= ID_REWARD_BUTTON11; ++dwID )
				HideChildWnd( dwID );
		}

		if( m_FailMap )
			m_FailMap->Render( iXPos, iYPos );

		if ( m_SwordBar )
			m_SwordBar->Render( iXPos, iYPos );

		if( m_WoodSword )
			m_WoodSword->Render( iXPos, iYPos );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( 0xFF7F3101 );
		g_FontMgr.PrintText( iXPos + 569, iYPos + 234, FONT_SIZE_13, "아쉽게도 여기까지~" );
		g_FontMgr.PrintText( iXPos + 569, iYPos + 254, FONT_SIZE_13, "힘내시라고 마지막 상품을 지급해 드립니다." );

		g_FontMgr.SetTextColor( 0xFF520404 );
		g_FontMgr.PrintText( iXPos + 569, iYPos + 358, FONT_SIZE_12, "※ 보관함에서 확인하세요" );

		if( m_pFailAcceptIcon )
			m_pFailAcceptIcon->Render( iXPos + 569, iYPos + 316, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_pSubIcon )
			m_pSubIcon->Render( iXPos + 569, iYPos + 316, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else
	{
		if( m_FailMap )
			m_FailMap->Render( iXPos, iYPos );

		if ( m_SwordBar )
			m_SwordBar->Render( iXPos, iYPos );

		if( m_WoodSword )
			m_WoodSword->Render( iXPos, iYPos );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( 0xFF7F3101 );
		g_FontMgr.PrintText( iXPos + 569, iYPos + 234, FONT_SIZE_13, "아쉽게도 여기까지~" );
		g_FontMgr.PrintText( iXPos + 569, iYPos + 254, FONT_SIZE_13, "힘내시라고 마지막 상품을 지급해 드립니다." );

		g_FontMgr.SetTextColor( 0xFF520404 );
		g_FontMgr.PrintText( iXPos + 569, iYPos + 358, FONT_SIZE_12, "※ 보관함에서 확인하세요" );

		if( m_pFailAcceptIcon )
			m_pFailAcceptIcon->Render( iXPos + 569, iYPos + 316, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_pSubIcon )
			m_pSubIcon->Render( iXPos + 569, iYPos + 316, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_2X );
	g_FontMgr.SetBkColor( 0xFF2D1C0E );
	g_FontMgr.SetTextColor( 0xFFFCB12E);
	g_FontMgr.PrintText( iXPos + 494, iYPos + 91, FONT_SIZE_18, "%d개", m_iStickedSwordCount );
}

void PriateRulletRenewalWnd::ProcessFailIconTooltip()
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	if ( m_pFailAcceptIcon )
	{
		RECT rc;
		int iXPos = GetDerivedPosX();
		int iYPos = GetDerivedPosY();
		int iXHalfSize = m_pFailAcceptIcon->GetImageWidth()/2;
		int iYHalfSize = m_pFailAcceptIcon->GetImageHeight()/2;
		SetRect( &rc, iXPos + 569 - iXHalfSize, iYPos + 316 - iYHalfSize,
			iXPos + 569 + iXHalfSize, 
			iYPos + 316 + iYHalfSize );
		if( PtInRect( &rc, pMouse->GetMousePos()) )
		{

			PriateTextTooltip *pTooltip = dynamic_cast<PriateTextTooltip*>( FindChildWnd( ID_GLOBAL_TOOLTIP ) );
			if ( !pTooltip || pTooltip->IsShow() )
				return;
			ioComplexStringPrinter kPrinter[TEXT_TOOLTIP_LINE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, "%s", m_szFailAcceptIconName );
			pTooltip->SetTooltipText( kPrinter );

		}
		else
		{
			PriateTextTooltip *pTooltip = dynamic_cast<PriateTextTooltip*>( FindChildWnd( ID_GLOBAL_TOOLTIP ) );
			if( pTooltip && pTooltip->IsShow() )
				pTooltip->HideWnd();
		}
	}
}

void PriateRulletRenewalWnd::UpdateHelpInfo()
{
	if( m_iCurPage == 0 )
	{
		ShowChildWnd( ID_HELP_NEXT_BTN );
		HideChildWnd( ID_HELP_PRE_BTN );

		ShowChildWnd( ID_GAME_RULE_RICH_LABEL1 );
		ShowChildWnd( ID_GAME_RULE_RICH_LABEL2 );
		ShowChildWnd( ID_GAME_RULE_RICH_LABEL3 );

		HideChildWnd( ID_GAME_RULE_RICH_LABEL4 );
		HideChildWnd( ID_GAME_RULE_RICH_LABEL5 );
		HideChildWnd( ID_GAME_RULE_RICH_LABEL6 );
	}
	else if ( m_iCurPage == 1)
	{
		ShowChildWnd( ID_GAME_RULE_RICH_LABEL4 );
		ShowChildWnd( ID_GAME_RULE_RICH_LABEL5 );
		ShowChildWnd( ID_GAME_RULE_RICH_LABEL6 );

		HideChildWnd( ID_GAME_RULE_RICH_LABEL1 );
		HideChildWnd( ID_GAME_RULE_RICH_LABEL2 );
		HideChildWnd( ID_GAME_RULE_RICH_LABEL3 );
	}

	if ( m_iCurPage == m_iMaxPage - 1 )
	{
		ShowChildWnd( ID_HELP_PRE_BTN );
		HideChildWnd( ID_HELP_NEXT_BTN );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
RenewalDevelopKBubble::RenewalDevelopKBubble() : m_CurrState(-1), m_CurrBubbleIndex( 0 ), m_CurrBubbleTime( 0 ), m_StartBubbleTime( 0 )
{
	m_pBubbleTail	= NULL;
}

RenewalDevelopKBubble::~RenewalDevelopKBubble()
{
	SAFEDELETE( m_pBubbleTail );
}

void RenewalDevelopKBubble::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void RenewalDevelopKBubble::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );

	m_bReverseTail = xElement.GetBoolAttribute_e( "ReverseTail" );
}

void RenewalDevelopKBubble::iwm_show()
{
}

void RenewalDevelopKBubble::iwm_hide()
{
}

void RenewalDevelopKBubble::ProcessRenewalDevelopKBubble( bool bMustChange /*= false */ )
{
	if( !IsShow() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	//보상가능 횟수가 달라졌다면 표정 교체
	ioPriateRenewalRullet* pRullet = g_MyInfo.GetPriateRenewalRullet();
	if( pRullet && ( pRullet->GetCurEnableRewardTurnNumber() != m_CurrState || bMustChange ) )
	{
		m_CurrState = pRullet->GetCurEnableRewardTurnNumber();

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

void RenewalDevelopKBubble::ProcessBubbleMessage()
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

void RenewalDevelopKBubble::OnProcess( float fTimePerSec )
{
	ProcessRenewalDevelopKBubble();
}

void RenewalDevelopKBubble::OnRender()
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

//////////////////////////////////////////////////////////////////////////
PriateTextTooltip::PriateTextTooltip()
{
}

PriateTextTooltip::~PriateTextTooltip()
{
}

void PriateTextTooltip::ClearList()
{
	for( int i=0; i < TEXT_TOOLTIP_LINE; ++i )
	{
		m_Printer[i].ClearList();
	}
}

void PriateTextTooltip::SetTooltipText( ioComplexStringPrinter *rkPrinter )
{
	ClearList();

	int i = 0;
	int iMaxLine = 0;
	for(i = 0;i < TEXT_TOOLTIP_LINE;i++)
	{
		if( rkPrinter[i].IsEmpty() )
			break;

		rkPrinter[i].CopyFullText( m_Printer[i] );
		iMaxLine++;
	}

	if( iMaxLine == 0 )
	{
		HideWnd();
		return;
	}

	ShowWnd();

	int iMaxWidth  = 0;
	int iMaxHeight = MIN_HEIGHT;
	for(i = 0;i < TEXT_TOOLTIP_LINE;i++)
	{
		if( m_Printer[i].IsEmpty() )
			break;

		int iCurLineWidth = m_Printer[i].GetFullWidth() + SIDE_TEXT_GAP;
		if( iMaxWidth < iCurLineWidth )
			iMaxWidth = iCurLineWidth;
	}
	iMaxHeight += ( TEXT_LINE_GAP * (iMaxLine - 1) );

	SetSize( iMaxWidth, iMaxHeight );
	ProcessPosition();
}

void PriateTextTooltip::ProcessPosition()
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

void PriateTextTooltip::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	//
	ProcessPosition();
}

void PriateTextTooltip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + 7;
	int iYPos = GetDerivedPosY() + 6;;

	for( int i=0; i < TEXT_TOOLTIP_LINE; i++ )
	{
		if( m_Printer[i].IsEmpty() )
			break;

		m_Printer[i].PrintFullText( iXPos, iYPos + TEXT_LINE_GAP*i, TAT_LEFT );
	}
}
//////////////////////////////////////////////////////////////////////////