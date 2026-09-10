#include "stdafx.h"
#include "ioMagneticItem.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioMagneticItem::ioMagneticItem()
{
	m_FireState						= FS_NONE;
	m_dwAttackStartTime				= 0;
	m_iCurCombo						= 0;

	// 캔슬 대시
	m_dwMotionEndTime				= 0;
	m_dwCancelDashAttackTime		= 0;
	m_SpecialState					= SS_NONE;

	// Change Buff
	m_dwChargeBuffStartTime			= 0.0f;
	m_bChangeBuff					= false;

	// 게이지
	m_fCurBullet					= 0.0f;

	// 무기 스킬 버프 상태
	m_bWeaponAttributeBuff			= false;

	// 기본 무기 활성화/비활성화 상태 확인 2020-12-21
	m_bShowWeaponEquipItem			= true;
}

ioMagneticItem::ioMagneticItem( const ioMagneticItem &rhs )
	: ioWeaponItem( rhs ),

	m_AttackRedNReadyAniList( rhs.m_AttackRedNReadyAniList ),
	m_NormalAttackRedNList(rhs.m_NormalAttackRedNList),

	m_AttackBlueSReadyAniList( rhs.m_AttackBlueSReadyAniList ),
	m_NormalAttackBlueSList(rhs.m_NormalAttackBlueSList),

	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),

	// Change Buff
	m_ChargeRedNDelayAnimation( rhs.m_ChargeRedNDelayAnimation ),
	m_ChargeBlueSDelayAnimation( rhs.m_ChargeBlueSDelayAnimation),
	m_vChargeBuff( rhs.m_vChargeBuff ),
	m_WeaponType( rhs.m_WeaponType ),
	m_WeaponAttributeBuffType( rhs.m_WeaponAttributeBuffType),
	m_fMaxBullet( rhs.m_fMaxBullet ),
	m_fNeedBullet( rhs.m_fNeedBullet ),
	m_fRecoverGaugeDelay( rhs.m_fRecoverGaugeDelay ),
	m_fRecoverGaugeRun( rhs.m_fRecoverGaugeRun ),
	m_fRecoverGaugeDash( rhs.m_fRecoverGaugeDash ),
	m_fRecoverGaugeWound( rhs.m_fRecoverGaugeWound ),
	m_fDecreaseChangeDamage( rhs.m_fDecreaseChangeDamage ),
	m_bBulletBuffInit( rhs.m_bBulletBuffInit ),
	m_WeaponRedNMeshList( rhs.m_WeaponRedNMeshList ),
	m_WeaponBlueSMeshList( rhs.m_WeaponBlueSMeshList ),
	m_iWeaponRedNMeshCount( rhs.m_iWeaponRedNMeshCount),
	m_iWeaponBlueSMeshCount( rhs.m_iWeaponBlueSMeshCount),
	m_NormalRedNCancelDashAttribute( rhs.m_NormalRedNCancelDashAttribute ),		// 캔슬 대시
	m_NormalBlueSCancelDashAttribute( rhs.m_NormalBlueSCancelDashAttribute ),	// 캔슬 대시
	m_strWeaponSkillName( rhs.m_strWeaponSkillName ),							// 무기 스킬
	m_strWeaponRedNABName( rhs.m_strWeaponRedNABName ),					
	m_strWeaponBlueSABName( rhs.m_strWeaponBlueSABName )
{
	m_FireState						= FS_NONE;
	m_dwAttackStartTime				= 0;
	m_iCurCombo						= 0;

	// 캔슬 대시
	m_dwMotionEndTime				= 0;
	m_dwCancelDashAttackTime		= 0;
	m_SpecialState					= SS_NONE;

	// Change Buff
	m_dwChargeBuffStartTime			= 0.0f;
	m_bChangeBuff					= false;

	// 게이지
	m_fCurBullet					= 0.0f;

	// 무기 스킬 버프 상태
	m_bWeaponAttributeBuff			= false;

	// 기본 무기 활성화/비활성화 상태 확인 2020-12-21
	m_bShowWeaponEquipItem			= true;
}

ioMagneticItem::~ioMagneticItem()
{
}

void ioMagneticItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	// 기본 Charge 준비 단계..
	int iMaxCombo = GetMaxCombo();

	//===========================================================================
	// N극(빨강)
	m_AttackRedNReadyAniList.clear();
	for( int i=0; i < iMaxCombo; i++ )
	{
		wsprintf_e( szKey, "attack_red_n_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackRedNReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_NormalAttackRedNList.clear();
	for(int i = 0;i < iMaxCombo; i++)
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "normal_red_n_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_NormalAttackRedNList.push_back( kAttribute );
	}

	// Change Buff
	rkLoader.LoadString_e( "charge_red_n_delay_animation", "", szBuf, MAX_PATH );
	m_ChargeRedNDelayAnimation	= szBuf;

	// 기본공격 중 캔슬 대시 
	LoadAttackAttribute_e( "normal_red_n_cancel_dash", m_NormalRedNCancelDashAttribute, rkLoader );
	//===========================================================================

	//===========================================================================
	// S극(파랑) 2020-12-02
	m_AttackBlueSReadyAniList.clear();
	for( int i=0; i < iMaxCombo; i++ )
	{
		wsprintf_e( szKey, "attack_blue_s_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackBlueSReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_NormalAttackBlueSList.clear();
	for(int i = 0;i < iMaxCombo; i++)
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "normal_blue_s_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_NormalAttackBlueSList.push_back( kAttribute );
	}

	rkLoader.LoadString_e( "charge_blue_s_delay_animation", "", szBuf, MAX_PATH );
	m_ChargeBlueSDelayAnimation	= szBuf;

	// 기본공격 중 캔슬 대시 
	LoadAttackAttribute_e( "normal_blue_s_cancel_dash", m_NormalBlueSCancelDashAttribute, rkLoader );
	//===========================================================================

	// Buff
	int	iCount	= rkLoader.LoadInt_e( "charge_buff_count", 0 );
	for( int i = 0; i < iCount; i++ )
	{
		sprintf_s( szKey, "charge_buff%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vChargeBuff.push_back( szBuf );
	}

	// S극(파랑), N극(빨강)
	m_WeaponType = (WeaponType)rkLoader.LoadInt_e( "weapon_type", WT_BLUE_S );
	m_WeaponAttributeBuffType = (WeaponAttributeBuffType)rkLoader.LoadInt_e( "weapon_attribute_type", WABT_BLUE_S );

	// Gauge 2020-11-27
	m_fMaxBullet	= rkLoader.LoadFloat_e( "max_gauge", 0.0f );
	m_fNeedBullet	= rkLoader.LoadFloat_e( "need_gauge", 0.0f );
	m_fRecoverGaugeDelay	= rkLoader.LoadFloat_e( "recover_gauge_delay", 0.0f );
	m_fRecoverGaugeRun	= rkLoader.LoadFloat_e( "recover_gauge_run", 0.0f );
	m_fRecoverGaugeDash	= rkLoader.LoadFloat_e( "recover_gauge_dash", 0.0f );
	m_fRecoverGaugeWound	= rkLoader.LoadFloat_e( "recover_gauge_wound", 0.0f );
	m_fReduceGaugeRate	= rkLoader.LoadFloat_e( "reduce_gauge_rate", 0.0f );

	// D~으로 생성한 마그네틱 필드 유지 시 tic 단위로 게이지 감소
	m_fDecreaseChangeDamage = rkLoader.LoadFloat_e( "decrease_change_damage", 0.0f );

	// 용병 교체 시 버프 해제 사용 유무
	m_bBulletBuffInit  = rkLoader.LoadBool_e( "bullet_buff_init", false );

	// 무기 변경
	m_iWeaponRedNMeshCount = rkLoader.LoadInt_e( "weapon_red_n_mesh_cnt", 0 );
	for( int i = 0; i < m_iWeaponRedNMeshCount; ++i )
	{
		wsprintf_e( szKey, "weapon_red_n_mesh%d", i );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_WeaponRedNMeshList.push_back( szBuf );
	}

	m_iWeaponBlueSMeshCount = rkLoader.LoadInt_e( "weapon_blue_s_mesh_cnt", 0 );
	for( int i = 0; i < m_iWeaponBlueSMeshCount; ++i )
	{
		wsprintf_e( szKey, "weapon_blue_s_mesh%d", i );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_WeaponBlueSMeshList.push_back( szBuf );
	}


	// 무기 스킬
	rkLoader.LoadString_e( "skill0", "", szBuf, MAX_PATH );
	m_strWeaponSkillName = szBuf;

	rkLoader.LoadString_e( "weapon_red_n_buff_name", "", szBuf, MAX_PATH );
	m_strWeaponRedNABName = szBuf;

	rkLoader.LoadString_e( "weapon_blue_s_buff_name", "", szBuf, MAX_PATH );
	m_strWeaponBlueSABName = szBuf;
}

ioItem* ioMagneticItem::Clone()
{
	return new ioMagneticItem( *this );
}

ioWeaponItem::WeaponSubType ioMagneticItem::GetSubType() const
{
	return WST_MAGNETIC_ITEM;
}

void ioMagneticItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	m_bChangeBuff			= false;	
	m_FireState				= FS_NONE;

	// 무기 스킬 버프 상태
	m_bWeaponAttributeBuff	= false;
}

bool ioMagneticItem::CheckLanding( ioBaseChar *pChar )
{
	m_bChangeBuff			= false;

	// 무기 스킬 버프 상태
	m_bWeaponAttributeBuff	= false;

	return true;
}

void ioMagneticItem::SetLandingState( ioBaseChar *pOwner )
{
	m_bChangeBuff			= false;

	// 무기 스킬 버프 상태
	m_bWeaponAttributeBuff	= false;

	// 속성변경
	if( IsGetWeaponTypeRedN() )
	{
		m_WeaponAttributeBuffType = WABT_RED_N;
	}
	else if( IsGetWeaponTypeBlueS() )
	{
		m_WeaponAttributeBuffType = WABT_BLUE_S;
	}
}

void ioMagneticItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	// ClearKeyReserve 초기화
	ClearCancelInfo();

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );		
		m_FireState		= FS_NONE;
		m_iCurCombo		= iCurCombo;
		m_SpecialState	= SS_NONE;	// 2020-12-23
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = -1;
		// N극(빨강), // S극(파랑)
		if( WT_RED_N == m_WeaponType )
		{
			iAniID = pGrp->GetAnimationIdx( m_AttackRedNReadyAniList[iCurCombo] );
			if ( iAniID == -1 )
			{
				return;	
			}
		}
		else if( WT_BLUE_S == m_WeaponType )
		{
			iAniID = pGrp->GetAnimationIdx( m_AttackBlueSReadyAniList[iCurCombo] );
			if ( iAniID == -1 )
			{
				return;
			}
		}

		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_SpecialState			= SS_NONE;	// 2020-12-23
		m_FireState				= FS_CHARGING;
		m_dwAttackStartTime		= FRAMEGETTIME();
		m_iCurCombo				= iCurCombo;
		DWORD dwTrackingTime	= FRAMEGETTIME();

		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		pOwner->SetState( CS_ATTACK );
	}
}

void ioMagneticItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_FireState )
	{
		case FS_CHARGING:
		{
			OnCharging( pOwner );
			break;
		}

		case FS_NORMAL_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{			
				CheckCancelReserveByConditionalSpecialDash( pOwner ); // m_bEnableDashState 체크 

				if( ProcessCancelBySpeicalDash( pOwner ) )
				{
					return;
				}

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
			break;
		}

		case FS_CHANGE_BUFF:
		{
			OnChargeBuff( pOwner );
			break;
		}		
	}
}

void ioMagneticItem::OnChargeBuff( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	//if( !pOwner->IsNeedProcess() )	
	//	return;

	if( m_dwChargeBuffStartTime < FRAMEGETTIME() )
	{
		pOwner->SetState( CS_DELAY );
		m_FireState	= FS_NONE;
		m_bChangeBuff = false;
	}
}

void ioMagneticItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_fNeedBullet <= m_fCurBullet && !IsChargeBuffCheck( pOwner ) )
			{
				// 최초 사용 시엔 무기와 동일한 속성 버프 생성
				if( WT_RED_N == m_WeaponType )			{	m_WeaponAttributeBuffType = WABT_RED_N;		}
				else if( WT_BLUE_S == m_WeaponType )	{	m_WeaponAttributeBuffType = WABT_BLUE_S;	}

				// 게이지 감소
				m_fCurBullet -= m_fNeedBullet;
				ChangeToChargeBuff( pOwner );
			}
			else if( IsChargeBuffCheck( pOwner ) )
			{
				// 마그네틱 필드 제거
				RemoveChargeBuff( pOwner );
				pOwner->SetState( CS_DELAY );
				m_FireState	= FS_NONE;
				// 필드 제거
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)FS_NONE;
					kPacket << m_iCurCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			else
			{
				pOwner->SetChargingState( false );
				ChangeToNormalAttack( pOwner );
			}
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioMagneticItem::RemoveWeaponAttributeBuff( ioBaseChar* pOwner )
{
	if( WABT_RED_N == m_WeaponAttributeBuffType )		// N극(빨강) 속성
	{
		pOwner->RemoveBuff( m_strWeaponRedNABName );
	}
	else if( WABT_BLUE_S == m_WeaponAttributeBuffType )	// S극(파랑) 속성
	{
		pOwner->RemoveBuff( m_strWeaponBlueSABName );
	}
}

