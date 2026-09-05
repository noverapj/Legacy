
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioPlayMode.h"
#include "ioGaugeHelper.h"

#include "ioWeaponFeature.h"

#include "ioNpcChar.h"
#include "ioNpcDeveloperK.h"

#include "ioAI.h"

ioNpcDeveloperK::ioNpcDeveloperK( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode ) : ioNpcChar( pGrp, pCreator, pMode )
{	
}

ioNpcDeveloperK::~ioNpcDeveloperK()
{
}

void ioNpcDeveloperK::Initialize( const ITEM_DATA *pItem )
{
	ioNpcChar::Initialize( pItem );	
}

NpcType ioNpcDeveloperK::GetNpcType() const
{
	return NT_DEVELOPER_K;
}

DWORD ioNpcDeveloperK::GetNoInputDelayTime() 
{ 
	if( m_dwCustomNoInputDelayTime > 0 )
		return m_dwCustomNoInputDelayTime;

	return ioBaseChar::GetNoInputDelayTime(); 
}

DWORD ioNpcDeveloperK::GetLimiteDieFlyTime() 
{ 
	if( m_dwCustomLimiteDieFlyTime > 0 )
		return m_dwCustomLimiteDieFlyTime;

	return ioBaseChar::GetLimiteDieFlyTime(); 
}

float ioNpcDeveloperK::GetDieActionRate() 
{ 
	if( m_fCustomDieActionRate > 0.0f )
		return m_fCustomDieActionRate;

	return ioBaseChar::GetDieActionRate(); 
}

float ioNpcDeveloperK::GetDieDownActionRate() 
{ 
	if( m_fCustomDieDownActionRate > 0.0f )
		return m_fCustomDieDownActionRate;

	return ioBaseChar::GetDieDownActionRate(); 
}

const ioHashString &ioNpcDeveloperK::GetAniDieAction()
{ 
	if( !m_AniCustomDieAction.IsEmpty() )
		return m_AniCustomDieAction;

	return ioBaseChar::GetAniDieAction(); 
}

const ioHashString &ioNpcDeveloperK::GetAniBackDieAction()
{ 
	if( !m_AniCustomBackDieAction.IsEmpty() )
		return m_AniCustomBackDieAction;

	return ioBaseChar::GetAniBackDieAction(); 
}

const ioHashString &ioNpcDeveloperK::GetAniBlowDieAction()
{ 
	if( !m_AniCustomBlowDieAction.IsEmpty() )
		return m_AniCustomBlowDieAction;

	return ioBaseChar::GetAniBlowDieAction(); 
}

const ioHashString &ioNpcDeveloperK::GetAniBackBlowDieAction()
{ 
	if( !m_AniCustomBackBlowDieAction.IsEmpty() )
		return m_AniCustomBackBlowDieAction;

	return ioBaseChar::GetAniBackBlowDieAction(); 
}

const ioHashString &ioNpcDeveloperK::GetAniDownBlowDieAction()
{ 
	if( !m_AniCustomDownBlowDieAction.IsEmpty() )
		return m_AniCustomDownBlowDieAction;

	return ioBaseChar::GetAniDownBlowDieAction(); 
}

const ioHashString &ioNpcDeveloperK::GetAniBackdownBlowDieAction()
{ 
	if( !m_AniCustomBackdownBlowDieAction.IsEmpty() )
		return m_AniCustomBackdownBlowDieAction;

	return ioBaseChar::GetAniBackdownBlowDieAction(); 
}

void ioNpcDeveloperK::ProcessAI()
{
	ioNpcChar::ProcessAI();
}

void ioNpcDeveloperK::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	ProcessAI();
	//
	ioBaseChar::Update( fTimePerSec, dwPreTime, dwCurTime );
}

int ioNpcDeveloperK::GetDelayAnimationIdx() const
{
	if( m_bPrisonerMode || m_bCatchState )
		return m_pGroup->GetAnimationIdx( m_AniRopeDelay );

	if( HasBuff( BT_CHANGE_MOTION ) && !m_ChangeDelayAni.IsEmpty() )
		return m_pGroup->GetAnimationIdx( m_ChangeDelayAni );

	if( m_bEnableHide && !m_HideDelayAni.IsEmpty() )
	{
		return m_pGroup->GetAnimationIdx( m_HideDelayAni );
	}

	ioAttackableItem *pItem = GetPriorityItem( SPT_DELAY );
	if( pItem && pItem->HasDelayAnimation() )
	{
		return GetAnimationIdxAfterConvertName( pItem->GetDelayAnimation() , m_bMale );
	}

	return m_pGroup->GetAnimationIdx( m_AniDeveloperKMotion );
}

void ioNpcDeveloperK::FaceChange( const char *szTexPostfix, int iDuration )
{
	ioHashString szNewFaceName;
	StringVector vParam = ioStringConverter::Split( std::string(szTexPostfix), ".", 1 );
	if( vParam.size() == 2 )
	{
		char szFullName[MAX_PATH] = "";
		wsprintf( szFullName, "%s_developer_k.%s", vParam[0].c_str(), vParam[1].c_str() );

		szNewFaceName = szFullName;
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioBaseChar::FaceChange - %s Texture Name Error", szTexPostfix );
		return;
	}

	m_dwFaceChangeTime = FRAMEGETTIME() + iDuration;

	if( m_pEquipSlot && !m_pEquipSlot->GetCurFaceMesh().IsEmpty() )
	{
		ioEntity *pFace = m_pGroup->GetEntity( m_pEquipSlot->GetCurFaceMesh() );
		if( pFace )
		{
			Help::FaceChangeColorTexAndOutLine( pFace, szNewFaceName );
			m_szFaceChangeName = szNewFaceName;
		}
	}
}