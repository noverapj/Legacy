

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioStatAttribute.h"

#include "ioWeapon.h"
#include "ioPlayStage.h"

CEncrypt<float> ioWeapon::m_fPushPowerCtrlValue;
CEncrypt<float> ioWeapon::m_fBlowPowerCtrlValue;
CEncrypt<float> ioWeapon::m_fAirPushPowerCtrlValue;
CEncrypt<float> ioWeapon::m_fAirBlowPowerCtrlValue;

ioWeapon::ConstPowerValue ioWeapon::m_ConstPowerValue[BTT_MAX_TYPE];

ioWeapon::ioWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
{
	m_pSceneNode = NULL;

	m_pConstAttr = pAttr;
	m_pResistanceAttr = NULL;

	m_OwnerName = pOwner->GetCharName();

	m_TeamType = TEAM_NONE;
	m_fOwnerDamageRate = FLOAT1;
	m_fOwnerDamageRateNPC = FLOAT1;

	m_fOwnerChargeForceRate = FLOAT1;
	m_fOwnerChargeDamageRate = FLOAT1;

	m_fOwnerChargeRangeRate = FLOAT1;
	m_fOwnerChargeSpeedRate = FLOAT1;

	m_fOwnerChargeBlowRate = FLOAT1;
	m_fOwnerChargeFloatRate = FLOAT1;

	m_fOwnerChargeScaleRate = FLOAT1;
	m_fOwnerChargeExplosionRate = FLOAT1;

	m_fOwnerChargeWoundedTimeRate = FLOAT1;
	m_fOwnerChargeLiveTimeRate = FLOAT1;

	m_fOwnerChargeBuffLiveTimeRate = FLOAT1;

	m_CollisionType = ACT_NONE;
	m_dwCreatedTime = FRAMEGETTIME();
	m_dwLiveTime = 0;

	m_dwWeaponIdx = 0;
	m_dwWoundedDuration = 0;

	m_dwWeaponPeculiarity = 0;

	m_bBuffWeapon = false;
	m_bBuffWeapon2 = false;
	m_bUseSkill = false;
	m_bJumpAttack = false;

	m_bGameOwnerWeapon = false;
	m_bLoopSoundPlayed = false;
	m_bLive = true;

	m_fWeakRate = -FLOAT1;
	m_bResistance = false;
	m_dwWeakElement = 0;

	m_dwActionStopDuration = 0;
	m_fActionStopLooseRate = 0.0f;

	m_bSetCollision = false;
	m_dwPiercingEndTime = 0;

	m_fEnhancePushPowerRate = FLOAT1;
	m_fEnhanceBlowPowerRate = FLOAT1;
	m_fEnhanceAirPushPowerRate = FLOAT1;
	m_fEnhanceAirBlowPowerRate = FLOAT1;

	m_fGrowthMoveRange = 0.0f;
	m_fGrowthExplosionRange = 0.0f;

	m_fGrowthAttackRate = FLOAT1;
	m_fGrowthPushRate = 0.0f;
	m_fGrowthAirPushRate = 0.0f;
	m_fGrowthBlowRate = 0.0f;
	m_fGrowthAirBlowRate = 0.0f;

	m_bOnlyDownAttack = false;
	m_bCollisionCancel = false;
	m_bLoopWoundedAni = false;

	m_dwAttachAreaWeaponIndex = 0;

	m_bStructWeapon = false;
	m_iStructIndex = 0;
	
	m_bMachineWeapon = false;
	m_iMachineIndex = 0;

	m_bDummyCharWeapon = false;
	m_iDummyIndex = 0;

	m_bMoveDelay = true;
	m_bSetSylphid = false;
	m_bPassCalcDamageActionStopDelay = false;

	m_bReserveCallWeapon = false;

	m_fArmorPenetration = 0;
	m_fCriticalRate = 0;
	m_fCriticalDamageRate = 0;

	m_dwNetSyncTime = 0;

	m_nUniqID = 0;
}

ioWeapon::~ioWeapon()
{
	StopLoopSound();
}

void ioWeapon::SetUniqID(int nID)
{
	m_nUniqID = nID;
}


int ioWeapon::GetUniqID()
{
	return m_nUniqID;
}


void ioWeapon::InitStaticValue( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "static_value" );

	m_fPushPowerCtrlValue = rkLoader.LoadFloat_e( "push_power_control", FLOAT1 );
	m_fBlowPowerCtrlValue = rkLoader.LoadFloat_e( "blow_power_control", FLOAT1 );
	m_fAirPushPowerCtrlValue = rkLoader.LoadFloat_e( "air_push_power_control", FLOAT1 );
	m_fAirBlowPowerCtrlValue = rkLoader.LoadFloat_e( "air_blow_power_control", FLOAT1 );

	char szBuf[MAX_PATH];
	char *szPowerText[] = { "weak", "blow", "flatten", "mid_stiff", "str_stiff",
							"stand_ko", "stand_ko2", "blow2", "blow3", "boundblow",
							"mid_blow", "getup", "getup_stand_ko", "blow_extend" };

	for( int i=0 ; i<BTT_MAX_TYPE ; i++ )
	{
		wsprintf_e( szBuf, "%s_push_power_const", szPowerText[i] );
		m_ConstPowerValue[i].m_fPushConst = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "%s_blow_power_const", szPowerText[i] );
		m_ConstPowerValue[i].m_fBlowConst = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "%s_air_push_power_const", szPowerText[i] );
		m_ConstPowerValue[i].m_fAirPushConst = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "%s_air_blow_power_const", szPowerText[i] );
		m_ConstPowerValue[i].m_fAirBlowConst = rkLoader.LoadFloat( szBuf, 0.0f );
	}
}