bool ioMagneticItem::IsWeaponAttributeBuff( ioBaseChar* pOwner )
{
	ioBuff* pBuff = NULL;
	if( WABT_RED_N == m_WeaponAttributeBuffType )		// N극(빨강) 속성
	{
		pBuff = pOwner->GetBuff( m_strWeaponRedNABName );
		if( pBuff )
		{
			return true;
		}
	}

	if( WABT_BLUE_S == m_WeaponAttributeBuffType )	// S극(파랑) 속성
	{
		pBuff = pOwner->GetBuff( m_strWeaponBlueSABName );
		if( pBuff )
		{
			return true;
		}
	}
	
	// 없다.
	return false;
}

bool ioMagneticItem::IsChargeBuffCheck( ioBaseChar* pOwner )
{
	ioBuff* pBuff = NULL;
	int nSize = (int)m_vChargeBuff.size();
	for( int i = 0; i < nSize; ++i )
	{
		pBuff = pOwner->GetBuff( m_vChargeBuff[i] );
		if( pBuff )
		{
			return true;
		}
	}	

	return false;
}

void ioMagneticItem::RemoveChargeBuff( ioBaseChar* pOwner )
{
	int nSize = (int)m_vChargeBuff.size();
	for( int i = 0; i < nSize; ++i )
	{
		pOwner->RemoveBuff( m_vChargeBuff[i] );
	}	

	// 필드 제거
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FS_CHANGE_BUFF_REMOVE;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagneticItem::ChangeToChargeBuff( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_FireState	= FS_CHANGE_BUFF;
	m_dwChargeBuffStartTime	= FRAMEGETTIME();

	int iAniID = -1;
	if( WT_RED_N == m_WeaponType )
	{
		iAniID = pGrp->GetAnimationIdx( m_ChargeRedNDelayAnimation );
		if ( iAniID == -1 )
		{
			return;
		}
	}
	else if( WT_BLUE_S == m_WeaponType )
	{
		iAniID = pGrp->GetAnimationIdx( m_ChargeBlueSDelayAnimation );
		if ( iAniID == -1 )
		{
			return;
		}
	}

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );
	m_dwChargeBuffStartTime	+= pGrp->GetAnimationFullTime( iAniID );

	if( WABT_RED_N == m_WeaponAttributeBuffType )		// N극(빨강) 속성
	{
		pOwner->AddNewBuff( m_vChargeBuff[0], pOwner->GetCharName(), GetName(), NULL );
	}
	else if( WABT_BLUE_S == m_WeaponAttributeBuffType )	// S극(파랑) 속성
	{
		pOwner->AddNewBuff( m_vChargeBuff[1], pOwner->GetCharName(), GetName(), NULL );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << (int)m_WeaponAttributeBuffType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagneticItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	const AttackAttribute *pAttribute = NULL;
	if( WT_RED_N == m_WeaponType )
	{
		pAttribute= &m_NormalAttackRedNList[m_iCurCombo];
	}
	else if( WT_BLUE_S == m_WeaponType )
	{
		pAttribute= &m_NormalAttackBlueSList[m_iCurCombo];
	}

	//const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni					= pAttribute->m_AttackAnimation;
		m_fCurAniRate				= pAttribute->m_fAttackAniRate;
		m_dwInputCancelCheckTime	= dwCurTime;
		m_dwCancelCheckTime			= dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_FireState = FS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagneticItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
	{
		return;
	}

	int nState;
	rkPacket >> nState;
	rkPacket >> m_iCurCombo;

	switch( nState )
	{
		case SS_SPECIAL_DASH:
		{
			D3DXVECTOR3 vWorldPos;
			rkPacket >> vWorldPos;
			pOwner->SetWorldPosition( vWorldPos);

			int	iCurDir, iCallerType;
			rkPacket >> iCurDir;
			rkPacket >> iCallerType;

			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)iCurDir, iCallerType );
			break;
		}
	}
}

void ioMagneticItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
		case FS_NORMAL_ATTACK:
		{	
			ChangeToNormalAttack( pOwner );
			break;
		}
		case FS_CHANGE_BUFF:
		{
			int iWeaponAttributeBuffType = 0;
			rkPacket >> iWeaponAttributeBuffType;
			m_WeaponAttributeBuffType = (WeaponAttributeBuffType)iWeaponAttributeBuffType;

			ChangeToChargeBuff( pOwner );
			break;
		}
		case FS_CHANGE_MESH:
		{
			ChangeMesh( pOwner );
			break;
		}
		case FS_CHANGE_BUFF_REMOVE:
		{
			RemoveChargeBuff( pOwner );
			break;
		}
		case FS_CHANGE_DEFENSE_BUFF:
		{
			int iWeaponAttributeBuffType = 0;
			rkPacket >> iWeaponAttributeBuffType;
			m_WeaponAttributeBuffType = (WeaponAttributeBuffType)iWeaponAttributeBuffType;
			// 생성 
			if( WABT_RED_N == m_WeaponAttributeBuffType )		// N극(빨강)
			{
				pOwner->AddNewBuff( m_vChargeBuff[0], pOwner->GetCharName(), GetName(), NULL );
			}
			else if( WABT_BLUE_S == m_WeaponAttributeBuffType )	// S극(파랑)
			{
				pOwner->AddNewBuff( m_vChargeBuff[1], pOwner->GetCharName(), GetName(), NULL );
			}
			break;
		}

		case FS_NONE:
		{
			pOwner->SetState( CS_DELAY );
			m_FireState	= FS_NONE;
			break;
		}
	}
}

bool ioMagneticItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )			{	return false;	}
	if( m_dwInputCancelCheckTime == 0 )	{	return false;	}
	if( !pOwner )						{	return false;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return false;	}

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );			
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fCurAniRate;
		dwInputSpecialDashTime += m_dwInputCancelCheckTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		if( m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner ) )
		{
			// 무기변경( 파랑 --> 빨강 ) 애니 
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

bool ioMagneticItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if( m_SpecialState == SS_SPECIAL_DASH )
	{
		return	false;
	}

	return	true;
}

void ioMagneticItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	// N극(빨강), S극(파랑)
	int iAniID = -1;
	AttackAttribute attackattribute;
	if( WT_RED_N == m_WeaponType )
	{
		attackattribute = m_NormalRedNCancelDashAttribute;
	}
	else if( WT_BLUE_S == m_WeaponType )
	{
		attackattribute = m_NormalBlueSCancelDashAttribute;
	}

	iAniID = pGrp->GetAnimationIdx( attackattribute.m_AttackAnimation );
	if( iAniID == -1 )	{	return;	}

	DWORD dwPreDelay = attackattribute.m_dwPreDelay;
	DWORD dwEndDelay = attackattribute.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );


	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	float fTimeRate = attackattribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( attackattribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		attackattribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	m_dwKeyReserveTime	= 0;
	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	DWORD dwInputSpecialDashTime = pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if( dwInputSpecialDashTime > 0 )
		m_dwCancelDashAttackTime = dwStartTime + dwInputSpecialDashTime;

	m_SpecialState	= SS_SPECIAL_DASH;
	pOwner->SetState( CS_MAGNETIC_SPECIAL );
	
	// 무기 변경
	pOwner->ShowEquipItemMesh( false, ES_WEAPON );
	m_bShowWeaponEquipItem = false;	// 2020-12-21

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_SpecialState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( pOwner->IsNeedProcess() )
	{
		ChangeMesh( pOwner );
	}
}

// ioBaseChar::CheckEnableDashCheck() 함수를 체크 하기 위해서는 m_CurDashType == DT_MAGNETIC 추가해줘야 함 
// 또한 ioAttackableItem 기본은 무조건 false 리턴시킨다.
bool ioMagneticItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	// bCheck : ioBaseChar:: CheckEnableDashCheck() 함수에서는 true
	// bCheck : ioBaseChar::CheckNormalAttackState() 함수에서는 false
	if ( !pOwner )
		return	false;
	
	// 기본공격중 일때 m_bEnableDashCheck 상태 값을 변경하여 ioBaseChar::ProcessKeyInput() 함수에서 체크 중..
	CharState charstate = pOwner->GetState();
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_FireState )
		{
			case FS_NORMAL_ATTACK:
			{
				if( bCheck ) // ioBaseChar::CheckEnableDashCheck() 함수에서 bCheck = true 경우 처리 ( 주로 캔슬 대시 할때 )
				{
					return true;
				}

				break;
			}
		}
	}

	return	false;
}

