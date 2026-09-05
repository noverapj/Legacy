
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioPlayStage.h"
#include "ItemDefine.h"
#include "BuffDefine.h"

//Uroborus
void ioBaseChar::SetUroborusState( const ioHashString &szAniName,
	float fAniRate,
	D3DXVECTOR3 vTargetPos,
	float fSpeed,
	float fOffSetAngle,
	float fMoveRate,
	float fEndJumpAmt )
{
	int iAniID = m_pGroup->GetAnimationIdx( szAniName );
	if( iAniID == -1 )
		return;

	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	D3DXVECTOR3 vCurDir = vTargetPos - GetMidPositionByRate();
	if( fOffSetAngle != 0.0f )
	{
		D3DXVECTOR3 vAxis;
		if( vCurDir != ioMath::UNIT_Y && vCurDir != -ioMath::UNIT_Y )
		{
			D3DXVec3Cross( &vAxis, &vCurDir, &ioMath::UNIT_Y );
		}
		else
		{
			D3DXVECTOR3 vSightDir = m_qtTargetRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vSightDir, &vSightDir );

			D3DXVec3Cross( &vAxis, &vSightDir, &vCurDir );
		}

		D3DXQUATERNION qtNewRot;
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fOffSetAngle) );

		vCurDir = qtNewRot * vCurDir;
	}

	D3DXVECTOR3 vMoveDir = vCurDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fMoveRange = D3DXVec3Length( &vCurDir ) * fMoveRate;
	D3DXVECTOR3 vMoveTargetPos = GetMidPositionByRate() + (vMoveDir * fMoveRange);
	
	CheckCharColSkipTime( iAniID, fTimeRate, 0 );

	SetState( CS_UROBORUS );

	ioUroborusSpecialState *pUroborusSpecialState = ToUroborusSpecialState( m_pCharSpeicalState );
	if ( pUroborusSpecialState )
		pUroborusSpecialState->SetUroborusInfo( vMoveDir, vMoveTargetPos, fSpeed, fMoveRange, fEndJumpAmt );
}

//WereWolf
void ioBaseChar::CheckEnableChangeWereWolfGauge()
{
	if( !IsNeedProcess() )
		return;

	if( !CheckGrapplingExceptionState() )
		return;

	if( IsObejctEquipState( OES_WEREWOLF ) )
		return;

	ioWereWolfItem *pWereWolf = ToWereWolfItem( GetPriorityItem( SPT_ATTACK ) );
	if( pWereWolf )
		pWereWolf->CheckEnableChangeDamage( this );

	ioDrSpiderItem *pDrSpider = ToDrSpiderItem( GetPriorityItem( SPT_ATTACK ) );
	if( pDrSpider )
		pDrSpider->CheckEnableChangeDamage( this );

}

bool ioBaseChar::CheckChangeWereWolfState( int iExtendType )
{
	if( !CheckGrapplingExceptionState() )
		return false;

	switch( m_CharState )
	{
	case CS_USING_SKILL:
	case CS_FLY:
		return false;
	}

	if( ToHookItem( GetWeapon() ) && ToHookItem( GetWeapon() )->CheckOnHand() )
		return false;

	if( ToGunnerItem( GetWeapon() ) && ToGunnerItem( GetWeapon() )->CheckOnHand() )
		return false;

	if( IsHasCrown() )
		return false;

	if( IsGangsi() )
		return false;

	if( IsBoss() && iExtendType == ioAttackableItem::ECT_RIDER )
		return false;
	if( IsBoss() && iExtendType == ioAttackableItem::ECT_GRIFFIN )
		return false;
	if( IsBoss() && iExtendType == ioAttackableItem::ECT_BUBBLE )
		return false;

	if( GetObject() )
		return false;

	if( m_pEquipSlot && m_pEquipSlot->IsEquipWait( ES_OBJECT ) )
		return false;

	if( m_bPrisonerMode || m_bCatchState )
		return false;

	if( m_iEtcItemSkeleton != 0 )
		return false;

	return true;
}

void ioBaseChar::SetWereWolfState( const ioHashString& szAni, float fTimeRate )
{
	int iAniID = m_pGroup->GetAnimationIdx( szAni );

	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_AnimationTime.dwStartTime = FRAMEGETTIME();
	m_AnimationTime.dwFullTime  = m_pGroup->GetAnimationFullTime( iAniID ) * fTimeRate;

	SetState( CS_WEREWOLF );
}

void ioBaseChar::ApplyWereWolfState( SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	float fCurChangeDamage;
	rkPacket >> fCurChangeDamage;

	// 권왕 변신 2020-06-12
	ioGwonwangItem *pGwonwang = ToGwonwangItem( m_pEquipSlot->GetWeapon() );	
	ioWereWolfItem *pWereWolf = ToWereWolfItem( m_pEquipSlot->GetWeapon() );
	ioDrSpiderItem *pDrSpider = ToDrSpiderItem( m_pEquipSlot->GetWeapon() );

	switch( iType )
	{
	case WEREWOLF_SET:
		if( pGwonwang ) // 권왕 변신 2020-06-12
		{
			pGwonwang->SetChangeWereWolfState( this );
			pGwonwang->SetCurBullet( fCurChangeDamage );			
		}
		if( pWereWolf )
		{
			pWereWolf->SetChangeWereWolfState( this );
			pWereWolf->SetCurChangeDamage( fCurChangeDamage );
		}
		break;
	case WEREWOLF_RELEASE:
		{
		}
		break;
	case WEREWOLF_ENABLE_GAUGE:
		if( pGwonwang ) // 권왕 변신 2020-06-12
		{
			pGwonwang->SetEnableChangeDamageEffect( this );
			pGwonwang->SetCurBullet( fCurChangeDamage );			
		}

		if( pWereWolf )
		{
			pWereWolf->SetEnableChangeDamageEffect( this );
			pWereWolf->SetCurChangeDamage( fCurChangeDamage );
		}
		break;
	case WEREWOLF_RIDING_SET:
		if( pGwonwang ) // 권왕 변신 2020-06-12
		{
			pGwonwang->SetChangeWereWolfState( this );
			pGwonwang->SetCurBullet( fCurChangeDamage );
		}

		if( pWereWolf )
		{
			pWereWolf->SetChangeWereWolfState( this );
			pWereWolf->SetCurChangeDamage( fCurChangeDamage );

			int iDummyIndex;
			rkPacket >> iDummyIndex;

			if( iDummyIndex > 0 )
				pWereWolf->CreateDummyChar( this, iDummyIndex );
		}
		break;
	case BUBBLE_RIDING_SET:
		{
			ioBubbleItem *pBubble = ToBubbleItem( m_pEquipSlot->GetWeapon() );
			if( pBubble )
			{
				pBubble->SetChangeWereWolfState( this );

				int iDummyIndex;
				rkPacket >> iDummyIndex;
				if( iDummyIndex > 0 )
					pBubble->CreateDummyChar( this, iDummyIndex );
			}
		}
		break;
	case CHANGE_ON_SET:
		{
			int iState;
			rkPacket >> iState;

			ioChangeOnItem* pChangeOn = ToChangeOnItem( m_pEquipSlot->GetWeapon() );
			if( pChangeOn )				
			{
				pChangeOn->SetChangeWereWolfState( this, (ioChangeOnItem::WerewolfChangeState)iState );
				
				int iDummyIndex;
				rkPacket >> iDummyIndex;
				if( iDummyIndex > 0 )
					pChangeOn->CreateWerewolfDummy( this, iDummyIndex, (ioChangeOnItem::WerewolfChangeState)iState );
			}
		}
		break;
	case DR_SPIDER_SET:
		if( pDrSpider )
		{
			pDrSpider->SetChangeWereWolfState( this );
			pDrSpider->SetCurChangeDamage( fCurChangeDamage );
		}
		break;
	case DR_SPIDER_ENABLE_GAUGE:
		if( pDrSpider )
		{
			pDrSpider->SetEnableChangeDamageEffect( this );
			pDrSpider->SetCurChangeDamage( fCurChangeDamage );
		}
		break;
	case DR_SPIDER_RIDING_SET:
		if( pDrSpider )
		{
			pDrSpider->SetChangeWereWolfState( this );
			pDrSpider->SetCurChangeDamage( fCurChangeDamage );

			int iDummyIndex;
			rkPacket >> iDummyIndex;

			if( iDummyIndex > 0 )
				pDrSpider->CreateDummyChar( this, iDummyIndex );
		}
		break;
	}
}

