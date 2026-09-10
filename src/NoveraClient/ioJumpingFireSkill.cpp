
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioWeapon.h"

#include "ioJumpingFireSkill.h"

ioJumpingFireSkill::ioJumpingFireSkill()
{
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	
	m_dwCheckLoopTime = 0;
	m_dwPreActionEndTime = 0;
	m_dwAttackActionEndTime = 0;
	m_dwEndActionEndTime = 0;

	m_bPreActionInit = false;
	m_bLoopActionInit = false;
	m_bAttackActionInit = false;
	m_bEndActionInit = false;
	m_dwCheckFireReloadTime = 0;
	m_nFireCurCnt = 0;
	m_nFireWeaponIndex = 0;
	m_bSetReduceGauge = true;
	m_dwCheckTick = 0;
	m_bInputAttackKey = false;
}

ioJumpingFireSkill::ioJumpingFireSkill( const ioJumpingFireSkill &rhs )
	: ioAttackSkill( rhs ),
      m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_szAreaWeaponName( rhs.m_szAreaWeaponName ),
	  m_vAreaWeaponOffset( rhs.m_vAreaWeaponOffset ),
	  m_fJumpAngle( rhs.m_fJumpAngle ),
	  m_fJumpPower( rhs.m_fJumpPower ),
	  m_dwLoopTime( rhs.m_dwLoopTime ),
	  m_szPreActionName ( rhs.m_szPreActionName ),
	  m_fPreActionRate( rhs.m_fPreActionRate ),
	  m_szLoopActionName( rhs.m_szLoopActionName ),
	  m_szAttackActionName( rhs.m_szAttackActionName ),
	  m_fAttackActionRate( rhs.m_fAttackActionRate ),
	  m_szEndActionName( rhs.m_szEndActionName ),
	  m_fEndActionRate( rhs.m_fEndActionRate ),
	  m_bSetFirstAttack ( rhs.m_bSetFirstAttack ),
	  m_dwFireReloadTime( rhs.m_dwFireReloadTime ),
	  m_nFireMaxCnt( rhs.m_nFireMaxCnt ),
	  m_fRandomRange( rhs.m_fRandomRange ),
	  m_FireWeaponList( rhs.m_FireWeaponList ),
	  m_dwTick( rhs.m_dwTick ),
	  m_fUseGuage( rhs.m_fUseGuage ),
	  m_bSetNoJump( rhs.m_bSetNoJump ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList ),
	  m_bSetChangeState( rhs.m_bSetChangeState )
{
	m_SkillState = SS_NONE;

	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;
	
	m_dwCheckLoopTime = 0;
	m_dwPreActionEndTime = 0;
	m_dwAttackActionEndTime = 0;
	m_dwEndActionEndTime = 0;

	m_bPreActionInit = false;
	m_bLoopActionInit = false;
	m_bAttackActionInit = false;
	m_bEndActionInit = false;
	m_bRefreshFireTimeList = m_bSetFirstAttack;
	m_dwCheckFireReloadTime = 0;
	m_nFireCurCnt = 0;
	m_nFireWeaponIndex = 0;
	m_bSetReduceGauge = true;
	m_dwCheckTick = 0;
	m_bInputAttackKey = false;
}

ioJumpingFireSkill::~ioJumpingFireSkill()
{
	m_FireWeaponList.clear();
}

void ioJumpingFireSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	
	//스킬 무적 시간
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	rkLoader.LoadString( "area_weapon_name", "", szBuf, MAX_PATH );
	m_szAreaWeaponName = szBuf;

	m_vAreaWeaponOffset.x = rkLoader.LoadFloat_e( "area_weapon_offset_x", 0.0f);
	m_vAreaWeaponOffset.y = rkLoader.LoadFloat_e( "area_weapon_offset_y", 0.0f);
	m_vAreaWeaponOffset.z = rkLoader.LoadFloat_e( "area_weapon_offset_z", 0.0f);

	m_bSetNoJump = rkLoader.LoadBool_e( "set_no_jump", false );
	m_fJumpAngle = rkLoader.LoadFloat_e( "jump_angle", 0.0f );
	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );

	rkLoader.LoadString_e( "pre_action_name", "", szBuf, MAX_PATH);
	m_szPreActionName = szBuf;
	m_fPreActionRate = rkLoader.LoadFloat_e( "pre_action_rate", FLOAT1 );

	rkLoader.LoadString_e( "loop_action_name", "", szBuf, MAX_PATH);
	m_szLoopActionName = szBuf;
	m_dwLoopTime = rkLoader.LoadInt_e( "loop_action_time" , 0 );
	
	rkLoader.LoadString_e( "attack_action_name", "", szBuf, MAX_PATH);
	m_szAttackActionName = szBuf;
	m_fAttackActionRate = rkLoader.LoadFloat_e( "attack_action_rate", FLOAT1 );

	rkLoader.LoadString_e( "end_action_name", "", szBuf, MAX_PATH);
	m_szEndActionName = szBuf;
	m_fEndActionRate = rkLoader.LoadFloat_e( "end_action_rate", FLOAT1 );

	m_dwTick = rkLoader.LoadInt_e( "tick_time", 0);
	m_fUseGuage = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );

	m_bSetFirstAttack = rkLoader.LoadBool_e( "set_first_attack", true );
	m_dwFireReloadTime = rkLoader.LoadInt_e( "fire_reload_time", 0 );
	m_nFireMaxCnt = rkLoader.LoadInt_e( "fire_max_cnt",0 );

	m_fRandomRange	= rkLoader.LoadFloat_e( "fire_random_range", 0.0f );

	m_bSetChangeState = rkLoader.LoadBool_e( "set_change_state", false );

	int nWeaponCnt = rkLoader.LoadInt_e( "fire_weapon_type_count", 0 );
	for (int i=0; i<nWeaponCnt; ++i)
	{
		wsprintf_e( szBuf, "fire_weapon_type%d", i+1);
		int nWeaponIndex = rkLoader.LoadInt( szBuf, NO_WEAPON_ATTRIBUTE );
		m_FireWeaponList.push_back( nWeaponIndex );
	}
	
	m_OwnerBuffList.clear();
	ioHashString szBuffName;
	int nBuffCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( int i=0; i<nBuffCnt; ++i )
	{
		wsprintf_e( szKey, "owner_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szBuffName = szBuf;
		m_OwnerBuffList.push_back( szBuffName );
	}
}

ioSkill* ioJumpingFireSkill::Clone()
{
	return new ioJumpingFireSkill( *this );
}

bool ioJumpingFireSkill::OnSkillStart( ioBaseChar *pChar )
{	
	m_SkillState = SS_NONE;
	
	m_bSetHandMesh = false;
	m_dwCurSkillProtectTime = 0;

	m_bRefreshFireTimeList = m_bSetFirstAttack;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;
	
	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
	
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;	

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );

	m_dwCheckLoopTime = 0;
	m_dwPreActionEndTime = 0;
	m_dwAttackActionEndTime = 0;
	m_dwEndActionEndTime = 0;

	m_bPreActionInit = false;
	m_bLoopActionInit = false;
	m_bAttackActionInit = false;
	m_bEndActionInit = false;
	m_dwCheckFireReloadTime = 0;
	m_nFireCurCnt = 0;
	m_nFireWeaponIndex = 0;
	m_bSetReduceGauge = true;
	m_dwCheckTick = 0;
	m_bInputAttackKey = false;

	return true;
}

void ioJumpingFireSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				AttackSkillFire( pChar );
				CreateAreaWeapon( pChar );
				SetChangeState( pChar );
				SetOwnerBuff( pChar );
				SetCharJumping(pChar);				
			}
		}
		break;
	case SS_PRE_ACTION:
		{
			//PreAction 초기화
			if ( !m_bPreActionInit && m_dwMotionEndTime < dwCurTime )
			{
				SetPreAction( pChar );
				m_SkillState = SS_LOOP;
				m_bPreActionInit = true;
			}
		}
		break;
	case SS_LOOP:
		{
			//LoopAction 초기화
			if ( !m_bLoopActionInit && m_dwPreActionEndTime < dwCurTime )
			{
				SetLoopAction( pChar );
				m_bLoopActionInit = true;
			}

			//LoopAction 초기화 완료 후 가능
			if ( m_bLoopActionInit )
			{
				CheckInputKey( pChar );
				ReduceGauge( pChar );
			}			

			if ( m_bInputAttackKey )
			{
				m_bInputAttackKey = false;
				m_SkillState = SS_ATTACK;
			}
		}
		break;
	case SS_ATTACK:
		{
			if ( !m_bAttackActionInit )
			{
				SetAttackAction( pChar );
				m_bAttackActionInit = true;
			}

			if ( m_dwAttackActionEndTime < dwCurTime )
			{
				m_bLoopActionInit = false;
				m_bAttackActionInit = false;
				m_SkillState = SS_LOOP;
			}
		}
		break;
	case SS_END_ACTION:
		{
			if ( !m_bEndActionInit )
			{
				SetEndAction( pChar );
				m_bEndActionInit = true;
			}
		}
		break;
	}		
	//종료 조건
	if ( m_bEndActionInit && m_dwEndActionEndTime < dwCurTime )
	{
		m_SkillState = SS_END;
	}
}

void ioJumpingFireSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	m_bSetHandMesh = false;	

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	GetOwnerItem()->SetCurSkillGauge(0.0f);
}

void ioJumpingFireSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	m_SkillState = SS_PRE_ACTION;
}

bool ioJumpingFireSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}
	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwPreDelayStartTime == 0 )
	{		
		if( m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioJumpingFireSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if ( m_bSetNoJump && m_SkillState == SS_END )
	{
		return true;
	}

	return false;
}

bool ioJumpingFireSkill::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioJumpingFireSkill::IsNoDropState() const
{
	return true;
}

bool ioJumpingFireSkill::IsJumpState() const
{
	if ( m_SkillState == SS_LOOP )
	{
		return true;
	}

	return false;
}

bool ioJumpingFireSkill::IsAttackEndState() const
{
	return IsEndState();
}

void ioJumpingFireSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioJumpingFireSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{		
	case SS_CreateWeapon:
		{
			rkPacket >> m_nFireWeaponIndex;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			D3DXVECTOR3 vDir = -ioMath::UNIT_Y;

			rkPacket >> m_dwWeaponBaseIndex;
			
			FireTime kFireTime;
			kFireTime.dwStart = FRAMEGETTIME();
			kFireTime.iAttributeIdx = m_FireWeaponList[m_nFireWeaponIndex];
			kFireTime.iResistanceIdx = 0;
			kFireTime.szWoundedAni  = "";
			kFireTime.dwWoundedAniDuration = 0;
			kFireTime.bLoopWoundedAni = false;
			kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex;
			kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

			pChar->SetWeaponIndexBase( m_dwWeaponBaseIndex );

			ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			if ( pWeapon )
			{
				D3DXQUATERNION qtRot;
				D3DXQuaternionRotationAxis(&qtRot,&(-ioMath::UNIT_X),DEGtoRAD(FLOAT90));
				pWeapon->SetOrientation(qtRot);
			}

			rkPacket >> m_bInputAttackKey;
		}
		break;
	case SS_END_ACTION:
		{
			m_SkillState = SS_END_ACTION;
		}
		break;
	}
}

//범위 무기 생성
void ioJumpingFireSkill::CreateAreaWeapon( ioBaseChar *pChar )
{	
	if( !pChar || m_szAreaWeaponName.IsEmpty() )
		return;

	D3DXQUATERNION qtRot = pChar->GetWorldOrientation();
	D3DXVECTOR3 vPos = pChar->GetWorldPosition() + ( qtRot * m_vAreaWeaponOffset );

	g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),	"",	GetName(), m_szAreaWeaponName, vPos, qtRot, ioAreaWeapon::CT_NORMAL );
}

void ioJumpingFireSkill::SetCharJumping( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	D3DXVECTOR3 vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	float fJumpAngle = m_fJumpAngle;

	float fAngleV = ioMath::ArrangeHead( fJumpAngle );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	vTargetDir = qtRot * vTargetDir;
		
	pChar->SetForcePower( vTargetDir, fabs(m_fJumpPower), FLOAT1, true );
}

void ioJumpingFireSkill::SetPreAction( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szPreActionName );
	if( iAniID == -1 )	return;

	float fTimeRate = m_fPreActionRate;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwPreActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}

void ioJumpingFireSkill::SetLoopAction( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szLoopActionName );
	if( iAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_dwCheckLoopTime = FRAMEGETTIME();
}

