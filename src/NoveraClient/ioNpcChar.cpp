
#include "stdafx.h"

#include "ioGaugeHelper.h"
#include "ioCharEmoticon.h"
#include "ioCallByNpcBuff.h"

#include "ioPlayStage.h"

#include "ioMonsterSurvivalMode.h"
#include "ioDungeonAMode.h"
#include "ioTowerDefMode.h"
#include "ioTeamSurvivalAIMode.h"

#include "ioMagicCircle.h"

#include "ioAI.h"
#include "ioNpcChar.h"

ioNpcChar::ioNpcChar( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode ) : ioBaseChar( pGrp, pCreator, pMode )
{	
	m_pAI = NULL;
	m_pCallByNpcBuff = NULL;
	m_dwNpcCode = 0;
	m_dwCustomNoInputDelayTime = m_dwCustomLimiteDieFlyTime = 0;
	m_dwDieAlphaTime = 3000;
	m_fCustomDieActionRate = m_fCustomDieDownActionRate = 0.0f;

	m_iStuckActiveRand = m_iStuckActiveSeed = 0;
	m_dwStuckActiveTime = 0;
	m_dwCurrentStartTime = m_dwStartTime = 0;
	m_dwStartReduceTime = 0;

	m_fStartXPos = m_fStartZPos = 0.0f;
	m_vStartPos.x = m_fStartXPos;
	m_vStartPos.y = 0.0f;
	m_vStartPos.z = m_fStartZPos;

	m_dwCustomStartProtectionTime = 0;
	m_dwCustomBlowProtectionTime = 0;
	m_fPrevHPRate = 2.0f;
	m_iBossMonsterType = NBT_NORMAL;
	m_bDeathMonster= false;
	m_bShowGrade   = false;
	m_dwGradeUpTime= 0;

	m_fMonsterDefaultHP	= -FLOAT1;
	m_fMonsterDefaultSpeed = -FLOAT1;

	m_bStartAICreateMotion = false;
	m_bClassLevelShow = false;
	m_bActionTrigger = true;

	m_dwDisplayMotionItem = 0;
	m_dwDisplayMotionItemTime = 0;

	// HeadMesh
	m_bUseHeadMeshChange = false;
	m_HeadMeshInfoMap.clear();

	int iNumEntity = m_pGroup->GetNumEntities();
	for( int i=0 ; i<iNumEntity ; i++ )
	{
		ioEntity *pEntity = m_pGroup->GetEntity( i );

		if( strstr( pEntity->GetMeshName().c_str(), "_head" ) )
		{
			m_szDefaultHeadMeshName = pEntity->GetMeshName();
			m_szChangeHeadMeshName = m_szDefaultHeadMeshName;
			break;
		}
	}
	//
}

ioNpcChar::~ioNpcChar()
{
	SAFEDELETE( m_pAI );

	m_BalanceBuff.clear();
	m_HeadMeshInfoMap.clear();
	m_NPCStartBuffList.clear();
}

void ioNpcChar::InitCustomInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";

	rkLoader.SetTitle_e( "animation" );

	// Custom Name
	rkLoader.LoadString_e( "ani_custom_name", "", szBuf, MAX_PATH );
	m_AniCustomName = szBuf;

	// Start Delay
	m_dwCustomNoInputDelayTime = rkLoader.LoadInt_e( "no_input_delay_time", 0 );

	m_dwDieAlphaTime = rkLoader.LoadInt_e( "die_alpha_time", 3000 );

	// Start Ani
	rkLoader.LoadString_e( "ani_start_motion", "", szBuf, MAX_PATH );
	m_AniCustomStartMotion = szBuf;

	// DieState
	m_dwCustomLimiteDieFlyTime = rkLoader.LoadInt_e( "limite_die_fly_time", 0 );
	m_fCustomDieActionRate = rkLoader.LoadFloat_e( "die_action_ani_rate", 0.0f );
	m_fCustomDieDownActionRate = rkLoader.LoadFloat_e( "die_down_action_ani_rate", 0.0f );

	// Die Ani
	rkLoader.LoadString_e( "ani_die_action", "", szBuf, MAX_PATH );
	m_AniCustomDieAction = szBuf;
	rkLoader.LoadString_e( "ani_back_die_action", "", szBuf, MAX_PATH );
	m_AniCustomBackDieAction = szBuf;
	rkLoader.LoadString_e( "ani_blow_die_action", "", szBuf, MAX_PATH );
	m_AniCustomBlowDieAction = szBuf;
	rkLoader.LoadString_e( "ani_back_blow_die_action", "", szBuf, MAX_PATH );
	m_AniCustomBackBlowDieAction = szBuf;
	rkLoader.LoadString_e( "ani_down_blow_die_action", "", szBuf, MAX_PATH );
	m_AniCustomDownBlowDieAction = szBuf;
	rkLoader.LoadString_e( "ani_backdown_blow_die_action", "", szBuf, MAX_PATH );
	m_AniCustomBackdownBlowDieAction = szBuf;
	rkLoader.LoadString_e( "ani_falling", "", szBuf, MAX_PATH );
	m_AniCustomFalling = szBuf;
	rkLoader.LoadString_e( "ani_dorp_down_action", "", szBuf, MAX_PATH );
	m_AniCustomDropAction = szBuf;
	rkLoader.LoadString_e( "ani_blow_getup", "", szBuf, MAX_PATH );
	m_AniCustomBlowGetUp = szBuf;
	rkLoader.LoadString_e( "ani_back_blow_getup", "", szBuf, MAX_PATH );
	m_AniCustomBackBlowGetUp = szBuf;

	if( m_pGroup )
		m_pGroup->SetCustomSkeletonAniName( m_AniCustomName );

	// Protected
	rkLoader.SetTitle_e( "protected" );
	m_dwCustomStartProtectionTime = rkLoader.LoadInt_e( "start_protection_time", m_dwStartProtectionTime );
	m_dwCustomBlowProtectionTime = rkLoader.LoadInt_e( "blow_state_protection_time", m_dwBlowProtectionTime );
	m_nNpcProtectType = rkLoader.LoadInt_e( "npc_protect_type", NPT_NORMAL);


	// HP & SPEED Stat
	rkLoader.SetTitle_e( "default_stat" );
	m_fMonsterDefaultHP		= rkLoader.LoadFloat_e( "default_hp", -FLOAT1 );
	m_fMonsterDefaultSpeed	= rkLoader.LoadFloat_e( "default_speed", -FLOAT1 );
}

void ioNpcChar::InitCustomEffect( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";

	// Custom Effect
	rkLoader.SetTitle_e( "custom_effect_info" );
	rkLoader.LoadString_e( "custom_effect", "", szBuf, MAX_PATH );
	m_CustomEffect = szBuf;

	if( !m_CustomEffect.IsEmpty() )
		AttachEffect( m_CustomEffect );
}

void ioNpcChar::SetCustomEffect()
{
	if( !m_CustomEffect.IsEmpty() )
		EndEffect( m_CustomEffect, false );

	if( !m_CustomEffect.IsEmpty() )
		AttachEffect( m_CustomEffect );
}

void ioNpcChar::SetCustomEffectEnd()
{
	if( !m_CustomEffect.IsEmpty() )
		EndEffect( m_CustomEffect, false );
}