void ioBaseChar::FillWereWolfStateInfo( SP2Packet &rkPacket )
{
	ioWereWolfItem *pWereWolf = ToWereWolfItem( GetWeapon() );
	ioDrSpiderItem *pDrSpider = ToDrSpiderItem( GetWeapon() );
	if( !pWereWolf && !pDrSpider )
		return;

	if( pWereWolf )
		rkPacket << pWereWolf->GetCurChangeDamage();
	else if( pDrSpider )
		rkPacket << pDrSpider->GetCurChangeDamage();
	return;
}

void ioBaseChar::SetWereWolfStateInfo( SP2Packet &rkPacket )
{
	float fCurChangeDamage;
	rkPacket >> fCurChangeDamage;

	ioWereWolfItem *pWereWolf = ToWereWolfItem( GetWeapon() );
	if( pWereWolf )
	{
		pWereWolf->ApplyChangeWereWolfState( this );
		pWereWolf->SetCurChangeDamage( fCurChangeDamage );
	}

	ioDrSpiderItem *pDrSpider = ToDrSpiderItem( GetWeapon() );
	if( pDrSpider )
	{
		pDrSpider->ApplyChangeDrSpiderState( this );
		pDrSpider->SetCurChangeDamage( fCurChangeDamage );
	}
}

void ioBaseChar::ReleaseWereWolfState()
{
	ioAttackableItem *pAttackable = ToAttackableItem(m_pEquipSlot->GetWeapon());
	if( pAttackable )
	{
		pAttackable->ReleseWereWolfState( this );
	}
}

bool ioBaseChar::IsWereWolfState()
{
	ioAttackableItem *pAttackable = ToAttackableItem(m_pEquipSlot->GetWeapon());
	if( pAttackable && pAttackable->IsWereWolfState() )
		return true;

	return false;
}

//레이첼
void ioBaseChar::CheckSylphid( bool bDefense )
{
	CheckSylphidByRachelItem( bDefense );
	if( !bDefense )
		CheckSylphidByWereWolfObjItem();
}

