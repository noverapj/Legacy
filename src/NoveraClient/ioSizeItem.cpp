#include "stdafx.h"
#include "ioGenisysarmyItem.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioSizeItem::ioSizeItem()
{
	m_FireState				= FS_NONE;
	m_dwAttackStartTime		= 0;
	m_iCurCombo				= 0;
	m_fCurCoolTime			= 0.0f;

	// D~ 크기가 작아지면서 전진하며 공격
	m_bScale				= false;
	m_vBeforeScale			= D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
	m_dwFullAnimationTime	= 0.0f;

	// 이펙트
	m_dwExtendScaleEffectID	= -1;
}

ioSizeItem::ioSizeItem( const ioSizeItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_dwMaxCoolTime( rhs.m_dwMaxCoolTime ),
	m_dwNeedCoolTime( rhs.m_dwNeedCoolTime ),
	m_fRecoveryTime( rhs.m_fRecoveryTime ),
	m_fScaleValue( rhs.m_fScaleValue ),
	m_ExtendAttribute( rhs.m_ExtendAttribute),
	m_szExtendScaleEffectName( rhs.m_szExtendScaleEffectName ),
	m_szScaleEffectCreateSound( rhs.m_szScaleEffectCreateSound )
{
	m_FireState				= FS_NONE;
	m_dwAttackStartTime		= 0;
	m_iCurCombo				= 0;
	m_fCurCoolTime			= 0.0f;

	// D~ 크기가 작아지면서 전진하며 공격
	m_bScale				= false;
	m_vBeforeScale			= D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
	m_dwFullAnimationTime	= 0.0f;

	// 이펙트
	m_dwExtendScaleEffectID	= -1;
}

ioSizeItem::~ioSizeItem()
{
}

void ioSizeItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	// 기본 Charge 준비 단계..
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	// 특별공격
	m_dwMaxCoolTime = (DWORD)rkLoader.LoadInt_e( "max_cool_time", 0 );
	m_dwNeedCoolTime = (DWORD)rkLoader.LoadInt_e( "use_cool_time", 0 );
	m_fRecoveryTime = rkLoader.LoadFloat_e( "recover_cool_time", 0.0f );

	// D~ 크기가 작아지면서 전진하며 공격
	m_fScaleValue = rkLoader.LoadFloat_e( "scale_value", 0.0f );

	LoadAttackAttribute( "extend_attack", m_ExtendAttribute, rkLoader );

	// 이펙트
	rkLoader.LoadString_e( "extend_scale_effect_name", "", szBuf, MAX_PATH );	// 작아질때
	m_szExtendScaleEffectName = szBuf;
	rkLoader.LoadString_e( "scale_effect_create_sound", "", szBuf, MAX_PATH );
	m_szScaleEffectCreateSound = szBuf;
}

ioItem* ioSizeItem::Clone()
{
	return new ioSizeItem( *this );
}

ioWeaponItem::WeaponSubType ioSizeItem::GetSubType() const
{
	return WST_SIZE_ITEM;
}

void ioSizeItem::SetNoneState( ioBaseChar* pOwner, int eNewState /*= 0 */ )
{
	if( !pOwner ) return;

	m_FireState				= FS_NONE;
	// CS_FALL 상태에 SetNoneState 함수를 호출 해서 m_bScale 값이 true 이라 
	// 원래대로 돌아가지 않는다. 주석처리함 2020-10-29
	if( !pOwner->HasBuff( BT_KING_SCALE )  /*&& !m_bScale*/ )
	{
		pOwner->SetWorldScale( m_vBeforeScale );
	}

	// 이펙트
	pOwner->EndEffect( m_dwExtendScaleEffectID );
	m_dwExtendScaleEffectID	= -1;
}

void ioSizeItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	// ClearKeyReserve 초기화
	ClearCancelInfo();

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );		
		m_FireState				= FS_NONE;
		m_iCurCombo				= iCurCombo;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		if( iAniID == -1 )
		{
			LOG.PrintTimeAndLog( 0, "SetNormalAttack::SetNormalAttack - %s Animation is Not Exist",
				m_AttackReadyAniList[iCurCombo].c_str() );
			return;
		}

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_FireState				= FS_CHARGING;
		m_dwAttackStartTime		= FRAMEGETTIME();
		m_iCurCombo				= iCurCombo;
		DWORD dwTrackingTime	= FRAMEGETTIME();

		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		pOwner->SetState( CS_ATTACK );
	}
}

void ioSizeItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FireState )
	{
		case FS_CHARGING:
		{
			OnCharging( pOwner );
			break;
		}

		case FS_NORMAL_ATTACK:
		case FS_CHARGING_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
				{
					return;
				}
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
			break;
		}		
	}

	// D~ 애니메이션이 끝나면
	if( m_dwFullAnimationTime > 0 && m_dwFullAnimationTime < dwCurTime && m_bScale )
	{
		// 크기를 원래 상태로
		m_bScale = false;
		m_dwFullAnimationTime = 0.0f;
		pOwner->SetWorldScale( m_vBeforeScale );

		// 이펙트
		pOwner->EndEffect( m_dwExtendScaleEffectID );
		m_dwExtendScaleEffectID	= -1;
	}
}

void ioSizeItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_fCurCoolTime >= m_dwNeedCoolTime )
			{
				ChangeToChargeAttackState( pOwner );
			}
			else
			{
				// 기본 공격 4타 캔슬 점프 가능 input_jump_s
				ChangeToNormalAttack( pOwner );
			}
		}
	}
	else
	{
		// 기본 공격 4타 캔슬 점프 가능 input_jump_s
		ChangeToNormalAttack( pOwner );
	}
}

void ioSizeItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni					= pAttribute->m_AttackAnimation;
		m_fCurAniRate				= pAttribute->m_fAttackAniRate;

		// 캔슬 체크 시간 
		m_dwInputCancelCheckTime	= dwCurTime;
		m_dwCancelCheckTime			= dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_FireState = FS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( ST_NORMAL_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// ioBaseChar::CheckEnableDashCheck() 함수를 체크한다.
bool ioSizeItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	 // 기본 대시 일 경우 m_CharState == CS_EXTEND_DASH 
	if( pOwner->GetState() == CS_EXTEND_DASH )
	{
		return true;
	}

	return	false;
}

bool ioSizeItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	// 스킬 쪽에서도 검사를 하기때문에 bNormalAttack 변수를 하나 넣는다. 
	if( pOwner->GetState() == CS_EXTEND_DASH && bNormalAttack)
	{
		return false;
	}
	return true;
}

void ioSizeItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	BYTE btState;
	rkPacket >> btState;

	switch( btState )
	{
		case ST_NORMAL_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			rkPacket >> m_iCurCombo;
			ChangeToNormalAttack( pOwner );
			break;
		}

		case ST_CHARGING_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			rkPacket >> m_iCurCombo;
			ChangeToChargeAttackState( pOwner );
			break;
		}
	}
}

void ioSizeItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;		}

	// Check Bullet
	if( m_dwMaxCoolTime <= m_fCurCoolTime )
	{
		return;
	}

	switch( pOwner->GetState() )
	{
		case CS_DELAY:	// 딜레이
		case CS_RUN:	// 걷기
		case CS_DASH:	// 달리기 
			break;

		default:
			return;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = m_fRecoveryTime * fTimePerSec;

	m_fCurCoolTime += fGauge;
	m_fCurCoolTime = (float)min( m_fCurCoolTime, (float)m_dwMaxCoolTime );
}

int ioSizeItem::GetMaxBullet()
{
	return (int)m_dwMaxCoolTime;
}

int ioSizeItem::GetNeedBullet()
{
	return (int)m_dwNeedCoolTime;
}