void ioNpcChar::Initialize( const ITEM_DATA *pItem )
{
	m_KeyInput.SetUpAIKeyValue( true );

	SetIsOwnerChar( false );
	EnableKeyInputCheck( false );

	SetIndex( NPC_INDEX );
	if( pItem )
	{
		DestroyAllItem();
		for(int i = 0;i < MAX_INVENTORY;i++)
		{
			ITEM_DATA kItem = pItem[i];
			if( kItem.m_item_code == 0 ) continue;

			ioItem *pItem = m_pCreator->CreateItem( kItem.m_item_code );
			if( pItem )
			{
				pItem->ReLoadProperty();
				pItem->SetOwnerName( GetCharName() );
				pItem->SetItemCreateIndex( GetIndex() );

				ioItem *pPreItem = EquipItem( pItem, true );
				SAFEDELETE( pPreItem );

				// VoiceType Set
				int iVoiceType = 0;
				const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( kItem.m_item_code - 1 );
				if( pSetInfo )
					iVoiceType = pSetInfo->GetVoiceType();
				SetVoiceType( iVoiceType );
			}
		}
	}
}

void ioNpcChar::LoadMonsterAIInfo( ioINILoader &rkLoader )
{
	int i=0;
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	// BOSS
	rkLoader.SetTitle_e( "Info" );
	rkLoader.LoadString_e( "monster_icon", "", szBuf, MAX_PATH );
	m_szMonsterIcon = szBuf;
	rkLoader.LoadString_e( "monster_emoticon", "", szBuf, MAX_PATH );
	m_szBossMonsterEmoticon = szBuf;
	rkLoader.LoadString_e( "monster_img_name", "", szBuf, MAX_PATH );
	m_szImageName = szBuf;
	m_iBossMonsterType = rkLoader.LoadInt_e( "boss", 0 );
	m_bActionTrigger = rkLoader.LoadBool_e("ActionTrigger", true);


	// MENT
	m_MonsterMent.m_dwShowTime = rkLoader.LoadInt_e( "ment_show_time", 0 ); 
	m_MonsterMent.m_fFontScale = rkLoader.LoadFloat_e( "ment_scale", 0.0f );
	rkLoader.LoadString_e( "ment_color", "", szBuf, MAX_PATH );
	char *pTemp = NULL;
	m_MonsterMent.m_dwColor = strtoul( szBuf, &pTemp, 16 );
	rkLoader.LoadString_e( "ment_text", "", szBuf, MAX_PATH );
	m_MonsterMent.m_szMent = szBuf;

	// 계급 표시 여부
	m_bShowGrade = rkLoader.LoadBool_e( "show_grade", false );

	// NPC 시작될때 적용하는 버프
	m_NPCStartBuffList.clear();
	int iStartBuffCnt = rkLoader.LoadInt_e( "npc_start_buff_cnt", 0 );
	for( i=0; i < iStartBuffCnt; ++i )
	{
		wsprintf_e( szKey, "npc_start_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_NPCStartBuffList.push_back( szBuf );
	}

	// HP 변화에 따른 버프 적용
	m_BalanceBuff.clear();
	rkLoader.SetTitle_e( "BalanceBuff" );
	int iMaxHPBuff = rkLoader.LoadInt_e( "max_hp_buff", 0 );
	for( i=0; i < iMaxHPBuff; i++ )
	{
		BalanceBuff kBalanceBuff;
		sprintf_e( szKey, "hp_buff%d_rate", i + 1 );
		kBalanceBuff.m_fCheckHPRate = rkLoader.LoadFloat( szKey, 0.0f );

		sprintf_e( szKey, "hp_buff%d_name", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kBalanceBuff.m_szHPBuff = szBuf;

		m_BalanceBuff.push_back( kBalanceBuff );
	}
}

void	ioNpcChar::LoadMonsterMent( ioXMLElement* xmlNode )
{
	// kaseton : xml loading
	if( xmlNode != (ioXMLElement*)0 && !xmlNode->IsEmpty() )
	{
		// MENT
		m_MonsterMent.m_dwShowTime = xmlNode->GetIntAttribute_e( "duration" );
		m_MonsterMent.m_fFontScale = xmlNode->GetFloatAttribute_e( "ment_scale" );
		char *pTemp = NULL;
		m_MonsterMent.m_dwColor = strtoul( xmlNode->GetStringAttribute_e( "ment_color" ), &pTemp, 16 );
		m_MonsterMent.m_szMent = xmlNode->GetStringAttribute_e( "ment_text" );
	}
}

void	ioNpcChar::LoadMonsterInfo( ioXMLElement* xmlNode )
{
	// kaseton : xml loading
	if( xmlNode != (ioXMLElement*)0 && !xmlNode->IsEmpty() )
	{
		int i = 0;
//		char szKey[MAX_PATH], szBuf[MAX_PATH];
		// just for compatibility : AI type 2 will not use this
		m_szMonsterIcon = "";
		m_szBossMonsterEmoticon = "";
		m_iBossMonsterType = 0;

		// 계급 표시 여부
		m_bShowGrade = false;

		// HP 변화에 따른 버프 적용 : will not use this feature on AIType 2, ChampionAI
	}
}

NpcType ioNpcChar::GetNpcType() const
{
	return NT_DEFAULT;
}

bool ioNpcChar::IsNeedProcess() const
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return true;

	if( g_MyInfo.GetPublicID() == m_szSyncUserName ) 
		return true;
	return false;
}

bool ioNpcChar::IsNeedSendNetwork() const
{
	if( m_pCreator && !m_pCreator->IsNetworkMode() )
		return false;
	if( !P2PNetwork::IsNetworkPlaying() )
		return false;
	
	if( g_MyInfo.GetPublicID() == m_szSyncUserName ) 
		return true;

	return false;
}

bool ioNpcChar::IsDropDamage()
{
	// 본부에서는 모두 데미지 
	if( ToHeadquartersMode( m_pModeParent ) )
		return true;

	ioPlayStage* pPlayState = ToPlayStage( GetCreator() );
	if( pPlayState )
	{
		if( pPlayState->GetModeType() == MT_TEAM_SURVIVAL_AI )
			return true;
	}

	return false;
}

bool ioNpcChar::IsItemDrop()
{
	// 본부에서는 아이템 드랍
	if( ToHeadquartersMode( m_pModeParent ) )
	{
		// AI가 있는 몬스터는 드랍하지 않는다.
		if( !m_pAI )
			return true;
	}
	ioPlayStage* pPlayState = ToPlayStage( GetCreator() );
	if( pPlayState )
	{
		if( pPlayState->GetModeType() == MT_TEAM_SURVIVAL_AI )
			return true;
	}
	return false;
}

void ioNpcChar::InitUserStat()
{
	ioBaseChar::InitUserStat();
}

void ioNpcChar::UpdateStatAttribute()
{
	if( m_fMonsterDefaultHP == -FLOAT1 || m_fMonsterDefaultSpeed == -FLOAT1 )
	{	
		// INI에 없으면 기본값 세팅
		ioBaseChar::UpdateStatAttribute();   
	}
	else
	{
		m_HP.m_fMaxValue = m_fMonsterDefaultHP;
		m_HP.m_fCurValue = max( 0.0f, min( m_HP.m_fCurValue, m_HP.m_fMaxValue ) );
		m_fMaxMoveSpeed  = m_fMonsterDefaultSpeed;
	}

	if( m_pCreator->GetModeType() == MT_TEAM_SURVIVAL_AI )
	{
		ioTeamSurvivalAIMode* pMode = ToTeamSurvivalAIMode( m_pModeParent );
		if( pMode )
		{
			float fRate = pMode->GetLevelHP();
			m_HP.m_fMaxValue *= fRate;
			m_HP.m_fCurValue = max( 0.0f, min( m_HP.m_fCurValue, m_HP.m_fMaxValue ) );
		}
	}
}

bool ioNpcChar::IsEscapeType()
{
	if( !m_pAI ) return false;

	return m_pAI->IsWayPoint();
}

DWORD ioNpcChar::GetNoInputDelayTime() 
{ 
	if( m_dwCustomNoInputDelayTime > 0 )
		return m_dwCustomNoInputDelayTime;

	return m_dwNpcNoInputDelayTime; 
}

DWORD ioNpcChar::GetLimiteDieFlyTime() 
{ 
	if( m_dwCustomLimiteDieFlyTime > 0 )
		return m_dwCustomLimiteDieFlyTime;

	return m_dwLimiteNpcDieFlyTime; 
}

float ioNpcChar::GetDieActionRate() 
{ 
	if( m_fCustomDieActionRate > 0.0f )
		return m_fCustomDieActionRate;

	return m_fNpcDieActionRate; 
}

float ioNpcChar::GetDieDownActionRate() 
{ 
	if( m_fCustomDieDownActionRate > 0.0f )
		return m_fCustomDieDownActionRate;

	return m_fNpcDieDownActionRate; 
}

const ioHashString &ioNpcChar::GetAniStartAction( bool &bWeaponAni )
{
	bWeaponAni = false;
	if( !m_AniCustomStartMotion.IsEmpty() )
	{
		return m_AniCustomStartMotion;
	}
	return ioBaseChar::GetAniStartAction( bWeaponAni );
}

const ioHashString &ioNpcChar::GetAniDieAction()
{ 
	if( !m_AniCustomDieAction.IsEmpty() )
		return m_AniCustomDieAction;

	return m_AniNpcDieAction; 
}

const ioHashString &ioNpcChar::GetAniBackDieAction()
{ 
	if( !m_AniCustomBackDieAction.IsEmpty() )
		return m_AniCustomBackDieAction;

	return m_AniNpcBackDieAction; 
}

const ioHashString &ioNpcChar::GetAniBlowDieAction()
{ 
	if( !m_AniCustomBlowDieAction.IsEmpty() )
		return m_AniCustomBlowDieAction;

	return m_AniNpcBlowDieAction; 
}

const ioHashString &ioNpcChar::GetAniBackBlowDieAction()
{ 
	if( !m_AniCustomBackBlowDieAction.IsEmpty() )
		return m_AniCustomBackBlowDieAction;

	return m_AniNpcBackBlowDieAction; 
}

const ioHashString &ioNpcChar::GetAniDownBlowDieAction()
{ 
	if( !m_AniCustomDownBlowDieAction.IsEmpty() )
		return m_AniCustomDownBlowDieAction;

	return m_AniNpcDownBlowDieAction; 
}

const ioHashString &ioNpcChar::GetAniBackdownBlowDieAction()
{ 
	if( !m_AniCustomBackdownBlowDieAction.IsEmpty() )
		return m_AniCustomBackdownBlowDieAction;

	return m_AniNpcBackdownBlowDieAction; 
}

ioHashString &ioNpcChar::GetAniFalling()
{
	if( !m_AniCustomFalling.IsEmpty() )
		return m_AniCustomFalling;

	return m_AniNpcFalling; 
}

ioHashString &ioNpcChar::GetDropDownAction()
{
	if( !m_AniCustomDropAction.IsEmpty() )
		return m_AniCustomDropAction;

	return m_AniNpcDropAction; 
}

ioHashString &ioNpcChar::GetAniBlowGetUp()
{
	if( !m_AniCustomBlowGetUp.IsEmpty() )
		return m_AniCustomBlowGetUp;

	return m_AniNpcBlowGetUp; 
}

ioHashString &ioNpcChar::GetAniBackBlowGetUp()
{
	if( !m_AniCustomBackBlowGetUp.IsEmpty() )
		return m_AniCustomBackBlowGetUp;

	return m_AniNpcBackBlowGetUp; 
}





void ioNpcChar::CreateMagicCircle( const ioHashString &ReadyCircle,
								   const ioHashString &EnableCircle,
								   const ioHashString &GuidCircle,
								   float fOffSet )
{
	DestroyMagicCircle();

	if( ReadyCircle.IsEmpty() || EnableCircle.IsEmpty() )
		return;

	D3DXVECTOR3 vPos = GetWorldPosition();
	if( m_pAI && m_pAI->IsTargetBaseChar() )
	{
		vPos = m_pAI->GetTargetPos();
	}
	else
	{		
		D3DXVECTOR3 vOffSet = GetMoveDir() * fOffSet;

		vPos.y = 0;
		vPos += vOffSet;		
	}

	vPos.y = m_pCreator->GetMapHeight( vPos.x, vPos.z );
	if( vPos.y <= 0.0f )
	{
		vPos.y = GetBottomHeight();
	}

	m_pMagicCircle = new ioMagicCircle;
	m_pMagicCircle->InitResource( ReadyCircle, EnableCircle, GuidCircle, vPos, m_pCreator );
	m_pMagicCircle->SetVisible( false );
	m_pMagicCircle->SetTargetRot( GetTargetRot() );
	m_pMagicCircle->UpdatePos( false, vPos, this, m_pCreator );

	SetDirectionKey( -1, KEY_NONE );
}

void ioNpcChar::SetAI( const ioHashString &rkAI )
{
	if( rkAI.IsEmpty() ) return;

	ioINILoader kLoader( rkAI.c_str() );

	if( Help::IsMonsterDungeonMode( m_pCreator->GetModeType() ) )
	{
		ioAIPathFind *pAI = new ioAIPathFind( this, m_pCreator );
		pAI->LoadAI( kLoader );
		SetAI( pAI );
	}
	else
	{
		ioAI *pAI = new ioAI( this, m_pCreator );
		pAI->LoadAI( kLoader );
		SetAI( pAI );
	}
}

void ioNpcChar::SetAI( ioAI *pAI )
{
	SAFEDELETE( m_pAI );
	m_pAI = pAI;
}

void ioNpcChar::SetDirectionKey( int iDirection, WORD wKeyData )
{
	/*
	3   4   5
	2  NPC  6
	1	0	7          -1 = All Stop
	*/
	switch( iDirection )
	{
	case -1:
		{
			m_KeyInput.SetAIDirKeyInput( KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, 
										 KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE );
		}
		break;
	case 0:
		{
			m_KeyInput.SetAIDirKeyInput( KEY_NONE, KEY_NONE, KEY_NONE, wKeyData, 
										 KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE );
		}
		break;
	case 1:
		{
			m_KeyInput.SetAIDirKeyInput( KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, 
										 KEY_NONE, wKeyData, KEY_NONE, KEY_NONE );
		}
		break;
	case 2:
		{
			m_KeyInput.SetAIDirKeyInput( wKeyData, KEY_NONE, KEY_NONE, KEY_NONE, 
				                         KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE );
		}
		break;
	case 3:
		{
			m_KeyInput.SetAIDirKeyInput( KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, 
				                         wKeyData, KEY_NONE, KEY_NONE, KEY_NONE );
		}
		break;
	case 4:
		{
			m_KeyInput.SetAIDirKeyInput( KEY_NONE, KEY_NONE, wKeyData, KEY_NONE, 
										 KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE );
		}
		break;
	case 5:
		{
			m_KeyInput.SetAIDirKeyInput( KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, 
				                         KEY_NONE, KEY_NONE, wKeyData, KEY_NONE );
		}
		break;
	case 6:
		{
			m_KeyInput.SetAIDirKeyInput( KEY_NONE, wKeyData, KEY_NONE, KEY_NONE, 
				                         KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE );
		}
		break;
	default:
		{
			m_KeyInput.SetAIDirKeyInput( KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, 
										 KEY_NONE, KEY_NONE, KEY_NONE, wKeyData );
		}
		break;
	}
}

void ioNpcChar::SetActionKey( WORD wDefenseKey, WORD wJumpKey, WORD wAttackKey )
{
	m_KeyInput.SetAIActionKeyInput( wDefenseKey, wJumpKey, wAttackKey );
}

void ioNpcChar::SetNpcViewName( const ioHashString &szViewName )
{
	m_szNpcViewName = szViewName;
}

void ioNpcChar::SetNpcGradeTitle( const ioHashString &szGradeTitle )
{
	m_szGradeTitle = szGradeTitle;
}

void ioNpcChar::SetSyncUser( const ioHashString &rkSyncName )
{	
	m_szSyncUserName = rkSyncName;
}

void ioNpcChar::SetStuckInfo( int iStuckActiveRand, int iStuckActiveSeed, DWORD dwSturkActiveTime, ioHashStringVec &rkStackBuffList )
{
	m_iStuckActiveRand = iStuckActiveRand;
	m_iStuckActiveSeed = iStuckActiveSeed;
	m_dwStuckActiveTime= dwSturkActiveTime;

	m_StuckBuffList.clear();
	if( !rkStackBuffList.empty() )
	{
		m_StuckBuffList.resize( rkStackBuffList.size() );
		std::copy( rkStackBuffList.begin(), rkStackBuffList.end(), m_StuckBuffList.begin() );
	}	

	m_StuckExData.clear();
}

void ioNpcChar::SetStuckExList( const ioHashString &rkWeaponType, int iStuckActiveRand )
{
	StuckExData kStuckExData;
	StringVector vStringVector = ioStringConverter::Split( rkWeaponType.c_str(), "." );
	StringVector::iterator iter;
	for( iter = vStringVector.begin() ; iter!=vStringVector.end() ; ++iter )
	{
		kStuckExData.m_vWeaponTypeList.push_back( atoi( iter->c_str() ) );
	}
	kStuckExData.m_iStuckActiveRand = iStuckActiveRand;
	m_StuckExData.push_back( kStuckExData );
}

void ioNpcChar::SetFixingTarget( const ioHashString &rkTargetName, float fFixingTime )
{
	if( m_pAI )
		m_pAI->SetFixingTarget( rkTargetName, fFixingTime );
}

void ioNpcChar::SetDisplayMotion( DWORD dwDisplayMotionItem )
{
	m_dwDisplayMotionItem = dwDisplayMotionItem;

	if( m_dwDisplayMotionItem == 0 )
	{
		if( GetState() == CS_ETCITEM_MOTION )
		{
			SetState( CS_DELAY );
		}
		m_dwDisplayMotionItemTime = 0;
	}
	else
	{
		if( GetState() == CS_DELAY || GetState() == CS_ETCITEM_MOTION )
		{
			SetEtcItemMotionState( m_dwDisplayMotionItem );
			m_dwDisplayMotionItemTime = 0;
		}
	}
}

int ioNpcChar::GetGradeUpRandom( int iRandom )
{
	return m_GradeUpRandom.Random( iRandom );
}

void ioNpcChar::SetGradeUpRandomSeed( DWORD dwRandomSeed )
{
	m_GradeUpRandom.SetRandomSeed( dwRandomSeed );
}

void ioNpcChar::ReSetGradeLevel( int iGradeLevel )
{
	SetLevel( iGradeLevel );
	if( m_bShowGrade )
	{
		// 변경 사항
		DWORD dwProtected;
		float fRecoveryHpRate;
		ioHashString szSkeleton;
		bool bEquipItem, bLevelUpMotion;
		g_NpcMgr.GetGradeAbilityValue( GetLevel(), bEquipItem, dwProtected, bLevelUpMotion, fRecoveryHpRate, szSkeleton );

		if( !szSkeleton.IsEmpty() )
		{
			// 본 교체
			ChangeSkeleton( szSkeleton );
		}
	
		// 장비 & AI 교체
		g_NpcMgr.ChangeRandomEquipItem( this, bEquipItem );

		// 무적
		if( dwProtected > 0 )
		{
			SetProtectionMode( PM_GETUP, dwProtected );      
		}

		// Level Up Ani Start
		if( bLevelUpMotion )
		{			
			SetGradeUpMotionState();
		}

		// 회복
		if( fRecoveryHpRate > 0.0f )
		{			
			RecoveryHP( GetHP().m_fMaxValue * fRecoveryHpRate );
		}
// 		LOG.PrintTimeAndLog( 0, "%s - ReSetGradeLevel : %d : %d / %d / %d / %.2f / %s", GetCharName().c_str(), GetLevel(), 
// 								(int)bEquipItem, (int)dwProtected, (int)bLevelUpMotion, fRecoveryHpRate, szSkeleton.c_str() );

		// 능력치
		float fArmorClass, fMaxArmorClass, fSpeedClass, fMaxSpeedClass;
		g_NpcMgr.GetGradeAbilityArmor( GetLevel(), fArmorClass, fMaxArmorClass );
		g_NpcMgr.GetGradeAbilitySpeed( GetLevel(), fSpeedClass, fMaxSpeedClass );
		SetAddCharArmorClass( fArmorClass, fMaxArmorClass );
		SetAddCharSpeedClass( -( fSpeedClass ), fMaxSpeedClass );
//		LOG.PrintTimeAndLog( 0, "%s - ReSetGradeLevel : %d : %.2f / %.2f", GetCharName().c_str(), GetLevel(), fArmorClass, fSpeedClass );
	}
}

void ioNpcChar::RandomizeGradeLevel()
{
	if( m_bShowGrade )
	{
		if( m_pCreator->IsRoundSetEnd() ) return;
		if( GetState() == CS_DIE ) return;
		if( GetState() == CS_LOADING ) return;
		if( IsApplyDownState() ) return;
		if( m_dwGradeUpTime == 0 ) return;

		DWORD dwGapTime = FRAMEGETTIME() - m_dwGradeUpTime;
		if( dwGapTime > g_NpcMgr.GetRandomGradeUpTime() )
		{
			int iMin, iMax;
			g_NpcMgr.GetRandomGradeTable( GetLevel(), iMin, iMax );
			if( iMax == 0 )
			{
				m_dwGradeUpTime = 0;
			}
			else
			{
				ReSetGradeLevel( m_GradeUpRandom.Random( iMax ) + iMin );
				m_dwGradeUpTime = FRAMEGETTIME();
			}
		}
	}
}

void ioNpcChar::AddItemBalance( float fAddArmorClass, float fAddSpeedClass )
{
	for(int i = 0;i < MAX_INVENTORY;i++)
	{
		ioItem *pItem = NULL;
		if( m_pEquipSlot )
			pItem = m_pEquipSlot->GetEquipedItem( i );
		if( pItem )
		{
			pItem->AddItemArmorClass( fAddArmorClass );
			pItem->AddItemSpeedClass( fAddSpeedClass );
		}
	}
}

void ioNpcChar::ReEquipItem()
{
	int i = 0;
	ITEM_DATA ItemList[MAX_INVENTORY];
	for(i = 0;i < MAX_INVENTORY;i++)
	{
		ioItem *pItem = GetEquipedItem( i );
		if( pItem )
		{
			ItemList[i].m_item_code = pItem->GetItemCode();
			ItemList[i].m_item_reinforce = pItem->GetItemReinforce();
			ItemList[i].m_item_male_custom = pItem->GetItemMaleCustom();
			ItemList[i].m_item_female_custom = pItem->GetItemFemaleCustom();
		}
	}
	DestroyAllItem();
	for(i = 0;i < MAX_INVENTORY;i++)
	{
		ITEM_DATA kItem = ItemList[i];
		if( kItem.m_item_code == 0 ) continue;

		ioItem *pItem = m_pCreator->CreateItem( kItem.m_item_code );
		if( pItem )
		{
			pItem->ReLoadProperty();
			pItem->SetOwnerName( GetCharName() );
			pItem->SetItemCreateIndex( GetIndex() );

			ioItem *pPreItem = EquipItem( pItem, true );
			SAFEDELETE( pPreItem );

			// VoiceType Set
			int iVoiceType = 0;
			const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( kItem.m_item_code - 1 );
			if( pSetInfo )
				iVoiceType = pSetInfo->GetVoiceType();
			SetVoiceType( iVoiceType );
		}
	}
}

void ioNpcChar::ChangeItemSkill( EquipSlot eSlot, const ioHashString &rkSkillName )
{
	if( !m_pEquipSlot ) return;
	if( rkSkillName.IsEmpty() ) return;

	m_pEquipSlot->ChangeEquipedSkill( eSlot, rkSkillName );
}

void ioNpcChar::ExceptionStartAI()
{
	// 뭔가 문제가 있어서 몬스터가 등장하지 않았다. 일단 강제로 등장 시키고 로그를 날리자.
	if( m_pCreator && m_pCreator->IsRoundSetEnd() ) return;
	if( GetState() != CS_LOADING ) return;

	SetStartMotionState();

	char szExceptionLOG[MAX_PATH] = "";
	m_pCreator->GetRoomIndex();
	sprintf( szExceptionLOG, "[해골영웅](%d) - %s - %s - %s - %s : %u : %u", m_pCreator->GetRoomIndex(),      //Except Extracting Hangeul
							 g_MyInfo.GetPublicID().c_str(), GetCharName().c_str(), GetViewName().c_str(), GetSyncUser().c_str(),
							 m_dwCurrentStartTime, m_dwStartTime );

	SP2Packet kPacket( LUPK_LOG );
	kPacket << "CheckError";  // 로그 파일 타입
	kPacket << szExceptionLOG;
	kPacket << 320;  // 오류번호:320
	kPacket << true; // write db
	P2PNetwork::SendLog( kPacket );
}

void ioNpcChar::StartAI( DWORD dwStartTime, float fStartXPos, float fStartZPos, bool bCreateMotion, DWORD dwIndex )
{
	m_dwStartTime = dwStartTime;
	m_fStartXPos = fStartXPos;
	m_fStartZPos = fStartZPos;
	D3DXVECTOR3 vCenter( fStartXPos, 0.0f, fStartZPos );
	vCenter.y = m_pCreator->GetMapHeight( vCenter.x, vCenter.z );
	SetWorldPosition( vCenter );
	m_vStartPos = vCenter;

	m_bStartAICreateMotion = bCreateMotion;
	m_dwCurrentStartTime = FRAMEGETTIME();
	m_dwStartReduceTime  = 0;

	m_fPrevHPRate = 2.0f;

	m_dwUserIndex = dwIndex;

}

void ioNpcChar::StopNPC()
{
	m_dwCurrentStartTime = 0;
	m_dwStartReduceTime  = 0;
	SetState( CS_LOADING );
}

void ioNpcChar::StartReduceTime( DWORD dwStartReduceTime )
{
	m_dwStartReduceTime += dwStartReduceTime;
}

bool ioNpcChar::IsStartCheck()
{
	if( m_dwCurrentStartTime == 0 ) return false;
	if( m_pCreator && m_pCreator->IsRoundSetEnd() ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = m_dwStartReduceTime + ( dwCurTime - m_dwCurrentStartTime );
	if( dwGapTime > m_dwStartTime )
	{
		m_dwGradeUpTime = dwCurTime;
		m_dwCurrentStartTime = 0;
		if( m_bStartAICreateMotion )
		{
			SetCreateMotionState();
		}
		else
		{
			SetStartMotionState();
		}

		SetNPCStartBuff();

		return true;
	}
	return false;
}

void ioNpcChar::SetNPCStartBuff()
{
	int iStartBuffSize = m_NPCStartBuffList.size();
	for( int i=0; i < iStartBuffSize; ++i )
	{
		if( m_NPCStartBuffList[i].IsEmpty() )
			continue;

        AddNewBuff( m_NPCStartBuffList[i], GetCharName(), "", NULL );
	}
}

void ioNpcChar::ProcessAI()
{	
	if( m_pCreator->IsRoundSetEnd() ) return;
	if( GetState() == CS_DIE ) return;
	if( GetState() == CS_LOADING )
	{
		if( !IsStartCheck() ) return;
	}

	if( !IsNeedProcess() ) return;

	if( !m_bActionTrigger ) return;

	if( m_pAI )
	{
		m_pAI->ProcessAI();
	}
}

void ioNpcChar::ProcessBalanceCheck()
{
	if( m_pCreator->IsRoundSetEnd() ) return;
	if( GetState() == CS_DIE ) return;
	if( GetState() == CS_LOADING ) return;

	StatusValue &rkStatus = GetHP();
	if( rkStatus.m_fMaxValue == 0.0f ) return;
	
	float fHPRate = rkStatus.m_fCurValue / rkStatus.m_fMaxValue;
	if( fHPRate == m_fPrevHPRate ) return;

	int iBalanceBuffSize = m_BalanceBuff.size();
	for(int i = 0;i < iBalanceBuffSize;i++)
	{
		BalanceBuff &rkBuff = m_BalanceBuff[i];    
		if( m_fPrevHPRate <= rkBuff.m_fCheckHPRate ) continue;
		if( fHPRate > rkBuff.m_fCheckHPRate ) continue;

		ioBuff* pBuff = AddNewBuff( rkBuff.m_szHPBuff, GetCharName(), "", NULL );

		if(pBuff)
			m_pCallByNpcBuff = ToCallByNpcBuff(pBuff);
	}
	m_fPrevHPRate = fHPRate;
}

void ioNpcChar::SetCallByNpcBuff(const DWORD dwOccurNpc, bool bAlive)
{
	if( m_pCreator->IsRoundSetEnd() ) return;
	if( GetState() == CS_DIE ) return;
	if( GetState() == CS_LOADING ) return;

	if(m_pCallByNpcBuff)
	{
		if( GetTeam() == TEAM_BLUE)
			m_pCallByNpcBuff->SetBlueBuff(dwOccurNpc, bAlive);
		else
			m_pCallByNpcBuff->SetRedBuff(dwOccurNpc, bAlive);
	}
}

void ioNpcChar::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	ProcessAI();
	ProcessBalanceCheck();
	RandomizeGradeLevel();
	//
	ioBaseChar::Update( fTimePerSec, dwPreTime, dwCurTime );

	ProcessBossMonsterEmoticon();
	ProcessMotionCheck();
}

void ioNpcChar::ProcessBossMonsterEmoticon()
{
	if( IsDisableGauge() ) return;
	if( m_iBossMonsterType == NBT_NORMAL ) return;
	if( !m_pCharEmoticon ) return;
	if( m_pCharEmoticon->IsEmoticonEvent() ) return;
	if( GetState() == CS_DIE || GetState() == CS_VIEW || GetState() == CS_LOADING || GetState() == CS_OBSERVER || GetState() == CS_STARTMOTION ) return;

	if( !m_szBossMonsterEmoticon.IsEmpty() )
		SetEmoticon( m_szBossMonsterEmoticon, EMT_CHAT_ICON, 0xFFFFFF00 );        //무한
}

bool ioNpcChar::CehckStuckWeaponType( int iWeaponType, int iRandValue )
{	
	int iSize = m_StuckExData.size();
	for(int i = 0;i < iSize;i++)
	{
		StuckExData &rkStuckData = m_StuckExData[i];
		int iWeaponTypeList = rkStuckData.m_vWeaponTypeList.size();
		for(int w = 0;w < iWeaponTypeList;w++)
		{
			if( iWeaponType == rkStuckData.m_vWeaponTypeList[w] )
			{
				if( iRandValue < rkStuckData.m_iStuckActiveRand )
					return true;
				return false;
			}
		}
	}
	return false;
}

bool ioNpcChar::CheckStuckActive( int iWeaponType )
{
	if( !IsNeedProcess() ) return false;
	if( m_iStuckActiveSeed == 0 ) return false;
	if( m_ProtectionMode != PM_NONE ) return false;
    	
	int iActiveValue = rand()%m_iStuckActiveSeed;
	if( CehckStuckWeaponType( iWeaponType, iActiveValue ) || iActiveValue < m_iStuckActiveRand ) 
	{
		ActiveStuck();
		return true;
	}
	return true;
}

void ioNpcChar::ProcessMotionCheck()
{
	ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pModeParent );
	if( pHeadquartersMode == NULL ) return;

	if( pHeadquartersMode->GetCharState() == ioHeadquartersMode::STATE_DISPLAY_CHAR )
	{
		if( m_dwDisplayMotionItem == 0 )
		{
			if( GetState() == CS_ETCITEM_MOTION )
				SetState( CS_DELAY );
		}
		else
		{
			if( GetState() == CS_DELAY && GetState() != CS_ETCITEM_MOTION )
			{
				if( FRAMEGETTIME() - m_dwDisplayMotionItemTime > 2000 )
				{
					m_dwDisplayMotionItemTime = 0;
					SetEtcItemMotionState( m_dwDisplayMotionItem );
				}
			}
			else
			{
				m_dwDisplayMotionItemTime = FRAMEGETTIME();
			}
		}
	}
}

