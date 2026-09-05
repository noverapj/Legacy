
#include "stdafx.h"

#include "ioDummyMine.h"
#include "BuffDefine.h"

ioDummyMine::ioDummyMine( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_bStartInit = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetDownKey = false;
	m_bSetUpKey = false;
	m_bMineVisible = false;
	m_MineState = DMS_START;
	m_nConveyerMoveFrameIdx = -1;
	m_dwCheckEndDelayTime = 0;
}

ioDummyMine::~ioDummyMine()
{
}

void ioDummyMine::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );
	
	LoadInitProperty( rkLoader );
}

void ioDummyMine::LoadInitProperty( ioINILoader &rkLoader )
{
	m_fVisibleRange = rkLoader.LoadFloat_e( "mine_visible_range", 0.0f );

	//앞방향
	m_fColRange_Front = rkLoader.LoadFloat_e( "front_target_range", 0.0f );
	m_fColAngle_Front = rkLoader.LoadFloat_e( "front_target_angle", 0.0f );
	m_fColUpHeight_Front = rkLoader.LoadFloat_e( "front_target_up_height", 0.0f );
	m_fColUpHeight_Front = max( FLOAT1, m_fColUpHeight_Front );
	m_fColUnderHeight_Front = rkLoader.LoadFloat_e( "front_target_under_height", 0.0f );
	m_fColUnderHeight_Front = max( FLOAT1, m_fColUnderHeight_Front );

	//뒷방향
	m_fColRange_Back = rkLoader.LoadFloat_e( "back_target_range", 0.0f );
	m_fColAngle_Back = rkLoader.LoadFloat_e( "back_target_angle", 0.0f );
	m_fColUpHeight_Back = rkLoader.LoadFloat_e( "back_target_up_height", 0.0f );
	m_fColUpHeight_Back = max( FLOAT1, m_fColUpHeight_Back );
	m_fColUnderHeight_Back = rkLoader.LoadFloat_e( "back_target_under_height", 0.0f );
	m_fColUnderHeight_Back = max( FLOAT1, m_fColUnderHeight_Back );

	//오프셋
	m_vCreateWeaponOffset.x = rkLoader.LoadFloat_e( "attack_offset_x", 0.0f );
	m_vCreateWeaponOffset.y = rkLoader.LoadFloat_e( "attack_offset_y", 0.0f );
	m_vCreateWeaponOffset.z = rkLoader.LoadFloat_e( "attack_offset_z", 0.0f );

	m_nCreateWeaponIndex_Front = rkLoader.LoadInt_e( "front_attack_type", NO_WEAPON_ATTRIBUTE );
	m_nCreateWeaponIndex_Back = rkLoader.LoadInt_e( "back_attack_type", NO_WEAPON_ATTRIBUTE );

	m_dwSensingTime = rkLoader.LoadInt_e( "sensing_delay_time", 0 );

	m_dwEndDelayTime = rkLoader.LoadInt_e( "end_delay_time", 0);

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "explosion_sound", "", szBuf, MAX_PATH );
	m_szExplosionSound = szBuf;
}

void ioDummyMine::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_MineState )
	{
	case DMS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DMS_SENSING:
		ProcessSensingState( fTimePerSec );
		break;
	case DMS_EXPLOSION:
		ProcessExplosionState( fTimePerSec );
		break;
	case DMS_END:
		{
			if ( m_dwCheckEndDelayTime + m_dwEndDelayTime < FRAMEGETTIME() )
			{
				SetDieState( true );
			}
		}
		break;
	}
}

void ioDummyMine::ProcessStartState( float fTimePerSec )
{	
	if ( !m_bStartInit )
	{
		SetAttachWeapon();

		//위치 오프셋 설정
		D3DXVECTOR3 vPos = GetWorldPosition();
		vPos.y += m_fHeightOffset;
		SetWorldPosition( vPos );

		m_vFrontDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		m_vBackDir= GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 );
		m_MineState = DMS_SENSING;
		m_dwCheckSensingTime = FRAMEGETTIME();
		m_bStartInit = true;
		m_dwCheckEndDelayTime = 0;
	}	
}

void ioDummyMine::ProcessSensingState( float fTimePerSec )
{
	if ( m_dwCheckSensingTime + m_dwSensingTime > FRAMEGETTIME() )
		return;

	ioBaseChar *pOnwer = GetOwner();

	VisibleCheck( pOnwer );
	CheckTarget( pOnwer );
	CheckDropZone( pOnwer );
}

void ioDummyMine::ProcessExplosionState( float fTimePerSec )
{
	D3DXVECTOR3 vPos = GetWorldPosition() + ( GetWorldOrientation() * m_vCreateWeaponOffset );

	CreateWeapon( m_nCreateWeaponIndex_Front, vPos, m_vFrontDir );
	CreateWeapon( m_nCreateWeaponIndex_Back, vPos, m_vBackDir );

	m_dwCheckEndDelayTime = FRAMEGETTIME();

	m_MineState = DMS_END;
}

ApplyWeaponExp ioDummyMine::ApplyWeapon( ioWeapon *pWeapon )
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

void ioDummyMine::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;	
	
	int iType;
	rkPacket >> iType;
	
}

void ioDummyMine::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
	ioDummyChar::TranslateByConveyer( vMove );

	if( m_MineState != DMS_EXPLOSION )
	{
		m_nConveyerMoveFrameIdx = g_FrameTimer.GetFrameCounter();
	}
}

//더미 죽는 상태 설정
void ioDummyMine::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	if ( !m_szExplosionSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szExplosionSound );
	}

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioDummyMine::IsActivateWeapon()
{
	if( m_MineState == DMS_START )
		return false;

	return true;
}

