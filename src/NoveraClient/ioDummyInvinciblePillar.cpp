#include "StdAfx.h"
#include "ioDummyInvinciblePillar.h"
#include "BuffDefine.h"
#include "WeaponDefine.h"
#include "ioPowerChargeShieldItem.h"

ioDummyInvinciblePillar::ioDummyInvinciblePillar( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_DummyStructState = DSS_START;
	m_bTrigerWeapon = false;
	m_dwHealStartTime = 0;
}

ioDummyInvinciblePillar::~ioDummyInvinciblePillar()
{
	m_OwnerBuffList.clear();
	m_RemoveBuffList.clear();
}

void ioDummyInvinciblePillar::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "", szKey[MAX_PATH];

	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_szCheckBuff = szBuf;

	m_bSetAllWeaponDefence = rkLoader.LoadBool_e( "set_all_weapon_defence", false );
	m_bCheckRange = rkLoader.LoadBool_e( "set_check_range", false );
	m_fRange = rkLoader.LoadFloat_e( "check_range_dist", 0.0f );
	m_bOnlyOwnerAddBuff = rkLoader.LoadBool_e( "set_owner_add_buff", false );

	LoadOwnerBuffList( rkLoader );
	LoadRemoveBuffList( rkLoader );

	int iSize = rkLoader.LoadInt_e( "area_weapon_triger_cnt", 0 );
	for( int i=0; i<iSize; ++i )
	{
		wsprintf_e( szKey, "area_weapon_triger%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioHashString szName = szBuf;

		m_AreaWeaponTrigerNameVec.push_back( szName );
	}

	iSize = rkLoader.LoadInt_e( "triger_attach_weapon_type_count", 0 );
	for( int i=0; i<iSize; ++i )
	{
		WeaponInfo kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "triger_attach_weapon_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "triger_attach_weapon_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "triger_attach_weapon_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "triger_attach_weapon_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "triger_attach_weapon_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_TrigerAttachWeaponInfoList.push_back( kInfo );
	}
	iSize = rkLoader.LoadInt_e( "triger_area_weapon_type_count", 0 );
	for( int i=0; i<iSize; ++i )
	{
		wsprintf_e( szKey, "triger_area_weapon_type%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioHashString szName = szBuf;

		m_TrigerAreaWeaponVec.push_back( szName );
	}

	iSize = rkLoader.LoadInt_e( "heal_triger_area_weapon_cnt", 0 );
	for( int i=0; i<iSize; ++i )
	{
		wsprintf_e( szKey, "heal_triger_area_weapon%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioHashString szName = szBuf;

		m_HealAreaWeaponNameVec.push_back( szName );
	}
	m_dwHealTime = rkLoader.LoadInt_e( "heal_time", 0 );
	m_dwHealTicTime = rkLoader.LoadInt_e( "heal_tic_time", 0 );
	m_fHealHp = rkLoader.LoadFloat_e( "heal_increase_hp", 0.0f );

	rkLoader.LoadString_e( "attacker_buff", "", szBuf, MAX_PATH );
	m_szAttackerBuff	= szBuf;
}

void ioDummyInvinciblePillar::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DummyStructState )
	{
	case DSS_START:
		{
			if( !m_DummyEffect.IsEmpty() )
			{
				m_pGroup->AttachEffect( m_DummyEffect, NULL );
				m_bMoveEffectCreated = true;
			}

			SetAttachWeapon();
			SetOwenrBuff();

			m_DummyStructState = DSS_LOOP;
		}
		break;
	case DSS_LOOP:
		{
			CheckBuff();
			CheckRange();
		}
		break;
	}
	if( m_dwStartAniEndTime != 0 && m_dwStartAniEndTime < dwCurTime )
	{
		if( !m_DelayAnimation.IsEmpty() )
		{
			int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
			if( iAniID != -1 )
			{
				float fAniRate = FLOAT1;
				if( m_fDelayAniRate > 0.0f )
					fAniRate = m_fDelayAniRate;

				m_pGroup->ClearAllActionAni( FLOAT10, true );
				m_pGroup->ClearAllLoopAni( FLOAT10, true );

				m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
			}
		}
		m_dwStartAniEndTime = 0;
		return;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );
}