void ioNpcChar::ActiveStuck()
{
	SetProtectionMode( PM_BOSS, m_dwStuckActiveTime );
	for( int i = 0;i < (int)m_StuckBuffList.size();i++)
	{
		if( m_StuckBuffList[i].IsEmpty() ) continue;
		AddNewBuff( m_StuckBuffList[i], GetCharName(), "", NULL );
	}	

	if( IsNeedSendNetwork() )
	{
		// 유저들에게 스턱 상태 발동 전송
		SP2Packet kPacket( CUPK_NPC_STUCK_STATE );
		kPacket << GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioNpcChar::GetComboAttackTimeline( unsigned int frontMargin, unsigned int tailMargin )
{
	DWORD dwStart		 = m_AnimationTime.dwStartTime;
	DWORD dwDuration	 = m_dwActionStopTotalDelay + m_AnimationTime.dwFullTime;
	DWORD dwDelay		 = m_dwNormalAttackEndDelay;

	DWORD dwPastTime	 = FRAMEGETTIME() - dwStart;
	DWORD dwEndCheckTime = dwDuration + dwDelay;

//	LOG.PrintTimeAndLog( 0, 
//		"m_dwComboCheckTime = %u, dwPastTime = %u, dwEndCheckTime = %u",
//			m_dwComboCheckTime, dwPastTime, dwEndCheckTime );
	
	if( dwPastTime < m_dwComboCheckTime + frontMargin ) return -1;
	else if( dwPastTime > dwEndCheckTime - tailMargin ) return 1;
	else return 0;
}

bool ioNpcChar::IsComboAttackPossible()
{
	// combo를 받을 수 있는 시간 안이다.
	return ( GetComboAttackTimeline() == 0 );
}

bool ioNpcChar::RenderGaugeHide()
{
	if( IsDisableGauge() ) return true;
	if( IsDeathMonster() ) return true;

	ioTowerDefMode *pTowerDef = ToTowerDefMode(m_pModeParent);
	if( pTowerDef )
	{
		if(	m_CharState == CS_ESCAPE_PRISONER || 
			m_CharState == CS_READY ||
			m_CharState == CS_VIEW ||
			m_CharState == CS_OBSERVER ||
			m_CharState == CS_LOADING ||
			m_CharState == CS_DIE ||
			m_CharState == CS_STARTMOTION ||
			!m_pGroup->IsVisible() )
		{
			m_bGaugeAlpha = false;

			if( m_pGauge )
				m_pGauge->SetGaugeAlpha( m_bGaugeAlpha );

			return true;
		}

		int iXPos = 0;
		int iYPos = 0;

		if( m_pGauge )
		{
			D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
			vPos.y += 40.0f;

			if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
			{
				g_FontMgr.SetTextStyle( TS_BOLD_OUTLINE_FULL );
				g_FontMgr.SetBkColor( 0, 0, 0 );

				switch( GetTeam() )
				{
				case TEAM_BLUE:
					g_FontMgr.SetTextColor( 0, 150, 255 );
					break;
				case TEAM_RED:
					g_FontMgr.SetTextColor( 255, 60, 0 );
					break;
// 				case TEAM_PRIVATE_1:
// 					g_FontMgr.SetTextColor( 128, 255, 0 );
// 					break;
// 				case TEAM_PRIVATE_2:
// 					g_FontMgr.SetTextColor( 212, 212, 212);
// 					break;
				default: //개인전
					{
						if( GetSingleConvertTeam() == TEAM_BLUE )
							g_FontMgr.SetTextColor( 0, 150, 255 );
						else
							g_FontMgr.SetTextColor( 255, 60, 0 );
					}				
					break;
				}

// 				if( IsVisibleID() )
// 				{
// 					g_FontMgr.SetAlignType( TAT_LEFT );
// 
// 					switch( GetTeam() )
// 					{
// 					case TEAM_BLUE:
// 						g_LevelMgr.RenderGrade( iXPos, iYPos - 12, GetLevel(), TEAM_BLUE );
// 						break;
// 					case TEAM_RED:
// 						g_LevelMgr.RenderGrade( iXPos, iYPos - 12, GetLevel(), TEAM_RED );
// 						break;
// 					default: //개인전
// 						if( GetSingleConvertTeam() == TEAM_BLUE )
// 							g_LevelMgr.RenderGrade( iXPos, iYPos - 12, GetLevel(), TEAM_BLUE );
// 						else
// 							g_LevelMgr.RenderGrade( iXPos, iYPos - 12, GetLevel(), TEAM_RED );
// 						break;
// 					}
// 
// 					if( !m_szGradeTitle.IsEmpty() )
// 					{
// 						g_FontMgr.SetAlignType( TAT_CENTER );
// 						g_FontMgr.PrintText( iXPos, iYPos + 15, FONT_SIZE_9, m_szGradeTitle.c_str() );
// 					}
// 				}		

				if( IsVisibleHP() )
				{
					if( m_pModeParent && m_pModeParent->GetModeState() != ioPlayMode::MS_PLAY )
						m_bGaugeAlpha = false;

					if( m_bClassLevelShow )
					{
						enum { LEVEL_X_OFFSET = -1, LEVEL_Y_OFFSET = 16, };

						switch( GetTeam() )
						{
						case TEAM_BLUE:
						case TEAM_RED:
							g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, GetClassLevel(), GetTeam(), IsOwnerChar() );
							break;
						default: //개인전
							if( GetSingleConvertTeam() == TEAM_BLUE )
								g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, GetClassLevel(), TEAM_BLUE, IsOwnerChar() );
							else
								g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, GetClassLevel(), TEAM_RED, IsOwnerChar() );
							break;
						}
					}					

					m_pGauge->SetGaugeAlpha( m_bGaugeAlpha );
					m_pGauge->RenderGauge( iXPos, iYPos + 6, g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) );				
				}			
			}
		}
