

#include "stdafx.h"

#include "ioBaseChar.h"




#include "ioMultiFlyJumpBuff.h"

ioMultiFlyJumpBuff::ioMultiFlyJumpBuff()
{
}

ioMultiFlyJumpBuff::ioMultiFlyJumpBuff( const ioMultiFlyJumpBuff &rhs )
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
 m_fJumpExtraPower( rhs.m_fJumpExtraPower ),
 m_fJumpExtraForce( rhs.m_fJumpExtraForce ),
 m_fExtraGravityRate( rhs.m_fExtraGravityRate ),
 m_bContinueJump( rhs.m_bContinueJump ),
 m_bEnableMoveRotate( rhs.m_bEnableMoveRotate ),
 m_dwMoveRotateSpeed( rhs.m_dwMoveRotateSpeed ),
 m_bInvisibleMeshOnFlyJump( rhs.m_bInvisibleMeshOnFlyJump )
{
	m_vJumpAnimationList.clear();
	int iCnt = rhs.m_vJumpAnimationList.size();
	for( int i=0; i < iCnt; i++ )
		m_vJumpAnimationList.push_back( rhs.m_vJumpAnimationList[i] );

	m_vJumpExtraAnimationList.clear();
	iCnt = rhs.m_vJumpExtraAnimationList.size();
	for( int i=0; i < iCnt; i++ )
		m_vJumpExtraAnimationList.push_back( rhs.m_vJumpExtraAnimationList[i] );
}

ioMultiFlyJumpBuff::~ioMultiFlyJumpBuff()
{
}

ioBuff* ioMultiFlyJumpBuff::Clone()
{
	return new ioMultiFlyJumpBuff( *this );
}

void ioMultiFlyJumpBuff::LoadProperty( ioINILoader &rkLoader )
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

	m_fJumpExtraPower = rkLoader.LoadFloat_e( "jump_extra_power", 0.0f );
	m_fJumpExtraForce = rkLoader.LoadFloat_e( "jump_extra_force", 0.0f );
	m_fExtraGravityRate = rkLoader.LoadFloat_e( "jump_extra_gravity_rate", FLOAT1 );

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

	m_vJumpExtraAnimationList.clear();
	m_vJumpExtraAnimationList.reserve( 4 );

	rkLoader.LoadString_e( "jump_extra_animation_front", "", szBuf, MAX_PATH );
	m_vJumpExtraAnimationList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "jump_extra_animation_right", "", szBuf, MAX_PATH );
	m_vJumpExtraAnimationList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "jump_extra_animation_back", "", szBuf, MAX_PATH );
	m_vJumpExtraAnimationList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "jump_extra_animation_left", "", szBuf, MAX_PATH );
	m_vJumpExtraAnimationList.push_back( ioHashString(szBuf) );

	m_bEnableMoveRotate = rkLoader.LoadBool_e( "enable_move_rotate", false );
	m_dwMoveRotateSpeed = (DWORD)rkLoader.LoadInt_e( "move_rotate_speed", 0 );

	m_bInvisibleMeshOnFlyJump = rkLoader.LoadBool_e( "invisible_mesh_on_fly_jump", false );
}

void ioMultiFlyJumpBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	CharBuffJumpInfo kInfo;
	kInfo.m_BuffJumpType = BJT_MULT_FLY;
	kInfo.m_Name = GetName();
	kInfo.m_bEnableRotate = m_bEnableRotate;
	kInfo.m_bContinue = m_bContinueJump;
	kInfo.m_fBuffJumpPower = m_fJumpPower;
	kInfo.m_fBuffJumpForce = m_fJumpForce;
	kInfo.m_fBuffJumpSpeedRate = m_fJumpSpeedRate;
	kInfo.m_fBuffJumpGravityRate = m_fGravityRate;

	kInfo.m_fBuffJumpExtraPower = m_fJumpExtraPower;
	kInfo.m_fBuffJumpExtraForce = m_fJumpExtraForce;
	kInfo.m_fBuffJumpExtraGravityRate = m_fExtraGravityRate;

	kInfo.m_fBuffJumpHeight = m_fEnableHeight;
	kInfo.m_iBuffJumpMaxCnt = max( 2, m_iMaxJumpCnt);

	kInfo.m_dwBuffJumpEnableTime = m_dwJumpEnableTime;

	kInfo.m_fBuffJumpAniRate = m_fAniRate;
	kInfo.m_vBuffJumpAnimationList = m_vJumpAnimationList;
	kInfo.m_vBuffJumpExtraAnimationList = m_vJumpExtraAnimationList;

	kInfo.m_bEnableMoveRotate = m_bEnableMoveRotate;
	kInfo.m_dwMoveRotateSpeed = m_dwMoveRotateSpeed;

	pOwner->SetBuffMultiFlyJumpValue( kInfo );

	m_bReduceGauge = false;
	m_bHideMesh = true;
}

