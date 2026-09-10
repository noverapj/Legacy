
#include "stdafx.h"

#include "ioSymbolStruct.h"
#include "ioGaugeHelper.h"

ioSymbolStruct::ioSymbolStruct( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioPlayEntity( pGrp, pMode )
{
	m_iArrayIndex = 0;
	m_State = SS_DELAY;
	m_pGauge = NULL;

	if( m_pGroup )
	{
		m_pGroup->SetLightApplyMask( APPLY_SECOND_MAIN_LIGHT );
	}

	m_ArmorType = AT_UNARMOR;

	m_fDefaultMaxHP = FLOAT1;
	m_OrgTeam = TEAM_NONE;

	m_fSymbolRange = FLOAT1;
	m_iRevivalCnt = 0;
	m_dwWoundedSetTime = 0;
	m_bShakeLeft = false;

	m_fExtraHP = 0.0f;
	m_fExtraRate = FLOAT1;

	m_fRevivalRate = 0.0f;
	m_fRevivalHP = 0.0f;

	m_fCurMaxHP = 0.0f;
	m_fDecreaseCurRate = 0.0f;
	m_dwDecreaseCheckTime = 0;

	m_fRecoveryRate = 0.0f;

	m_bDie = false;

	m_dwSymbolChangeTime = 0;
}

ioSymbolStruct::~ioSymbolStruct()
{
	SAFEDELETE( m_pGauge );
}

void ioSymbolStruct::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ProcessWoundedState();
	ProcessRecovery();
}

void ioSymbolStruct::UpdateAfter( float fFrameGap )
{
	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioPlayEntity::UpdateAfter( fFrameGap );
}

