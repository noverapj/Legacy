#include "stdafx.h"

#include "ioSmartBoomObjectItem.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"
#include "BuffDefine.h"

ioSmartBoomObjectItem::ioSmartBoomObjectItem()
{
	m_bDropBomb = false;
	m_WeaponIndexList.clear();
	m_dwMotionEndTime = 0;
	m_dwMotionFireTime = 0;
	m_dwEquipTime = 0;
	m_bGenerateWeaponDirect = false;
	m_bWounded = false;
}

ioSmartBoomObjectItem::ioSmartBoomObjectItem( const ioSmartBoomObjectItem &rhs )
: ioObjectItem( rhs ),
m_EquipedEffect( rhs.m_EquipedEffect ),
m_dwMutipleUseCnt( rhs.m_dwMutipleUseCnt ),
m_fAngleHList( rhs.m_fAngleHList ),
m_bDisableDefense( rhs.m_bDisableDefense ),
m_fJumpAttackEndNextJumpAmt( rhs.m_fJumpAttackEndNextJumpAmt ),
m_bCheckWeaponOnRelease( rhs.m_bCheckWeaponOnRelease ),
m_GenerateAnimation( rhs.m_GenerateAnimation ),
m_fGenerateAniRate( rhs.m_fGenerateAniRate ),
m_dwProtectTime( rhs.m_dwProtectTime ),
m_iReduceGaugeSlot( rhs.m_iReduceGaugeSlot ),
m_bDirectAttackOnKey( rhs.m_bDirectAttackOnKey ),
m_bUseTimeOver( rhs.m_bUseTimeOver ),
m_dwMaxUseTime( rhs.m_dwMaxUseTime ),
m_bDecSkillGaugeAllTime( rhs.m_bDecSkillGaugeAllTime ),
m_bAllDeadCreatedWeapon( rhs.m_bAllDeadCreatedWeapon )
{
	m_bDropBomb = false;
	m_dwMutipleCurUseCnt = 1;
	m_WeaponIndexList.clear();
	m_dwMotionEndTime = 0;
	m_dwMotionFireTime = 0;
	m_dwEquipTime = 0;
	m_bGenerateWeaponDirect = false;
	m_bWounded = false;
}

ioSmartBoomObjectItem::~ioSmartBoomObjectItem()
{
}

void ioSmartBoomObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "equiped_effect", "", szBuf, MAX_PATH );
	m_EquipedEffect = szBuf;

	m_dwMutipleUseCnt = (DWORD)rkLoader.LoadInt_e( "item_max_cnt", 0 );

	LoadFireDirectionAppointList( rkLoader );

	m_bDisableDefense = rkLoader.LoadBool_e( "disable_defense", false );
	m_bCheckWeaponOnRelease = rkLoader.LoadBool_e( "check_weapon_on_release", false );
	m_fJumpAttackEndNextJumpAmt = rkLoader.LoadFloat_e( "jump_attack_end_next_jump_amt", 0.0f );

	rkLoader.LoadString_e( "generate_animation", "", szBuf, MAX_PATH );
	m_GenerateAnimation = szBuf;
	m_fGenerateAniRate = rkLoader.LoadFloat_e( "generate_ani_rate", FLOAT1 );

	m_dwProtectTime = (DWORD)rkLoader.LoadInt_e( "protect_time", 0 );

	m_iReduceGaugeSlot = rkLoader.LoadInt_e( "reduce_gauge_slot", 0 );

	m_bDirectAttackOnKey = rkLoader.LoadBool_e( "direct_attack_on_defense_key", false );

	//////////////////////////////////////////////////////////////////////////
	m_bUseTimeOver = rkLoader.LoadBool_e( "use_time_over", false );
	m_dwMaxUseTime = rkLoader.LoadInt_e( "use_time_over_max_time", 0 );

	m_bDecSkillGaugeAllTime = rkLoader.LoadBool_e( "dec_skill_gauge_all_time", false );

	m_bAllDeadCreatedWeapon = rkLoader.LoadBool_e( "all_dead_created_weapon", false );
}