void ioBaseChar::CheckSylphidByRachelItem( bool bDefense )
{
	if( !m_KeyInput.IsDefenseKey() )
		return;

	ioRachelItem *pRachelItem = ToRachelItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pRachelItem )		// 오브젝트 아이템등을 들고 있을때 무시됨...
		return;

	if( pRachelItem->GetCurBullet() <= 0 )
		return;

	const SylphidInfo &rkInfo = pRachelItem->GetSylphidInfo();
	ioRachelJump *pRachelJump = ToRachelJump( pRachelItem->GetCurExtendJump() );

	if( bDefense )
	{
		ClearSylphid();

		if( CheckObjectSylphid( true ) )
		{
			if( !rkInfo.m_DefaultEffect.IsEmpty() )
			{
				EndEffect( rkInfo.m_DefaultEffect, false );
				AttachEffect( rkInfo.m_DefaultEffect );
			}

			if( IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_CHAR_ACTION_INFO );
				kPacket << GetCharName();
				kPacket << CHAR_ACTION_SYLPHID;
				kPacket << true;		// defense
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}

		return;
	}

	int iNeedSylphid = (int)rkInfo.m_dwDefaultNeed;

	switch( m_CharState )
	{
	case CS_DASH_ATTACK:
		if( !m_bSetExtraDashAttack && !m_bSetExtraOtherDashAttack )
		{
			iNeedSylphid += (int)rkInfo.m_dwDashAttackNeed;
			break;
		}
		return;
	case CS_JUMP:
		if( pRachelJump && pRachelJump->IsJumpDashState() )
		{
			iNeedSylphid += (int)rkInfo.m_dwJumpDashNeed;
			break;
		}
		else if( pRachelJump && m_JumpState == JS_JUMPPING )
		{
			if( m_fCurJumpPowerAmt >= m_fGravityAmt )
			{
				if( pRachelJump->IsCanDoubleJumpAttack() )
					iNeedSylphid += (int)rkInfo.m_dwUpJumpNeed2;
				else
					iNeedSylphid += (int)rkInfo.m_dwUpJumpNeed;
			}
			else
			{
				if( pRachelJump->IsCanDoubleJumpAttack() )
					iNeedSylphid += (int)rkInfo.m_dwDownJumpNeed2;
				else
					iNeedSylphid += (int)rkInfo.m_dwDownJumpNeed;
			}
			break;
		}
		return;
	default:
		return;
	}

	ClearSylphid();

	bool bUseSylphid = false;
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_CharState )
	{
	case CS_DASH_ATTACK:
		{
			bUseSylphid = true;
			m_fForceAmt = 0.0f;
			m_fForceFrictionRate = FLOAT1;

			SetForcePower( m_vMoveDir, rkInfo.m_fDashAttackForceAmt, rkInfo.m_fDashAttackFriction );

			pRachelItem->DecreaseCoolTime( iNeedSylphid );

			if( !rkInfo.m_DashAttackEffect.IsEmpty() )
			{
				EndEffect( rkInfo.m_DashAttackEffect, false );
				AttachEffect( rkInfo.m_DashAttackEffect );
			}
		}
		break;
	case CS_JUMP:
		if( pRachelJump && pRachelJump->IsJumpDashState() )
		{
			bUseSylphid = true;
			m_fForceAmt = 0.0f;
			m_fForceFrictionRate = FLOAT1;

			SetForcePower( m_vMoveDir, rkInfo.m_fJumpDashForceAmt, rkInfo.m_fJumpDashFriction );

			pRachelItem->DecreaseCoolTime( iNeedSylphid );

			if( !rkInfo.m_JumpDashEffect.IsEmpty() )
			{
				EndEffect( rkInfo.m_JumpDashEffect, false );
				AttachEffect( rkInfo.m_JumpDashEffect );
			}
		}
		else if( pRachelJump && m_JumpState == JS_JUMPPING )
		{
			if( m_fCurJumpPowerAmt >= m_fGravityAmt )	// up
			{
				bUseSylphid = true;
				m_fCurJumpPowerAmt = 0.0f;
				m_fGravityAmt = 0.0f;
				m_fForceAmt = 0.0f;
				m_fForceFrictionRate = FLOAT1;

				if( pRachelJump->IsCanDoubleJumpAttack() )
				{
					m_fSylphidJumpSpeedRate = rkInfo.m_fUpJumpSpeedRate2;
					SetJumpPower( rkInfo.m_fUpJumpPowerAmt2 );

					if( !rkInfo.m_UpJumpEffect2.IsEmpty() )
					{
						EndEffect( rkInfo.m_UpJumpEffect2, false );
						AttachEffect( rkInfo.m_UpJumpEffect2 );
					}
				}
				else
				{
					m_fSylphidJumpSpeedRate = rkInfo.m_fUpJumpSpeedRate;
					SetJumpPower( rkInfo.m_fUpJumpPowerAmt );

					if( !rkInfo.m_UpJumpEffect.IsEmpty() )
					{
						EndEffect( rkInfo.m_UpJumpEffect, false );
						AttachEffect( rkInfo.m_UpJumpEffect );
					}
				}
			}
			else	// down
			{
				bUseSylphid = true;
				m_fCurJumpPowerAmt = 0.0f;
				m_fGravityAmt = 0.0f;
				m_fForceAmt = 0.0f;
				m_fForceFrictionRate = FLOAT1;

				if( pRachelJump->IsCanDoubleJumpAttack() )
				{
					m_fSylphidJumpSpeedRate = rkInfo.m_fDownJumpSpeedRate2;
					m_fSylphidJumpGravityRate = rkInfo.m_fDownJumpGrabityRate2;

					if( !rkInfo.m_DownJumpEffect2.IsEmpty() )
					{
						EndEffect( rkInfo.m_DownJumpEffect2, false );
						AttachEffect( rkInfo.m_DownJumpEffect2 );
					}
				}
				else
				{
					m_fSylphidJumpSpeedRate = rkInfo.m_fDownJumpSpeedRate;
					m_fSylphidJumpGravityRate = rkInfo.m_fDownJumpGrabityRate;

					if( !rkInfo.m_DownJumpEffect.IsEmpty() )
					{
						EndEffect( rkInfo.m_DownJumpEffect, false );
						AttachEffect( rkInfo.m_DownJumpEffect );
					}
				}
			}

			pRachelItem->DecreaseCoolTime( iNeedSylphid );
		}
		break;
	}

	if( CheckObjectSylphid( true ) )
		bUseSylphid = true;

	if( bUseSylphid )
	{
		if( !rkInfo.m_DefaultEffect.IsEmpty() )
		{
			EndEffect( rkInfo.m_DefaultEffect, false );
			AttachEffect( rkInfo.m_DefaultEffect );
		}
	}

	if( IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_CHAR_ACTION_INFO );
		kPacket << GetCharName();
		kPacket << CHAR_ACTION_SYLPHID;
		kPacket << false;		// defense
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBaseChar::CheckSylphidByWereWolfObjItem()
{
	if( !IsNeedProcess() )
		return;

	if( m_CharState != CS_JUMP || m_JumpState != JS_JUMPPING )
		return;

	ioObjectWereWolfItem *pWereWolfObjItem = ToObjectWereWolfItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pWereWolfObjItem || !pWereWolfObjItem->IsEnableSylphid() )
		return;

	int iUseSylphidType = pWereWolfObjItem->GetUseSylphidType();
	bool bChange = false;
	if( m_KeyInput.IsDefenseKey() )
	{
		if( iUseSylphidType == ioObjectWereWolfItem::ST_NotUse )
			iUseSylphidType = ioObjectWereWolfItem::ST_Up;
		else
			iUseSylphidType = ioObjectWereWolfItem::ST_NotUse;

		bChange = true;
		ClearSylphid();
		pWereWolfObjItem->SetUseSylphidType( iUseSylphidType );

		if( IsNeedSendNetwork() && iUseSylphidType == ioObjectWereWolfItem::ST_NotUse )
		{
			SP2Packet kPacket( CUPK_CHAR_ACTION_INFO );
			kPacket << GetCharName();
			kPacket << CHAR_WEREWOLF_SYLPHID;
			kPacket << iUseSylphidType;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	
	if( iUseSylphidType == ioObjectWereWolfItem::ST_NotUse )
		return;

	const SylphidInfo &rkInfo = pWereWolfObjItem->GetSylphidInfo();
	DWORD dwCurTime = FRAMEGETTIME();

	if( bChange && !rkInfo.m_DefaultEffect.IsEmpty() )
	{
		EndEffect( rkInfo.m_DefaultEffect, false );
		AttachEffect( rkInfo.m_DefaultEffect );
	}

	if( m_fCurJumpPowerAmt >= m_fGravityAmt )	// up
	{
		if( bChange )
		{
			m_fCurJumpPowerAmt = 0.0f;
			m_fGravityAmt = 0.0f;
			m_fForceAmt = 0.0f;
			m_fForceFrictionRate = FLOAT1;

			m_fSylphidJumpSpeedRate = rkInfo.m_fUpJumpSpeedRate;
			SetJumpPower( rkInfo.m_fUpJumpPowerAmt );			

			if( !rkInfo.m_UpJumpEffect.IsEmpty() )
			{
				EndEffect( rkInfo.m_UpJumpEffect, false );
				AttachEffect( rkInfo.m_UpJumpEffect );
			}
		}
	}
	else if( bChange || iUseSylphidType == ioObjectWereWolfItem::ST_Up )
	{
		m_fCurJumpPowerAmt = 0.0f;
		m_fGravityAmt = 0.0f;
		m_fForceAmt = 0.0f;
		m_fForceFrictionRate = FLOAT1;
		iUseSylphidType = ioObjectWereWolfItem::ST_Down;		

		m_fSylphidJumpSpeedRate = rkInfo.m_fDownJumpSpeedRate;
		m_fSylphidJumpGravityRate = rkInfo.m_fDownJumpGrabityRate;

		pWereWolfObjItem->SetUseSylphidType( iUseSylphidType );

		if( bChange && !rkInfo.m_DownJumpEffect.IsEmpty() )
		{
			EndEffect( rkInfo.m_DownJumpEffect, false );
			AttachEffect( rkInfo.m_DownJumpEffect );
		}
		bChange = true;
	}	

	if( bChange && IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_CHAR_ACTION_INFO );
		kPacket << GetCharName();
		kPacket << CHAR_WEREWOLF_SYLPHID;
		kPacket << iUseSylphidType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBaseChar::SetSylphidByRachelItem( bool bDefense )
{
	ioRachelItem *pRachelItem = ToRachelItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pRachelItem )		// 오브젝트 아이템등을 들고 있을때 무시됨...
		return;

	const SylphidInfo &rkInfo = pRachelItem->GetSylphidInfo();
	ioRachelJump *pRachelJump = ToRachelJump( pRachelItem->GetCurExtendJump() );

	if( bDefense )
	{
		ClearSylphid();

		if( CheckObjectSylphid( false ) )
		{
			if( !rkInfo.m_DefaultEffect.IsEmpty() )
			{
				EndEffect( rkInfo.m_DefaultEffect, false );
				AttachEffect( rkInfo.m_DefaultEffect );
			}
		}
		return;
	}

	ClearSylphid();

	bool bUseSylphid = false;
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_CharState )
	{
	case CS_DASH_ATTACK:
		{
			bUseSylphid = true;
			m_fForceAmt = 0.0f;
			m_fForceFrictionRate = FLOAT1;

			SetForcePower( m_vMoveDir, rkInfo.m_fDashAttackForceAmt, rkInfo.m_fDashAttackFriction );
		}
		break;
	case CS_JUMP:
		if( pRachelJump && pRachelJump->IsJumpDashState() )
		{
			bUseSylphid = true;
			m_fForceAmt = 0.0f;
			m_fForceFrictionRate = FLOAT1;

			SetForcePower( m_vMoveDir, rkInfo.m_fJumpDashForceAmt, rkInfo.m_fJumpDashFriction );
		}
		else if( pRachelJump && m_JumpState == JS_JUMPPING )
		{
			if( m_fCurJumpPowerAmt >= m_fGravityAmt )	// up
			{
				bUseSylphid = true;
				m_fCurJumpPowerAmt = 0.0f;
				m_fGravityAmt = 0.0f;
				m_fForceAmt = 0.0f;
				m_fForceFrictionRate = FLOAT1;

				if( pRachelJump->IsCanDoubleJumpAttack() )
				{
					m_fSylphidJumpSpeedRate = rkInfo.m_fUpJumpSpeedRate2;
					SetJumpPower( rkInfo.m_fUpJumpPowerAmt2 );
				}
				else
				{
					m_fSylphidJumpSpeedRate = rkInfo.m_fUpJumpSpeedRate;
					SetJumpPower( rkInfo.m_fUpJumpPowerAmt );
				}
			}
			else	// down
			{
				bUseSylphid = true;
				m_fCurJumpPowerAmt = 0.0f;
				m_fGravityAmt = 0.0f;
				m_fForceAmt = 0.0f;
				m_fForceFrictionRate = FLOAT1;

				if( pRachelJump->IsCanDoubleJumpAttack() )
				{
					m_fSylphidJumpSpeedRate = rkInfo.m_fDownJumpSpeedRate2;
					m_fSylphidJumpGravityRate = rkInfo.m_fDownJumpGrabityRate2;
				}
				else
				{
					m_fSylphidJumpSpeedRate = rkInfo.m_fDownJumpSpeedRate;
					m_fSylphidJumpGravityRate = rkInfo.m_fDownJumpGrabityRate;
				}
			}
		}
		break;
	}

	if( CheckObjectSylphid( false ) )
		bUseSylphid = true;

	if( bUseSylphid )
	{
		if( !rkInfo.m_DefaultEffect.IsEmpty() )
		{
			EndEffect( rkInfo.m_DefaultEffect, false );
			AttachEffect( rkInfo.m_DefaultEffect );
		}
	}
}

void ioBaseChar::SetSylphidByWereWolfObjItem()
{
	if( m_CharState != CS_JUMP || m_JumpState != JS_JUMPPING )
		return;

	ioObjectWereWolfItem *pWereWolfObjItem = ToObjectWereWolfItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pWereWolfObjItem || !pWereWolfObjItem->IsEnableSylphid() )
		return;

	int iUseSylphidType = pWereWolfObjItem->GetUseSylphidType();
	if( iUseSylphidType == ioObjectWereWolfItem::ST_NotUse )
	{
		ClearSylphid();
		return;
	}

	const SylphidInfo &rkInfo = pWereWolfObjItem->GetSylphidInfo();
	DWORD dwCurTime = FRAMEGETTIME();

	if( iUseSylphidType == ioObjectWereWolfItem::ST_Up )	// up
	{
		m_fCurJumpPowerAmt = 0.0f;
		m_fGravityAmt = 0.0f;
		m_fForceAmt = 0.0f;
		m_fForceFrictionRate = FLOAT1;

		m_fSylphidJumpSpeedRate = rkInfo.m_fUpJumpSpeedRate;
		SetJumpPower( rkInfo.m_fUpJumpPowerAmt );			

		if( !rkInfo.m_UpJumpEffect.IsEmpty() )
		{
			EndEffect( rkInfo.m_UpJumpEffect, false );
			AttachEffect( rkInfo.m_UpJumpEffect );
		}
	}
	else
	{
		m_fCurJumpPowerAmt = 0.0f;
		m_fGravityAmt = 0.0f;
		m_fForceAmt = 0.0f;
		m_fForceFrictionRate = FLOAT1;

		m_fSylphidJumpSpeedRate = rkInfo.m_fDownJumpSpeedRate;
		m_fSylphidJumpGravityRate = rkInfo.m_fDownJumpGrabityRate;

		if( !rkInfo.m_DownJumpEffect.IsEmpty() )
		{
			EndEffect( rkInfo.m_DownJumpEffect, false );
			AttachEffect( rkInfo.m_DownJumpEffect );
		}
	}

	if( !rkInfo.m_DefaultEffect.IsEmpty() )
	{
		EndEffect( rkInfo.m_DefaultEffect, false );
		AttachEffect( rkInfo.m_DefaultEffect );
	}
}