#ifndef SHIPPING 
		if( Setting::ShowExtraInfo() )
		{
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextStyle( TS_SHADOW );

			g_FontMgr.PrintText( m_iGaugeXPos, m_iGaugeYPos + 10, 0.6f, GetStateText() );
			g_FontMgr.PrintText( m_iGaugeXPos, m_iGaugeYPos - 22.0f, 0.6f, STR(1), (int)m_HP.m_fCurValue,
				(int)m_HP.m_fMaxValue );
		}
#endif




		return true;
	}

	if( m_bShowGrade ) return false;

	ioMonsterSurvivalMode *pMonsterSurvival = ToMonsterSurvivalMode( m_pModeParent );
	if( pMonsterSurvival )
	{
		if( pMonsterSurvival->IsMonsterBossTurn() )
		{
			if( !IsMonsterHalfBoss() && !IsMonsterLastBoss() )
				return true;
		}
	}

	ioDungeonAMode *pDungeonAMode = ToDungeonAMode( m_pModeParent );
	if( pDungeonAMode )
	{
		if( pDungeonAMode->IsMonsterBossTurn() )
		{
			if( !IsMonsterHalfBoss() && !IsMonsterLastBoss() )
				return true;
		}
	}

	return false;
}

void ioNpcChar::RenderGauge()
{
	if( RenderGaugeHide() ) return;
	
	if(	m_CharState == CS_ESCAPE_PRISONER || 
		m_CharState == CS_READY ||
		m_CharState == CS_VIEW ||
		m_CharState == CS_OBSERVER ||
		m_CharState == CS_LOADING ||
		m_CharState == CS_DIE ||
		m_CharState == CS_STARTMOTION ||
		!m_pGroup->IsVisible() )
	{
		m_bGaugeAlpha = false;

		if( m_pGauge )
			m_pGauge->SetGaugeAlpha( m_bGaugeAlpha );

		return;
	}

	int iXPos = 0;
	int iYPos = 0;

	if( m_pGauge )
	{
		D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
		vPos.y += 40.0f;

		if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
		{
			g_FontMgr.SetTextStyle( TS_BOLD_OUTLINE_FULL );
			g_FontMgr.SetBkColor( 0, 0, 0 );

			switch( GetTeam() )
			{
			case TEAM_BLUE:
				g_FontMgr.SetTextColor( 0, 150, 255 );
				break;
			case TEAM_RED:
				g_FontMgr.SetTextColor( 255, 60, 0 );
				break;
			default: //개인전
				{
					if( GetSingleConvertTeam() == TEAM_BLUE )
						g_FontMgr.SetTextColor( 0, 150, 255 );
					else
						g_FontMgr.SetTextColor( 255, 60, 0 );
				}				
				break;
			}

			if( IsVisibleID() )
			{
				if( m_bShowGrade )
				{

					g_FontMgr.SetAlignType( TAT_LEFT );
					char szCharName[MAX_PATH] = "";
					sprintf( szCharName, "%s", m_szNpcViewName.c_str() );
					int iNameWidth = g_FontMgr.GetTextWidth( szCharName, TS_BOLD_OUTLINE_FULL, FONT_SIZE_12 );
					int iHalfWidth = ( iNameWidth + 21 ) / 2;	// 20 은 계급사이즈

					switch( GetTeam() )
					{
					case TEAM_BLUE:
						g_LevelMgr.RenderGrade( GetCharName(), iXPos - iHalfWidth, iYPos - 12, GetLevel(), TEAM_BLUE );
						break;
					case TEAM_RED:
						g_LevelMgr.RenderGrade( GetCharName(), iXPos - iHalfWidth, iYPos - 12, GetLevel(), TEAM_RED );
						break;
					default: //개인전
						if( GetSingleConvertTeam() == TEAM_BLUE )
							g_LevelMgr.RenderGrade( GetCharName(), iXPos - iHalfWidth, iYPos - 12, GetLevel(), TEAM_BLUE );
						else
							g_LevelMgr.RenderGrade( GetCharName(), iXPos - iHalfWidth, iYPos - 12, GetLevel(), TEAM_RED );
						break;
					}

					g_FontMgr.PrintText( iXPos - iHalfWidth + 21, iYPos - 12, FONT_SIZE_12, szCharName );
				}
				else
				{
					g_FontMgr.SetAlignType( TAT_CENTER );
					g_FontMgr.PrintText( iXPos, iYPos - 12, FONT_SIZE_12, m_szNpcViewName.c_str() );				
				}

				if( !m_szGradeTitle.IsEmpty() )
				{
					g_FontMgr.SetAlignType( TAT_CENTER );
					g_FontMgr.PrintText( iXPos, iYPos + 15, FONT_SIZE_9, m_szGradeTitle.c_str() );
				}
			}			
			if( IsVisibleHP() )
			{
				if( m_pModeParent && m_pModeParent->GetModeState() != ioPlayMode::MS_PLAY )
					m_bGaugeAlpha = false;

				if( m_bClassLevelShow )
				{
					enum { LEVEL_X_OFFSET = -1, LEVEL_Y_OFFSET = 16, };
					switch( GetTeam() )
					{
					case TEAM_BLUE:
					case TEAM_RED:
						g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, GetClassLevel(), GetTeam(), IsOwnerChar() );
						break;
					default: //개인전
						if( GetSingleConvertTeam() == TEAM_BLUE )
							g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, GetClassLevel(), TEAM_BLUE, IsOwnerChar() );
						else
							g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, GetClassLevel(), TEAM_RED, IsOwnerChar() );
						break;
					}
				}					

				m_pGauge->SetGaugeAlpha( m_bGaugeAlpha );
				m_pGauge->RenderGauge( iXPos, iYPos + 6, g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) );				
			}			
		}
	}
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_SHADOW );

		g_FontMgr.PrintText( m_iGaugeXPos, m_iGaugeYPos + 10, 0.6f, GetStateText() );
		g_FontMgr.PrintText( m_iGaugeXPos, m_iGaugeYPos - 22.0f, 0.6f, STR(1), (int)m_HP.m_fCurValue,
			(int)m_HP.m_fMaxValue );
	}