void ioWeapon::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_ChangeJumppingType = (ChangeJumpType)rkLoader.LoadInt_e( "change_jumpping", CJT_NONE );
	m_ChangeNextItemType = (ChangeNextItemType)rkLoader.LoadInt_e( "change_next_item_tpye", CNIT_NONE );
	m_ChangeNextSkillType = (ChangeNextSkillType)rkLoader.LoadInt_e( "change_next_skill_tpye", CNST_NONE );

	m_fChangeJumppingPowerRate = rkLoader.LoadFloat_e( "change_jumpping_power_rate", FLOAT1 );
	m_iChangeJumppingCnt = rkLoader.LoadInt_e( "change_jumpping_limit_cnt", 0 );

	rkLoader.LoadString_e( "col_skip_block_buff", "", szBuf, MAX_PATH );
	m_CharCollisionBlockBuff = szBuf;

	m_iIgnoreProtect = rkLoader.LoadInt_e( "ignore_protect", IPT_NONE );
	m_bCheckOnlyDownAttack = rkLoader.LoadBool_e( "check_only_down_attack", false );
	m_bNoWoundedWeapon = rkLoader.LoadBool_e( "no_wounded_weapon", false );
	m_bSkillDependency = rkLoader.LoadBool_e( "skill_dependency", false );

	m_bEndEffectGrace  = rkLoader.LoadBool_e( "end_effect_grace", false );
	m_bFloatStateBreak = rkLoader.LoadBool_e( "float_state_break", false );

	//
	int iAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count", 0 );
	m_vCreateWeaponList.clear();
	m_vCreateWeaponList.reserve(iAttackTypeCnt);

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_vCreateWeaponList.push_back( kInfo );
	}

	//
	rkLoader.LoadString_e( "attach_area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	m_dwAttachAreaWeaponTime = (DWORD)rkLoader.LoadInt_e( "attach_area_weapon_time", 0 );

	m_bEnableWoundSkill = rkLoader.LoadBool_e( "enable_wound_skill", true );
	m_CheckGrapplingType = (GrapplingType)rkLoader.LoadInt_e( "enable_grappling_state", 0 );

	//
	m_dwMoveDelayTime = (DWORD)rkLoader.LoadInt_e( "move_delay_time", 0 );

	int iRandomDamageCnt = rkLoader.LoadInt_e( "random_damage_cnt", 0 );
	for( int i=0; i<iRandomDamageCnt; ++i )
	{
		wsprintf_e( szKey, "random_damage_rate%d", i+1 );
		float fDamage = rkLoader.LoadFloat( szKey, FLOAT1 );
		m_vRandomDamageList.push_back( fDamage );
	}

	m_IgnoreGhostState = (IgnoreGhostStateType)rkLoader.LoadInt_e( "ignore_ghost_state", IGT_NONE );
	m_bIgnoreMapCollision	= rkLoader.LoadBool_e( "ignore_map_collision", false );
	m_bIgnoreMapCollisionEffect	= rkLoader.LoadBool_e( "ignore_map_collision_effect", false );

	LoadDummyAndPushStructDestroyList( rkLoader );
	m_bTeamAttack = rkLoader.LoadBool_e( "team_attack", false );

	m_bSetPush = rkLoader.LoadBool_e( "set_push", false );
	m_fEnablePushPowerRate = rkLoader.LoadFloat_e( "enable_push_power_rate", FLOAT1 );

	m_bPassCalcDamageActionStopDelay = rkLoader.LoadBool_e( "pass_calc_action_delay", false );

	m_iPushStructType		= rkLoader.LoadInt_e( "target_push_struct_type", 0 );
	m_fPushStructDamgeRate	= rkLoader.LoadFloat_e( "target_push_struct_damge_rate", FLOAT1 );
	m_bPushStructOnlyAttack = rkLoader.LoadBool_e( "only_push_struct_attack", false );
	
	int iMax =  rkLoader.LoadInt_e( "dead_call_area_weapon_name_max", 0 );
	for( int i = 0; i < iMax; ++i  )
	{
		DeadCallAreaInfo CallInfo;
		sprintf_s_e( szKey, "dead_call_area_weapon%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		CallInfo.CallAreaName = szBuf;

		sprintf_s_e( szKey, "dead_call_area_weapon%d_time", i+1 );
		CallInfo.dwReservedTime = rkLoader.LoadInt( szKey, 0 );

		m_DeadCallAreaWeaponNameList.push_back( CallInfo );
	}

	m_bDestroyByCharacterImmediatelyChange	= rkLoader.LoadBool_e( "destroy_char_change", true );
}

void ioWeapon::LoadDummyAndPushStructDestroyList( ioINILoader &rkLoader  )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	int iCnt = rkLoader.LoadInt_e( "destroy_object_cnt", 0 );
	m_DummyAndPushStructDestroyList.clear();
	m_DummyAndPushStructDestroyList.reserve(iCnt);

	for( int i=0 ; i<iCnt ; i++ )
	{
		DummyAndPushStruct kInfo;

		wsprintf_e( szKey, "destory_object_type%d", i+1 );
		kInfo.m_Type = (DummyAndPushStructType)rkLoader.LoadInt( szKey, OBJ_NONE );

		wsprintf_e( szKey, "destory_object_sub_type%d", i+1 );
		kInfo.m_dwSubIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "destory_object_dummy_name%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_DummyCharName = szBuf;

		m_DummyAndPushStructDestroyList.push_back( kInfo );
	}
}

void ioWeapon::CheckCreateWeaponList()
{
	if( m_vCreateWeaponList.empty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	D3DXVECTOR3 vFirePos = GetPosition();
	D3DXVECTOR3 vFireDir = GetMoveDir();

	pOwner->WeaponByWeaponExplicitList( m_vCreateWeaponList, FTT_SKILL_ATTACK, vFirePos, vFireDir );
}

bool ioWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( !pEntity )
		return false;

	bool bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision) );

	CheckPiercing( bCollision );

	return bCollision;
}

bool ioWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	return true;
}

bool ioWeapon::IsAlreadyCollisioned( ioPlayEntity *pEntity )
{
	DWORD dwCurTime = FRAMEGETTIME();
	bool bCollision = false;

	vCollisionInfoList::iterator iter;
	for( iter = m_vCollisionInfoList.begin(); iter != m_vCollisionInfoList.end(); ++iter )
	{
		if( (*iter).m_pEntity == pEntity )
		{
			if( GetRefreshDuration() > 0 )
			{
				if( dwCurTime < (*iter).m_dwCollisionTime+GetRefreshDuration() )
					bCollision = true;
				else
					bCollision = false;

				break;
			}
			else
			{
				bCollision = true;
				break;
			}
		}
	}

	return bCollision;
}

void ioWeapon::AddCollisionEntity( ioPlayEntity *pEntity )
{
	DWORD dwCurTime = FRAMEGETTIME();
	vCollisionInfoList::iterator iter;
	for( iter = m_vCollisionInfoList.begin(); iter != m_vCollisionInfoList.end(); ++iter )
	{
		if( (*iter).m_pEntity == pEntity )
		{
			(*iter).m_dwCollisionTime = dwCurTime;
			return;
		}
	}

	CollisionInfo kInfo;
	kInfo.m_pEntity = pEntity;
	kInfo.m_dwCollisionTime = dwCurTime;

	m_vCollisionInfoList.push_back( kInfo );
}


void ioWeapon::ClearCollisionList()
{
	m_vCollisionInfoList.clear();
}

void ioWeapon::UpdatePreWorldPosition()
{
	if( HasPeculiarity( WP_MISSILE_WEAPON ) )
	{
		m_vPreWorldPosition = GetPosition();
	}
}

void ioWeapon::SetOwnerDamageRate( float fRate, float fNPCRate )
{
	m_fOwnerDamageRate = fRate;
	m_fOwnerDamageRateNPC = fNPCRate;

	if( m_fOwnerDamageRateNPC == FLOAT1 )
		m_fOwnerDamageRateNPC = m_fOwnerDamageRate;
}

void ioWeapon::SetOwnerChargeDamageRate( float fRate )
{
	m_fOwnerChargeDamageRate = fRate;
}

void ioWeapon::SetOwnerChargeForceRate( float fRate )
{
	m_fOwnerChargeForceRate = fRate;
}

void ioWeapon::SetOwnerChargeRangeRate( float fRate )
{
	m_fOwnerChargeRangeRate = fRate;
}

void ioWeapon::SetOwnerChargeSpeedRate( float fRate )
{
	m_fOwnerChargeSpeedRate = fRate;
}

void ioWeapon::SetOwnerChargeBlowRate( float fRate )
{
	m_fOwnerChargeBlowRate = fRate;
}

void ioWeapon::SetOwnerChargeFloatRate( float fRate )
{
	m_fOwnerChargeFloatRate = fRate;
}

void ioWeapon::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;
}

void ioWeapon::SetOwnerChargeExplosionRate( float fRate )
{
	m_fOwnerChargeExplosionRate = fRate;
}

void ioWeapon::SetOwnerChargeWoundedTimeRate( float fRate )
{
	m_fOwnerChargeWoundedTimeRate = fRate;
}

void ioWeapon::SetOwnerChargeLiveTimeRate( float fRate )
{
	m_fOwnerChargeLiveTimeRate = fRate;
}

void ioWeapon::SetOwnerChargeBuffLiveTimeRate( float fRate )
{
	m_fOwnerChargeBuffLiveTimeRate = fRate;
}

void ioWeapon::SetGameOwnerWeapon( bool bOwner )
{
	m_bGameOwnerWeapon = bOwner;
}

void ioWeapon::SetTeam( ioPlayStage *pStage, TeamType eType )
{
	m_TeamType = eType;
}

void ioWeapon::SetLiveTime( DWORD dwTime )
{
	m_dwLiveTime = dwTime;
}

void ioWeapon::SetWeaponIndex( DWORD dwIndex )
{
	m_dwWeaponIdx = dwIndex;
}

void ioWeapon::SetAniCollisionType( AniCollisionType eType )
{
	m_CollisionType = eType;
}