//무기 생성
void ioDummyMine::CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir )
{
	ioBaseChar *pOwner = GetOwner();

	if ( !pOwner )
		return;
	
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
	}	
}

// 이부분 보이는거 체크
void ioDummyMine::VisibleCheck( ioBaseChar *pOwner )
{
	if( !m_pGroup || !pOwner )	
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioBaseChar *pTarget = pStage->GetCameraTargetChar();
	if( pTarget )
	{
		if( GetTeam() == pTarget->GetTeam() || pTarget->HasBuff(BT_TRAP_EYE_BUFF) )
		{
			m_pGroup->SetVisible( true );
		}		
		else
		{
			float fVisibleRangeSq = m_fVisibleRange * m_fVisibleRange;

			D3DXVECTOR3 vDiff = GetWorldPosition() - pTarget->GetWorldPosition();

			if( D3DXVec3LengthSq( &vDiff ) < fVisibleRangeSq )
				m_bMineVisible = true;
			else
				m_bMineVisible = false;

			m_pGroup->SetVisible( m_bMineVisible );			
		}
	}
	else
	{
		m_pGroup->SetVisible( false );
	}
}

//실제 타겟에서 표시버프가 있는가 확인
void ioDummyMine::CheckTarget( ioBaseChar *pOwner )
{		
	if( !pOwner ) 
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	//타겟 리스트
	BaseCharList vFrontTargetList, vBackTargetList;

	//위치 설정
	D3DXVECTOR3 vPos = GetWorldPosition();

	GetTargetList( pStage, vPos, m_vFrontDir, vFrontTargetList, m_fColUpHeight_Front, m_fColUnderHeight_Front, m_fColRange_Front, m_fColAngle_Front );
	GetTargetList( pStage, vPos, m_vBackDir, vBackTargetList, m_fColUpHeight_Back, m_fColUnderHeight_Back, m_fColRange_Back, m_fColAngle_Back );

	//앞 타겟 리스트가 비어있지 않다면 검사
	if ( !vFrontTargetList.empty() )
	{
		int nFrontTargetCnt = vFrontTargetList.size();	
		for ( int i=0; i < nFrontTargetCnt; ++i )
		{
			ioBaseChar* pTarget = vFrontTargetList[i];
			//같은 팀이면 확인하지 않음
			if ( pOwner->GetTeam() == pTarget->GetTeam() )
				continue;
			
			SetExplosionState();
			return;
		}
	}	
	
	//뒷 타겟 리스트가 비어있지 않다면 검사
	if ( !vBackTargetList.empty() )
	{
		int nBackTargetCnt = vBackTargetList.size();	
		for ( int i=0; i < nBackTargetCnt; ++i )
		{
			ioBaseChar* pTarget = vBackTargetList[i];
			//같은 팀이면 확인하지 않음
			if ( pOwner->GetTeam() == pTarget->GetTeam() )
				continue;

			SetExplosionState();
			return;
		}
	}
}

void ioDummyMine::GetTargetList( ioPlayStage* pStage, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vColDir, BaseCharList &vCharList, float fColUpHeight, float fColUnderHeight, float fColRange, float fColAngle )
{
	if ( !pStage )
		return;

	vCharList.clear();

	if( fColUpHeight == 0.0f && fColUnderHeight == 0.0f )
		return;

	D3DXVECTOR3 vTop, vBottom, vCenter;
	vTop = vBottom = vPos;
	vTop.y += fColUpHeight;
	vBottom.y -= fColUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, fColRange );
	vCenter = kCylinder.GetCenter();

	float fRangeSq = fColRange * fColRange;
	float fHalfCosine = cosf( DEGtoRAD( fColAngle ) * FLOAT05 );

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar ) continue;

		D3DXVECTOR3 vCollisionPoint;
		if( !pChar->CheckCylinderPoint( kCylinder, vCenter, vCollisionPoint ) )
			continue;

		// 수평 거리 체크
		D3DXVECTOR3 vDiff1 = vCollisionPoint - vCenter;
		vDiff1.y = 0.0f;

		float fTargetDist = D3DXVec3LengthSq( &vDiff1 );
		if( fTargetDist > fRangeSq )
			continue;

		// 수평 각 체크
		D3DXVec3Normalize( &vDiff1, &vDiff1 );
		if( D3DXVec3Dot( &vColDir, &vDiff1 ) < fHalfCosine )
			continue;

		// 높이 체크
		if( vCollisionPoint.y >= vBottom.y && vCollisionPoint.y <= vTop.y )
			vCharList.push_back( pChar );
	}
}

void ioDummyMine::SetExplosionState()
{
	m_MineState = DMS_EXPLOSION;
}

void ioDummyMine::CheckDropZone( ioBaseChar *pOwner )
{
	bool bCheckDropZone = false;

	D3DXVECTOR3 vCurPos = GetWorldPosition();

	if( pOwner &&
		pOwner->GetCreator() &&
		pOwner->GetCreator()->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z, vCurPos.y+3.0f ) <= 0.0f )
		bCheckDropZone = true;

	if( bCheckDropZone )
	{
		m_HP.m_fCurValue = 0.0f;
		m_LastAttacker = m_OwnerName;

		SetExplosionState();
		return;
	}

	if( m_nConveyerMoveFrameIdx > 0 )
	{
		if( m_nConveyerMoveFrameIdx + 1 < (int)g_FrameTimer.GetFrameCounter() )
		{
			SetExplosionState();
			return;
		}
	}
}