bool ioBaseChar::CheckObjectSylphid( bool bCheckBullet )
{
	ioRachelItem *pRachelItem = ToRachelItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pRachelItem )		// 오브젝트 아이템등을 들고 있을때 무시됨...
		return false;

	if( bCheckBullet && pRachelItem->GetCurBullet() <= 0 )
		return false;

	int iNeedCnt = g_WeaponMgr.CheckSetWeaponSylphid( this );
	if( iNeedCnt > 0 )
	{
		pRachelItem->DecreaseCoolTime( iNeedCnt );
		return true;
	}

	return false;
}

void ioBaseChar::ClearSylphid()
{
	m_fSylphidJumpSpeedRate = FLOAT1;
	m_fSylphidJumpGravityRate = FLOAT1;
}

//일지매
void ioBaseChar::SetReturnMark()
{
	if( !IsNeedProcess() )	return;

	ioItem *pItem = GetEquipedItem(ES_WEAPON);
	ioIljimaeItem *pIljimaeItem = ToIljimaeItem( pItem );
	if( !pIljimaeItem )
	{
		if( m_dwReturnMarkEffectID != -1 )
		{
			ioMapEffect *pMapEffect = m_pCreator->FindMapEffect( m_dwReturnMarkEffectID );
			if( pMapEffect )
			{
				pMapEffect->EndEffectForce();
				m_dwReturnMarkEffectID = -1;
			}
		}
		return;
	}

	if( m_dwReturnMarkEffectID != -1 )
	{
		ioMapEffect *pMapEffect = m_pCreator->FindMapEffect( m_dwReturnMarkEffectID );
		if( pMapEffect )
		{
			pMapEffect->EndEffectForce();
			m_dwReturnMarkEffectID = -1;
		}
	}

	if( m_dwReturnMarkEffectID == -1 )
	{
		D3DXVECTOR3 vPos = GetWorldPosition();
		vPos.y = GetBottomHeight();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		ioHashString szEffect = pIljimaeItem->GetReturnMarkEffet();
		ioMapEffect *pMapEffect = m_pCreator->CreateMapEffect( szEffect, vPos, vScale );
		if( pMapEffect )
		{
			m_dwReturnMarkEffectID = pMapEffect->GetUniqueID();
		}
	}
}

void ioBaseChar::CheckReturnMark()
{
	if( !IsNeedProcess() )	return;

	ioItem *pItem = GetEquipedItem(ES_WEAPON);
	ioIljimaeItem *pIljimaeItem = ToIljimaeItem( pItem );
	if( !pIljimaeItem )
	{
		if( m_dwReturnMarkEffectID != -1 )
		{
			ioMapEffect *pMapEffect = m_pCreator->FindMapEffect( m_dwReturnMarkEffectID );
			if( pMapEffect )
			{
				pMapEffect->EndEffectForce();
				m_dwReturnMarkEffectID = -1;
			}
		}
		return;
	}

	if( pIljimaeItem->IsShowMarkEffect() )
	{
		if( m_dwReturnMarkEffectID == -1 )
		{
			D3DXVECTOR3 vPos = GetWorldPosition();
			vPos.y = GetBottomHeight();
			D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
			ioHashString szEffect = pIljimaeItem->GetReturnMarkEffet();
			ioMapEffect *pMapEffect = m_pCreator->CreateMapEffect( szEffect, vPos, vScale );
			if( pMapEffect )
				m_dwReturnMarkEffectID = pMapEffect->GetUniqueID();
		}
	}
	else
	{
		if( m_dwReturnMarkEffectID != -1 )
		{
			ioMapEffect *pMapEffect = m_pCreator->FindMapEffect( m_dwReturnMarkEffectID );
			if( pMapEffect )
			{
				pMapEffect->EndEffectForce();
				m_dwReturnMarkEffectID = -1;
			}
		}
		pIljimaeItem->SetDisableReturnMarkEffect();
	}
}

void ioBaseChar::CheckIljimaeTarget()
{
	if( !IsNeedProcess() )	return;

	ioItem *pItem = GetEquipedItem(ES_WEAPON);
	ioIljimaeItem *pIljimaeItem = ToIljimaeItem( pItem );
	if( !pIljimaeItem )
	{
		return;
	}
	pIljimaeItem->CheckTargetDuration( m_pCreator );
	pIljimaeItem->CheckTargetRange( m_pCreator );
}

//Witch
bool ioBaseChar::SetWitchFlyStateSkill( const ioHashString &szBuffName, const WitchFlyInfo &rkWitchFlyInfo )
{
	ioWitchItem *pWitchItem = ToWitchItem( GetPriorityItem( SPT_ATTACK ) );
	if( pWitchItem )
		pWitchItem->SetWitchFlyState( this, rkWitchFlyInfo, true );

	ioWitchFlySpecialState *pWitchFlySpecialState = ToWitchFlySpecialState( m_pCharSpeicalState );
	if ( pWitchFlySpecialState )
		pWitchFlySpecialState->SetWitchFlyBuffName( szBuffName );

	return true;
}

void ioBaseChar::CheckEnableWitchGauge()
{
	if( !IsNeedProcess() )
		return;

	if( !CheckGrapplingExceptionState() )
		return;

	if( m_CharState == CS_WITCH_FLY )
		return;

	ioWitchItem *pWitchItem = ToWitchItem( GetPriorityItem( SPT_ATTACK ) );
	if( pWitchItem )
		pWitchItem->CheckEnableWitchGauge( this );
}

void ioBaseChar::ApplyWitchState( SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	float fCurWitchGauge;
	rkPacket >> fCurWitchGauge;

	ioWitchItem *pWitchItem = ToWitchItem( m_pEquipSlot->GetWeapon() );
	switch( iType )
	{
	case WITCH_ENABLE_GAUGE:
		if( pWitchItem )
		{
			pWitchItem->SetEnableWitchEffect( this );
			pWitchItem->SetCurWitchGauge( fCurWitchGauge );
		}
		break;
	}
}

//Mechanics
bool ioBaseChar::SetMechanicsFlyState( const MechanicsFlyInfo &rkFlyInfo )
{
	ioMechanicsItem *pMechanics = ToMechanicsItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMechanics )
		return false;

	pMechanics->SetMechanicsFlyState( this, rkFlyInfo, false );
	return true;
}

bool ioBaseChar::SetMechanicsFlyStateSkill( const ioHashString &szBuffName, const MechanicsFlyInfo &rkFlyInfo )
{
	ioMechanicsItem *pMechanics = ToMechanicsItem( GetPriorityItem( SPT_ATTACK ) );
	if( pMechanics )
		pMechanics->SetMechanicsFlyState( this, rkFlyInfo, false );
	else
		return false;

	m_MechanicsBuffName = szBuffName;
	return true;
}

void ioBaseChar::CheckEnableMechanicsGauge()
{
	if( !IsNeedProcess() )
		return;

	if( !CheckGrapplingExceptionState() )
		return;

	if( m_CharState == CS_MECHANICS_FLY )
		return;

	ioMechanicsItem *pMechanics = ToMechanicsItem( GetPriorityItem( SPT_ATTACK ) );
	if( pMechanics )
		pMechanics->CheckEnableMechanicsGauge( this );
}

bool ioBaseChar::IsEnableMechanicsFly()
{
	ioMechanicsItem *pMechanics = ToMechanicsItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMechanics )
		return false;

	if( pMechanics->IsEnableMechanicsGauge() && !IsHasCrown() )
		return true;

	return false;
}

//Phantom
bool ioBaseChar::SetPhantomFlyStateAttack()
{
	ioPhantomItem *pPhantom = ToPhantomItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pPhantom )
		return false;

	pPhantom->SetPhantomFlyStateAttack( this );
	return true;
}

bool ioBaseChar::SetPhantomFlyState( const PhantomFlyInfo &rkFlyInfo )
{
	ioPhantomItem *pPhantom = ToPhantomItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pPhantom )
		return false;

	pPhantom->SetPhantomFlyState( this, rkFlyInfo, false );
	return true;
}