void ioWeapon::SetPosition( const D3DXVECTOR3 &vPos )
{
	GetParentSceneNode()->SetPosition( vPos );
}

void ioWeapon::SetOrientation( const D3DXQUATERNION &qtRot )
{
	GetParentSceneNode()->SetOrientation( qtRot );
}

void ioWeapon::SetScale( const D3DXVECTOR3 &vScale )
{
	GetParentSceneNode()->SetScale( vScale );
}

void ioWeapon::PlayExplosionSound()
{
	if( !GetAttribute() )
		return;

	g_SoundMgr.PlaySound( GetAttribute()->m_ExplosionSound, GetParentSceneNode() );
	StopLoopSound();
}

void ioWeapon::PlayWoundedSound( ioPlayEntity *pEntity )
{
	if( HasPeculiarity( WP_WOUND_NO_SOUND ) )
		return;

	if( !GetAttribute() )
		return;

	const ioHashString &szSound = GetAttribute()->m_WoundedSound;

	if( ToBaseChar( pEntity ) )
	{
		std::string szNewSound = ToBaseChar(pEntity)->GetWoundedSoundName( szSound );
		g_SoundMgr.PlaySound( ioHashString(szNewSound.c_str()), GetParentSceneNode() );
	}
	else
	{
		g_SoundMgr.PlaySound( szSound, GetParentSceneNode() );
	}

	StopLoopSound();
}

void ioWeapon::PlayDefensedSound( ioPlayEntity *pEntity )
{
	if( HasPeculiarity( WP_DEF_NO_SOUND ) )
		return;

	if( !GetAttribute() )
		return;

	const ioHashString &szSound = GetAttribute()->m_DefensedSound;

	if( ToBaseChar( pEntity ) )
	{
		std::string szNewSound = ToBaseChar(pEntity)->GetDefensedSoundName( szSound );
		g_SoundMgr.PlaySound( ioHashString(szNewSound.c_str()), GetParentSceneNode() );
	}
	else
	{
		g_SoundMgr.PlaySound( szSound, GetParentSceneNode() );
	}

	StopLoopSound();
}

void ioWeapon::PlayLoopSound()
{
	if( m_pConstAttr && !m_bLoopSoundPlayed )
	{
		m_WeaponLoopSoundID = g_SoundMgr.PlaySound( m_pConstAttr->m_LoopSound,
													GetParentSceneNode(),
													PSM_LOOP | PSM_THREAD );

		m_bLoopSoundPlayed = true;
	}
}

void ioWeapon::StopLoopSound()
{
	if( m_pConstAttr )
	{
		g_SoundMgr.StopSound( m_pConstAttr->m_LoopSound, m_WeaponLoopSoundID );
		//g_SoundMgr.StopSound( m_pConstAttr->m_LoopSound, 0 );
		m_bLoopSoundPlayed = false;
	}
}

void ioWeapon::EnhancePushPower( float fPush, float fBlow, float fAirPush, float fAirBlow )
{
	m_fEnhancePushPowerRate = fPush;
	m_fEnhanceBlowPowerRate = fBlow;
	m_fEnhanceAirPushPowerRate = fAirPush;
	m_fEnhanceAirBlowPowerRate = fAirBlow;
}

void ioWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	SetPosition( vPos );
}

bool ioWeapon::IsTeamAttackZeroDamage( ioBaseChar *pTarget )
{
	if( !pTarget )
	{
		LOG.PrintTimeAndLog( 0, "ioWeapon::IsTeamAttackZeroDamage() - Target is NULL" );
		return false;
	}

	if( GetTeam() != pTarget->GetTeam() )
		return false;

	if( GetOwner() == pTarget )
		return false;

	return HasFeature( WF_TEAM_ATTACK_NO_DAMAGE );
}

bool ioWeapon::IsEnableItemDrop( ioBaseChar *pTarget )
{
	return !HasFeature( WF_NO_ITEM_DROP );
}

bool ioWeapon::HasFeature( DWORD dwFeature ) const
{
	if( !GetAttribute() )
		return false;

	if( GetAttribute()->m_dwWeaponFeature & dwFeature )
		return true;

	return false;
}

void ioWeapon::SetPeculiarity( DWORD dwPeculiarity )
{
	m_dwWeaponPeculiarity = dwPeculiarity;
}

bool ioWeapon::HasPeculiarity( DWORD dwPeculiarity ) const
{
	if( m_dwWeaponPeculiarity & dwPeculiarity )
		return true;

	return false;
}

int ioWeapon::GetAttributeIndex() const
{
	if( !GetAttribute() )
		return -1;

	return GetAttribute()->m_iAttributeIndex;
}

int ioWeapon::GetResistanceAttributeIndex() const
{
	if( m_pResistanceAttr )
		return m_pResistanceAttr->m_iAttributeIndex;

	return -1;
}

int ioWeapon::GetShakeCamera() const
{
	if( !GetAttribute() )
		return -1;

	return GetAttribute()->m_iShakeCamera;
}

const BuffInfoList& ioWeapon::GetTargetAirBuffList() const
{
	return GetAttribute()->m_TargetAirBuffList;
}

const BuffInfoList& ioWeapon::GetTargetBuffList() const
{
	return GetAttribute()->m_TargetBuffList;
}

const BuffInfoList& ioWeapon::GetTargetDownBuffList() const
{
	return GetAttribute()->m_TargetDownBuffList;
}

const BuffInfoList& ioWeapon::GetTargetDefenseBuffList() const
{
	return GetAttribute()->m_TargetDefenseBuffList;
}

// 2020-02-19
const BuffInfoList& ioWeapon::GetTargetAttackBuffList() const
{
	return GetAttribute()->m_TargetAttackBuffList;
}

// 2020-02-20
bool ioWeapon::IsTargetAttackBuffActive() const
{
	return GetAttribute()->m_bTargetAttackBuffActive;
}

// 2020-02-19
const BuffInfoList& ioWeapon::GetTargetDashBuffList() const
{
	return GetAttribute()->m_TargetDashBuffList;
}

// 2020-02-20
bool ioWeapon::IsTargetDashBuffActive() const
{
	return GetAttribute()->m_bTargetDashBuffActive;
}

const D3DXVECTOR3& ioWeapon::GetPosition() const
{
	return m_pSceneNode->GetDerivedPosition();
}

const D3DXQUATERNION& ioWeapon::GetOrientation() const
{
	return m_pSceneNode->GetDerivedOrientation();
}

const D3DXVECTOR3& ioWeapon::GetScale() const
{
	return m_pSceneNode->GetDerivedScale();
}

bool ioWeapon::IsNoExplosionEffectWithChar() const
{
	return GetAttribute()->m_bNoExplosionEffectWithChar;
}

const ioHashString& ioWeapon::GetExplosionEffectName() const
{
	return GetAttribute()->m_ExplosionEffect;
}

const ioHashString& ioWeapon::GetExtraExplosionEffectName() const
{
	return GetAttribute()->m_ExtraExplosionEffect;
}

const ioHashString& ioWeapon::GetWeaponEndEffect() const
{
	return GetAttribute()->m_WeaponEndEffect;
}

BlowTargetType ioWeapon::GetBlowType() const
{
	return GetAttribute()->m_BlowTargetType;
}

CharPiercingType ioWeapon::GetPiercingType() const
{
	return GetAttribute()->m_PiercingType;
}

DefenseBreakType ioWeapon::GetDefenseBreakType() const
{
	if( IsDefenseBreakBuff() && GetAttribute()->m_DefenseBreakType == DBT_NONE )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->HasBuff(BT_DEFENSE_BREAK_BUFF) )
		{
			return DBT_CHANGE;
		}
	}

	return GetAttribute()->m_DefenseBreakType;
}

bool ioWeapon::IsDefenseBreakBuff() const
{
	return GetAttribute()->m_bDefenseBreakBuffEnable;
}