ApplyWeaponExp ioSymbolStruct::ApplyWeapon( ioWeapon *pWeapon )
{
	if( !m_bActivity )
	{
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;
	
	if( P2PNetwork::IsNetworkPlaying() && 
		m_pModeParent->IsNetworkMode() &&
		!pWeapon->IsGameOwnerWeapon() )
	{
		SetWoundedState();
		return AWE_EXPLOSION;
	}

	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioSymbolStruct::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);

	float fDamage = pWeapon->GetDamage( this );
	float fPushPower = pWeapon->GetDefensePushPower( fDamage );
	float fMemberBalanceRate = pWeapon->GetExtraDamageRate();
	if( fMemberBalanceRate > FLOAT1 )
	{
		fDamage = fDamage / fMemberBalanceRate;
	}

	if( m_pModeParent->GetModeState() != ioPlayMode::MS_PLAY )
		fDamage = 0.0f;

	// Block Damage
	float fBlockRate = (float)pWeapon->GetBlockingRate() / FLOAT100;
	fDamage *= fBlockRate;

	ActionStopType eActionStop = pWeapon->GetActionStopType();
	int iShakeCamera = pWeapon->GetShakeCamera();

	if( eActionStop == AST_BOTH || eActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
		pWeapon->SetActionStopDuration( ioBaseChar::CalcActionStopDelay( fDamage, false ),
										ioBaseChar::GetActionStopLooseRate( false ) );
	}

	bool bAddDamage = false;
	bool bBlock = false;
	ioSymbolMode *pMode = ToSymbolMode( m_pModeParent );

	if( pWeapon->GetTeam() != GetTeam() )
	{
		if( pMode )
		{
			if( pMode->IsEnableSymbolDamage( this, pWeapon->GetTeam() ) )
			{
				m_HP.m_fCurValue -= fDamage;
				AddHeadDamageInfo( (int)fDamage );
			}
			else
			{
				bBlock = true;
			}
		}
		else
		{
			m_HP.m_fCurValue -= fDamage;
			AddHeadDamageInfo( (int)fDamage );
		}
	}
	else
	{
		// 상대팀에서 때릴수 없는 상징물이면 우리도 못때림
		if( pMode && GetTeam() == TEAM_RED )
		{
			if( !pMode->IsEnableSymbolDamage( this, TEAM_BLUE ) )
				bBlock = true;
		}
		else if( pMode && GetTeam() == TEAM_BLUE )
		{
			if( !pMode->IsEnableSymbolDamage( this, TEAM_RED ) )
				bBlock = true;
		}

		if( !bBlock )
		{
			fDamage *= m_fTeamExtraDamageRate;

			if( m_HP.m_fCurValue < m_HP.m_fMaxValue )
			{
				if( m_HP.m_fCurValue + fDamage < m_HP.m_fMaxValue )
				{
					m_HP.m_fCurValue += fDamage;
				}
				else
				{
					fDamage = m_HP.m_fMaxValue - m_HP.m_fCurValue;
					m_HP.m_fCurValue = m_HP.m_fMaxValue;
				}

				if( fDamage > 0.0f )
				{
					AddHeadDamageInfo( (int)fDamage, true );
				}
				bAddDamage = true;
			}
			else 
			{
				fDamage    = 0.0f;  
				bAddDamage = false;
			}
		}

		//같은팀 상징물의 HP가 MAX일 때 타격하면 삽질맨!
		if( fDamage == 0.0f )
			g_AwardMgr.AddBlockStoneAttack( pWeapon->GetOwnerName() );
	}

	bool bOnlyEmoticon = true;
	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT3 )
		bContactType = true;

	if( m_HP.m_fCurValue > 0.0f )
	{
		if( bBlock )
		{
			if( bContactType && !pWeapon->CheckWeaponBySkill() )
			{
				if( pAttacker->IsEnableAttackCancelState() && !pWeapon->IsChangeJumpping( false, false )
					&& !pAttacker->IgnoreBlock() )
				{
					pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
					pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );
					bOnlyEmoticon = false;
				}
			}

			if( pAttacker->IsOwnerChar() )
			{
				if( GetTeam() == pWeapon->GetTeam() )
					pAttacker->CheckNamedEmoticon( ioModeHelpManager::HT_TEAM_SYMBOL_ATTACK, false );
				else
					pAttacker->CheckNamedEmoticon( ioModeHelpManager::HT_SYMBOL_ATTACK, false );
			}
		}

		SetWoundedState();

		ioSymbolMode *pSymbolMode = ToSymbolMode( m_pModeParent );
		if( pSymbolMode )
			pSymbolMode->NotifySymbolWounded( this, pAttacker, fDamage );

		int iWeaponType = (int)pWeapon->GetType();
		if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_SYMBOL_DAMAMGED );
			kPacket << pWeapon->GetOwnerName();
			kPacket << m_iArrayIndex;
			kPacket << iWeaponType;
			kPacket << m_iRevivalCnt;
			kPacket << m_HP.m_fMaxValue;
			kPacket << bBlock;
			kPacket << bOnlyEmoticon;
			kPacket << vAttackDir;
			kPacket << pWeapon->GetTeam();
			kPacket << bAddDamage;
			kPacket << fDamage;
			kPacket << fPushPower;
			kPacket << (int)eActionStop;
			kPacket << iShakeCamera;
			kPacket << pWeapon->GetWeaponIndex();
			P2PNetwork::SendToAllPlayingUser( kPacket );

			g_AbuseMgr.AddDamage( (int)fDamage );

			ioSymbolMode *pSymbolMode = ToSymbolMode( m_pModeParent );
			if( pSymbolMode && fDamage > 0.0f && !bBlock )
			{
				pSymbolMode->SymbolDamageContribute( GetTeam(), pWeapon->GetOwnerName(), fDamage, iWeaponType );

				if( GetTeam() != pWeapon->GetTeam() )
				{
					g_ModeUIController.SetSymbolAttack( GetTeam() );
				}
			}
		}
	}
	else
	{
		if( !m_bDie )
		{
			m_pModeParent->NotifyEntityDie( this, pAttacker, NULL, FLOAT1, FLOAT1 );
			m_bDie = true;
		}
	}

	if( pAttacker->IsOwnerChar() )
	{
		// ChangeJumpping
		if( pWeapon && pWeapon->IsChangeJumpping( false, false ) )
		{
			int iLimitCnt = pWeapon->GetChangeJumppingLimitCnt();
			pAttacker->SetChangeJumppingState( pWeapon->GetChangeJumppingPowerRate(), true, iLimitCnt );
		}
	}

	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	return AWE_EXPLOSION;
}