int ioSizeItem::GetCurBullet()
{
	DWORD dwCurCoolTime = m_fCurCoolTime;
	dwCurCoolTime = max( 0, min( dwCurCoolTime, m_dwMaxCoolTime ) );

	return (int)dwCurCoolTime;
}

void ioSizeItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	DWORD dwCurBullet = iCurBullet;
	dwCurBullet = max( 0, min( dwCurBullet, m_dwMaxCoolTime ) );

	m_fCurCoolTime = (float)dwCurBullet;
}

void ioSizeItem::InitCoolTime()
{
	m_fCurCoolTime = 0.0f;
}

void ioSizeItem::MaxCoolTime()
{
	m_fCurCoolTime = (float)m_dwMaxCoolTime;
}

void ioSizeItem::DecreaseCoolTime()
{
	if( m_fCurCoolTime < (float)m_dwNeedCoolTime )
	{
		m_fCurCoolTime = 0.0f;
	}
	else
	{
		m_fCurCoolTime -= (float)m_dwNeedCoolTime;
	}
}

// 특별 공격 크기가 작아지면서 전진하며 공격 D~
void ioSizeItem::ChangeToChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	int iAniID	     = pGrp->GetAnimationIdx( m_ExtendAttribute.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioSizeItem::ChangeToChargeAttackState - %s Animation is Not Exist",
								m_ExtendAttribute.m_AttackAnimation.c_str() );
		return;
	}

	// D~ 사용시 감소 
	DecreaseCoolTime();

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();
	pOwner->SetChargingState( false );

	m_dwCancelCheckTime			= dwCurTime;
	m_dwInputCancelCheckTime	= dwCurTime;

	// 스케일
	if( !m_bScale )
	{	
		// 이펙트
		CreateScaleEffectChargeEffect( pOwner );

		m_bScale = true;		
		D3DXVECTOR3 vScale = m_vBeforeScale - (m_vBeforeScale * m_fScaleValue);
		pOwner->SetWorldScale( vScale );
	}

	m_szCurAni					= m_ExtendAttribute.m_AttackAnimation;
	m_fCurAniRate				= m_ExtendAttribute.m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;

	pOwner->SetNormalAttackByAttribute( m_ExtendAttribute );

	float fTimeRate  = m_ExtendAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_ExtendAttribute.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_dwKeyReserveTime = 0;
	float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserve > 0.0f )
	{
		m_dwKeyReserveTime = dwPreDelay + dwCurTime + fReserve;
	}
	m_dwFullAnimationTime = dwCurTime + (pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay);

	m_FireState = FS_CHARGING_ATTACK;
	pOwner->ApplyNormalAttackResult( m_iCurCombo );		

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( ST_CHARGING_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// 이펙트
void ioSizeItem::CallItemProcess( ioBaseChar* pOwner )
{
	if( !pOwner )						{	return;		}

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
	{
		return;
	}

	// 크기가 작아지면서 상대의 위치로 이동하며 공격한다.
	if ( m_dwExtendScaleEffectID == -1 )
	{
		return;
	}

	ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwExtendScaleEffectID );
	if( pMapEffect )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		pMapEffect->SetWorldPosition( vPos );
	}
}

void ioSizeItem::CreateScaleEffectChargeEffect( ioBaseChar *pOwner )
{
	if ( !pOwner )
	{
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
	{
		return;
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vScale = pOwner->GetWorldScale();

	// 크기가 작아지면서 상대의 위치로 이동하며 공격한다.
	if ( m_szExtendScaleEffectName.IsEmpty() )
	{
		return;
	}

	ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szExtendScaleEffectName, vPos, m_vBeforeScale );
	if ( pMapEffect )
	{
		m_dwExtendScaleEffectID =  pMapEffect->GetUniqueID();
	}


	if( !m_szScaleEffectCreateSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szScaleEffectCreateSound );
	}
}