const ioHashString& ioWeapon::GetDefenseBreakBuff() const
{
	return GetAttribute()->m_DefenseBreakBuff.m_BuffName;
}

EnableSideType ioWeapon::GetEnableSideType() const
{
	return GetAttribute()->m_EnableSideType;
}

bool ioWeapon::IsSelectCollisionBuff() const
{
	return GetAttribute()->m_bSelectCollisionBuffEnable;
}

bool ioWeapon::IsSelectCollisionBuffReverse() const
{
	return GetAttribute()->m_bSelectCollisionBuffEnableReverse;
}

DefenseSuccessType ioWeapon::GetDefenseSuccessType() const
{
	return GetAttribute()->m_DefenseSuccessType;
}

DWORD ioWeapon::GetWeaponPeculiarity() const
{
	return m_dwWeaponPeculiarity;
}

DWORD ioWeapon::GetWeaponFeature() const
{
	return GetAttribute()->m_dwWeaponFeature;
}

//( 배율 * 데미지 + 상수 ) * 기본밀림 * 강화버프배율

float ioWeapon::GetPushPower( float fDamage ) const
{
	float fPushPower = m_fPushPowerCtrlValue * fDamage;
	fPushPower += m_ConstPowerValue[GetBlowType()].m_fPushConst;
	fPushPower *= (GetAttribute()->m_fPushPower+m_fGrowthPushRate) * m_fEnhancePushPowerRate;
	fPushPower *= m_fOwnerChargeForceRate;

	if( GetBlowType() == BTT_BLOW ||
		GetBlowType() == BTT_BLOW2 ||
		GetBlowType() == BTT_BLOW3 )
		fPushPower = g_StatAttribute.GetCalcStatAttribute( fPushPower, SA_STR_PUSH_AMT_UP, GetOwner() );

	return fPushPower;
}

float ioWeapon::GetBlowPower( float fDamage ) const
{
	float fBlowPower = m_fBlowPowerCtrlValue * fDamage;
	fBlowPower += m_ConstPowerValue[GetBlowType()].m_fBlowConst;

	float fMainBlow = GetAttribute()->m_fBlowPower;
	if( fMainBlow > 0.0f )
		fMainBlow += m_fGrowthBlowRate;

	fBlowPower *= fMainBlow * m_fEnhanceBlowPowerRate;
	fBlowPower *= m_fOwnerChargeBlowRate;

	if( GetBlowType() == BTT_BLOW ||
		GetBlowType() == BTT_BLOW2 ||
		GetBlowType() == BTT_BLOW3 )
		fBlowPower = g_StatAttribute.GetCalcStatAttribute( fBlowPower, SA_STR_BLOW_AMT_UP, GetOwner() );

	return fBlowPower;
}

float ioWeapon::GetAirPushPower( float fDamage ) const
{
	float fAirPushPower = m_fAirPushPowerCtrlValue * fDamage;
	fAirPushPower += m_ConstPowerValue[GetBlowType()].m_fAirPushConst;
	fAirPushPower *= (GetAttribute()->m_fAirPushPower+m_fGrowthAirPushRate) * m_fEnhanceAirPushPowerRate;
	fAirPushPower *= m_fOwnerChargeForceRate;

	return fAirPushPower;
}

float ioWeapon::GetAirBlowPower( float fDamage ) const
{
	float fAirBlowPower = m_fAirBlowPowerCtrlValue * fDamage;
	fAirBlowPower += m_ConstPowerValue[GetBlowType()].m_fAirBlowConst;

	float fMainBlow = GetAttribute()->m_fAirBlowPower;
	if( fMainBlow > 0.0f )
		fMainBlow += m_fGrowthAirBlowRate;

	fAirBlowPower *= fMainBlow * m_fEnhanceAirBlowPowerRate;
	fAirBlowPower *= m_fOwnerChargeBlowRate;

	return fAirBlowPower;
}

float ioWeapon::GetDefensePushPower( float fDamage ) const
{
	float fDefensePushPower = m_fPushPowerCtrlValue * fDamage;
	fDefensePushPower += m_ConstPowerValue[GetBlowType()].m_fPushConst;
	fDefensePushPower *= GetAttribute()->m_fDefensePushPower * m_fEnhancePushPowerRate;

	return fDefensePushPower;
}

float ioWeapon::GetDownStateBlowPower()
{
	return GetAttribute()->m_fDownStateBlowPowerRate;
}

float ioWeapon::GetDownStatePushPower()
{
	return GetAttribute()->m_fDownStatePushPowerRate;
}

float ioWeapon::GetDamage( ioPlayEntity *pTarget )
{
	float fDamage = 0.0f;

	if( !ToNpcChar( (ioGameEntity*)pTarget ) )
		fDamage = CalcFinalDamage();
	else
		fDamage = CalcFinalNPCDamage();

	if( pTarget->GetSubType() == ioPlayEntity::PST_CHAR )
	{
		if( m_bResistance && m_fWeakRate >= 0.0f )
			fDamage *= (1.0f - m_fWeakRate);
	}

	return fDamage;
}

float ioWeapon::GetBaseDamage( ioPlayEntity *pTarget )
{
	float fDamage = 0.0f;

	if( !m_vRandomDamageList.empty() )
	{
		IORandom random;
		DWORD dwSeed = 0;
		int iRand = 0;
		float fRandomDamage = 0.0f;

		dwSeed = GetOwner()->GetRandomSeed();
		random.SetRandomSeed( dwSeed );

		int iCnt = m_vRandomDamageList.size();
		iRand = random.Random( iCnt );
		if( COMPARE( iRand, 0, iCnt ) )
			fRandomDamage = m_vRandomDamageList[iRand];

		fDamage = fRandomDamage;
	}
	else
	{
		fDamage = GetAttribute()->m_fDamageRate;
	}

	if( pTarget->GetSubType() == ioPlayEntity::PST_CHAR )
	{
		if( m_bResistance && m_fWeakRate >= 0.0f )
			fDamage *= (1.0f - m_fWeakRate);
	}

	return fDamage;
}

bool ioWeapon::InitResource()
{
	return true;
}

void ioWeapon::DestroyResource( ioPlayStage *pStage )
{
}

void ioWeapon::SetStartCollisionBox()
{
}

void ioWeapon::SetMoveDir( const D3DXVECTOR3 &vMoveDir )
{
}

D3DXVECTOR3 ioWeapon::GetMoveDir()
{
	D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
	return vDir;
}

void ioWeapon::SetMoveSpeed( float fSpeed )
{
}

void ioWeapon::SetFloatPower( float fFloatPower )
{
}

void ioWeapon::CallReserveAreaWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( !m_DeadCallAreaWeaponNameList.empty() )
	{
		float fHeight = pStage->GetMapHeight( GetPosition().x, GetPosition().z );
		D3DXVECTOR3 vPos = GetPosition();
		vPos.y = fHeight;

		DWORD dwCurTime = FRAMEGETTIME();
		for( int i = 0; i < (int)m_DeadCallAreaWeaponNameList.size(); ++i )
		{
			const DeadCallAreaInfo& Info = m_DeadCallAreaWeaponNameList[i];
			g_AreaWeaponMgr.ReserveAreaWeapon( dwCurTime + Info.dwReservedTime, GetOwnerName(), "", "", Info.CallAreaName, vPos, ioMath::QUAT_IDENTITY, ioAreaWeapon::CT_NORMAL );
		}
	}
}

void ioWeapon::SetWeaponDead()
{
	CallReserveAreaWeapon();
	StopLoopSound();
	m_bLive = false;
}