void ioSymbolStruct::Revival()
{
	if( m_fRevivalHP == 0.0f )
		m_HP.m_fCurValue = m_HP.m_fMaxValue;
	else
		m_HP.m_fCurValue = m_fRevivalHP;

	m_HP.m_dwPreRecoveryTime = FRAMEGETTIME();

	m_fRevivalHP = 0.0f;

	m_bDie = false;
}

void ioSymbolStruct::SetRevivalHP( float fHP, bool bInit )
{
	if( bInit )
	{
		m_fRevivalHP = m_HP.m_fMaxValue;

		m_dwDecreaseCheckTime = FRAMEGETTIME();
		m_fDecreaseCurRate = 0.0f;
	}
	else
	{
		m_fRevivalHP = m_HP.m_fMaxValue;
	}
}

void ioSymbolStruct::SetTeam( TeamType eType )
{
	ioPlayEntity::SetTeam( eType );
	ioBaseChar *pOwner = m_pModeParent->GetOwnerChar();
	
	if( m_pGauge )
	{
		if( eType == TEAM_NONE )
			m_pGauge->SetTeam( TEAM_NONE );
		else if(eType == TEAM_BLUE )
			m_pGauge->SetTeam( TEAM_BLUE );
		else if(eType == TEAM_RED )
			m_pGauge->SetTeam( TEAM_RED );
		else
		{
			if( !pOwner )
				return;

			if( pOwner->GetTeam() == eType )
				m_pGauge->SetTeam( TEAM_BLUE );
			else
				m_pGauge->SetTeam( TEAM_RED );
		}
	}

	ioHashString szNewSymbolModel;
	switch( eType )
	{
	case TEAM_NONE:
		if( m_bActivity )
			szNewSymbolModel = m_NoneSymbolModel;
		else
			szNewSymbolModel = m_NoneActivitySymbolModel;
		break;
	case TEAM_RED:
		szNewSymbolModel = m_RedSymbolModel;
		break;
	case TEAM_BLUE:
		szNewSymbolModel = m_BlueSymbolModel;
		break;
	default:
		if( !pOwner )
			return;

		if( pOwner->GetTeam() == eType )
			szNewSymbolModel = m_BlueSymbolModel;
		else
			szNewSymbolModel = m_RedSymbolModel;
	}

	if( m_pGroup->GetFileName() == szNewSymbolModel )
		return;

	ioSceneNode *pSceneNode = m_pGroup->GetParentSceneNode();
	if( !pSceneNode )	return;

	pSceneNode->DetachObject( m_pGroup );
	pSceneNode->GetSceneMgr()->DestroyEntityGrp( m_pGroup );

	m_pGroup = pSceneNode->GetSceneMgr()->CreateEntityGrp();
	pSceneNode->AttachObject( m_pGroup );

	m_pGroup->SetLightApplyMask( APPLY_SECOND_MAIN_LIGHT );
	m_pGroup->LoadGroup( szNewSymbolModel.c_str() );
	m_pGroup->SetAlphaRate( m_iCurAlphaRate );

	PlaySound( m_ExplosionSound );
}

bool ioSymbolStruct::IsMountAvailable() const
{
	return true;
}

bool ioSymbolStruct::IsMountingAvailable() const
{
	return false;
}

bool ioSymbolStruct::IsNowMountEnable() const
{
	return true;
}

void ioSymbolStruct::SetOrgTeam( TeamType eTeam )
{
	m_OrgTeam = eTeam;
	SetTeam( eTeam );
}

