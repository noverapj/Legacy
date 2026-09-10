

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioFlyJumpBuff.h"

ioFlyJumpBuff::ioFlyJumpBuff()
{
}

ioFlyJumpBuff::ioFlyJumpBuff( const ioFlyJumpBuff &rhs )
: ioBuff( rhs ),
 m_fAniRate( rhs.m_fAniRate ),
 m_fJumpPower( rhs.m_fJumpPower ),
 m_fJumpForce( rhs.m_fJumpForce ),
 m_fJumpSpeedRate( rhs.m_fJumpSpeedRate ),
 m_fEnableHeight( rhs.m_fEnableHeight ),
 m_dwJumpEnableTime( rhs.m_dwJumpEnableTime ),
 m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
 m_bJumpAttack( rhs.m_bJumpAttack ),
 m_bEnableRotate( rhs.m_bEnableRotate ),
 m_fGravityRate( rhs.m_fGravityRate ),
 m_bContinueJump( rhs.m_bContinueJump ),
 m_bEnableMoveRotate( rhs.m_bEnableMoveRotate ),
 m_dwMoveRotateSpeed( rhs.m_dwMoveRotateSpeed ),
 m_bInvisibleMeshOnFlyJump( rhs.m_bInvisibleMeshOnFlyJump ),
 m_bWeaponColSkipOnFly( rhs.m_bWeaponColSkipOnFly ),
 m_bHideOnFly( rhs.m_bHideOnFly ),
 m_fHideAlphaRate( rhs.m_fHideAlphaRate ),
 m_FlyStartEffect( rhs.m_FlyStartEffect ),
 m_FlyEndEffect( rhs.m_FlyEndEffect ),
 m_iHideRate( rhs.m_iHideRate ),
 m_iWeaponColSkipRate( rhs.m_iWeaponColSkipRate ),
 m_fJumpEnableGauge( rhs.m_fJumpEnableGauge ),
 m_fBuffAirWalkStartGauge( rhs.m_fBuffAirWalkStartGauge )
{
	m_vJumpAnimationList.clear();
	int iCnt = rhs.m_vJumpAnimationList.size();
	for( int i=0; i < iCnt; i++ )
		m_vJumpAnimationList.push_back( rhs.m_vJumpAnimationList[i] );
}

ioFlyJumpBuff::~ioFlyJumpBuff()
{
}

ioBuff* ioFlyJumpBuff::Clone()
{
	return new ioFlyJumpBuff( *this );
}

void ioFlyJumpBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	
	m_bEnableRotate = rkLoader.LoadBool_e( "enable_rotate", false );
	m_bContinueJump = rkLoader.LoadBool_e( "continue_fly", false );

	m_fAniRate = rkLoader.LoadFloat_e( "jump_ani_rate", FLOAT1 );
	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );
	m_fJumpForce = rkLoader.LoadFloat_e( "jump_force", 0.0f );
	m_fJumpSpeedRate = rkLoader.LoadFloat_e( "jump_speed_rate", FLOAT1 );
	m_fGravityRate = rkLoader.LoadFloat_e( "jump_gravity_rate", FLOAT1 );

	m_fEnableHeight = rkLoader.LoadFloat_e( "jump_enable_height", 0.0f );
	m_dwJumpEnableTime = rkLoader.LoadInt_e( "jump_enable_time", 0 );

	m_iMaxJumpCnt = rkLoader.LoadInt_e( "jump_max_cnt", 2 );
	m_bJumpAttack = rkLoader.LoadBool_e( "jump_attack_enable", false );

	m_vJumpAnimationList.clear();
	m_vJumpAnimationList.reserve( 4 );

	rkLoader.LoadString_e( "jump_animation_front", "", szBuf, MAX_PATH );
	m_vJumpAnimationList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "jump_animation_right", "", szBuf, MAX_PATH );
	m_vJumpAnimationList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "jump_animation_back", "", szBuf, MAX_PATH );
	m_vJumpAnimationList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "jump_animation_left", "", szBuf, MAX_PATH );
	m_vJumpAnimationList.push_back( ioHashString(szBuf) );

	m_bEnableMoveRotate = rkLoader.LoadBool_e( "enable_move_rotate", false );
	m_dwMoveRotateSpeed = (DWORD)rkLoader.LoadInt_e( "move_rotate_speed", 0 );

	m_bInvisibleMeshOnFlyJump = rkLoader.LoadBool_e( "invisible_mesh_on_fly_jump", false );

	m_bWeaponColSkipOnFly = rkLoader.LoadBool_e( "weapon_col_skip_on_fly", false );
	m_bHideOnFly = rkLoader.LoadBool_e( "hide_on_fly", false );
	m_fHideAlphaRate = rkLoader.LoadFloat_e( "hide_alpha_rate", FLOAT1 );
	rkLoader.LoadString_e( "fly_start_hide_effect", "", szBuf, MAX_PATH );
	m_FlyStartEffect = szBuf;

	rkLoader.LoadString_e( "fly_end_hide_effect", "", szBuf, MAX_PATH );
	m_FlyEndEffect = szBuf;

	m_fJumpEnableGauge = rkLoader.LoadFloat_e( "jump_enable_gauge", 0.0f );
	m_fBuffAirWalkStartGauge = rkLoader.LoadFloat_e( "buff_walk_start_gauge", 0.0f );

	//전자 날틀 옵션
	m_iHideRate = rkLoader.LoadInt_e( "hide_on_fly_rate", 0 );
	m_iHideRate = min( m_iHideRate, 100 );
	m_iWeaponColSkipRate = rkLoader.LoadInt_e( "weapon_col_skip_on_fly_rate", 0 );
	m_iWeaponColSkipRate = min( m_iWeaponColSkipRate, 100 );
}

void ioFlyJumpBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	CharBuffJumpInfo kInfo;
	kInfo.m_BuffJumpType = BJT_FLY;
	kInfo.m_Name = GetName();
	kInfo.m_bEnableRotate = m_bEnableRotate;
	kInfo.m_bContinue = m_bContinueJump;
	kInfo.m_fBuffJumpPower = m_fJumpPower;
	kInfo.m_fBuffJumpForce = m_fJumpForce;
	kInfo.m_fBuffJumpSpeedRate = m_fJumpSpeedRate;
	kInfo.m_fBuffJumpGravityRate = m_fGravityRate;

	kInfo.m_fBuffJumpHeight = m_fEnableHeight;
	kInfo.m_iBuffJumpMaxCnt = max( 2, m_iMaxJumpCnt);

	kInfo.m_dwBuffJumpEnableTime = m_dwJumpEnableTime;

	kInfo.m_fBuffJumpAniRate = m_fAniRate;
	kInfo.m_vBuffJumpAnimationList = m_vJumpAnimationList;

	kInfo.m_bEnableMoveRotate = m_bEnableMoveRotate;
	kInfo.m_dwMoveRotateSpeed = m_dwMoveRotateSpeed;
	kInfo.m_fBuffJumpEnableGauge = m_fJumpEnableGauge;
	kInfo.m_fBuffAirWalkStartGauge = m_fBuffAirWalkStartGauge;

	pOwner->SetBuffFlyJumpValue( kInfo );

	m_bReduceGauge = false;
	m_bHideMesh = true;
	m_bSetVisible = false;
	m_bHide = false;
	m_bWeaponColSkip = false;
}

bool ioFlyJumpBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
	{
		return false;
	}

	CharBuffJumpInfo kInfo;
	kInfo.m_BuffJumpType = BJT_FLY;
	kInfo.m_Name = GetName();
	kInfo.m_bEnableRotate = m_bEnableRotate;
	kInfo.m_bContinue = m_bContinueJump;
	kInfo.m_fBuffJumpPower = m_fJumpPower;
	kInfo.m_fBuffJumpForce = m_fJumpForce;
	kInfo.m_fBuffJumpSpeedRate = m_fJumpSpeedRate;
	kInfo.m_fBuffJumpGravityRate = m_fGravityRate;

	kInfo.m_fBuffJumpHeight = m_fEnableHeight;
	kInfo.m_iBuffJumpMaxCnt = max( 2, m_iMaxJumpCnt);

	kInfo.m_dwBuffJumpEnableTime = m_dwJumpEnableTime;

	kInfo.m_fBuffJumpAniRate = m_fAniRate;
	kInfo.m_vBuffJumpAnimationList = m_vJumpAnimationList;

	kInfo.m_bEnableMoveRotate = m_bEnableMoveRotate;
	kInfo.m_dwMoveRotateSpeed = m_dwMoveRotateSpeed;
	kInfo.m_fBuffJumpEnableGauge = m_fJumpEnableGauge;

	pOwner->SetBuffFlyJumpValue( kInfo );

	m_bReduceGauge = false;
	m_bHideMesh = true;
	m_bSetVisible = false;
	m_bHide = false;
	m_bWeaponColSkip = false;

	return true;
}

void ioFlyJumpBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		ProcessPassive();
		CheckCallingAreaWeapon();
		break;
	}

	if( m_bHideOnFly && m_bHide )
	{
		if( m_bReduceGauge && !m_bSetVisible )
		{
			int iAlphaRate = MAX_ALPHA_RATE * m_fHideAlphaRate;
			m_pOwner->SetHideChar( true );
			m_pOwner->SetAlphaRateDirect( iAlphaRate );
			m_bSetVisible = true;

			ioBaseChar *pOwner = GetOwner();
			if( pOwner )
			{
				ioPlayStage *pStage = pOwner->GetCreator();
				if( pStage )
				{
					pStage->CreateMapEffect( m_FlyStartEffect, pOwner->GetMidPositionByRate(), pOwner->GetWorldScale() );
				}
			}
		}
		else if( !m_bReduceGauge && m_bSetVisible )
		{
			m_pOwner->SetHideChar( false );
			m_pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
			m_bSetVisible = false;

			ioBaseChar *pOwner = GetOwner();
			if( pOwner )
			{
				ioPlayStage *pStage = pOwner->GetCreator();
				if( pStage )
				{
					pStage->CreateMapEffect( m_FlyEndEffect, pOwner->GetMidPositionByRate(), pOwner->GetWorldScale() );
				}
			}
		}
	}
}