bool ioMultiFlyJumpBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
	{
		return false;
	}

	CharBuffJumpInfo kInfo;
	kInfo.m_BuffJumpType = BJT_MULT_FLY;
	kInfo.m_Name = GetName();
	kInfo.m_bEnableRotate = m_bEnableRotate;
	kInfo.m_bContinue = m_bContinueJump;
	kInfo.m_fBuffJumpPower = m_fJumpPower;
	kInfo.m_fBuffJumpForce = m_fJumpForce;
	kInfo.m_fBuffJumpSpeedRate = m_fJumpSpeedRate;
	kInfo.m_fBuffJumpGravityRate = m_fGravityRate;

	kInfo.m_fBuffJumpExtraPower = m_fJumpExtraPower;
	kInfo.m_fBuffJumpExtraForce = m_fJumpExtraForce;
	kInfo.m_fBuffJumpExtraGravityRate = m_fExtraGravityRate;

	kInfo.m_fBuffJumpHeight = m_fEnableHeight;
	kInfo.m_iBuffJumpMaxCnt = max( 2, m_iMaxJumpCnt);

	kInfo.m_dwBuffJumpEnableTime = m_dwJumpEnableTime;

	kInfo.m_fBuffJumpAniRate = m_fAniRate;
	kInfo.m_vBuffJumpAnimationList = m_vJumpAnimationList;
	kInfo.m_vBuffJumpExtraAnimationList = m_vJumpExtraAnimationList;

	kInfo.m_bEnableMoveRotate = m_bEnableMoveRotate;
	kInfo.m_dwMoveRotateSpeed = m_dwMoveRotateSpeed;

	pOwner->SetBuffMultiFlyJumpValue( kInfo );

	m_bReduceGauge = false;
	m_bHideMesh = true;
	return true;
}

void ioMultiFlyJumpBuff::ProcessBuff( float fTimePerSec )
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
}

bool ioMultiFlyJumpBuff::IsCanJumpAttack() const
{
	return m_bJumpAttack;
}

void ioMultiFlyJumpBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		pOwner->RemoveBuffMultiFlyJumpValue( GetName() );

	ioBuff::EndBuff();
}

bool ioMultiFlyJumpBuff::OnReduceGauge() const
{
	return m_bReduceGauge;
}

void ioMultiFlyJumpBuff::ProcessPassive()
{
	if( !m_pOwner )
		return;

	if( m_fGaugePerTick <= 0.0f || m_dwTickTime <= 0 )
		return;
	
	if( GetName() != m_pOwner->GetBuffFlyJumpName() )
	{
		return;
	}


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

				m_bReduceGauge = true;
			}
			else
			{
				m_bReduceGauge = false;
			}

			//if( m_bInvisibleMeshOnFlyJump && ( m_bHideMesh != m_bReduceGauge ) )
			if( m_bInvisibleMeshOnFlyJump && ( m_bHideMesh != m_bReduceGauge ) && !m_ItemName.IsEmpty() )
			{
				//int iItemType = m_pOwnerItem->GetType();
				ioItem *pOwnerItem = m_pOwner->GetEquipedItem( m_ItemName );
				if( !pOwnerItem )
					return;

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