void ioWeapon::CheckCurAttribute( const ioPlayEntity *pTarget )
{
	m_fWeakRate = -FLOAT1;
	m_dwWeakElement = 0;
	m_bResistance = false;

	if( !pTarget ) return;
	if( !m_pResistanceAttr ) return;

	if( pTarget->GetSubType() == ioPlayEntity::PST_CHAR )
	{
		ioBaseChar *pChar = (ioBaseChar*)pTarget;

		// Magic
		if( pChar->HasBuff(BT_WEAK_MAGIC) )
		{
			m_fWeakRate = pChar->GetWeakMagicType( GetAttribute()->m_ElementType );

			if( m_fWeakRate >= 0.0f )
			{
				m_bResistance = true;
				m_dwWeakElement = GetAttribute()->m_ElementType;
				return;
			}
		}

		// Bomb
		if( pChar->HasBuff(BT_WEAK_BOMB) )
		{
			if( GetAttribute()->m_ElementType == ET_BOMB )
			{
				m_fWeakRate = pChar->GetWeakBombType( BET_BOMB );

				if( m_fWeakRate >= 0.0f )
				{
					m_bResistance = true;
					m_dwWeakElement = ET_BOMB;
					return;
				}
			}

			if( GetAttribute()->m_ElementType == ET_MINE )
			{
				m_fWeakRate = pChar->GetWeakBombType( BET_MINE );

				if( m_fWeakRate >= 0.0f )
				{
					m_bResistance = true;
					m_dwWeakElement = ET_MINE;
					return;
				}
			}

			/*if( GetAttribute()->m_ElementType == ET_AIRMINE )
			{
				m_fWeakRate = pChar->GetWeakBombType( BET_AIRMINE );

				if( m_fWeakRate >= 0.0f )
				{
					m_bResistance = true;
					m_dwWeakElement = ET_AIRMINE;
					return;
				}
			}*/
		}
	}

	// Team
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		bool bEnableTeamAttack = pOwner->IsEnableTeamAttack();

		if( !bEnableTeamAttack &&
			pTarget != pOwner &&
			pTarget->GetTeam() == pOwner->GetTeam() )
		{
			m_bResistance = true;
			return;
		}
	}
}

bool ioWeapon::CheckItemNotDrop( ioBaseChar *pChar )
{
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioWeapon::CheckItemNotDrop() - Char is NULL" );
		return false;
	}

	if( !HasPeculiarity( WP_SAME_TEAM_NO_ITEM_DROP ) )
		return false;

	bool bEnableTeamAttack = pChar->IsEnableTeamAttack();

	if( !bEnableTeamAttack && GetTeam() == pChar->GetTeam() )
		return true;

	return false;
}

void ioWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreatedTime > dwCurTime )
	{
		LOG.PrintTimeAndLog( 0, "ioWeapon::Process - Create Time Error(%d/%d)", m_dwCreatedTime, dwCurTime );
		SetWeaponDead();
		return;
	}

	if( !CheckPiercing() )
		return;

	DWORD dwLooseTime = 0;
	if( GetOwner() )
	{
		dwLooseTime = GetOwner()->GetActionStopCurAdd();
	}

	if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < dwCurTime )
	{
		SetWeaponDead();
		return;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();
	m_WorldAttackBoxForMap = m_WorldAttackBox;

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		m_WorldAttackBox.Transform( GetPosition(),
									GetScale(),
									GetParentSceneNode()->GetDerivedOrientation() );

		m_WorldAttackBoxForMap.Transform( GetPosition(),
										  GetScale(),
										  GetParentSceneNode()->GetDerivedOrientation() );
	}
}

D3DXVECTOR3 ioWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	if( HasFeature( WF_ATTACK_DIRECTION_STRAIGHT ))
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			D3DXVECTOR3	vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize(&vMoveDir,&vMoveDir);
			return vMoveDir;
		}
	}
	else
	{
		if( pEntity )
		{
			D3DXVECTOR3 vMoveDir = pEntity->GetWorldPosition() - GetPosition();
			vMoveDir.y = 0.0f;
			D3DXVec3Normalize( &vMoveDir, &vMoveDir );
			return vMoveDir;
		}		
	}

	return D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );
}

const ioOrientBox& ioWeapon::GetLocalAttackBox() const
{
	static ioOrientBox sBox;
	return sBox;
}

const ioOrientBox& ioWeapon::GetWorldAttackBox( bool bMap ) const
{
	if( bMap )
	{
		return m_WorldAttackBoxForMap;
	}

	return m_WorldAttackBox;
}

void ioWeapon::GetWorldAttackBoxList( std::vector< ioOrientBox > &vBoxList )
{
	vBoxList.push_back( GetWorldAttackBox() );
}

const WeaponAttribute* ioWeapon::GetAttribute() const
{
	if( !m_bResistance )
		return m_pConstAttr;

	if( m_pResistanceAttr )
		return m_pResistanceAttr;

	LOG.PrintTimeAndLog( 0, "ioWeapon::GetAttribute - Not exist ResistanceAtr" );

	return m_pConstAttr;
}


bool ioWeapon::IsWeaponType(int nType)
{
	if( GetAttribute()->m_WeaponType == nType )
		return true;

	return false;
}

float ioWeapon::CalcFinalDamage()
{
	float fFinalDamage = 0.0f;

	if( !m_vRandomDamageList.empty() )
	{
		IORandom random;
		DWORD dwSeed = 0;
		int iRand = 0;
		float fRandomDamage = 0.0f;
		
		dwSeed = GetOwner()->GetRandomSeed();
		random.SetRandomSeed( dwSeed );

		int iCnt = m_vRandomDamageList.size();
		iRand = random.Random( iCnt );
		if( COMPARE( iRand, 0, iCnt ) )
			fRandomDamage = m_vRandomDamageList[iRand];

		fFinalDamage = fRandomDamage * m_fOwnerChargeDamageRate;
	}
	else
	{
		fFinalDamage = GetAttribute()->m_fDamageRate * m_fOwnerChargeDamageRate;
	}


	float fExtraRate = FLOAT1;

	if( GetOwner() )
	{
		TeamType eTeam = GetTeam();
		if( CheckWeaponByBuff() )
		{
			if( eTeam == TEAM_BLUE )
				eTeam = TEAM_RED;
			else if( eTeam == TEAM_RED )
				eTeam = TEAM_BLUE;
		}

		fExtraRate = GetOwner()->GetMemberBalanceRateForDamage( eTeam );
	}

	return fFinalDamage * m_fOwnerDamageRate * fExtraRate;
}

float ioWeapon::CalcFinalBoundDamage()
{
	float fFinalDamage = GetAttribute()->m_fBoundBlowBoundDamage * m_fOwnerChargeDamageRate;
	float fExtraRate = FLOAT1;

	if( GetOwner() )
	{
		TeamType eTeam = GetTeam();
		if( CheckWeaponByBuff() )
		{
			if( eTeam == TEAM_BLUE )
				eTeam = TEAM_RED;
			else if( eTeam == TEAM_RED )
				eTeam = TEAM_BLUE;
		}

		fExtraRate = GetOwner()->GetMemberBalanceRateForDamage( eTeam );
	}

	return fFinalDamage * m_fOwnerDamageRate * fExtraRate;
}

float ioWeapon::CalcFinalNPCBoundDamage()
{
	float fFinalDamage = GetAttribute()->m_fBoundBlowBoundDamage * m_fOwnerChargeDamageRate;
	float fExtraRate = FLOAT1;

	if( GetOwner() )
	{
		TeamType eTeam = GetTeam();
		if( CheckWeaponByBuff() )
		{
			if( eTeam == TEAM_BLUE )
				eTeam = TEAM_RED;
			else if( eTeam == TEAM_RED )
				eTeam = TEAM_BLUE;
		}

		fExtraRate = GetOwner()->GetMemberBalanceRateForDamage( eTeam );
	}

	return fFinalDamage * m_fOwnerDamageRateNPC * fExtraRate;
}

