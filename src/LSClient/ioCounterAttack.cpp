

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioCounterAttack.h"
#include "ioKungfuMasterItem.h"
#include "ioChargeContactWeaponItem.h"
#include "ioHeraclesItem.h"

ioCounterAttack::ioCounterAttack()
{
	m_dwInputSpecialDashTime = 0;
	m_dwInputCancelTime = 0;
	m_dwInputCancelJumpTime = 0;
}

ioCounterAttack::ioCounterAttack( const ioCounterAttack &rhs )
: m_CounterAttack( rhs.m_CounterAttack ),
  m_JumpKeyCounterAttack( rhs.m_JumpKeyCounterAttack ),
  m_bEnableDashCheck( rhs.m_bEnableDashCheck )
{
}

ioCounterAttack::~ioCounterAttack()
{
	m_dwInputSpecialDashTime = 0;
	m_dwInputCancelJumpTime = 0;
}

void ioCounterAttack::LoadProperty( ioINILoader &rkLoader )
{
	m_CounterAttack.Init();
	LoadAttackAttribute_e( "counter_attack", m_CounterAttack, rkLoader );
	m_bEnableDashCheck = rkLoader.LoadBool_e( "counter_attack_dash_check", false );

	m_JumpKeyCounterAttack.Init();
	LoadAttackAttribute_e( "jump_key_counter_attack", m_JumpKeyCounterAttack, rkLoader );
}

void ioCounterAttack::ApplyCAT( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int nCounterState;
	rkPacket >> nCounterState;

	switch( nCounterState )
	{
	case CAS_ChargeContactWeapon:
		{
			ioChargeContactWeaponItem *pChargeContact = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_DEFENSE ) );
			if( pChargeContact )
				pChargeContact->SetGaugeCounter( pOwner );
		}
		break;
	}
}

bool ioCounterAttack::StartCAT( ioBaseChar *pOwner, int iCounterAttackKeyState )
{
	if( !pOwner )
		return false;

	if( iCounterAttackKeyState == CAKS_ATTACK_KEY )
	{
		ioChargeContactWeaponItem *pChargeContact = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_DEFENSE ) );
		if( pChargeContact && pChargeContact->IsEnableGaugeCounter() )
		{
			pChargeContact->SetGaugeCounter( pOwner );

			if ( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_COUNTER_ATTACK_SYNC );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << pOwner->GetHP().m_fCurValue;
				kPacket << CAS_ChargeContactWeapon;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return false;
		}
		else
		{
			pOwner->SetCounterAttackByAttribute( m_CounterAttack );
			SetInputSpecialDash( pOwner, m_CounterAttack );
			SetInputCancelJump( pOwner, m_CounterAttack );
			return true;
		}
	}
	return false;
}

void ioCounterAttack::ProcessCAT( ioBaseChar *pOwner )
{
}

void ioCounterAttack::InitAniRotate()
{
	m_iCurAniRotate = 0;
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();
}


void ioCounterAttack::SetAniRotate( ioBaseChar *pChar, int iAniID, float fAniRate )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	InitAniRotate();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fAniRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += dwCurTime;
		else
			kInfo.m_dwTime = 0;

		if( !(*iter_r)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_r)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fAngle = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vAniRotateInfoList.push_back(kInfo);

		++iter_r;
	}

	if( !m_vAniRotateInfoList.empty() )
	{
		m_iCurAniRotate = 0;
		m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
		m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
	}
}


void ioCounterAttack::CheckAniRotate( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_vAniRotateInfoList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwAniRotateTime > 0 && m_dwAniRotateTime < dwCurTime )
	{
		if( m_fAniRotateAngle > 0.0f )
		{
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(m_fAniRotateAngle) );
			D3DXVECTOR3 vMoveDir = qtRot * pChar->GetMoveDir();

			D3DXQUATERNION qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

			pChar->SetTargetRotAndMoveDirChange( qtTargetRot );
		}

		m_iCurAniRotate++;

		int iSize = m_vAniRotateInfoList.size();
		if( COMPARE(m_iCurAniRotate, 0, iSize) )
		{
			m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
			m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
		}
		else
		{
			m_dwAniRotateTime = 0;
			m_fAniRotateAngle = 0.0f;

		}
	}
}

bool ioCounterAttack::CheckInputSpecialDash( ioBaseChar *pOwner )
{
	if ( m_dwInputSpecialDashTime && m_dwInputSpecialDashTime < FRAMEGETTIME() )
	{
		ioAttackableItem *pAttack = pOwner->GetPriorityItem( SPT_ATTACK );
		if( pAttack && pAttack->IsConditionalSpecialDashReserve( pOwner ) && pOwner->IsDirKeyDoubleClick() 
			&& pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
		{
			pAttack->SetChangeConditionalSpecialDash( pOwner, pOwner->CheckCurDirKey(), ioAttackableItem::CCSD_COUNTER_ATTACK );
			return true;
		}
	}

	return false;
}

bool ioCounterAttack::CheckInputCancel( ioBaseChar *pOwner )
{
	if ( m_dwInputCancelTime && m_dwInputCancelTime < FRAMEGETTIME() )
	{
		ioKungfuMasterItem* pKungfu = ToKungfuMasterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( !pKungfu || !pKungfu->EnableGauge() )
			return false;

		if( pOwner->IsDefenseKey() )
		{
			pKungfu->SetSleepStart( pOwner, true );
			return true;
		}
	}

	return false;
}

bool ioCounterAttack::CheckInputCancelJump( ioBaseChar* pOwner )
{
	if( !pOwner )
		return false;

	if ( m_dwInputCancelJumpTime && m_dwInputCancelJumpTime < FRAMEGETTIME() )
	{
		ioHeraclesItem* pHeracles = ToHeraclesItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( !pHeracles || !pHeracles->IsEnableJumpGauge() )
			return false;

		if( pOwner->IsJumpKey() )
		{
			pHeracles->SetCancelJump( pOwner );
			return true;
		}
	}

	return false;
}

void ioCounterAttack::SetInputCancelJump( ioBaseChar* pOwner, const AttackAttribute& rkAtt )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( rkAtt.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwStartTime = FRAMEGETTIME() + rkAtt.m_dwPreDelay;
	float fTimeRate = FLOAT1;
	if ( rkAtt.m_fAttackAniRate > 0.0f )
		fTimeRate = rkAtt.m_fAttackAniRate;

	DWORD dwJumpTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_counter_jump_s" ) * fTimeRate;
	if ( dwJumpTime != 0 )
		m_dwInputCancelJumpTime = dwStartTime + dwJumpTime;

	/*DWORD dwCancelTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if ( dwCancelTime != 0 )
		m_dwInputCancelTime = dwStartTime + dwCancelTime;*/
}

void ioCounterAttack::SetInputSpecialDash( ioBaseChar *pOwner, const AttackAttribute& rkAttri )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( rkAttri.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwStartTime = FRAMEGETTIME() + rkAttri.m_dwPreDelay;
	float fTimeRate = FLOAT1;
	if ( rkAttri.m_fAttackAniRate > 0.0f )
		fTimeRate = rkAttri.m_fAttackAniRate;

	DWORD dwDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if ( dwDashTime != 0 )
		m_dwInputSpecialDashTime = dwStartTime + dwDashTime;

	DWORD dwCancelTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if ( dwCancelTime != 0 )
		m_dwInputCancelTime = dwStartTime + dwCancelTime;
}