void ioSymbolStruct::RestoreOrgTeam()
{
	SetTeam( m_OrgTeam );
}

void ioSymbolStruct::ResetTeam()
{
	SetTeam( m_TeamType );
}

void ioSymbolStruct::UpdateGauge()
{
	m_bGaugeVisible = false;

	if( !m_bActivity )	return;

	D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
	vPos.y += 40.0f;

	if( Setting::Check2DInScreen( vPos, m_iGaugeXPos, m_iGaugeYPos ) )
	{
		m_bGaugeVisible = true;

		if( m_pGauge )
		{
			m_pGauge->UpdateGauge( m_HP.m_fCurValue, m_HP.m_fMaxValue );
		}
	}
}

void ioSymbolStruct::RenderGauge()
{
	if( !m_pGauge || !m_bGaugeVisible ) return;

	bool bCheckMode = g_GUIMgr.IsNoneUIMode();
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( pStage && !bCheckMode )
	{
		if( pStage->HasCameraFixedStyle(CAMERA_STYLE_MOVIE) || pStage->HasCameraFixedStyle(CAMERA_STYLE_VIEW) )
			bCheckMode = true;
	}
	if( bCheckMode && !Setting::NoUIModeHPShow() ) return;

	D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
	vPos.y += 40.0f;

	int iXPos = 0, iYPos = 0;
	if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
	{
		m_pGauge->RenderGauge( iXPos, iYPos + 1 );
	}
	else
	{
		m_pGauge->RenderGauge( m_iGaugeXPos + 12, m_iGaugeYPos + 2 );
	}
}

bool ioSymbolStruct::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( !IsCollisionAvailableRange( pWeapon ) )
		return false;

	if( pWeapon->IsAlreadyCollisioned( this ) )
		return false;

	return true;
}

int ioSymbolStruct::DontMoveEntityLevel() const
{
	return 6;
}

ioPlayEntity::PlaySubType ioSymbolStruct::GetSubType() const
{
	return PST_SYMBOL;
}

ArmorType ioSymbolStruct::GetArmorType() const
{
	return m_ArmorType;
}

void ioSymbolStruct::UpdateBottomHeight() const
{
	m_fCurBottomHeight = GetWorldPosition().y;
}

void ioSymbolStruct::SetHP( float fCurHP, float fMaxHP, float fDecreaseCurRate )
{
	m_HP.m_fCurValue = fCurHP;
	m_HP.m_fMaxValue = fMaxHP;

	m_HP.m_fCurValue = min( m_HP.m_fCurValue, m_HP.m_fMaxValue );

	m_fDecreaseCurRate = fDecreaseCurRate;
}

void ioSymbolStruct::UpdateMaxHP( float fMaxHP )
{
	float fPreHP = m_HP.m_fMaxValue;
	m_HP.m_fMaxValue = fMaxHP;

	if( fPreHP == 0.0f )
		m_HP.m_fCurValue = m_HP.m_fMaxValue;
	else
		m_HP.m_fCurValue *= m_HP.m_fMaxValue / fPreHP;
}

void ioSymbolStruct::ReduceMaxHP( float fRate )
{
	m_HP.m_fCurValue *= fRate;
	m_HP.m_fMaxValue *= fRate;
}

void ioSymbolStruct::SetWoundedState()
{
	m_State = SS_WOUNDED;
	m_dwWoundedSetTime = FRAMEGETTIME();
}

void ioSymbolStruct::SetEndFocusEffect( const ioHashString &szEndFocusEffect )
{
	if( !szEndFocusEffect.IsEmpty() )
	{
		if( m_pGroup )
			m_pGroup->AttachEffect( szEndFocusEffect, NULL );
	}
}