void ioBaseChar::CheckEnablePhantomGauge()
{
	if( !IsNeedProcess() )
		return;

	if( !CheckGrapplingExceptionState() )
		return;

	if( m_CharState == CS_PHANTOM_FLY )
		return;

	ioPhantomItem *pPhantom = ToPhantomItem( GetPriorityItem( SPT_ATTACK ) );
	if( pPhantom )
		pPhantom->CheckEnablePhantomGauge( this );
}

bool ioBaseChar::IsEnablePhantomFly()
{
	ioPhantomItem *pPhantom = ToPhantomItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pPhantom )
		return false;

	if( IsHasCrown() )
		return false;

	if( !pPhantom->IsEnablePhantomGauge() )
		return false;

	if( !pPhantom->CheckPhantomFlyEnableTime() )
		return false;

	return true;
}

//Lucifer
bool ioBaseChar::SetLuciferFlyState( const LuciferFlyInfo &rkFlyInfo )
{
	ioLuciferItem *pLucifer = ToLuciferItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pLucifer )
		return false;

	pLucifer->SetFlyState( this, rkFlyInfo, false );
	return true;
}

bool ioBaseChar::IsEnableLuciferFly()
{
	ioLuciferItem *pLucifer = ToLuciferItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pLucifer )
		return false;

	if( pLucifer->IsEnableGauge() && !IsHasCrown() )
		return true;

	return false;
}

//MegaAgent
bool ioBaseChar::SetMegaAgentFlyState()
{
	ioMegaAgentItem *pMegaAgent = ToMegaAgentItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMegaAgent )
		return false;

	pMegaAgent->SetFlyState( this, false );
	return true;
}

bool ioBaseChar::IsEnableMegaAgentFly()
{
	ioMegaAgentItem *pMegaAgent = ToMegaAgentItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMegaAgent || !pMegaAgent->IsEnableBullet( pMegaAgent->GetNeedBullet() ) )
		return false;

	if( !IsHasCrown() )
		return true;

	return false;
}

void ioBaseChar::ChangeToSmileJokerFlyState()
{
	ioPowerChargeItem3 *pPowerItem3 = ToPowerChargeItem3( GetPriorityItem( SPT_ATTACK ) );
	if( !pPowerItem3 )
		return;

	pPowerItem3->SetFlyState( this, false );
}

//Nakoruru
void ioBaseChar::CheckEnableNakoruruGauge()
{
	if( !IsNeedProcess() )
		return;

	if( !CheckGrapplingExceptionState() )
		return;

	if( m_CharState == CS_NAKORURU_FLY )
		return;

	ioNakoruruItem *pNakoruru  = ToNakoruruItem( GetPriorityItem( SPT_ATTACK ) );
	if( pNakoruru )
		pNakoruru->CheckEnableWitchGauge( this );
}

void ioBaseChar::ApplyNakoruruState( SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	float fCurWitchGauge;
	rkPacket >> fCurWitchGauge;

	ioNakoruruItem *pNakoruru = ToNakoruruItem( m_pEquipSlot->GetWeapon() );
	switch( iType )
	{
	case WITCH_ENABLE_GAUGE:
		if( pNakoruru )
		{
			pNakoruru->SetEnableWitchEffect( this );
			pNakoruru->SetCurWitchGauge( fCurWitchGauge );
		}
		break;
	}
}

//Destroyer
bool ioBaseChar::SetDestroyerFlyState( const DestroyerFlyInfo &rkFlyInfo )
{
	ioDestroyerItem *pDestroyer = ToDestroyerItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pDestroyer )
		return false;

	pDestroyer->SetDestroyerFlyState( this, rkFlyInfo, false );
	return true;
}

bool ioBaseChar::SetDestroyerFlyStateSkill( const ioHashString &szBuffName, const DestroyerFlyInfo &rkFlyInfo )
{
	ioDestroyerItem *pDestroyer = ToDestroyerItem( GetPriorityItem( SPT_ATTACK ) );
	if( pDestroyer )
		pDestroyer->SetDestroyerFlyState( this, rkFlyInfo, false );
	else
		return false;

	m_DestroyerBuffName = szBuffName;
	return true;
}

void ioBaseChar::CheckEnableDestroyerGauge()
{
	if( !IsNeedProcess() )
		return;

	if( !CheckGrapplingExceptionState() )
		return;

	if( m_CharState == CS_DESTROYER_FLY )
		return;

	ioDestroyerItem *pDestroyer = ToDestroyerItem( GetPriorityItem( SPT_ATTACK ) );
	if( pDestroyer )
		pDestroyer->CheckEnableDestroyerGauge( this );
}

bool ioBaseChar::IsEnableDestroyerFly()
{
	ioDestroyerItem *pDestroyer = ToDestroyerItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pDestroyer )
		return false;

	if( pDestroyer->IsEnableDestroyerGauge() && !IsHasCrown() )
		return true;

	return false;
}

//Michael
bool ioBaseChar::SetMichaelFlyStateByJumpDash( const MichaelFlyInfo &rkFlyInfo )
{
	ioMichaelItem *pMichael = ToMichaelItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMichael )
		return false;

	pMichael->SetMichaelFlyStateByJumpDash( this, rkFlyInfo );
	return true;
}

void ioBaseChar::CheckEnableMichaelGauge()
{
	if( !IsNeedProcess() )
		return;

	if( !CheckGrapplingExceptionState() )
		return;

	if( m_CharState == CS_MICHAEL_FLY )
		return;

	ioMichaelItem *pMichael = ToMichaelItem( GetPriorityItem( SPT_ATTACK ) );
	if( pMichael )
		pMichael->CheckEnableMichaelGauge( this );
}

bool ioBaseChar::IsEnableMichaelFly()
{
	ioMichaelItem *pMichael = ToMichaelItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMichael )
		return false;

	if( pMichael->IsEnableMichaelGauge() && !IsHasCrown() )
		return true;

	return false;
}

//Goomiho
bool ioBaseChar::SetGoomihoFlyStateByJumpDash( const GoomihoFlyInfo &rkFlyInfo )
{
	ioGoomihoItem *pGoomiho = ToGoomihoItem( GetPriorityItem( SPT_ATTACK) );
	if( !pGoomiho )
		return false;

	pGoomiho->SetGoomihoFlyStateByJumpDash( this, rkFlyInfo );
	return true;
}

void ioBaseChar::CheckEnableGoomihoGauge()
{
	if( !IsNeedProcess() )
		return;

	if( m_CharState == CS_GOOMIHO_LAND_FLY )
		return;

	ioGoomihoItem *pGoomiho = ToGoomihoItem( GetPriorityItem( SPT_ATTACK ) );
	if( pGoomiho )
		pGoomiho->CheckEnableGoomihoGauge( this );
}

bool ioBaseChar::IsEnableGoomihoFly()
{
	ioGoomihoItem *pGoomiho = ToGoomihoItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pGoomiho )
		return false;

	if( pGoomiho->IsEnableGoomihoGauge() && !IsHasCrown() )
		return true;

	return false;
}

void ioBaseChar::SetRunRotateChargeState()
{
	ioPowerChargeItem5 *pPowerItem5 = ToPowerChargeItem5( GetPriorityItem( SPT_ATTACK) );
	if( !pPowerItem5 )
		return;

	SetState( CS_ATTACK );
	SetCurNormalAttackItem( GetPriorityItem( SPT_ATTACK) );
	pPowerItem5->ChangeToDashAttackFire( this );
}

//Peterpan
bool ioBaseChar::SetPeterpanFlyState( const PeterpanFlyInfo &rkFlyInfo )
{
	ioPeterpanItem *pPeterpan = ToPeterpanItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pPeterpan )
		return false;

	pPeterpan->SetFlyState( this, rkFlyInfo, false );
	return true;
}

bool ioBaseChar::IsEnablePeterpanFly()
{
	ioPeterpanItem *pPeterpan = ToPeterpanItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pPeterpan )
		return false;

	if( pPeterpan->IsEnableGauge() && !IsHasCrown() )
		return true;

	return false;
}

//Succubus
bool ioBaseChar::SetSuccubusStateByDash( const SuccubusAttackInfo &rkInfo )
{
	ioSuccubusItem *pSuccubus =	ToSuccubusItem( GetPriorityItem( SPT_ATTACK) );
	if( !pSuccubus )
		return false;

	pSuccubus->SetSuccubusStateByDash( this, rkInfo );
	return true;
}

void ioBaseChar::CheckEnableSuccubusGauge()
{
	if( !IsNeedProcess() )
		return;

	if( m_CharState == CS_SUCCUBUS_ATTACK )
		return;

	ioSuccubusItem *pSuccubus = ToSuccubusItem( GetPriorityItem( SPT_ATTACK ) );
	if( pSuccubus )
		pSuccubus->CheckEnableGauge( this );
}