#endif
}

ApplyWeaponExp ioNpcChar::ApplyWeapon( ioWeapon *pWeapon )
{
	if( pWeapon )
	{
		CheckStuckActive( (int)pWeapon->GetType() );
		if( !m_bActionTrigger )
		{
			m_bActionTrigger = true;
			SetState( CS_DELAY);
		}
	}
	return ioBaseChar::ApplyWeapon( pWeapon );
}

bool ioNpcChar::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	return ioBaseChar::IsCollisionAvailableWeapon( pWeapon );
}

void ioNpcChar::InitHeadMeshInfo( ioINILoader &rkLoader )
{
	m_HeadMeshInfoMap.clear();

	char szBuf[MAX_PATH] = "";
	ioHashString szTemp;

	rkLoader.SetTitle_e( "head_mesh_info" );

	m_bUseHeadMeshChange = rkLoader.LoadBool_e( "use_head_mesh_change", false );
	if( !m_bUseHeadMeshChange )
		return;
	
	// attack_face
	rkLoader.LoadString_e( "attack_face", "", szBuf, MAX_PATH );
	szTemp = szBuf;

	if( !szTemp.IsEmpty() )
	{
		m_HeadMeshInfoMap.insert( HeadMeshInfoMap::value_type( "attack_face", szTemp ) );
	}

	// chaos_face
	rkLoader.LoadString_e( "chaos_face", "", szBuf, MAX_PATH );
	szTemp = szBuf;

	if( !szTemp.IsEmpty() )
	{
		m_HeadMeshInfoMap.insert( HeadMeshInfoMap::value_type( "chaos_face", szTemp ) );
	}

	// damage_face
	rkLoader.LoadString_e( "damage_face", "", szBuf, MAX_PATH );
	szTemp = szBuf;

	if( !szTemp.IsEmpty() )
	{
		m_HeadMeshInfoMap.insert( HeadMeshInfoMap::value_type( "damage_face", szTemp ) );
	}

	// defence_face
	rkLoader.LoadString_e( "defence_face", "", szBuf, MAX_PATH );
	szTemp = szBuf;

	if( !szTemp.IsEmpty() )
	{
		m_HeadMeshInfoMap.insert( HeadMeshInfoMap::value_type( "defence_face", szTemp ) );
	}

	// lose_face
	rkLoader.LoadString_e( "lose_face", "", szBuf, MAX_PATH );
	szTemp = szBuf;

	if( !szTemp.IsEmpty() )
	{
		m_HeadMeshInfoMap.insert( HeadMeshInfoMap::value_type( "lose_face", szTemp ) );
	}

	// pickup_face
	rkLoader.LoadString_e( "pickup_face", "", szBuf, MAX_PATH );
	szTemp = szBuf;

	if( !szTemp.IsEmpty() )
	{
		m_HeadMeshInfoMap.insert( HeadMeshInfoMap::value_type( "pickup_face", szTemp ) );
	}

	// talk_face
	rkLoader.LoadString_e( "talk_face", "", szBuf, MAX_PATH );
	szTemp = szBuf;

	if( !szTemp.IsEmpty() )
	{
		m_HeadMeshInfoMap.insert( HeadMeshInfoMap::value_type( "talk_face", szTemp ) );
	}

	// win_face
	rkLoader.LoadString_e( "win_face", "", szBuf, MAX_PATH );
	szTemp = szBuf;

	if( !szTemp.IsEmpty() )
	{
		m_HeadMeshInfoMap.insert( HeadMeshInfoMap::value_type( "win_face", szTemp ) );
	}
}