ApplyWeaponExp ioDummyInvinciblePillar::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyChar::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	m_LastAttacker = pWeapon->GetOwnerName();

	float fDamage = pWeapon->GetDamage( this ) * m_fDamageRate;
	float fPushPower = pWeapon->GetDefensePushPower( fDamage );

	if( !m_bNoDamage && !m_bPiercingProtected )
		m_HP.m_fCurValue -= fDamage;
	AddHeadDamageInfo( (int)fDamage );

	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 )
		bContactType = true;

	bool bStructWeapon = false;
	if( pWeapon->CheckWeaponByStruct() || pWeapon->CheckWeaponByDummyChar() )
		bStructWeapon = true;

	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);

	if( bContactType && !bStructWeapon && !pWeapon->CheckWeaponBySkill() && !m_BlockBuff.IsEmpty() )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pWeapon->IsChangeJumpping( false, false )
			&& !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );
		}
	}

	if( !m_BlockBuff.IsEmpty() )
		pAttacker->SetDisableAttackEmoticon();

	int iShakeCamera = pWeapon->GetShakeCamera();
	ActionStopType eActionStop = pWeapon->GetActionStopType();

	if( eActionStop == AST_BOTH || eActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
		pWeapon->SetActionStopDuration( ioBaseChar::CalcActionStopDelay( fDamage, false ),
			ioBaseChar::GetActionStopLooseRate( false ) );
	}

	if( m_HP.m_fCurValue <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		SetDieState( true, true );

		// 여기서 스킬 캔슬 처리 필요
		switch( m_DummyCharType )
		{
		case DCT_OWNER_CONTROL:
		case DCT_OWNER_CONTROL3:
		case DCT_OWNER_ATTACH:
			if( !m_ControlSkillName.IsEmpty() )
			{
				if( GetOwner() && GetOwner()->IsCurUsingSkill( m_ControlSkillName ) )
				{
					GetOwner()->ClearAttackFireTimeAndSkill( CAFT_APPLY_WOUNDED_BY_DUMMY );
					m_ControlSkillName.Clear();
				}
			}
			break;
		}
	}

	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	if( !m_szAttackerBuff.IsEmpty() )
		pAttacker->AddNewBuff( m_szAttackerBuff, "", "", NULL );

	return AWE_EXPLOSION;
}

void ioDummyInvinciblePillar::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) 
		return;	

	int iType;
	rkPacket >> iType;	
	ioDummyChar::ApplyDummyCharSyncByType( iType, rkPacket, pStage );
}

void ioDummyInvinciblePillar::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
}

int ioDummyInvinciblePillar::DontMoveEntityLevel() const
{
	return 2;
}

//더미 죽는 상태 설정
void ioDummyInvinciblePillar::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ProcessEnd();
	DestoryTrigerAttachWeapon();
	DestoryTrigerAreaWeapon();

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioDummyInvinciblePillar::IsActivateWeapon()
{
	return true;
}

void ioDummyInvinciblePillar::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_OwnerBuffList.push_back( szBuf );
	}
}

void ioDummyInvinciblePillar::LoadRemoveBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	m_RemoveBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_RemoveBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "remove_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_RemoveBuffList.push_back( szBuf );
	}
}

void ioDummyInvinciblePillar::CheckBuff()
{
	if( !m_pModeParent || m_szCheckBuff.IsEmpty() )
		return;

	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage )
		return;

	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if ( pOwner )
	{
		if ( !pOwner->HasBuff( m_szCheckBuff ) )
			SetDieState( true );
	}
}

bool ioDummyInvinciblePillar::IsAllWeaponDefence() const
{
	return m_bSetAllWeaponDefence;
}

void ioDummyInvinciblePillar::ProcessEnd()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	int nBuffCnt = (int)m_RemoveBuffList.size();	
	for ( int i=0; i<nBuffCnt; ++i )
		pOwner->RemoveBuff( m_RemoveBuffList[i], true );
}

bool ioDummyInvinciblePillar::IsDummyVsDummyCollisionSkipState( const ioDummyChar *pDummy )
{
	if( pDummy )
		return true;

	return ioDummyChar::IsDummyVsDummyCollisionSkipState( pDummy );
}

void ioDummyInvinciblePillar::SetOwenrBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	if ( m_bOnlyOwnerAddBuff && !pOwner->IsNeedProcess() )
		return;

	int nBuffCnt = (int)m_OwnerBuffList.size();	
	for ( int i=0; i<nBuffCnt; ++i )
		pOwner->AddNewBuff( m_OwnerBuffList[i], pOwner->GetCharName(), "", NULL );
}

void ioDummyInvinciblePillar::CheckRange()
{
	if ( !m_bCheckRange )
		return;

	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	D3DXVECTOR3 vDist = GetWorldPosition() - pOwner->GetWorldPosition();
	if ( D3DXVec3LengthSq( &vDist ) > m_fRange * m_fRange )
		SetDieState( true );
}