void ioSmartBoomObjectItem::LoadFireDirectionAppointList( ioINILoader &rkLoader )
{	

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	if( m_dwMutipleUseCnt <= 0 ) return;

	m_fAngleHList.clear();
	m_fAngleHList.reserve( m_dwMutipleUseCnt );

	for( DWORD i= 0; i < m_dwMutipleUseCnt; ++i )
	{
		wsprintf_e( szBuf, "item_drop_angle%d_v", i+1 );
		float fAngle = rkLoader.LoadFloat( szBuf, 0.0f );

		m_fAngleHList.push_back( fAngle );
	}	
}

ioItem* ioSmartBoomObjectItem::Clone()
{		
	return new ioSmartBoomObjectItem( *this );
}

void ioSmartBoomObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	if( !pWeapon ) return;

	AddWeaponIndex( pWeapon->GetWeaponIndex() );

	ioObjectItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
	ioThrowBomb2Weapon *pBomb2 = ToThrowBomb2Weapon( pWeapon );
	ioThrowBombWeapon *pBomb = ToThrowBombWeapon( pWeapon );

	if( m_bDropBomb )
	{
		if( pBomb )
		{
			//pBomb->SetBombDrop();
			pBomb->SetMoveDir( m_vDropMoveDir );
			pBomb->SetReserveExplosion( g_WeaponMgr.GetCreator() );
		}
	}
	else if( bJumpAttack )
	{
		if( pBomb )
		{
			pBomb->SetJumpThrow();
		}
		else if( pBomb2 )
		{
			pBomb2->SetJumpThrow();
		}
	}

	ioBaseChar *pOwner = pWeapon->GetOwner();
	if( pOwner )
	{
		if( pBomb2 && m_bGenerateWeaponDirect )
		{
			pBomb2->SetPosition( pOwner->GetMidPositionByRate() );
			pBomb2->SetExplosionState( g_WeaponMgr.GetCreator() );
		}
		else
		{
			D3DXVECTOR3	vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize(&vMoveDir,&vMoveDir);

			D3DXVECTOR3 vPos = pWeapon->GetPosition() + (vMoveDir * m_fOffSet);
			vPos.y += m_fHeightOffSet;

			pWeapon->SetPosition( vPos );
		}
	}
}

float ioSmartBoomObjectItem::GetAngleH( DWORD dwIndex )
{	
	if( COMPARE( dwIndex, 0, m_fAngleHList.size() ) )
		return m_fAngleHList[dwIndex];
	else
		return ioMath::FLOAT_INFINITY;
}

ioObjectItem::ObjectSubType ioSmartBoomObjectItem::GetObjectSubType() const
{
	return OST_SMARTBOOM;
}

bool ioSmartBoomObjectItem::IsOneTimeWeaponItem() const
{	
	if( m_bDirectAttackOnKey && !m_bGenerateWeaponDirect )
		return true;

	return false;
}

bool ioSmartBoomObjectItem::isMutipleUseWaeponItem() const
{		
	if( m_dwMutipleCurUseCnt < m_dwMutipleUseCnt  )
		return true;
	else
		return false;
}

void ioSmartBoomObjectItem::SetOwnerItemEquipSlot( EquipSlot iSlot )
{
	m_OwnerItemEquipSlot = iSlot;
}

bool ioSmartBoomObjectItem::IsEnableDefense() const
{
	if( m_bDisableDefense )
		return false;

	return true;
}

bool ioSmartBoomObjectItem::IsProtectState()
{
	if( m_dwProtectTime > 0 && m_dwEquipTime + m_dwProtectTime > FRAMEGETTIME() )
		return true;

	return false;
}

void ioSmartBoomObjectItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ReduceSkillGauge( pOwner );
	ioObjectItem::SetNormalAttack(pOwner, iCurCombo);
}

bool ioSmartBoomObjectItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	ReduceSkillGauge( pOwner );
	return ioObjectItem::SetJumpAttack( pOwner, iAniID, fTimeRate, eType );
}

void ioSmartBoomObjectItem::IncreaseUseObjectCnt()
{
	m_dwMutipleCurUseCnt++;
}

void ioSmartBoomObjectItem::ChangeDropState( const D3DXVECTOR3 &vDropDir )
{
	m_vDropMoveDir = vDropDir;
	m_bDropBomb = true;
}