bool ioBaseChar::IsEnableSuccubusAttack()
{
	ioSuccubusItem *pSuccubus = ToSuccubusItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pSuccubus )
		return false;

	if( pSuccubus->IsEnableGauge() && !IsHasCrown() )
		return true;

	return false;
}

//Cyvogue
bool ioBaseChar::SetCyvogueModeState( const CyvogueFlyInfo &rkFlyInfo )
{
	ioCyvogueItem *pCyvogue = ToCyvogueItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pCyvogue )
		return false;

	pCyvogue->SetCyvogueState( this, rkFlyInfo, false );
	return true;
}

bool ioBaseChar::SetCyvogueModeStateSkill( const CyvogueFlyInfo &rkFlyInfo )
{
	ioCyvogueItem *pCyvogue = ToCyvogueItem( GetPriorityItem( SPT_ATTACK ) );
	if( pCyvogue )
		pCyvogue->SetCyvogueState( this, rkFlyInfo, false );
	else
		return false;

	return true;
}

void ioBaseChar::CheckEnableCyvogueGauge()
{
	if( !IsNeedProcess() )
		return;

	if( !CheckGrapplingExceptionState() )
		return;

	if( m_CharState == CS_CYVOGUE_STATE )
		return;

	ioCyvogueItem *pCyvogue = ToCyvogueItem( GetPriorityItem( SPT_ATTACK ) );
	if( pCyvogue )
		pCyvogue->CheckEnableCyvogueGauge( this );
}

bool ioBaseChar::IsEnableCyvogueModeState()
{
	ioCyvogueItem *pCyvogue = ToCyvogueItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pCyvogue )
		return false;

	if( pCyvogue->IsEnableCyvogueGauge() && !IsHasCrown() )
		return true;

	return false;
}

//Trinity
bool ioBaseChar::SetTrinityState( const TrinityInfo &rkInfo )
{
	ioTrinityItem *pTrinityItem = ToTrinityItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pTrinityItem )
		return false;

	pTrinityItem->SetTrinityState( this, rkInfo, false );
	return true;
}

bool ioBaseChar::IsEnableTrinityState()
{
	ioTrinityItem *pTrinity = ToTrinityItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pTrinity )
		return false;

	if( IsHasCrown() )
		return false;

	return true;
}

//GhostStealer
void ioBaseChar::SetGhostStealerState(const ioHashString &szAniName,
	float fAniRate,
	const D3DXVECTOR3& vTargetPos,
	float fSpeed,
	float fMoveRate,
	float fEndJumpAmt,
	float fVolumeRate)
{
	int iAniID = m_pGroup->GetAnimationIdx( szAniName );
	if( iAniID == -1 )
		return;

	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	D3DXVECTOR3 vCurDir = vTargetPos - GetMidPositionByRate();	
	D3DXVECTOR3 vMoveDir = vCurDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fMoveRange = D3DXVec3Length( &vCurDir ) * fMoveRate;

	CheckCharColSkipTime( iAniID, fTimeRate, 0 );

	SetState( CS_GHOST_STEALER_STATE );

	ioGhostStealerSpecialState *pGhostStealerSpecialState = ToGhostStealerSpecialState( m_pCharSpeicalState );
	if ( pGhostStealerSpecialState )
		pGhostStealerSpecialState->SetGhostStealerInfo( vMoveDir, fSpeed, fMoveRange, fEndJumpAmt, fVolumeRate );
}

//Mutant
void ioBaseChar::SetMutantMoveState()
{
	SetState( CS_MUTANT_SPECIAL_STATE );

	ioMutantItemSpecialState *pMutantSpecialState = ToMutantSpecialState( m_pCharSpeicalState );
	if( pMutantSpecialState )
		pMutantSpecialState->SetMutantMoveInfo( this );
}

//Dracula
bool ioBaseChar::SetDraculaFlyState( const DraculaFlyInfo &rkInfo )
{
	ioDraculaItem *pDraculaItem = ToDraculaItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pDraculaItem )
		return false;

	pDraculaItem->SetFlyState( this, rkInfo, false );
	return true;
}

bool ioBaseChar::IsEnableDraculaFly()
{
	ioDraculaItem *pDracula = ToDraculaItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pDracula )
		return false;

	if( pDracula->IsEnableGauge() && !IsHasCrown() )
		return true;

	return false;
}

bool ioBaseChar::SetNightmareFlyState()
{
	ioNightmareItem *pNightmare = ToNightmareItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pNightmare )
		return false;

	return pNightmare->SetDashFlyState( this, false );
}

bool ioBaseChar::IsEnableNightmareFly()
{
	ioNightmareItem *pNightmare = ToNightmareItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pNightmare )
		return false;

	if( pNightmare->IsEnableGauge( false ) && !IsHasCrown() )
		return true;

	return false;
}

bool ioBaseChar::IsEnableAzazelAttack()
{
	ioAzazelItem *pAzazel = ToAzazelItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pAzazel )
		return false;

	if( pAzazel->IsEnableGauge() && !IsHasCrown() )
		return true;

	return false;
}

bool ioBaseChar::IsEnableAzazelSpecialAttack()
{
	ioAzazelItem *pAzazel = ToAzazelItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pAzazel )
		return false;

	if( !pAzazel->CheckTeleportTarget( this ) )
		return true;

	return false;
}

//Hakumen
bool ioBaseChar::SetHakumenDefenseState( const HakumenDefenseInfo &rkDefenseInfo )
{
	ioHakumenItem *pHakumenItem = ToHakumenItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pHakumenItem )
		return false;

	pHakumenItem->SetDefenseState( this, rkDefenseInfo, false );
	return true;
}

bool ioBaseChar::IsEnableHakumenDefense()
{
	ioHakumenItem *pHakumen = ToHakumenItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pHakumen )
		return false;

	if( pHakumen->IsEnableGauge() && !IsHasCrown() )
		return true;

	return false;
}

//Devil
bool ioBaseChar::SetDevilFlyState( const DevilFlyInfo &rkFlyInfo )
{
	ioDevilItem *pDevilItem = ToDevilItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pDevilItem )
		return false;

	pDevilItem->SetFlyState( this, rkFlyInfo, false );
	return true;
}

bool ioBaseChar::IsEnableDevilFly()
{
	ioDevilItem *pDevilItem = ToDevilItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pDevilItem )
		return false;

	if( pDevilItem->IsEnableGauge() && !IsHasCrown() )
		return true;

	return false;
}

//Flash
bool ioBaseChar::SetFlashSpecialState( bool bDashCharge )
{
	ioFlash_Item *pFlashItem = ToFlashItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pFlashItem )
		return false;

	pFlashItem->SetFlashSpecialState( this, false );

	if ( bDashCharge )
		pFlashItem->SetDashCharge( this );

	return true;
}

bool ioBaseChar::IsEnableFlashSpecial()
{
	ioFlash_Item *pFlashItem = ToFlashItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pFlashItem )
		return false;

	if( pFlashItem->IsEnableGauge() && !IsHasCrown() )
		return true;

	return false;
}

//Evy
bool ioBaseChar::SetEvyBlinkStateByJumpDash( const EvyBlinkInfo &rkInfo )
{
	ioEvyItem *pEvyItem = ToEvyItem( GetPriorityItem( SPT_ATTACK) );
	if( !pEvyItem )
		return false;

	pEvyItem->SetEvyBlinkState( this, rkInfo, false );
	return true;
}

//SuddenAttack
bool ioBaseChar::IsSuddenAttackState()
{
	ioAttackableItem *pAttackable = ToAttackableItem(m_pEquipSlot->GetWeapon());
	if( pAttackable && ToSuddenAttackItem( pAttackable ) )
		return true;

	return false;
}

void ioBaseChar::FillSuddenAttackStateInfo( SP2Packet &rkPacket )
{
	ioSuddenAttackItem *pSuddenItem = ToSuddenAttackItem( GetWeapon() );
	if( !pSuddenItem )
		return;

	rkPacket << (int)pSuddenItem->GetMeshType();
	return;
}

void ioBaseChar::SetSuddenAttackStateInfo( SP2Packet &rkPacket )
{
	int nMeshType;
	rkPacket >> nMeshType;

	m_nSuddenMeshType = nMeshType;

	ioSuddenAttackItem *pSuddenItem = ToSuddenAttackItem( GetWeapon() );
	if( pSuddenItem )
	{
		pSuddenItem->ChangeAttackType( this, (ioSuddenAttackItem::ItemMeshType)nMeshType );
	}
}