void ioNpcChar::HeadMeshChange( const char *szMeshName, int iDuration )
{
	HeadMeshInfoMap::iterator iter = m_HeadMeshInfoMap.find( szMeshName );
	if( iter == m_HeadMeshInfoMap.end() )
		return;

	ioHashString szNewHeadMesh = iter->second;
	if( szNewHeadMesh.IsEmpty() )
		return;

	m_dwHeadMeshChangeTime = FRAMEGETTIME() + iDuration;

	DestroyEntity( m_szChangeHeadMeshName );

	AddEntity( szNewHeadMesh );
	m_szChangeHeadMeshName = szNewHeadMesh;
}

void ioNpcChar::CheckHeadMeshChange()
{
	if( m_dwHeadMeshChangeTime == 0 )
		return;

	if( m_CharState == CS_STOP_MOTION || m_CharState == CS_NOT_MOVE_PROTECT )
		return;

	if( m_dwHeadMeshChangeTime < FRAMEGETTIME() &&
		m_CharState != CS_BLOW_WOUNDED &&
		m_CharState != CS_BOUND_BLOW_WOUNDED &&
		m_CharState != CS_BLOW_EXTEND_WOUND )
	{
		StopHeadMeshChange();
	}
}

void ioNpcChar::StopHeadMeshChange()
{
	if( m_dwHeadMeshChangeTime == 0 )
		return;

	m_dwHeadMeshChangeTime = 0;

	DestroyEntity( m_szChangeHeadMeshName );
	AddEntity( m_szDefaultHeadMeshName );
	m_szChangeHeadMeshName = m_szDefaultHeadMeshName;
}