void ioSmartBoomObjectItem::OnEquiped( ioBaseChar *pOwner )
{
	m_dwTicCheckTime = FRAMEGETTIME();
	m_dwMutipleCurUseCnt = 1;
	m_WeaponIndexList.clear();
	m_dwMotionEndTime = 0;
	m_dwMotionFireTime = 0;
	m_bGenerateWeaponDirect = false;
	m_bWounded = false;

	if( pOwner )
	{
		if( pOwner->IsMale() )
		{
			if( !m_ItemEquipEffect.IsEmpty() )
			{
				pOwner->AttachEffect( m_ItemEquipEffect );
				m_CurItemEquipEffect = m_ItemEquipEffect;
			}
		}
		else
		{
			if( !m_ItemEquipEffectHw.IsEmpty() )
			{
				pOwner->AttachEffect( m_ItemEquipEffectHw );
				m_CurItemEquipEffect = m_ItemEquipEffectHw;
			}
			else if( !m_ItemEquipEffect.IsEmpty() )
			{
				pOwner->AttachEffect( m_ItemEquipEffect );
				m_CurItemEquipEffect = m_ItemEquipEffect;
			}
		}
	}

	// Check changeskill
	CheckChangeSkillByBuff( pOwner );
	CheckChangeSkillByHP( pOwner );
	CheckChangeSkillByGauge( pOwner );

	int iEquipBuffCnt = m_EquipBuffList.size();
	for( int i=0; i < iEquipBuffCnt; ++i )
	{
		ioHashString szBuffName = m_EquipBuffList[i];
		ioBuff *pBuff = pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );

		ioActiveCountBuff* pActive = ToActiveCountBuff( pBuff );
		if( pActive )
		{	
			pActive->SetActiveCount( m_dwMutipleUseCnt );
		}
	}
	
	if( !m_EquipedEffect.IsEmpty() )
	{
		pOwner->AttachEffect( m_EquipedEffect );
	}

	m_dwEquipTime = FRAMEGETTIME();
}

void ioSmartBoomObjectItem::OnReleased( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioObjectItem::OnReleased( pOwner );

	int iEquipBuffCnt = m_EquipBuffList.size();
	for( int i=0; i < iEquipBuffCnt; ++i )
	{
		ioHashString szBuffName = m_EquipBuffList[i];
		pOwner->RemoveBuff( szBuffName );
	}

	int iCnt = m_WeaponIndexList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_WeaponIndexList[i] );
		ioThrowBombWeapon *pBomb = ToThrowBombWeapon( pWeapon );
		ioMineWeapon5 *pMineWeapon5 = ToMineWeapon5( pWeapon );
		if( pBomb )
		{
			pBomb->SetReserveExplosion( pStage );
		}
		else if( pMineWeapon5 )
		{
			pMineWeapon5->SetWaitExplosionMine( pStage, false );
		}
		else if ( m_bAllDeadCreatedWeapon && pWeapon )
			pWeapon->SetWeaponDead();
	}
	m_WeaponIndexList.clear();
	m_dwMotionEndTime = 0;
	m_dwMotionFireTime = 0;
	m_bGenerateWeaponDirect = false;
	m_bWounded = false;

	if( !m_EquipedEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_EquipedEffect );
	}
}

void ioSmartBoomObjectItem::OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		if( m_bDirectAttackOnKey )
		{
			CharState eState = pOwner->GetState();
			if( pOwner->IsDefenseKey() && !m_bGenerateWeaponDirect && ( eState == CS_DELAY || eState == CS_RUN || eState == CS_DASH ) )
			{
				ReduceSkillGauge( pOwner );
				SetGenerateWeaponDirect( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SMART_BOOM_GENERATE_WEAPON );
					kPacket << pOwner->GetCharName();
					kPacket << GENERATE_WEAPON_DIRECT;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		else
		{
			if( pOwner->IsDefenseKey() )
			{
				if( !m_WeaponIndexList.empty() )
				{
					GenerateAllDropWeapon( pOwner );

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_SMART_BOOM_GENERATE_WEAPON );
						kPacket << pOwner->GetCharName();
						kPacket << GENERATE_ALL_DROP_WEAPON;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
		}
	}

	if( m_dwMotionFireTime > 0 && m_dwMotionFireTime < FRAMEGETTIME() )
	{
		m_dwMotionFireTime = 0;
		ReduceSkillGauge( pOwner );

		const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_NORMAL );
		if( pAttr )
		{
			pOwner->SetNormalAttackByAttribute( *pAttr, true );
			pOwner->ApplyNormalAttackResult( 0 );
			pOwner->SetState( CS_ATTACK );

			pOwner->SetCurNormalAttackItem( NULL );
		}
	}

	if( m_bDecSkillGaugeAllTime )
		ReduceSkillGauge( pOwner );
}