//Slasher
bool ioBaseChar::SetSlasherSpecialState( const SlasherRunInfo& RunInfo )
{
	ioSlasher_Item *pSlasherItem = ToSlasherItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pSlasherItem )
		return false;

	pSlasherItem->Init();
	pSlasherItem->SetCurRunInfo( RunInfo, true );
	pSlasherItem->SetSpecialState( ioSlasher_Item::SSS_Find_Target );

	SetState( CS_SLASHER_SPECIAL );

	return true;
}

bool ioBaseChar::IsEnableSlasherSpecial()
{
	ioSlasher_Item *pSlasherItem = ToSlasherItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pSlasherItem )
		return false;

	if( pSlasherItem->IsEnableGauge() && !IsHasCrown() )
		return true;

	return false;
}

//TopBl
bool ioBaseChar::IsTopBlAttackState()
{
	ioAttackableItem *pAttackable = ToAttackableItem(m_pEquipSlot->GetWeapon());
	if( pAttackable && ToDummyCharItem2( pAttackable ) && 
		ToDummyCharItem2( pAttackable )->GetMeshType() == ioDummyCharItem2::IMT_EXTRA )
		return true;

	return false;
}

void ioBaseChar::SetTopBlExtendStateInfo( SP2Packet &rkPacket )
{
	ioDummyCharItem2 *pDummyCharItem2 = ToDummyCharItem2( GetWeapon() );
	if( pDummyCharItem2 )
		pDummyCharItem2->ChangeAttackType( this, (ioDummyCharItem2::IMT_EXTRA) );
}

//Titan
void ioBaseChar::SetTitanExtendMoveState( const ioHashString &szAniName, 
	float fAniRate, D3DXVECTOR3 vTargetPos, 
	float fSpeed, float fMoveRate, bool bJumpLandAttack )
{
	int iAniID = m_pGroup->GetAnimationIdx( szAniName );
	if( iAniID == -1 )
		return;
	
	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	CheckCharColSkipTime( iAniID, fTimeRate, 0 );

	D3DXVECTOR3 vCurDir = vTargetPos - GetMidPositionByRate();
	D3DXVECTOR3 vMoveDir = vCurDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fMaxRange = D3DXVec3Length( &vCurDir ) * fMoveRate;
	D3DXVECTOR3 vMoveTargetPos = GetMidPositionByRate() + ( vMoveDir * fMaxRange );

	SetState( CS_TITAN_EXTEND_MOVE );

	ioTitanExtendMoveSpecialState *pTitanExtendMoveSpecialState = ToTitanExtendMoveSpecialState( m_pCharSpeicalState );
	if ( pTitanExtendMoveSpecialState )
		pTitanExtendMoveSpecialState->SetTitanExtendMoveInfo( vMoveDir, vMoveTargetPos, fSpeed, fMaxRange, bJumpLandAttack );
}

//JeonWooChi
void ioBaseChar::SetJeonWooChiExtenDashState( D3DXVECTOR3 vMoveDir, D3DXVECTOR3 vTargetPos, float fMoveSpeed, float fMaxRange )
{
	SetState( CS_JEONWOOCHI_EXTEND_DASH );

	ioJeonWooChiExtendDashSpecialState *pJeonWooChiExtendDashState = ToJeonWooChiExtendDashSpecialState( m_pCharSpeicalState );
	if( pJeonWooChiExtendDashState )
		pJeonWooChiExtendDashState->SetExtendDashInfo( vMoveDir, vTargetPos, fMoveSpeed, fMaxRange );
}

//SuperDimension
bool ioBaseChar::IsSuperDimensionWeaponIncState()
{
	ioAttackableItem *pAttackable = ToAttackableItem(m_pEquipSlot->GetWeapon());
	if( pAttackable && ToSuperDimensionItem( pAttackable ) && ToSuperDimensionItem( pAttackable )->GetWeaponStep() >= 0 )
		return true;

	return false;
}

void ioBaseChar::FillSuperDimensionWeaponInfo( SP2Packet &rkPacket )
{
	ioSuperDimensionItem* pSuperDimensionItem = ToSuperDimensionItem( GetWeapon() );
	if( !pSuperDimensionItem )
		return;

	rkPacket << pSuperDimensionItem->GetWeaponStep();
	return;
}

void ioBaseChar::SetSuperDimensionWeaponInfo( SP2Packet &rkPacket )
{
	int nWeaponStep;
	rkPacket >> nWeaponStep;

	ioSuperDimensionItem* pSuperDimensionItem = ToSuperDimensionItem( GetWeapon() );
	if( pSuperDimensionItem )
		pSuperDimensionItem->SetWeaponStep( this, nWeaponStep );
}

//Galaxy
void ioBaseChar::FillGalaxyInfo( SP2Packet &rkPacket )
{
	ioGalaxyItem* pGalaxy = ToGalaxyItem( GetWeapon() );
	if( pGalaxy )
		pGalaxy->FillGalaxyInfo( this, rkPacket );
}

void ioBaseChar::SetGalaxyInfo( SP2Packet &rkPacket )
{
	ioGalaxyItem* pGalaxy = ToGalaxyItem( GetWeapon() );
	if( pGalaxy )	
		pGalaxy->SetGalaxyInfo( this, rkPacket );
}

//썬더버드
void ioBaseChar::SetThunderBirdObjectSpecialState( bool bInit )
{
	ioThunderBirdObjectItem *pThunderBirdObjectItem = ToThunderBirdObjectItem( GetObject() );
	if ( !pThunderBirdObjectItem )
		return;

	SetState( CS_THUNDER_BIRD_OBJECT_SPECIAL );

	if ( bInit )
		pThunderBirdObjectItem->Init();
}

bool ioBaseChar::IsEnableThunderBirdObjectSpecialState()
{
	ioThunderBirdObjectItem *pThunderBirdObjectItem = ToThunderBirdObjectItem( GetObject() );
	if( !pThunderBirdObjectItem )
		return false;

	if( pThunderBirdObjectItem->CheckThunderBirdGauge( this ) )
		return false;

	return true;
}

void ioBaseChar::CheckThunderBirdSpecialBuff( bool bTeamAttacked )
{
	for( int i=0; i<m_aBuffArray->MaxCount(); i++ )
	{
		ioThunderBirdHoldBuff *pThunderBirdHoldBuff = ToThunderBirdHoldBuff( m_aBuffArray->GetAt(i) );
		if ( pThunderBirdHoldBuff )
			pThunderBirdHoldBuff->SetReserveEndBuff();

		if ( bTeamAttacked )
		{			
			if ( pThunderBirdHoldBuff )
				pThunderBirdHoldBuff->SetTeamAttack( true, IsFloating() );
		}
		else
		{
			ioCheckStateEndCallBuff *pCheckStateEndCallBuff = ToCheckStateEndCallBuff( m_aBuffArray->GetAt(i) );
			if ( pCheckStateEndCallBuff && pCheckStateEndCallBuff->IsWait() )
				pCheckStateEndCallBuff->SetCallState( this );
		}
	}
}

void ioBaseChar::FillThunderBirdInfo( SP2Packet &rkPacket )
{
	ioThunderBirdItem* pThunderBirdItem = ToThunderBirdItem( GetWeapon() );
	if( pThunderBirdItem )
		pThunderBirdItem->FillThunderBirdInfo( this, rkPacket );
}

void ioBaseChar::SetThunderBirdInfo( SP2Packet &rkPacket )
{
	ioThunderBirdItem* pThunderBirdItem = ToThunderBirdItem( GetWeapon() );
	if( pThunderBirdItem )	
		pThunderBirdItem->SetThunderBirdInfo( this, rkPacket );
}

//Alice
void ioBaseChar::FillAliceInfo( SP2Packet &rkPacket )
{
	ioAliceItem* pAlice = ToAliceItem( GetWeapon() );
	if( pAlice )
		pAlice->FillAliceInfo( this, rkPacket );
}

void ioBaseChar::SetAliceInfo( SP2Packet &rkPacket )
{
	ioAliceItem* pAlice = ToAliceItem( GetWeapon() );
	if( pAlice )	
		pAlice->SetAliceInfo( this, rkPacket );
}

//Valkyrie
void ioBaseChar::ClearValkyrieDobuleJumpFly()
{
	ioValkyrie *pValkyrie = ToValkyrie( GetPriorityItem( SPT_ATTACK ) );
	if( pValkyrie )
	{		
		ioBuff* pBuff = GetBuff( pValkyrie->GetDoubleJumpFlyBuff() );
		if( pBuff )
		{
			pBuff->SetReserveEndBuff();
		}
	}

	ioSummonChargeItem3 *pSummonChargeItem3 = ToSummonChargeItem3(GetPriorityItem( SPT_ATTACK ) );
	if( pSummonChargeItem3 && !pSummonChargeItem3->GetDoubleJumpFlyBuff().IsEmpty() )
	{		
		ioBuff* pBuff = GetBuff( pSummonChargeItem3->GetDoubleJumpFlyBuff() );
		if( pBuff )
		{
			pBuff->SetReserveEndBuff();
		}
	}
}