float ioWeapon::CalcFinalNPCDamage()
{
	float fFinalDamage = 0.0f;

	if( !m_vRandomDamageList.empty() )
	{
		IORandom random;
		DWORD dwSeed = 0;
		int iRand = 0;
		float fRandomDamage = 0.0f;

		dwSeed = GetOwner()->GetRandomSeed();
		random.SetRandomSeed( dwSeed );

		int iCnt = m_vRandomDamageList.size();
		iRand = random.Random( iCnt );
		if( COMPARE( iRand, 0, iCnt ) )
			fRandomDamage = m_vRandomDamageList[iRand];

		fFinalDamage = fRandomDamage * m_fOwnerChargeDamageRate;
	}
	else
	{
		fFinalDamage = GetAttribute()->m_fDamageRate * m_fOwnerChargeDamageRate;
	}


	float fExtraRate = FLOAT1;

	if( GetOwner() )
	{
		TeamType eTeam = GetTeam();
		if( CheckWeaponByBuff() )
		{
			if( eTeam == TEAM_BLUE )
				eTeam = TEAM_RED;
			else if( eTeam == TEAM_RED )
				eTeam = TEAM_BLUE;
		}

		fExtraRate = GetOwner()->GetMemberBalanceRateForDamage( eTeam );
	}

	return fFinalDamage * m_fOwnerDamageRateNPC * fExtraRate;
}

float ioWeapon::GetExtraDamageRate()
{
	float fExtraRate = FLOAT1;

	if( GetOwner() )
	{
		TeamType eTeam = GetTeam();
		if( CheckWeaponByBuff() )
		{
			if( eTeam == TEAM_BLUE )
				eTeam = TEAM_RED;
			else if( eTeam == TEAM_RED )
				eTeam = TEAM_BLUE;
		}

		fExtraRate = GetOwner()->GetMemberBalanceRateForDamage( eTeam );
	}

	return fExtraRate;
}

AttackStatType ioWeapon::GetStatType()
{
	return GetAttribute()->m_StatType;
}

AttackMethodType ioWeapon::GetAttackMethodType()
{
	return GetAttribute()->m_AttackMethodType;
}

float ioWeapon::GetDefenseEnableRate() const
{
	return GetAttribute()->m_fDefenseEnableRate;
}

int ioWeapon::GetBlockingRate() const
{
	return GetAttribute()->m_iBlockingRate;
}

int ioWeapon::GetNpcBlockingRate( ioBaseChar *pTarget )
{
	ioNpcChar *pNpcChar = ToNpcChar( pTarget );
	if( !pNpcChar ) 
		return 100;

	if( (int)pNpcChar->GetNpcType() == NT_DEVELOPER_K )
		return 100;

//	if( (int)pNpcChar->GetNpcType() != GetAttribute()->m_iNpcBlockingType )
//		return 100;

	return GetAttribute()->m_iNpcBlockingRate;
}

int ioWeapon::GetGangsiBlockingRate( ioBaseChar *pTarget )
{
	if( !pTarget ) 
		return 100;
	if( !pTarget->IsGangsi() )
		return 100;

	return GetAttribute()->m_iGangsiBlockingRate;
}

int ioWeapon::GetFCBlockingRate( ioBaseChar *pTarget )
{
	if( !pTarget )
		return 100;

	return GetAttribute()->m_iFCBlockingRate;
}

float ioWeapon::GetOwnerChargeBuffDurationRate()
{
	return m_fOwnerChargeBuffLiveTimeRate;
}

void ioWeapon::SetActionStopDuration( DWORD dwDuration, float fLooseRate )
{
	m_dwActionStopDuration = dwDuration;
	m_fActionStopLooseRate = fLooseRate;
}

DWORD ioWeapon::GetCollisionEnableTime() const
{
	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

const ioHashString &ioWeapon::GetWeaponBySkillName() const
{
	static ioHashString sgNoneName = "";
	if( CheckWeaponBySkill() )
	{
		const ioItem *pItem = g_ItemMaker.GetItemConst( GetCreateItem(), __FUNCTION__ );	
		if( pItem == NULL )
			return sgNoneName;

		return pItem->GetSkillName();
	}
	return sgNoneName;
}

ioBaseChar* ioWeapon::GetOwner() const
{
	ioWeaponManager *pMgr = ioWeaponManager::GetSingletonPtr();
	if( pMgr )
		return pMgr->GetWeaponOwnerChar( m_OwnerName );

	return NULL;
}

bool ioWeapon::IsExperienceOwner()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return false;
	
	return ( pOwner->GetExperienceMode() == EMS_EXPERIENCE );
}

bool ioWeapon::IsCanMineExplosion()
{
	if( !IsLive() )
		return false;

	if( !HasPeculiarity( WP_MINE_EXPLOSION ) )
		return false;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	return true;
}

void ioWeapon::SetResistanceAttr( int iResistanceIdx )
{
	if( iResistanceIdx == 0 ) return;

	m_pResistanceAttr = g_WeaponMgr.FindAttribute( iResistanceIdx );
}

bool ioWeapon::CheckPiercing( bool bCollision )
{
	CharPiercingType eType = GetAttribute()->m_PiercingType;
	DWORD dwLiveTime = GetAttribute()->m_dwPiercingLiveTime;
	DWORD dwCurTime = FRAMEGETTIME();

	switch( eType )
	{
	case CPT_NONE:
	case CPT_NORMAL:
	case CPT_ALL:
		return true;
	}

	if( m_bSetCollision ) // 충돌후 생존시간에 의한 소멸 체크
	{
		if( m_dwPiercingEndTime > 0 && m_dwPiercingEndTime < dwCurTime )
		{
			SetWeaponDead();
			return false;
		}
	}
	else	// 충돌후 생존시간 체크
	{
		if( bCollision && dwLiveTime > 0 )
		{
			m_dwPiercingEndTime = dwCurTime + dwLiveTime;
			m_bSetCollision = true;
		}
	}

	return true;
}

void ioWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
}

void ioWeapon::CheckStructAndDummy( ioPlayEntity *pTarget, ioPlayStage *pStage )
{	
	if( m_DummyAndPushStructDestroyList.empty() )
		return;

	if( !pTarget )
		return;

	if( !pStage )
		return;

	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{

		DummyAndPushStructList::iterator iter = m_DummyAndPushStructDestroyList.begin();
		for( ; iter != m_DummyAndPushStructDestroyList.end(); ++iter )
		{			
			DummyAndPushStruct pInfo = (*iter);
			if( pInfo.m_Type == OBJ_PUSH_STRUCT )
			{
				ioPushStruct *pStruct = ToPushStruct( pTarget );
				if( pStruct && pStruct->GetStructType() != PSTT_FISHING_GROUND )
				{						
					ioBaseChar *pOwenr = GetOwner();
					
					if( pOwenr )
					{
						ioSkill *pSkill = GetOwner()->GetCurActiveSkill();
						if( pSkill )
						{
							pStruct->SetStructDie( pOwenr, true );
							pSkill->SetCollisionByApplySkill();
							continue;
						}
					}									
				}
			}
			else if( pInfo.m_Type == OBJ_DUMMY )
			{
				ioDummyChar *pDummyChar = ToDummyChar(pTarget);
				if( pDummyChar )
				{	
					if( pDummyChar->GetDummyCharName() == pInfo.m_DummyCharName )
					{						
						pDummyChar->SendDieState(true);						
						ioSkill *pSkill = GetOwner()->GetCurActiveSkill();
						if( pSkill )
						{	
							pSkill->SetCollisionByApplySkill();
							continue;
						}						
					}						
				}	
			}
		}
	}
}

float ioWeapon::GetPushAngle() const
{
	return GetAttribute()->m_fPushAngle;
}

int ioWeapon::IsResistanceMagic( ioBaseChar *pChar )
{
	if( m_bResistance )
	{
		switch( m_dwWeakElement )
		{
		case ET_FIRE:
			return (int)ioModeHelpManager::HT_FIRE_RES;
		case ET_ICE:
			return (int)ioModeHelpManager::HT_ICE_RES;
		}
	}

	return -1;
}