bool ioSmartBoomObjectItem::EnableReleaseItem( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	int iCnt = m_WeaponIndexList.size();
	if( m_bCheckWeaponOnRelease )
	{
		bool bAllWeaponDead = true;
		for( int i=0; i<iCnt; ++i )
		{
			ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_WeaponIndexList[i] );
			if( pWeapon && pWeapon->IsLive() )
			{
				bAllWeaponDead = false;
			}
		}
		return bAllWeaponDead;
	}

	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

void ioSmartBoomObjectItem::GenerateAllDropWeapon( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	int iCnt = m_WeaponIndexList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_WeaponIndexList[i] );
		ioThrowBombWeapon *pBomb = ToThrowBombWeapon( pWeapon );
		ioThrowBomb2Weapon *pBomb2 = ToThrowBomb2Weapon( pWeapon );
		ioMineWeapon5 *pMineWeapon = ToMineWeapon5( pWeapon );
		if( pBomb )
		{
			pBomb->SetReserveExplosion( pStage );
		}
		else if( pBomb2 )
		{
			pBomb2->SetReserveExplosion( pStage );
		}
		else if( pMineWeapon )
		{
			pMineWeapon->SetWaitExplosionMine( pStage, false );
		}
		else if ( m_bAllDeadCreatedWeapon && pWeapon )
			pWeapon->SetWeaponDead();
	}
	m_WeaponIndexList.clear();
}

void ioSmartBoomObjectItem::ApplySmartBoomObjectItem( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage )
		return;

	ioHashString szOwnerName = GetOwnerName();
	ioBaseChar *pOwner = pStage->GetBaseChar( szOwnerName );
	if( !pOwner )
		return;

	int iType;
	rkPacket >> iType;
	switch( iType )
	{
	case GENERATE_ALL_DROP_WEAPON:
		GenerateAllDropWeapon( pOwner );
		break;
	case GENERATE_WEAPON_DIRECT:
		SetGenerateWeaponDirect( pOwner );
		break;
	}
}

void ioSmartBoomObjectItem::SetGenerateWeaponDirect( ioBaseChar *pOwner )
{
	if( m_GenerateAnimation.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	int iAniID = pGrp->GetAnimationIdx( m_GenerateAnimation );
	if( iAniID == -1 )
		return;

	pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/m_fGenerateAniRate, 0.0f, 0 );
	m_dwMotionEndTime = m_dwMotionFireTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * m_fGenerateAniRate;
	m_dwMotionFireTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_fGenerateAniRate;

	m_bGenerateWeaponDirect = true;
}

void ioSmartBoomObjectItem::ReduceSkillGauge( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !COMPARE( m_iReduceGaugeSlot, ES_WEAPON, ES_CLOAK+1 ) )
		return;

	ioItem *pItem = pOwner->GetEquipedItem( m_iReduceGaugeSlot );
	if( pItem && pItem->HasSkillGauge() )
	{
		pItem->ReduceNeedGauge( pItem->GetMaxSkillGauge( pOwner->GetCharName() ), pOwner );
	}
}

bool ioSmartBoomObjectItem::IsTimeOver()
{
	if( !m_bUseTimeOver )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime > (m_dwEquipTime + m_dwMaxUseTime) )
		return true;

	return false;
}

bool ioSmartBoomObjectItem::CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	bool bRet = ioObjectItem::CheckReleaseProcess( pStage, pOwner );
	if( bRet )
		return true;
		
	if( ( GetCurUseWeaponCount() > GetMaxUseWeaponCount() ) && EnableReleaseItem( pOwner ) )
	{
		return true;
	}
	else if( IsDirectAttackOnKey() && EnableReleaseItem( pOwner ) )
	{
		return true;
	}
	else if( IsWounded() )
	{
		return true;
	}
	else if( IsTimeOver() )
	{
		return true;
	}

	return false;
}

bool ioSmartBoomObjectItem::IsNormalAttackStop()
{
	if( GetCurUseWeaponCount() > GetMaxUseWeaponCount() )
		return true;

	return false;
}

void ioSmartBoomObjectItem::AddWeaponIndex( DWORD dwIndex )
{
	m_WeaponIndexList.push_back( dwIndex );
}
