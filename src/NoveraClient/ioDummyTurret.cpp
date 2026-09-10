
#include "stdafx.h"

#include "ioDummyTurret.h"
#include "BuffDefine.h"
#include "ioTraceWeapon.h"

ioDummyTurret::ioDummyTurret( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_bStartInit = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetDownKey = false;
	m_bSetUpKey = false;
	m_TurretState = DMS_START;
}

ioDummyTurret::~ioDummyTurret()
{
}

void ioDummyTurret::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	LoadInitProperty( rkLoader );
}

void ioDummyTurret::LoadInitProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "explosion_sound", "", szBuf, MAX_PATH );
	m_szExplosionSound = szBuf;

	m_nCreateWeaponIndex = (DWORD)rkLoader.LoadInt( "fire_weapon", 0 );
	m_fMaxSearchingRange = rkLoader.LoadFloat( "fire_search_range",  0.f );
	m_fFireLandMinRange  = rkLoader.LoadFloat( "fire_land_min_range", 0.f );
	m_fFireHeightOffset  = rkLoader.LoadFloat( "fire_height_offset", 0.f );
}

void ioDummyTurret::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_TurretState )
	{
	case DMS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DMS_SENSING:
		ProcessSensingState( fTimePerSec );
		break;
	case DMS_END:
		SetDieState( true );	
		break;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_DIE )
		SendDieState( true );
}

void ioDummyTurret::ProcessStartState( float fTimePerSec )
{	
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwStartAniEndTime == 0 )
	{
		SetDieState( true );
		return;
	}

	if( m_dwStartAniEndTime < dwCurTime )
	{
		m_TurretState = DMS_SENSING;
		
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
	}
}

void ioDummyTurret::ProcessSensingState( float fTimePerSec )
{
	ioBaseChar *pOnwer = GetOwner();
	CheckTarget( pOnwer );
}

ApplyWeaponExp ioDummyTurret::ApplyWeapon( ioWeapon *pWeapon )
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

	return AWE_EXPLOSION;
}

void ioDummyTurret::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;	

	int iType;
	rkPacket >> iType;
}

//더미 죽는 상태 설정
void ioDummyTurret::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	if ( !m_szExplosionSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szExplosionSound );
	}

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioDummyTurret::IsActivateWeapon()
{
	if( m_TurretState == DMS_START )
		return false;

	return true;
}

//무기 생성
void ioDummyTurret::CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, ioBaseChar *pTarget )
{
	ioBaseChar *pOwner = GetOwner();

	if ( !pOwner )
		return;


	m_FiredTargetList.push_back( pTarget->GetCharName() );

	m_dwBaseWeaponIndex = pOwner->GetWeaponIndexBase() + 1;

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = nAttackIndex;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex;
	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

	pOwner->SetWeaponIndexBase( m_dwBaseWeaponIndex );

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if ( pWeapon )
	{
		pWeapon->SetPosition( vPos );
		ioTraceWeapon *pTaraceWeapon = ToTraceWeapon( pWeapon );
		if( pTaraceWeapon )
			pTaraceWeapon->SetTraceTarget( (ioHashString)pTarget->GetCharName() );
	}	
}

//실제 타겟에서 표시버프가 있는가 확인
void ioDummyTurret::CheckTarget( ioBaseChar *pOwner )
{		
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	//위치 설정
	D3DXVECTOR3 vPos = GetWorldPosition();

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget ) 
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )	
			continue;


		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 	
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		D3DXVECTOR3 vDiff = vTargetPos - vPos;
		float fDist = D3DXVec3Length( &vDiff );

		if( m_fMaxSearchingRange < fDist )
			continue;

		float fTargetMapHeight = pStage->GetMapHeight( vTargetPos.x, vTargetPos.z );
		if( (vTargetPos.y - fTargetMapHeight) < m_fFireLandMinRange )
			continue;

		bool bContinue =  false;
		//이미 공격을 했던 유저는 패스한다.
		for( int i = 0; i < (int)m_FiredTargetList.size(); i++ )
		{
			if( pTarget->GetCharName() == m_FiredTargetList[i] )
			{
				bContinue = true;
				break;
			}
		}

		if( !bContinue )
		{
			CreateWeapon( m_nCreateWeaponIndex, vPos + D3DXVECTOR3(0.f, m_fFireHeightOffset, 0.f ), D3DXVECTOR3( 0.f, FLOAT1, 0.f), pTarget );
		}
	}
}

void ioDummyTurret::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
	return;
}