void ioNpcChar::SetGaugeScale(float fScaleX, float fScaleY)
{
	if( m_pGauge )
	{
		m_pGauge->SetScale(fScaleX, fScaleY);
	}
}


void ioNpcChar::CheckDieState()
{
	DWORD dwGap;
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DieState )
	{
	case DS_FLYING:
		if( m_bJumpedBlowWounded )
		{
			if( IsDownState() && !IsFloating() )
			{
				int iAniID = m_pGroup->GetAnimationIdx( GetAniDownBlowDieAction() );
				if( m_AttackedSide == AS_BACK )
					iAniID = m_pGroup->GetAnimationIdx( GetAniBackdownBlowDieAction() );

				m_pGroup->ClearAllActionAni( 0.0f, true );
				m_pGroup->SetActionAni( iAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1/GetDieDownActionRate(), 0.0f, 0.0f, true );

				SetDieBlowEnd();
			}
			else if( m_dwSetDieStateTime > 0 && m_dwSetDieStateTime+GetLimiteDieFlyTime() < dwCurTime )
			{
				int iAniID = m_pGroup->GetAnimationIdx( GetAniDownBlowDieAction() );
				if( m_AttackedSide == AS_BACK )
					iAniID = m_pGroup->GetAnimationIdx( GetAniBackdownBlowDieAction() );

				m_pGroup->ClearAllActionAni( 0.0f, true );
				m_pGroup->SetActionAni( iAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1/GetDieDownActionRate(), 0.0f, 0.0f, true );

				m_dwSetDieStateTime = 0;

				SetDieBlowEnd( false );
			}
		}
		else
		{
			if( ( m_AnimationTime.EndTime() + m_dwActionStopTotalDelay < dwCurTime ) &&
				!IsFloating() )
			{
				SetDieBlowEnd();
			}
			else if( m_dwSetDieStateTime > 0 && m_dwSetDieStateTime+GetLimiteDieFlyTime() < dwCurTime )
			{
				SetDieBlowEnd( false );
			}
		}
		break;
	case DS_BLOW_END:
		if( m_dwDieAlphaTime == 0 || dwCurTime - m_dwDieBlowEndTime > 100 )
		{
			m_iFloatingWoundedCnt = 0;
			m_DieState = DS_EXP_UP;
		}
		break;
	case DS_EXP_UP:
		if( m_dwDieAlphaTime == 0 || dwCurTime - m_dwDieBlowEndTime > 900 )
		{
			m_pCreator->CreateMapEffect( "sp2_ghost.txt",
				GetEntityCenterPos(),
				GetWorldScale() );
			m_DieState = DS_FADE_OUT;
			m_pCreator->ChangeCurViewCharByDie( this );
		}
		break;
	case DS_FADE_OUT:
		dwGap = dwCurTime - m_dwDieBlowEndTime;

		if( m_dwDieAlphaTime <= 0 )
		{
			if( ToNpcChar( this ) )
			{
				m_pGroup->SetVisible( false );
			}
		}
		else
		{
			if( (m_dwDieAlphaTime > 0) && (m_pGroup && m_pGroup->IsVisible()) && (!ToFightClubMode( m_pModeParent ))  )        // 파이트클럽 모드는 페이드 아웃 안함
			{
				if( dwGap > m_dwDieAlphaTime )
				{
					dwGap -= m_dwDieAlphaTime;
					float fRate = FLOAT1 - ( (float)dwGap / 1500.0f );
					fRate = max( 0.0f, fRate );

					int iAlphaRate = fRate * MAX_ALPHA_RATE;
					D3DXVECTOR3 vPos = GetWorldPosition();
					if( vPos.y > 0.0f )
					{
						if( iAlphaRate > 10 )
						{
							SetAlphaRateDirect( iAlphaRate );
						}
						else
						{
							SetAlphaRateDirect( 10 );
							if( ToNpcChar( this ) )
							{
								m_pGroup->SetVisible( false );
							}
						}
					}
					else
					{
						SetAlphaRateDirect( iAlphaRate );
					}
				}
			}
		}
		break;
	}
}

void ioNpcChar::SetNpcMaxHpRate( float fMaxHpRate )
{
	if( fMaxHpRate == -1.0f )
	{	
		// INI에 없으면 기본값 세팅
		ioBaseChar::UpdateStatAttribute();   
	}
	else
	{
		m_HP.m_fMaxValue *= fMaxHpRate;
		m_HP.m_fCurValue = max( 0.0f, min( m_HP.m_fCurValue, m_HP.m_fMaxValue ) );
	}
}