int ioWeapon::IsResistanceBomb( ioBaseChar *pChar )
{
	if( m_bResistance )
	{
		switch( m_dwWeakElement )
		{
		//case ET_AIRMINE:
		case ET_BOMB:
		case ET_MINE:
			return BT_WEAK_BOMB;
		}
	}

	return -1;
}

ElementType ioWeapon::GetElementType()
{
	return GetAttribute()->m_ElementType;
}

void ioWeapon::SetGrapplingTargetName( const ioHashString &rkName )
{
	m_GrapplingTargetName = rkName;
}

void ioWeapon::SetCreateItem( const ioHashString &rkName )
{
	m_CreateItem = rkName;
}

void ioWeapon::SetGrowthAttackRate( float fAttackRate, float fPushRate, float fAirPushRate, float fBlowRate, float fAirBlowRate )
{
	m_fGrowthAttackRate = FLOAT1 + (fAttackRate/FLOAT100);

	m_fGrowthPushRate = fPushRate;
	m_fGrowthAirPushRate = fAirPushRate;
	m_fGrowthBlowRate = fBlowRate;
	m_fGrowthAirBlowRate = fAirBlowRate;
}

void ioWeapon::SetGrowthMoveRange( float fRange )
{
	m_fGrowthMoveRange = fRange;
}

void ioWeapon::SetGrowthExplosionRange( float fRange )
{
	m_fGrowthExplosionRange = fRange;
}

float ioWeapon::GetBallPushPower( float fDamage ) const
{
	float fPushPower = GetAttribute()->m_fBallPushPower;
	fPushPower *= m_fOwnerChargeForceRate;

	return fPushPower;
}

float ioWeapon::GetBallBlowPower( float fDamage ) const
{
	float fMainBlow = GetAttribute()->m_fBallBlowPower;
	fMainBlow *= m_fOwnerChargeBlowRate;

	return fMainBlow;
}

float ioWeapon::GetBallAirPushPower( float fDamage ) const
{
	float fAirPushPower = GetAttribute()->m_fBallAirPushPower;
	fAirPushPower *= m_fOwnerChargeForceRate;

	return fAirPushPower;
}

float ioWeapon::GetBallAirBlowPower( float fDamage ) const
{
	float fMainBlow = GetAttribute()->m_fBallAirBlowPower;
	fMainBlow *= m_fOwnerChargeBlowRate;

	return fMainBlow;
}

float ioWeapon::GetGangsiPushPower( float fDamage ) const
{
	float fPushPower = GetAttribute()->m_fGangsiPushPower;
	fPushPower *= m_fOwnerChargeForceRate;

	return fPushPower;
}

float ioWeapon::GetGangsiBlowPower( float fDamage ) const
{
	float fMainBlow = GetAttribute()->m_fGangsiBlowPower;
	fMainBlow *= m_fOwnerChargeBlowRate;

	return fMainBlow;
}

float ioWeapon::GetGangsiAirPushPower( float fDamage ) const
{
	float fAirPushPower = GetAttribute()->m_fGangsiAirPushPower;
	fAirPushPower *= m_fOwnerChargeForceRate;

	return fAirPushPower;
}

float ioWeapon::GetGangsiAirBlowPower( float fDamage ) const
{
	float fMainBlow = GetAttribute()->m_fGangsiAirBlowPower;
	fMainBlow *= m_fOwnerChargeBlowRate;

	return fMainBlow;
}

float ioWeapon::GetBoundBlowPower( float fDamage ) const
{
	float fBoundBlowPower = m_fAirBlowPowerCtrlValue * fDamage;
	fBoundBlowPower += m_ConstPowerValue[GetBlowType()].m_fAirBlowConst;

	float fMainBlow = GetAttribute()->m_fBoundBlowJumpAmt;
	if( fMainBlow > 0.0f )
		fMainBlow += m_fGrowthAirBlowRate;

	fBoundBlowPower *= fMainBlow * m_fEnhanceAirBlowPowerRate;
	fBoundBlowPower *= m_fOwnerChargeBlowRate;

	return fBoundBlowPower;
}

float ioWeapon::GetBoundPushPower( float fDamage ) const
{
	float fBoundPushPower = m_fAirPushPowerCtrlValue * fDamage;
	fBoundPushPower += m_ConstPowerValue[GetBlowType()].m_fAirPushConst;
	fBoundPushPower *= (GetAttribute()->m_fBoundBlowForceAmt+m_fGrowthAirPushRate) * m_fEnhanceAirPushPowerRate;
	fBoundPushPower *= m_fOwnerChargeForceRate;

	return fBoundPushPower;
}

float ioWeapon::GetBoundBlowGravityRate() const
{
	float fGravityRate = GetAttribute()->m_fBoundBlowGravityRate;

	return fGravityRate;
}

float ioWeapon::GetBoundDamage( ioPlayEntity *pTarget )
{
	float fDamage = 0.0f;

	if( !ToNpcChar( (ioGameEntity*)pTarget ) )
		fDamage = CalcFinalBoundDamage();
	else
		fDamage = CalcFinalNPCBoundDamage();

	if( pTarget->GetSubType() == ioPlayEntity::PST_CHAR )
	{
		if( m_bResistance && m_fWeakRate >= 0.0f )
			fDamage *= (1.0f - m_fWeakRate);
	}

	return fDamage;
}

float ioWeapon::GetDefenseDamageRate() const
{
	return GetAttribute()->m_fDefenseDamageRate;
}

float ioWeapon::GetDefenseDamageRate2() const
{
	return GetAttribute()->m_fDefenseDamageRate2;
}

const ioHashString& ioWeapon::GetExtendWoundAni() const
{
	return GetAttribute()->m_ExtendWoundAni;
}

DWORD ioWeapon::GetExtendWoundDuration() const
{
	return GetAttribute()->m_dwExtendWoundDuration;
}

float ioWeapon::GetExtendWoundBlowPower( float fDamage ) const
{
	float fBoundBlowPower = m_fBlowPowerCtrlValue * fDamage;
	fBoundBlowPower += m_ConstPowerValue[GetBlowType()].m_fBlowConst;

	float fMainBlow = GetAttribute()->m_fExtendWoundJumpAmt;

	if( fMainBlow > 0.0f )
		fMainBlow += m_fGrowthBlowRate;

	fBoundBlowPower *= fMainBlow * m_fEnhanceBlowPowerRate;
	fBoundBlowPower *= m_fOwnerChargeBlowRate;

	return fBoundBlowPower;
}

float ioWeapon::GetExtendWoundPushPower( float fDamage ) const
{
	float fBoundPushPower = m_fPushPowerCtrlValue * fDamage;
	fBoundPushPower += m_ConstPowerValue[GetBlowType()].m_fPushConst;
	fBoundPushPower *= (GetAttribute()->m_fExtendWoundForceAmt+m_fGrowthPushRate) * m_fEnhancePushPowerRate;
	fBoundPushPower *= m_fOwnerChargeForceRate;

	return fBoundPushPower;
}

const BlowWoundAniInfo& ioWeapon::GetBlowWoundAniInfo() const
{
	return GetAttribute()->m_BlowWoundAniInfo;
}

bool ioWeapon::HasDefenseCancelAni()
{
	if( !GetAttribute()->m_DefenseCancelAni.IsEmpty() )
		return true;

	return false;
}

const ioHashString& ioWeapon::GetDefenseCancelAni() const
{
	return GetAttribute()->m_DefenseCancelAni;
}

float ioWeapon::GetDefenseCancelAniRate() const
{
	return GetAttribute()->m_fDefenseCancelAniRate;
}

DWORD ioWeapon::GetRefreshDuration()
{
	return GetAttribute()->m_dwRefreshDuration;
}
 
DWORD ioWeapon::GetIgnoreDuration()
{
	return GetAttribute()->m_dwIgnoreDuration;
}

