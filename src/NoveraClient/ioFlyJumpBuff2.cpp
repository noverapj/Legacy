
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioFlyJumpBuff2.h"

ioFlyJumpBuff2::ioFlyJumpBuff2()
{
}

ioFlyJumpBuff2::ioFlyJumpBuff2( const ioFlyJumpBuff2 &rhs )
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
	  m_RemoveBuffList( rhs.m_RemoveBuffList ),
	  m_CheckBuffList( rhs.m_CheckBuffList ),
	  m_dwActiveTime( rhs.m_dwActiveTime ),
	  m_szEffectName( rhs.m_szEffectName ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower )
{
	m_vJumpAnimationList.clear();
	int iCnt = rhs.m_vJumpAnimationList.size();
	for( int i=0; i < iCnt; i++ )
		m_vJumpAnimationList.push_back( rhs.m_vJumpAnimationList[i] );

	m_dwEffectID = -1;
	m_CheckBuffListIndex.clear();
}

ioFlyJumpBuff2::~ioFlyJumpBuff2()
{
	m_RemoveBuffList.clear();
	m_CheckBuffList.clear();
	m_CheckBuffListIndex.clear();
}

ioBuff* ioFlyJumpBuff2::Clone()
{
	return new ioFlyJumpBuff2( *this );
}

void ioFlyJumpBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	
	m_dwActiveTime = rkLoader.LoadInt_e( "active_time", 0 ); 

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

	rkLoader.LoadString_e( "effect_name", "", szBuf, MAX_PATH );
	m_szEffectName = szBuf;
	
	m_bEnableMoveRotate = rkLoader.LoadBool_e( "enable_move_rotate", false );
	m_dwMoveRotateSpeed = (DWORD)rkLoader.LoadInt_e( "move_rotate_speed", 0 );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power",0.0f );

	m_bInvisibleMeshOnFlyJump = rkLoader.LoadBool_e( "invisible_mesh_on_fly_jump", false );	

	int nCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	char szRemoveBuf[MAX_PATH] = "";
	for (int i=0; i<nCnt; ++i)
	{
		wsprintf_e( szRemoveBuf, "remove_buff%d_name",i+1);
		rkLoader.LoadString( szRemoveBuf, "", szBuf, MAX_PATH );
		m_RemoveBuffList.push_back( szBuf );
	}

	nCnt = rkLoader.LoadInt_e( "check_buff_cnt", 0 );
	char szCheckBuf[MAX_PATH] = "";
	for (int i=0; i<nCnt; ++i)
	{
		wsprintf_e( szCheckBuf, "check_buff%d_name",i+1);
		rkLoader.LoadString( szCheckBuf, "", szBuf, MAX_PATH );
		m_CheckBuffList.push_back( szBuf );
	}	
}

void ioFlyJumpBuff2::StartBuff( ioBaseChar *pOwner )
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

	pOwner->SetBuffFlyJumpValue( kInfo );

	m_bReduceGauge = false;
	m_bHideMesh = true;

	m_dwCheckActiveTime = FRAMEGETTIME();

	m_dwEffectID = -1;

	//버프를 시작하기 전 버프를 체크하여 있다면 지워준다 
	for ( int i=0; i<(int)m_CheckBuffList.size(); ++i )
	{
		if ( pOwner->HasBuff( m_CheckBuffList[i] ) )
		{
			pOwner->RemoveBuff( m_CheckBuffList[i], true );
			m_CheckBuffListIndex.push_back(i);
		}
	}	

}

bool ioFlyJumpBuff2::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

	pOwner->SetBuffFlyJumpValue( kInfo );

	m_bReduceGauge = false;
	m_bHideMesh = true;

	m_dwCheckActiveTime = FRAMEGETTIME();

	m_dwEffectID = -1;

	//버프를 시작하기 전 버프를 체크하여 있다면 지워준다 
	for ( int i=0; i<(int)m_CheckBuffList.size(); ++i )
	{
		if ( pOwner->HasBuff( m_CheckBuffList[i] ) )
		{
			pOwner->RemoveBuff( m_CheckBuffList[i], true );
			m_CheckBuffListIndex.push_back(i);
		}
	}

	return true;
}