//박일표
void ioBaseChar::SetParkilpyoEquipObject( int nState )
{
	ioParkilpyoItem *pParkilpyoItem = ToParkilpyoItem( GetWeapon() );
	if( !pParkilpyoItem )
		return;

	pParkilpyoItem->ChangeEquipObjectState( this, (ioParkilpyoItem::EquipObjectState)nState );
}

bool ioBaseChar::IsEnableParkilpyoEquipObject()
{
	ioParkilpyoItem *pParkilpyoItem = ToParkilpyoItem( GetWeapon() );
	if( !pParkilpyoItem )
		return false;

	return pParkilpyoItem->IsEnableEquipObject();
}

void ioBaseChar::FillParkilpyoInfo( SP2Packet &rkPacket )
{
	ioParkilpyoItem* pParkilpyoItem = ToParkilpyoItem( GetWeapon() );
	if( pParkilpyoItem )
		pParkilpyoItem->FillParkilpyoInfo( this, rkPacket );
}

void ioBaseChar::SetParkilpyoInfo( SP2Packet &rkPacket )
{
	ioParkilpyoItem* pParkilpyoItem = ToParkilpyoItem( GetWeapon() );
	if( pParkilpyoItem )
		pParkilpyoItem->SetParkilpyoInfo( this, rkPacket );
}

//ShadowRanger
bool ioBaseChar::SetShadowRangerSpecialMoveState( float fUseGauge )
{
	ioShadowRangerItem *pShadowRangerItem = ToShadowRangerItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pShadowRangerItem )
		return false;

	SetState( CS_SHADOW_RANGER_SPECIAL );

	pShadowRangerItem->ClearState( this );
	pShadowRangerItem->SetSpecialState( ioShadowRangerItem::SS_Teleport_Move_Predelay );
	pShadowRangerItem->DecreaseGauge( fUseGauge );

	return true;
}

bool ioBaseChar::IsEnableShadowRangerSpecial( float fUseGauge )
{
	ioShadowRangerItem *pShadowRangerItem = ToShadowRangerItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pShadowRangerItem )
		return false;

	if( pShadowRangerItem->IsEnableGauge( fUseGauge ) && !IsHasCrown() )
		return true;

	return false;
}

//닌자(진화)
bool ioBaseChar::SetNinjaExpansionMove( bool bJump )
{
	ioNinjaExpansionItem *pNinjaExpansion = ToNinjaExpansionItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pNinjaExpansion )
		return false;

	SetState( CS_NINJA_EXPANSION_MOVE );

	pNinjaExpansion->SetNinjaExpansionMove( this, bJump );
	return true;
}

//Nata
bool ioBaseChar::SetNataSpecialComboState( bool bNormal, bool bDash, bool bJump )
{
	ioNataItem *pNataItem = ToNataItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pNataItem )
		return false;
	
	pNataItem->SetNataComboSpecialState( this, bNormal, bDash, bJump );

	return true;
}

bool ioBaseChar::IsEnableNataSpecialCombo( bool bNormal, bool bDash, bool bJump )
{
	ioNataItem *pNataItem = ToNataItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pNataItem )
		return false;

	if( pNataItem->IsEnableComboGauge( bNormal, bDash, bJump ) && !IsHasCrown() )
		return true;

	return false;
}

bool ioBaseChar::IsEnableNataSpecialFly()
{
	ioNataItem *pNataItem = ToNataItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pNataItem )
		return false;

	if( pNataItem->IsEnableFlyGauge() && !IsHasCrown() )
		return true;

	return false;
}

//귀무사
void ioBaseChar::SetOnimushaEquipObject( int nState )
{
	ioOniMushaItem *pOniMushaItem = ToOniMushaItem( GetWeapon() );
	if( !pOniMushaItem )
		return;

	pOniMushaItem->ChangeEquipObjectState( this, (ioOniMushaItem::EquipObjectState)nState );
}

bool ioBaseChar::IsEnableOnimushaEquipObject()
{
	ioOniMushaItem *pOniMushaItem = ToOniMushaItem( GetWeapon() );
	if( !pOniMushaItem )
		return false;

	return pOniMushaItem->IsEnableEquipObject();
}

void ioBaseChar::FillOnimushaInfo( SP2Packet &rkPacket )
{
	ioOniMushaItem *pOniMushaItem = ToOniMushaItem( GetWeapon() );
	if( pOniMushaItem )
		pOniMushaItem->FillOnimushaInfo( this, rkPacket );
}

void ioBaseChar::SetOnimushaInfo( SP2Packet &rkPacket )
{
	ioOniMushaItem *pOniMushaItem = ToOniMushaItem( GetWeapon() );
	if( pOniMushaItem )
		pOniMushaItem->SetOnimushaInfo( this, rkPacket );
}

//Druid
void ioBaseChar::FillDruidInfo( SP2Packet &rkPacket )
{
	ioDruidItem* pDruid = ToDruidItem( GetWeapon() );
	if( pDruid )
		pDruid->FillDruidInfo( this, rkPacket );
}

void ioBaseChar::SetDruidInfo( SP2Packet &rkPacket )
{
	ioDruidItem* pDruid = ToDruidItem( GetWeapon() );
	if( pDruid )	
		pDruid->SetDruidInfo( this, rkPacket );
}

bool ioBaseChar::SetMidnightStateByDash( const MidnightAttackInfo& rkInfo )
{
	ioMidnightItem *pMidnight =	ToMidnightItem( GetPriorityItem( SPT_ATTACK) );
	if( !pMidnight )
		return false;

	pMidnight->ChangeToDashChargeAttack( this, rkInfo );
	return true;
}

bool ioBaseChar::IsEnableMidnightAttack()
{
	ioMidnightItem *pMidnight = ToMidnightItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMidnight )
		return false;

	if( !IsHasCrown() )
		return true;

	return false;
}

void ioBaseChar::FillMidnightScytheInfo( SP2Packet& rkPacket )
{
	ioMidnightScytheObjectItem* pMidnightObject = ToObjectMidnightScytheItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMidnightObject )
		return;

	pMidnightObject->FillMidnightScytheInfo( this, rkPacket );
}

void ioBaseChar::SetMidnightScytheInfo( SP2Packet& rkPacket )
{
	ioMidnightScytheObjectItem* pMidnightObject = ToObjectMidnightScytheItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMidnightObject )
		return;

	pMidnightObject->SetMidngihtScytheInfo( this, rkPacket );
}

void ioBaseChar::FillMidnightGunInfo( SP2Packet& rkPacket )
{
	ioMidnightGunObjectItem* pMidnightObject = ToObjectMidnightGunItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMidnightObject )
		return;

	pMidnightObject->FillMidnightScytheInfo( this, rkPacket );
}

void ioBaseChar::SetMidnightGunInfo( SP2Packet& rkPacket )
{
	ioMidnightGunObjectItem* pMidnightObject = ToObjectMidnightGunItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMidnightObject )
		return;

	pMidnightObject->SetMidngihtScytheInfo( this, rkPacket );
}

// 이브 관전 OR 입장 2020-11-24 
void ioBaseChar::FillEvaSubWeaponInfo( SP2Packet& rkPacket )
{
	ioEvaItem* pEvaItem = ToEvaItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pEvaItem )
		return;

	pEvaItem->FillEvaSubWeaponInfo( this, rkPacket );
}

void ioBaseChar::SetEvaSubWeaponInfoList( SP2Packet& rkPacket )
{
	ioEvaItem* pEvaItem = ToEvaItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pEvaItem )
		return;

	pEvaItem->SetEvaSubWeaponInfoList( this, rkPacket );
}

// 마그네틱 관전 OR 입장 2020-12-16
void ioBaseChar::FillMagneticWeaponInfo( SP2Packet& rkPacket )
{
	ioMagneticItem* pMagneticItem = ToMagneticItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMagneticItem )
		return;

	pMagneticItem->FillMagneticWeaponInfo( this, rkPacket );
}

void ioBaseChar::SetMagneticWeaponInfoList( SP2Packet& rkPacket )
{
	ioMagneticItem* pMagneticItem = ToMagneticItem( GetPriorityItem( SPT_ATTACK ) );
	if( !pMagneticItem )
		return;

	pMagneticItem->SetMagneticWeaponInfoList( this, rkPacket );
}