void ioSymbolStruct::ProcessWoundedState()
{
	if( m_State != SS_WOUNDED )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwWoundedSetTime + 100 < dwCurTime )
	{
		SetWorldPosition( m_vBackUpPos );

		m_State = SS_DELAY;
		m_dwWoundedSetTime = 0;
		m_bShakeLeft = false;
	}
	else
	{
		D3DXVECTOR3 vShakePos = m_vBackUpPos;
		if( m_bShakeLeft )
			vShakePos.x += FLOAT10;
		else
			vShakePos.x -= FLOAT10;

		SetWorldPosition( vShakePos );
		m_bShakeLeft = !m_bShakeLeft;
	}
}

void ioSymbolStruct::InitResource()
{
	m_pGauge = new ioGaugeHelper;
	m_pGauge->InitResource();
}

void ioSymbolStruct::LoadProperty( ioINILoader &rkLoader )
{
	m_ArmorType	  = (ArmorType)rkLoader.LoadInt_e( "armor_type", AT_UNARMOR );

	m_fSymbolRange = rkLoader.LoadFloat_e( "range", FLOAT100 );

	m_fRevivalRate = rkLoader.LoadFloat_e( "revival_hp_rate", FLOAT05 );
	m_fExtraHP = rkLoader.LoadFloat_e( "extra_hp", 0.0f );
	m_fDefaultMaxHP = rkLoader.LoadFloat_e( "max_hp", FLOAT10 );
	m_HP.m_fMaxValue = 0.0f;
	m_HP.m_fCurValue = 0.0f;
	m_fCurMaxHP = m_fDefaultMaxHP;
	m_HP.m_fMaxValue = m_fCurMaxHP;
	m_HP.m_fCurValue = m_fCurMaxHP;

	m_fRecovery = rkLoader.LoadFloat_e( "hp_recovery", 0.0f );
	m_HP.m_dwRecoveryTick = rkLoader.LoadInt_e( "hp_recovery_tick", 2000 );

	m_fCharHpRecovery = rkLoader.LoadFloat_e( "heal_hp_recovery", FLOAT10 );
	m_fSkillGaugeRate = rkLoader.LoadFloat_e( "skill_gauge_rate", FLOAT1 );
	m_fCharDamageRate = rkLoader.LoadFloat_e( "damage_rate", FLOAT1 );

	m_fTeamExtraDamageRate = rkLoader.LoadFloat_e( "team_extra_damage_rate", FLOAT1 );

	m_fDecreaseRate = rkLoader.LoadFloat_e( "decrease_hp_rate", 0.0f );
	m_fDecreaseMaxRate = rkLoader.LoadFloat_e( "decrease_hp_max_rate", 0.0f );

	m_dwDecreaseTicTime = rkLoader.LoadInt_e( "decrease_hp_tic_time", 0 );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "red_symbol", "staticmodel/symbol_red.txt", szBuf, MAX_PATH );
	m_RedSymbolModel = szBuf;
	rkLoader.LoadString_e( "blue_symbol", "staticmodel/symbol_blue.txt", szBuf, MAX_PATH );
	m_BlueSymbolModel = szBuf;
	rkLoader.LoadString_e( "none_symbol", "staticmodel/symbol_nor.txt", szBuf, MAX_PATH );
	m_NoneSymbolModel = szBuf;
	rkLoader.LoadString_e( "none_activity_symbol", "staticmodel/symbol_disable.txt", szBuf, MAX_PATH );
	m_NoneActivitySymbolModel = szBuf;
	rkLoader.LoadString_e( "explosion_sound", "", szBuf, MAX_PATH );
	m_ExplosionSound = szBuf;

	rkLoader.LoadString_e( "symbol_block_buff", "", szBuf, MAX_PATH );
	m_BlockBuff = szBuf;
}

void ioSymbolStruct::InitSymbolInfo( int iIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale )
{
	m_iArrayIndex = iIndex;

	SetWorldPosition( vPos );
	m_vDefaultPos = vPos;
	m_vBackUpPos = vPos;
	SetWorldScale( vScale );
}