void ioFlyJumpBuff2::ProcessBuff( float fTimePerSec )
{	
	if( !m_pOwner->IsNeedProcess() || m_fGaugePerTick <= 0.0f || m_dwTickTime <= 0 )
		return;

	if( GetName() != m_pOwner->GetBuffFlyJumpName() )
		return;

	if ( m_dwCheckActiveTime + m_dwActiveTime < FRAMEGETTIME() )
	{
		if( (m_pOwner->GetState() == CS_JUMP && m_pOwner->GetJumpState() == JS_JUMPPING) &&
			m_pOwner->GetUsedBuffFlyJump() != BJUT_NONE &&
			m_pOwner->GetUsedBuffFlyJump() != BJUT_END &&
			m_pOwner->IsFloating() )
		{
			m_pOwner->SetBuffFlyJumpEnd();
		}
		SetReserveEndBuff();
		
		if ( m_pOwner->GetState() == CS_JUMP )
		{
			m_pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, true, true );
		}

		if ( m_pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << m_pOwner->GetCharName();
			kPacket << GetName();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
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
				if( m_pOwner->ReduceCurSkillGaugeBySkill( m_ItemName, fCurTicGauge ) <= 0.0f )
				{
					m_pOwner->SetBuffFlyJumpEnd();
					SetReserveEndBuff();

					if ( m_pOwner->GetState() == CS_JUMP )
					{
						m_pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, true, true );
					}

					if ( m_pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
						kPacket << m_pOwner->GetCharName();
						kPacket << GetName();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
					return;
				}	
				m_bReduceGauge = true;
				ChangeMesh( !m_bReduceGauge );

				if ( m_dwEffectID == -1 && !m_szEffectName.IsEmpty() )
				{
					ioEffect *pEffect = m_pOwner->AttachEffect( m_szEffectName );
					if ( pEffect )
					{
						m_dwEffectID = pEffect->GetUniqueID();
					}
				}
			}
			else
			{
				if ( m_bReduceGauge )
				{
					m_bReduceGauge = false;
					ChangeMesh( !m_bReduceGauge );

					if ( m_dwEffectID != -1 )
					{
						m_pOwner->EndEffect( m_dwEffectID );
						m_dwEffectID = -1;
					}
				}
			}		
		}
	}
}

void ioFlyJumpBuff2::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	SetReserveEndBuff();
}

bool ioFlyJumpBuff2::IsCanJumpAttack() const
{
	return m_bJumpAttack;
}

void ioFlyJumpBuff2::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		pOwner->RemoveBuffFlyJumpValue( GetName() );		
	}

	ioBuff::EndBuff();

	for ( int i=0; i<(int)m_RemoveBuffList.size(); ++i )
	{
		m_pOwner->RemoveBuff( m_RemoveBuffList[i], true );
	}
	m_RemoveBuffList.clear();	
	
	//버프를 사용하기 전에 체크한 버프를 다시 걸어준다
	for ( int i=0; i<(int)m_CheckBuffListIndex.size(); ++i )
	{
		if ( m_CheckBuffListIndex[i] >= 0 && m_CheckBuffListIndex[i] < (int)m_CheckBuffList.size() )
		{
			//m_pOwner->AddNewBuff( m_CheckBuffList[m_CheckBuffListIndex[i]], m_pOwner->GetCharName(), GetName(), NULL );
			m_pOwner->ReserveAddNewBuff( m_CheckBuffList[m_CheckBuffListIndex[i]], m_pOwner->GetCharName(), GetName(), NULL );
		}
	}
	m_CheckBuffList.clear();
	m_CheckBuffListIndex.clear();

	SetSkillGauge( 0.0f );
	ChangeMesh( true );
		
	if ( m_dwEffectID != -1 )
	{
		m_pOwner->EndEffect( m_dwEffectID );
		m_dwEffectID = -1;
	}	
}

bool ioFlyJumpBuff2::OnReduceGauge() const
{
	return m_bReduceGauge;
}

bool ioFlyJumpBuff2::IsCharCollisionSkip() const
{
	if( m_bCharCollisionSkip )
		return true;

	return false;
}

//스킬게이지 설정
void ioFlyJumpBuff2::SetSkillGauge( float fSkillGauge )
{
	//버프의 주인 얻기
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	//주인의 해당 아이템의 스킬 게이지를 재설정
	pOwner->SetCurItemSkillGauge( m_ItemName, fSkillGauge );
}

void ioFlyJumpBuff2::ChangeMesh( bool bShow )
{
	if( m_bInvisibleMeshOnFlyJump )
	{
		ioItem *pItem = m_pOwner->GetEquipedItem( m_ItemName );
		if ( pItem )
		{
			int iItemType = pItem->GetType();
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
				m_pOwner->ShowEquipItemMesh( bShow, eSlot );
		}
	}	
}