DWORD ioWeapon::GetIgnoreTimeTypeDuration()
{
	return GetAttribute()->m_dwTypeProtectedTime;
}


DWORD ioWeapon::GetIgnoreTimeType()
{
	return GetAttribute()->m_dwProtectedType;
}


void ioWeapon::CheckOnlyDownAttack( ioBaseChar *pOwner )
{
	m_bOnlyDownAttack = false;

	if( !pOwner ) return;

	if( !m_bCheckOnlyDownAttack ) return;

	if( !pOwner->IsJumpState() )
		m_bOnlyDownAttack = true;
}

DWORD ioWeapon::GetWoundedDuration()
{
	return (DWORD)(m_dwWoundedDuration * m_fOwnerChargeWoundedTimeRate);
}

bool ioWeapon::IsChangeJumpping( bool bDefense, bool bBlowWounded )
{
	if( m_ChangeJumppingType == CJT_NONE )
		return false;

	if( bDefense && m_ChangeJumppingType == CJT_WOUNDED_ONLY )
		return false;

	if( !bBlowWounded && m_ChangeJumppingType == CJT_BLOW_WOUNDED_ONLY )
		return false;

	return true;
}


bool ioWeapon::IsChangeNextItemType( bool bDefense )
{
	if( m_ChangeNextItemType == CNIT_NONE )
		return false;

	return true;
}


bool ioWeapon::IsChangeNextSkillType( bool bDefense )
{
	if( m_ChangeNextSkillType == CNST_NONE )
		return false;

	return true;
}

void ioWeapon::CreateAttachAreaWeapon()
{
	if( m_AreaWeaponName.IsEmpty() )
		return;

	if( m_dwAttachAreaWeaponIndex > 0 )
		return;

	if( m_dwAttachAreaWeaponIndex == -1 )
		return;

	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( GetOwnerName(),
															"",
															"",
															m_AreaWeaponName,
															GetPosition(),
															GetOrientation(),
															ioAreaWeapon::CT_NORMAL );

	if( pArea )
	{
		m_dwAttachAreaWeaponIndex = pArea->GetAreaWeaponIndex();
		pArea->SetCreateWeaponIndex( m_dwWeaponIdx );
	}
}

void ioWeapon::UpdateAttachAreaWeapon( ioPlayStage *pStage )
{
	ioAreaWeapon *pArea = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAttachAreaWeaponIndex );
	if( !pArea ) return;

	pArea->SetAreaPosition( GetPosition(), pStage );
	pArea->SetAreaRotation( GetOrientation() );

	D3DXVECTOR3 vPos = pArea->GetStartPos();
}

void ioWeapon::DestroyAttachAreaWeapon()
{
	if( m_dwAttachAreaWeaponIndex > 0 )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAttachAreaWeaponIndex );
		m_dwAttachAreaWeaponIndex = -1;
	}
}

void ioWeapon::AttachAreaWeaponEndState()
{
	m_dwAttachAreaWeaponIndex = -1;
}

void ioWeapon::AddEffectEvent( ioEffect *pEffect )
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		pOwner->CustomTextureEffectEvent( pEffect );
	}
}

DWORD ioWeapon::GetLiveTime()
{
	return m_dwLiveTime * m_fOwnerChargeLiveTimeRate;
}

void ioWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
}

GrapplingType ioWeapon::CheckGrapplingState()
{
	return m_CheckGrapplingType;
}

void ioWeapon::SetSkillDependencyAction()
{
}

bool ioWeapon::IsSkillDependency( const ioHashString &szSkillName )
{
	if( !m_bSkillDependency )
		return false;

	ioHashString szCurSkill = GetWeaponBySkillName();
	if( !szCurSkill.IsEmpty() && szCurSkill == szSkillName )
		return true;

	return false;
}

float ioWeapon::CheckDamageIncreaseBuff( ioBaseChar *pChar )
{
	if( !pChar ||  GetAttribute()->m_DamageIncreaseBuff.IsEmpty() )
		return FLOAT1;

	int iCnt = pChar->GetHasBuffCnt( GetAttribute()->m_DamageIncreaseBuff );
	if( iCnt > 0 )
	{
		return (float)iCnt;
	}

	return FLOAT1;
}

float ioWeapon::CheckPushIncreaseBuff( ioBaseChar *pChar )
{
	if( !pChar || GetAttribute()->m_PushIncreaseBuff.IsEmpty() )
		return FLOAT1;

	int iCnt = pChar->GetHasBuffCnt( GetAttribute()->m_PushIncreaseBuff );
	if( iCnt > 0 )
	{
		return (float)iCnt;
	}

	return FLOAT1;
}

float ioWeapon::CheckBlowIncreaseBuff( ioBaseChar *pChar )
{
	if( !pChar || GetAttribute()->m_BlowIncreaseBuff.IsEmpty() )
		return FLOAT1;

	int iCnt = pChar->GetHasBuffCnt( GetAttribute()->m_BlowIncreaseBuff );
	if( iCnt > 0 )
	{
		return (float)iCnt;
	}

	return FLOAT1;
}

EscapeCatchType ioWeapon::GetEnableEscapeCatchType() const
{
	return GetAttribute()->m_EscapeCatchType;
}

float ioWeapon::GetWereWolfChangeDamage()
{
	return GetAttribute()->m_fWereWolfChangeDamage;
}

void ioWeapon::SetMapCollision()
{
	// 맵충돌시 처리가 필요하다면 이곳에서..
}

void ioWeapon::CreateCollisionAreaWeapon()
{
	if( m_OwnerName.IsEmpty() )
		return;

	ioHashString szAreaWeaponName = GetAttribute()->m_AreaWeaponName;
	if( szAreaWeaponName.IsEmpty() )
		return;

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( m_OwnerName,
																  "",
																  "",
																  szAreaWeaponName,
																  GetPosition(),
																  GetOrientation(),
																  ioAreaWeapon::CT_NORMAL );
}

int ioWeapon::SetSylphid()
{
	if( GetAttribute()->m_bEnableSylphid )
		m_bSetSylphid = true;
	else
		m_bSetSylphid = false;

	return GetAttribute()->m_iSylphidNeed;
}

int ioWeapon::GetExtraGaugeType()
{
	if( !GetAttribute() )
		return WGT_NONE;

	return GetAttribute()->m_WeaponExtraGaugeType;
}

bool ioWeapon::IsEnablePossession()
{
	if( !GetAttribute() )
		return false;

	return GetAttribute()->m_bEnablePossession;
}

DWORD ioWeapon::GetIgnoreWeaponTime()
{
	if( !GetAttribute() )
		return 0;

	return GetAttribute()->m_dwIgnoreWeaponTime;
}


void ioWeapon::ApplyExtraReserveWeaponByWeaponInfo( ioWeapon* pWeapon )
{
	//SetReserveCallWeaponInfo() 함수에 의해 웨폰에서 웨폰을 발사하는 형태에서
	//지정된 시간에 웨폰을 생성할때 이 함수를 통해 스킬에서의 ApplyExtraSkillInfo() 함수와 같은 역할을 함
}

void ioWeapon::CheckWeaponCollision()
{
}

bool ioWeapon::IsUseGrapplingTarget()
{
	if( !GetAttribute() )
		return false;

	return GetAttribute()->m_bUseGrapplingTarget;
}

bool ioWeapon::IsIgnoreMapCollision() const
{
	return m_bIgnoreMapCollision;
}

bool ioWeapon::IsIgnoreTestMapCollision() const
{
	return false;
}

bool ioWeapon::IsIgnoreMapCollisionEffect() const
{
	return m_bIgnoreMapCollisionEffect;
}

bool ioWeapon::IsTeamAttack()
{
	return m_bTeamAttack;
}


ChangeNextItemType ioWeapon::GetChangeNextItemType()
{
	return m_ChangeNextItemType;
}


void ioWeapon::SetReserveCallWeapon()
{
	m_bReserveCallWeapon = true;
}