bool ioSymbolStruct::IsInSymbolRange( ioBaseChar *pChar )
{
	if( pChar->GetTeam() != GetTeam() )
		return false;

	CharState eState = pChar->GetState();
	if( eState == CS_DIE ||
		eState == CS_READY ||
		eState == CS_CHANGE_WAIT ||
		eState == CS_VIEW ||
		eState == CS_OBSERVER ||
		eState == CS_LOADING ||
		eState == CS_ENDMOTION )
		return false;

	float fRangeSq = m_fSymbolRange * m_fSymbolRange;

	D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - m_vBackUpPos;
	if( D3DXVec3LengthSq( &vDiff ) > fRangeSq )
		return false;

	return true;
}

void ioSymbolStruct::OnSymbolDamaged( SP2Packet &rkPacket, ioBaseChar *pAttacker )
{
	int iWeaponType, iRevivalCnt;
	rkPacket >> iWeaponType >> iRevivalCnt;
	if( iRevivalCnt != m_iRevivalCnt )
		return;

	float fMaxValue;
	rkPacket >> fMaxValue;

	bool bBlock, bOnlyEmoticon;
	D3DXVECTOR3 vAttackDir;
	rkPacket >> bBlock >> bOnlyEmoticon;
	rkPacket >> vAttackDir;

	int iTeam;
	rkPacket >> iTeam;
	TeamType eAttackTeam = (TeamType)iTeam;

	bool bAddDamage;
	float fDamage, fPushPower;
	rkPacket >> bAddDamage >> fDamage >> fPushPower;

	if( bBlock )
	{
		if( !bOnlyEmoticon && pAttacker->IsEnableAttackCancelState() && !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );
		}

		pAttacker->SetDisableAttackEmoticon();
	}
	else
	{
		if( fDamage > 0.0f )
		{
			if( !bAddDamage && eAttackTeam != GetTeam() )
			{
				m_HP.m_fMaxValue = fMaxValue;
				m_HP.m_fCurValue -= fDamage;
			}
			else if( bAddDamage && eAttackTeam == GetTeam() )
			{
				m_HP.m_fMaxValue = fMaxValue;
				m_HP.m_fCurValue += fDamage;
				m_HP.m_fCurValue = min( m_HP.m_fCurValue, m_HP.m_fMaxValue );
			}

			AddHeadDamageInfo( fDamage, bAddDamage );

			ioSymbolMode *pSymbolMode = ToSymbolMode( m_pModeParent );
			if( pSymbolMode && !bBlock )
			{
				pSymbolMode->SymbolDamageContribute( GetTeam(), pAttacker->GetCharName(), fDamage, iWeaponType );
				pSymbolMode->NotifySymbolWounded( this, pAttacker, fDamage );
				if( GetTeam() != eAttackTeam )
				{
					g_ModeUIController.SetSymbolAttack( GetTeam() );
				}
			}
		}
		else if( fDamage == 0.0f ) 
		{
			//같은팀 상징물의 HP가 MAX일 때 타격하면 삽질맨!
			if( GetTeam() == eAttackTeam )
			{
				if( fDamage == 0.0f )
					g_AwardMgr.AddBlockStoneAttack( pAttacker->GetCharName() );
			}
		}
	}

	int iActionStop = 0;
	rkPacket >> iActionStop;

	int iShakeCamera = 0;
	rkPacket >> iShakeCamera;

	if( iActionStop == AST_BOTH || iActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
	}

	int iWeaponIndex = 0;
	rkPacket >> iWeaponIndex;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pAttacker, iWeaponIndex );
	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	SetWoundedState();
}

void ioSymbolStruct::SetSymbolActivity( bool bActivity )
{
	m_bActivity = bActivity;

	if( m_bActivity )
		SetWorldPosition( m_vDefaultPos );
	else
		SetWorldPosition( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );

	SetTeam( TEAM_NONE );
}

void ioSymbolStruct::SetSymbolPriority( int iPriority )
{
	m_iSymbolPriority = iPriority;
}