bool ioFlyJumpBuff::IsCanJumpAttack() const
{
	return m_bJumpAttack;
}

void ioFlyJumpBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		pOwner->RemoveBuffFlyJumpValue( GetName() );
		if( pOwner->GetState() == CS_JUMP && pOwner->GetJumpState() != JS_LANDING )
			pOwner->SetBuffFlyJumpEnd();

		if( m_bSetVisible )
		{
			pOwner->SetHideChar( false );
			pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
		}
	}

	ioBuff::EndBuff();
}

bool ioFlyJumpBuff::OnReduceGauge() const
{
	return m_bReduceGauge;
}

bool ioFlyJumpBuff::IsCharCollisionSkip() const
{
	if( m_bCharCollisionSkip )
		return true;

	return false;
}

bool ioFlyJumpBuff::IsCollisionAvailableWeapon() const
{
	if( m_bWeaponColSkipOnFly && m_bWeaponColSkip && m_bReduceGauge )
		return false;

	return true;
}

void ioFlyJumpBuff::ProcessPassive()
{
	if( !m_pOwner )
		return;

	if( m_fGaugePerTick <= 0.0f || m_dwTickTime <= 0 )
		return;
	
	if( GetName() != m_pOwner->GetBuffFlyJumpName() )
		return;

	float fCurTicGauge = m_fGaugePerTick / m_fTicGaugeRateByGrowth;
	DWORD dwCurTime = FRAMEGETTIME();
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTickTime + m_dwTicTimeByGrowth;

	// 활강중에만 스킬 게이지가 감소한다.
	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );
		if( fCurTicGauge != 0.0f )
		{
			if( (m_pOwner->GetState() == CS_JUMP && m_pOwner->GetJumpState() == JS_JUMPPING) &&
				m_pOwner->GetUsedBuffFlyJump() != BJUT_NONE &&
				m_pOwner->GetUsedBuffFlyJump() != BJUT_END &&
				m_pOwner->IsFloating() )
			{
				
				for( int i=ES_WEAPON; i < MAX_EQUIP_SLOT; ++i )
				{
					ioSkill *pSkill = m_pOwner->GetEquipedSkill(i);
					ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);

					if( !pPassiveSkill )
						continue;

					if( GetSkillName().IsEmpty() ||  pPassiveSkill->GetName() != GetSkillName() )
						continue;

					if( pPassiveSkill->UsePassiveGauge() )
					{
						if( pPassiveSkill->ReduceCurPassiveGauge( fCurTicGauge ) <= 0.0f )
						{
							m_pOwner->SetBuffFlyJumpEnd();
						}
					}
				}

				if( !m_bReduceGauge )
				{
					IORandom random;
					DWORD dwSeed = m_pOwner->GetRandomSeed();
					random.SetRandomSeed( dwSeed );

					int iHideRate = random.Random( 100 );
					int iWeaponColSkipRate = random.Random( 100 );

					if( iHideRate <= m_iHideRate )
						m_bHide = true;
					else
						m_bHide = false;

					if( iWeaponColSkipRate <= m_iWeaponColSkipRate && m_bHide )
						m_bWeaponColSkip = true;
					else
						m_bWeaponColSkip = false;
				}
				m_bReduceGauge = true;
			}
			else
			{
				m_bReduceGauge = false;
			}

			if( m_bInvisibleMeshOnFlyJump && ( m_bHideMesh != m_bReduceGauge ) )
			{
				ioItem *pOwnerItem = m_pOwner->GetEquipedItem( m_ItemName );
				if ( pOwnerItem )
				{
					int iItemType = pOwnerItem->GetType();
					EquipSlot eSlot = ES_UNKNOWN;
					switch( iItemType )
					{
					case ioItem::IT_WEAPON:
						eSlot = ES_WEAPON;
						break;
					case ioItem::IT_ARMOR:
						eSlot = ES_ARMOR;
						break;
					case ioItem::IT_HELMET:
						eSlot = ES_HELMET;
						break;
					case ioItem::IT_CLOAK:
						eSlot = ES_CLOAK;
						break;
					}

					if( eSlot <= ES_CLOAK )
						m_pOwner->ShowEquipItemMesh( !m_bReduceGauge, eSlot );

					m_bHideMesh = m_bReduceGauge;
				}
			}				
		}
	}
}

bool ioFlyJumpBuff::IsHideState() const
{
	if ( m_bHideOnFly && m_bHide && m_bSetVisible )
	{
		return true;
	}

	return false;
}