void ioMagneticItem::ClearAttackState( ioBaseChar *pOwner )
{

}

void ioMagneticItem::ProcessSpecialState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
	{
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
	{
		return;
	}	

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_SpecialState )
	{
		case SS_NONE:
		{
			break;
		}

		case SS_SPECIAL_DASH:
		{			
			ProcessSpecialDash( pOwner );
			break;
		}
	}
}

void ioMagneticItem::ProcessSpecialDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
	{
		m_FireState		= FS_NONE;
		m_SpecialState	= SS_NONE;
		ChangeReserveKeyInputState( pOwner );

		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}
}

void ioMagneticItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
{
	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		ioObjectItem *pItem = pOwner->GetObject();

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				if ( !COMPARE( iCurCombo, 0, GetMaxCombo() ) )
					iCurCombo = 0;
				pOwner->SetNormalAttack( iCurCombo );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}

// 이펙트
void ioMagneticItem::CallItemProcess( ioBaseChar* pOwner )
{
	if( !pOwner )						{	return;		}

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
	{
		return;
	}

	// 스킬 사용시 m_bUseItemMesh 값이 true 일 경우 기본 무기를 활성화/비활성화 일 경우가 있다. 2020-12-21
	// N극(빨강)
	for( int i = 0; i < (int)m_WeaponRedNMeshList.size(); ++i )
	{
		ioEntity* pEntity = pOwner->GetEntity( m_WeaponRedNMeshList[i] );
		if( pEntity && pOwner->CheckInvisibleWeaponSkill() )
		{
			pEntity->SetVisible(false);
		}
		else if( pEntity )
		{
			pEntity->SetVisible(true);
		}
	}

	// 기본 무기 활성화/비활성화 상태 2020-12-21
	if( !m_bShowWeaponEquipItem )
	{
		pOwner->ShowEquipItemMesh( m_bShowWeaponEquipItem, ES_WEAPON );
	}

	// +S 방어 마그네틱 필드 속성 변경(N극<->S극) 지상에서 방어 시 발동  
	//if( CS_DEFENSE == pOwner->GetState() )
	if( pOwner->IsDefenseKey() && !IsWeaponAttributeBuff(pOwner) && CS_DEFENSE == pOwner->GetState() )
	{
		// 무기 스킬 버프 상태
		m_bWeaponAttributeBuff = false;

		// 있는지 검사 해서 있으면 속성 변경
		if( IsChargeBuffCheck( pOwner ) && !m_bChangeBuff )
		{
			//  버프 삭제 
			RemoveChargeBuff( pOwner );

			// 속성변경
			if( WABT_RED_N == m_WeaponAttributeBuffType ) m_WeaponAttributeBuffType = WABT_BLUE_S;
			else if( WABT_BLUE_S == m_WeaponAttributeBuffType ) m_WeaponAttributeBuffType = WABT_RED_N;

			// 생성 
			if( WABT_RED_N == m_WeaponAttributeBuffType )		// N극(빨강)
			{
				pOwner->AddNewBuff( m_vChargeBuff[0], pOwner->GetCharName(), GetName(), NULL );
			}
			else if( WABT_BLUE_S == m_WeaponAttributeBuffType )	// S극(파랑)
			{
				pOwner->AddNewBuff( m_vChargeBuff[1], pOwner->GetCharName(), GetName(), NULL );
			}

			m_bChangeBuff = true;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)FS_CHANGE_DEFENSE_BUFF;				
				kPacket << m_iCurCombo;
				kPacket << (int)m_WeaponAttributeBuffType;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else 
	{
		m_bChangeBuff = false;
	}
}

int ioMagneticItem::GetNeedBullet()
{
	return	(int)m_fNeedBullet;
}

int ioMagneticItem::GetMaxBullet()
{
	return	(int)m_fMaxBullet;
}

int ioMagneticItem::GetCurBullet()
{
	return	(int)m_fCurBullet;
}

void ioMagneticItem::SetCurBullet( int iCurBullet )
{
	if( m_bBulletBuffInit )
	{
		// 버프 해제 
		ioPlayStage *pStage = g_WeaponMgr.GetCreator();
		if( pStage )
		{
			ioHashString stOwnerName = this->GetOwnerName();
			ioBaseChar *pOwner = pStage->GetBaseChar( stOwnerName );
			if( pOwner )
			{
				RemoveChargeBuff( pOwner );
				m_bChangeBuff = false;
			}
		}
	}

	m_fCurBullet = (float)iCurBullet;
}

void ioMagneticItem::DecreaseChangeDamage( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsChargeBuffCheck( pOwner ) )
	{
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		float fCurDecrease = m_fDecreaseChangeDamage * fTimePerSec;		
		m_fCurBullet -= fCurDecrease;
		m_fCurBullet = (float)max( 0, min( m_fCurBullet, m_fMaxBullet ) );

		if( !pOwner->IsNeedProcess() )	return;

		if( m_fCurBullet <= 0.0f )
		{
			// 버프 해제
			RemoveChargeBuff( pOwner );
			m_bChangeBuff = false;
		}
	}
}

void ioMagneticItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float	fTimePerSec	= g_FrameTimer.GetSecPerFrame();
	float	fIncreaseGauge	= 0.0f;

	switch( pOwner->GetState() )
	{
	case	CS_DELAY:
		{
			fIncreaseGauge	= m_fRecoverGaugeDelay;
		}
		break;

	case	CS_RUN:
		{
			fIncreaseGauge	= m_fRecoverGaugeRun;
		}
		break;

	case	CS_DASH:
		{
			fIncreaseGauge	= m_fRecoverGaugeDash;
		}
		break;

	case	CS_WOUNDED:	// 다운
		{
			// 버프 해제
			RemoveChargeBuff( pOwner );
			m_bChangeBuff = false;
		}
		break;


	default:
		{
			fIncreaseGauge	= 0.0f;
		}
		break;
	}

	// 마그네틱 필드 유지 시 게이지 회복 X
	if( IsChargeBuffCheck( pOwner ) )
	{
		return;
	}

	if( m_fCurBullet < m_fMaxBullet )
	{
		float	fGauge	= fIncreaseGauge * fTimePerSec;

		m_fCurBullet	+= fGauge;
		m_fCurBullet	= min( m_fCurBullet, m_fMaxBullet );
	}
}

void ioMagneticItem::ChangeMesh( ioBaseChar* pOwner )
{
	// 현재 무기 타입 변경 
	if( WT_RED_N == m_WeaponType )			{	m_WeaponType = WT_BLUE_S;	}
	else if( WT_BLUE_S == m_WeaponType )	{	m_WeaponType = WT_RED_N;	}

	// 무기 변경 
	if( WT_RED_N == m_WeaponType )
	{
		// N극(빨강)
		for( int i = 0; i < (int)m_WeaponRedNMeshList.size(); ++i )
			pOwner->AddEntity( m_WeaponRedNMeshList[i] );
	}
	else if( WT_BLUE_S == m_WeaponType )
	{
		// 원래대로 기본 무기
		ReleaseMesh( pOwner );
		pOwner->ShowEquipItemMesh( true, ES_WEAPON );
		m_bShowWeaponEquipItem = true;	// 2020-12-21
	}

	// Send
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FS_CHANGE_MESH;
		kPacket << m_iCurCombo;	
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagneticItem::ReleaseMesh( ioBaseChar* pOwner )
{
	for( int i = 0; i < (int)m_WeaponRedNMeshList.size(); ++i )
	{
		pOwner->DestroyEntity( m_WeaponRedNMeshList[i] );
	}
}


void ioMagneticItem::SetChangeWeaponAttributeBuffType()
{
	// 속성변경
	if( WABT_RED_N == m_WeaponAttributeBuffType ) m_WeaponAttributeBuffType = WABT_BLUE_S;
	else if( WABT_BLUE_S == m_WeaponAttributeBuffType ) m_WeaponAttributeBuffType = WABT_RED_N;
}

void ioMagneticItem::SetWeaponAttributeBuff( bool bWeaponAttributeBuff )
{
	m_bWeaponAttributeBuff = bWeaponAttributeBuff;

	// 속성변경
	if( IsGetWeaponTypeRedN() && !m_bWeaponAttributeBuff )
	{
		m_WeaponAttributeBuffType = WABT_RED_N;
	}
	else if( IsGetWeaponTypeBlueS() && !m_bWeaponAttributeBuff )
	{
		m_WeaponAttributeBuffType = WABT_BLUE_S;
	}
}

bool ioMagneticItem::IsGetWeaponAttributeBuffTypeRedN()
{
	if( WABT_RED_N == m_WeaponAttributeBuffType )	
	{
		return true;
	}

	return false;
}

bool ioMagneticItem::IsGetWeaponAttributeBuffTypeBlueS()
{
	if( WABT_BLUE_S == m_WeaponAttributeBuffType )	
	{
		return true;
	}

	return false;
}

bool ioMagneticItem::IsGetWeaponTypeRedN()
{
	if( WT_RED_N == m_WeaponType )	
	{
		return true;
	}

	return false;
}

bool ioMagneticItem::IsGetWeaponTypeBlueS()
{
	if( WT_BLUE_S == m_WeaponType )	
	{
		return true;
	}

	return false;
}

void ioMagneticItem::Initialize( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if( !pOwner ) return;

	// 초기 무기 설정 
	//ChangeMesh( pOwner );
}

void ioMagneticItem::OnEquiped( ioBaseChar *pOwner )
{
	// 스킬 사용시 m_bUseItemMesh 값이 true 일 경우 기본 무기를 활성화/비활성화 일 경우가 있다. 2020-12-21
	if( WT_RED_N == m_WeaponType )
	{
		// 원래 문기 
		pOwner->ShowEquipItemMesh( false, ES_WEAPON );
		m_bShowWeaponEquipItem = false;	// 2020-12-21

		// N극(빨강)
		for( int i = 0; i < (int)m_WeaponRedNMeshList.size(); ++i )
			pOwner->AddEntity( m_WeaponRedNMeshList[i] );
	}

	ioAttackableItem::OnEquiped( pOwner );
}

void ioMagneticItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseMesh( pOwner );	

	// 2020-12-21
	RemoveChargeBuff( pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

// 2020-12-16
void ioMagneticItem::FillMagneticWeaponInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	// 현재 본인 
	if( !pOwner )	return;

	rkPacket << (int)m_WeaponType;
}

void ioMagneticItem::SetMagneticWeaponInfoList( ioBaseChar *pOwner, SP2Packet &rkPacket )
{	
	if( !pOwner )	return;

	int iWeaponType = 0;
	rkPacket >> iWeaponType;
	m_WeaponType = (WeaponType)iWeaponType;

	// 무기 변경 
	if( WT_RED_N == m_WeaponType )
	{
		// 기본 숨기기
		pOwner->ShowEquipItemMesh( false, ES_WEAPON );
		m_bShowWeaponEquipItem = false;	// 2020-12-21

		// N극(빨강)
		for( int i = 0; i < (int)m_WeaponRedNMeshList.size(); ++i )
			pOwner->AddEntity( m_WeaponRedNMeshList[i] );
	}
	else if( WT_BLUE_S == m_WeaponType )
	{
		// 원래대로 기본 무기
		ReleaseMesh( pOwner );
		pOwner->ShowEquipItemMesh( true, ES_WEAPON );
		m_bShowWeaponEquipItem = true;	// 2020-12-21
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioMagneticSpecialState::ioMagneticSpecialState()
{
}

ioMagneticSpecialState::~ioMagneticSpecialState()
{
}

void ioMagneticSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

// ioBaseChar::SetState 함수에서 상태 체크, 이전 특별 상태 삭제할때 호출
void ioMagneticSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioMagneticItem *pMagneticItem = ToMagneticItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMagneticItem )
	{
		pMagneticItem->ClearAttackState( pOwner );
	}
}

// ioBaseChar::ProcessState() 함수에서 디폴트로 함수 계속 호출한다.
void ioMagneticSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMagneticItem *pMagneticItem = ToMagneticItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMagneticItem )
	{
		// 무기스킬 공중
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

		if( pOwner->CheckGhostState() )
		{
			float fPreHeightForGhostState = pOwner->GetPreHeightForGhostState();
			if( pOwner->GetJumpState() == JS_JUMP_ATTACK && pOwner->GetBottomHeight() > 0.0f )
			{
				fPreHeightForGhostState = pOwner->GetBottomHeight();
			}
			fHeightGap = pOwner->GetWorldPosition().y - fPreHeightForGhostState;
		}

		pMagneticItem->ProcessSpecialState( pOwner, fHeightGap );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioMagneticSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

// 특별 상태가 지속되는지 검사해서 아니면 false로 리턴하여 ioBaseChar::ProcessMove()를 실행
bool ioMagneticSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioMagneticItem *pMagneticItem = ToMagneticItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pMagneticItem )
	{
		return false;
	}

	// 일반 공격 중 캔슬 대시
	if( !pMagneticItem->IsConditionalSpecialDashReserve( pOwner ) )
	{
		return false;
	}

	return true;
}