void ioJumpingFireSkill::SetAttackAction( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szAttackActionName );
	if( iAniID == -1 )	return;

	float fTimeRate = m_fAttackActionRate;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwAttackActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}

void ioJumpingFireSkill::SetEndAction( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szEndActionName );
	if( iAniID == -1 )	return;

	float fTimeRate = m_fEndActionRate;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwEndActionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}

void ioJumpingFireSkill::CheckInputKey( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if ( pChar->IsNeedProcess() )
	{
		if ( pChar->IsAttackKey() )
		{
			CreateWeapon( pChar );
		}
		
		//종료 조건
		if ( m_nFireCurCnt >= m_nFireMaxCnt || 
			m_dwCheckLoopTime + m_dwLoopTime <= FRAMEGETTIME() || 
			( m_bSetReduceGauge && GetOwnerItem()->GetCurSkillGuage() <= 0.0f ) )
		{
			m_SkillState = SS_END_ACTION;

			if ( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SS_END_ACTION;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioJumpingFireSkill::CreateWeapon( ioBaseChar *pChar )
{
	if ( !pChar || m_FireWeaponList.empty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_nFireCurCnt < m_nFireMaxCnt && m_dwCheckFireReloadTime + m_dwFireReloadTime < dwCurTime )
	{
		m_dwWeaponBaseIndex = pChar->GetWeaponIndexBase() + 1;
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();

		D3DXVECTOR3 vDir = -ioMath::UNIT_Y;
				
		IORandom random;
		DWORD dwSeed = pChar->GetRandomSeed() + m_nFireCurCnt;
		random.SetRandomSeed( dwSeed );

		int iRandX = random.Random( m_fRandomRange*2 ) - m_fRandomRange;
		int iRandZ = random.Random( m_fRandomRange*2 ) - m_fRandomRange;
		
		vPos.x += (float)iRandX;
		vPos.z += (float)iRandZ;

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;
		kFireTime.iAttributeIdx = m_FireWeaponList[m_nFireWeaponIndex];
		kFireTime.iResistanceIdx = 0;
		kFireTime.szWoundedAni  = "";
		kFireTime.dwWoundedAniDuration = 0;
		kFireTime.bLoopWoundedAni = false;
		kFireTime.dwWeaponIdx = m_dwWeaponBaseIndex;
		kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

		ioWeapon *pWeapon = pChar->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		if ( pWeapon )
		{
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis(&qtRot,&(-ioMath::UNIT_X),DEGtoRAD(FLOAT90));
			pWeapon->SetOrientation(qtRot);
		}
		
		m_dwCheckFireReloadTime = dwCurTime;

		m_bInputAttackKey = true;

		if ( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_CreateWeapon;
			kPacket << m_nFireWeaponIndex;
			kPacket << vPos;
			kPacket << m_dwWeaponBaseIndex;
			kPacket << m_bInputAttackKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );			
		}
		
		pChar->SetWeaponIndexBase( m_dwWeaponBaseIndex );
		m_nFireCurCnt++;
		m_nFireWeaponIndex++;

		if ( m_nFireWeaponIndex >= (int)m_FireWeaponList.size() )
		{
			m_nFireWeaponIndex = 0;
		}
	}
}

void ioJumpingFireSkill::ReduceGauge( ioBaseChar *pChar )
{
	if ( !pChar || !m_bSetReduceGauge)
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwCheckTick + m_dwTick < dwCurTime )
	{
		GetOwnerItem()->ReduceSkillGauge( m_fUseGuage );
		m_dwCheckTick = dwCurTime;
	}	
}

void ioJumpingFireSkill::SetOwnerBuff( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	
	ioItem *pItem = GetOwnerItem();
	if ( !pItem )
		return;

	int nBuffCnt = (int)m_OwnerBuffList.size();
	ioHashString szCharName = pChar->GetCharName();	
	ioHashString szItemName = pItem->GetName();
	for ( int i=0; i<nBuffCnt; ++i )
	{
		pChar->AddNewBuff( m_OwnerBuffList[i], szCharName, szItemName, this );			
	}
}

void ioJumpingFireSkill::SetChangeState( ioBaseChar *pChar )
{
	if ( !pChar || !m_bSetChangeState )
		return;

	ioItem *pItem = pChar->GetEquipedItem( ES_WEAPON );
	if ( !pItem )
		return;

	//특정 아이템일때 상태 변경
}