void ioDummyInvinciblePillar::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	float fSecPerTime = fTimePerSec * FLOAT1000;
	m_pGroup->Update( fSecPerTime );

	if( m_bMountEnable )
	{
		UpdateMountedEntityList();
		UpdateMountedWeaponList();
	}

	ProcessState( fTimePerSec, dwPreTime );

	//DWORD dwCurTime = FRAMEGETTIME();
	int iCount = m_HealAreaWeaponNameVec.size();
	for( int i = 0; i < iCount; i++ )
	{
		ioAreaWeapon* pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponInRangeByName( m_HealAreaWeaponNameVec[i], GetOwnerName(), GetWorldPosition() );
		if( pAreaWeapon )
		{
			m_dwHealStartTime = dwCurTime;
			m_dwCheckHPTime = dwCurTime;
			break;
		}
	}

	if( m_dwHealStartTime == 0 )
		CheckDieByHp();
	else if( m_dwHealStartTime + m_dwHealTime > dwCurTime )
		CheckHealHp();
	else
	{
		m_dwCheckHPTime = dwCurTime;
		m_dwHealStartTime = 0;
	}
	DropZoneCheck();
	CheckDestroyRange();

	bool bTriger = false;
	iCount = m_AreaWeaponTrigerNameVec.size();
	for( int i = 0; i < iCount; i++ )
	{
		ioAreaWeapon* pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponInRangeByName( m_AreaWeaponTrigerNameVec[i], GetOwnerName(), GetWorldPosition() );
		if( pAreaWeapon )
		{
			bTriger = true;
			break;
		}
	}

	if( m_bTrigerWeapon != bTriger )
	{
		m_bTrigerWeapon = bTriger;
		if( m_bTrigerWeapon )
		{
			CreateTrigerAttachWeapon();
			CreateTrigerAreaWeapon();
		}
		else
		{
			DestoryTrigerAttachWeapon();
			DestoryTrigerAreaWeapon();
		}
	}

	if( CheckOwnerDestroyState() )
	{
		SendDieState( true );
	}
}

void ioDummyInvinciblePillar::CreateTrigerAttachWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( m_TrigerAttachWeaponInfoList.empty() )
		return;

	if( !m_vTrigerAttachWeaponIndexList.empty() )
		return;

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	int iCnt = m_TrigerAttachWeaponInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kInfo = m_TrigerAttachWeaponInfoList[i];
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();

		kFireTime.iAttributeIdx  = kInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = kInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni   = kInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration  = kInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = kInfo.m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;
		kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
		kFireTime.m_iExtraValue = m_iCreateIndex;

		if( kFireTime.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
		{
			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vCurPos, vCurDir, "" );
			if( pWeapon )
			{
				ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
				if( pZone )
				{
					pZone->SetCollisionDir( vCurDir );
					pZone->SetZoneValue( true, false );
				}
				ioAdhesiveMissileWeapon3 *pAdhesiveWeapon3 = ToAdhesiveMissileWeapon3( pWeapon );
				if( pAdhesiveWeapon3 )
				{
					pAdhesiveWeapon3->CheckOwnerAttach( true );
				}
				ioAttachAeraWeapon *pAttachAreaWeapon = ToAttachAeraWeapon( pWeapon );
				if ( pAttachAreaWeapon )
				{
					pAttachAreaWeapon->CheckOwnerAttach( true );
				}
			}
		}
		m_vTrigerAttachWeaponIndexList.push_back( kFireTime.dwWeaponIdx );
	}
}

void ioDummyInvinciblePillar::DestoryTrigerAttachWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	int iCnt = m_vTrigerAttachWeaponIndexList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_vTrigerAttachWeaponIndexList[i] );
		if( pWeapon )
			pWeapon->SetWeaponDead();
	}
	m_vTrigerAttachWeaponIndexList.clear();
}

void ioDummyInvinciblePillar::CreateTrigerAreaWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( m_TrigerAreaWeaponVec.empty() )
		return;

	if( !m_vTrigerAreaWeaponIndexList.empty() )
		return;

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();
	D3DXQUATERNION qtRot = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 );
	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	int iCnt = m_TrigerAreaWeaponVec.size();
	for( int iIndex = 0; iIndex < iCnt; iIndex++ )
	{
		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "", m_TrigerAreaWeaponVec[iIndex], 
			vCurPos, qtRot, ioAreaWeapon::CT_STRUCT );

		if( pAreaWeapon )
			m_vTrigerAreaWeaponIndexList.push_back( pAreaWeapon->GetAreaWeaponIndex() );
	}
}

void ioDummyInvinciblePillar::DestoryTrigerAreaWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	int iCnt = m_vTrigerAreaWeaponIndexList.size();
	for( int i=0; i<iCnt; ++i )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_vTrigerAreaWeaponIndexList[i] );
	}
	m_vTrigerAreaWeaponIndexList.clear();
}

void ioDummyInvinciblePillar::CheckHealHp()
{
	if( m_State == DCS_DIE || m_dwHealTicTime <= 0 || m_fHealHp <= 0.0f )
	{
		m_dwHealStartTime = 0;
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	int iTickGap = (int)( dwCurTime - m_dwCheckHPTime );
	if( iTickGap >= (int)m_dwHealTicTime )
	{
		m_dwCheckHPTime = dwCurTime - ( iTickGap - m_dwHealTicTime );
		m_HP.m_fCurValue += m_fHealHp;
		if( m_HP.m_fCurValue > m_HP.m_fMaxValue )
			m_HP.m_fCurValue = m_HP.m_fMaxValue;
	}
}