void ioSymbolStruct::SetAddHP( int iCharCnt )
{
	float fChangeRate = FLOAT1;
	float fPreMaxHP = m_HP.m_fMaxValue;

	m_fCurMaxHP = (m_fDefaultMaxHP + (m_fExtraHP * iCharCnt)) * m_fExtraRate;
	m_HP.m_fMaxValue = m_fCurMaxHP * ( FLOAT1 - m_fDecreaseCurRate/FLOAT100 );

	fChangeRate = m_HP.m_fMaxValue / fPreMaxHP;

	m_HP.m_fCurValue *= fChangeRate;
	m_HP.m_fCurValue = min(m_HP.m_fCurValue, m_HP.m_fMaxValue);
}

void ioSymbolStruct::SetExtraRate( int iCharCnt, float fRate )
{
	m_fExtraRate = fRate;

	float fChangeRate = FLOAT1;
	float fPreMaxHP = m_HP.m_fMaxValue;

	m_fCurMaxHP = (m_fDefaultMaxHP + (m_fExtraHP * iCharCnt)) * m_fExtraRate;
	m_HP.m_fMaxValue = m_fCurMaxHP * ( FLOAT1 - m_fDecreaseCurRate/FLOAT100 );

	fChangeRate = m_HP.m_fMaxValue / fPreMaxHP;

	m_HP.m_fCurValue *= fChangeRate;
	m_HP.m_fCurValue = min(m_HP.m_fCurValue, m_HP.m_fMaxValue);
}

void ioSymbolStruct::ProcessHPDecrease()
{
	if( m_dwDecreaseTicTime == 0 )
		return;

	if( m_pModeParent->GetModeState() != ioPlayMode::MS_PLAY )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	int iTickGap = (int)( dwCurTime - m_dwDecreaseCheckTime );

	if( iTickGap >= (int)m_dwDecreaseTicTime )
	{
		m_dwDecreaseCheckTime = dwCurTime - ( iTickGap - m_dwDecreaseTicTime );
		m_fDecreaseCurRate += m_fDecreaseRate;
		m_fDecreaseCurRate = min( m_fDecreaseCurRate, m_fDecreaseMaxRate );

		float fPreMaxHP = m_HP.m_fMaxValue;
		m_HP.m_fMaxValue = m_fCurMaxHP * ( FLOAT1 - m_fDecreaseCurRate/FLOAT100 );

		float fChangeRate = m_HP.m_fMaxValue / fPreMaxHP;
		m_HP.m_fCurValue *= fChangeRate;
		m_HP.m_fCurValue = min(m_HP.m_fCurValue, m_HP.m_fMaxValue);
	}
}

float ioSymbolStruct::GetCurHPRate() 
{
	if( m_HP.m_fMaxValue == 0.0f )
		return FLOAT1;

	return m_HP.m_fCurValue / m_HP.m_fMaxValue;
}

void ioSymbolStruct::ProcessRecovery()
{
	if( !m_pModeParent ) return;
	if( m_pModeParent->GetModeState() != ioPlayMode::MS_PLAY ) return;

	m_HP.m_fRecovery = m_fRecovery + m_fRecoveryRate;

	ioPlayEntity::ProcessRecovery();
}

void ioSymbolStruct::SetChangeEvent()
{
	m_dwSymbolChangeTime = FRAMEGETTIME();
}

DWORD ioSymbolStruct::GetEventGapTime()
{
	DWORD dwGapTime = 0;

	if( m_dwSymbolChangeTime > 0 )
		dwGapTime = m_dwSymbolChangeTime;
	else
		return 0;

	return (FRAMEGETTIME() - dwGapTime );
}

D3DXVECTOR3 ioSymbolStruct::GetSpotPosition( float fPositionRate )
{
	D3DXVECTOR3 vCurPos = GetWorldPosition();

	if( vCurPos == m_vDefaultPos )
		return m_vDefaultPos;

	D3DXVECTOR3 vDiff = vCurPos - m_vDefaultPos;
	vDiff = vDiff * fPositionRate;

	vCurPos = m_vDefaultPos + vDiff;

	return vCurPos;
}