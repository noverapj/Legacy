
#include "stdafx.h"

#include "ioFlyMoveAttackSkill.h"

#include "ioBaseChar.h"
#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioFlyMoveAttackSkill::ioFlyMoveAttackSkill()
{	
	Init();
}

ioFlyMoveAttackSkill::ioFlyMoveAttackSkill( const ioFlyMoveAttackSkill &rhs )
	: ioNormalSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList ),
	  m_szDelayLoopAni( rhs.m_szDelayLoopAni ),
	  m_fDelayLoopAniRate( rhs.m_fDelayLoopAniRate ),
	  m_szMoveLoopAni( rhs.m_szMoveLoopAni ),
	  m_fMoveLoopAniRate( rhs.m_fMoveLoopAniRate ),
	  m_AttackInfo( rhs.m_AttackInfo ),
	  m_fUpDownMoveSpeed( rhs.m_fUpDownMoveSpeed ),
	  m_dwAttackTime( rhs.m_dwAttackTime ),
	  m_fGaugePerTic( rhs.m_fGaugePerTic ),
	  m_dwTicTime( rhs.m_dwTicTime ),
	  m_fExtraDecreaseGauge( rhs.m_fExtraDecreaseGauge ),
	  m_fGaugeDecreaseHeight( rhs.m_fGaugeDecreaseHeight ),
	  m_bNoInputKey( rhs.m_bNoInputKey ),
	  m_bCreateWeapon( rhs.m_bCreateWeapon ),
	  m_bAttachZoneEffectWeapon( rhs.m_bAttachZoneEffectWeapon ),
	  m_AttachWeapon( rhs.m_AttachWeapon ),
	  m_SubWeaponDataList( rhs.m_SubWeaponDataList ),
	  m_AreaWeaponInfo( rhs.m_AreaWeaponInfo ),
	  m_SkillAttackType( rhs.m_SkillAttackType ),
	  m_bDisableLoopMove( rhs.m_bDisableLoopMove ),
	  m_dwLoopDuration( rhs.m_dwLoopDuration ),
	  m_PreAttackAttribute( rhs.m_PreAttackAttribute ),
	  m_fMoveSpeed( rhs.m_fMoveSpeed ),
	  m_fMaxMoveDist( rhs.m_fMaxMoveDist ),
	  m_fFlyTargetRange( rhs.m_fFlyTargetRange),
	  m_fFlyTargetAngle( rhs.m_fFlyTargetAngle ),
	  m_fTargetRange( rhs.m_fTargetRange ),
	  m_fTargetAngle( rhs.m_fTargetAngle ),
	  m_fTargetGapRange( rhs.m_fTargetGapRange ),
	  m_fCharHeightRate( rhs.m_fCharHeightRate ),
	  m_vGrapplingTargetBuff( rhs.m_vGrapplingTargetBuff ),
	  m_bCheckWounedState( rhs.m_bCheckWounedState ),
	  m_TargetWoundType( rhs.m_TargetWoundType )
{	
	Init();
}

ioFlyMoveAttackSkill::~ioFlyMoveAttackSkill()
{
	m_SubWeaponDataList.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFlyMoveAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
		
	LoadBuffList( rkLoader );
	
	rkLoader.LoadString_e( "delay_loop_ani", "", szBuf, MAX_PATH );
	m_szDelayLoopAni = szBuf;
	m_fDelayLoopAniRate = rkLoader.LoadFloat_e( "delay_loop_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "move_loop_ani", "", szBuf, MAX_PATH );
	m_szMoveLoopAni = szBuf;
	m_fMoveLoopAniRate = rkLoader.LoadFloat_e( "move_loop_ani_rate", FLOAT1 );

	LoadAttackAttribute_e( "move_attack", m_AttackInfo, rkLoader );

	m_fUpDownMoveSpeed = rkLoader.LoadFloat_e( "up_down_move_speed", 0.0f );

	m_dwAttackTime = rkLoader.LoadInt_e( "attack_time", 0 );

	m_fGaugePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_fExtraDecreaseGauge = rkLoader.LoadFloat_e( "extra_decrease_gauge", 0.0f );
	m_fGaugeDecreaseHeight = rkLoader.LoadFloat_e( "decrease_gauge_height", 0.0f );

	m_bNoInputKey = rkLoader.LoadBool_e( "no_input_key", false );
	m_bCreateWeapon = rkLoader.LoadBool_e( "create_weapon", false );
	m_bAttachZoneEffectWeapon = rkLoader.LoadBool_e( "attach_zone_effect_weapon", false );

	m_AttachWeapon.Init();
	m_AttachWeapon.m_iWeaponIdx = rkLoader.LoadInt_e( "attach_weapon", NO_WEAPON_ATTRIBUTE );
	m_AttachWeapon.m_iResistanceIdx = rkLoader.LoadInt_e( "attach_weapon_resist", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "attach_weapon_wounded", "", szBuf, MAX_PATH );
	m_AttachWeapon.m_WoundedAnimation = szBuf;

	m_AttachWeapon.m_dwWoundedDuration = rkLoader.LoadInt_e( "attach_weapon_wounded_time", 0 );
	m_AttachWeapon.m_bWoundedLoopAni = rkLoader.LoadBool_e( "attach_weapon_wounded_loop_ani", false );

	m_SubWeaponDataList.clear();
	int nMaxSubCnt = rkLoader.LoadInt_e( "max_sub_weapon_cnt", 0 );
	for ( int i=0; i<nMaxSubCnt; i++ )
	{
		D3DXVECTOR3 vPos;
		wsprintf_e( szKey, "sub_weapon%d_offset_x", i+1 );
		vPos.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "sub_weapon%d_offset_y", i+1 );
		vPos.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "sub_weapon%d_offset_z", i+1 );
		vPos.z = rkLoader.LoadFloat( szKey, 0.0f );

		float fAngle;
		wsprintf_e( szKey, "sub_weapon%d_angle", i+1 );
		fAngle = rkLoader.LoadFloat( szKey, 0.0f );

		WeaponInfo sWeaponInfo;
		wsprintf_e( szKey, "sub_weapon%d_attack_type", i+1 );
		sWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
		wsprintf_e( szKey, "sub_weapon%d_attack_type_resist", i+1 );
		sWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
		wsprintf_e( szKey, "sub_weapon%d_attack_type_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sWeaponInfo.m_WoundedAnimation = szBuf;
		wsprintf_e( szKey, "sub_weapon%d_attack_type_wounded_time", i+1 );
		sWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "sub_weapon%d_attack_type_wounded_loop_ani", i+1 );
		sWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		SubWeaponInfo sSubWeaponInfo;
		sSubWeaponInfo.m_vPos = vPos;
		sSubWeaponInfo.m_fAngle = fAngle;
		sSubWeaponInfo.m_WeaponInfo = sWeaponInfo;

		m_SubWeaponDataList.push_back( sSubWeaponInfo );
	}

	rkLoader.LoadString_e( "area_weapon", "", szBuf, MAX_PATH );
	m_AreaWeaponInfo.m_szAreaWeapon = szBuf;

	m_AreaWeaponInfo.m_nCreateType = rkLoader.LoadInt_e( "area_weapon_create_type", 0 );
	m_AreaWeaponInfo.m_vPos.x = rkLoader.LoadFloat_e( "area_weapon_offset_x", 0.0f );
	m_AreaWeaponInfo.m_vPos.y = rkLoader.LoadFloat_e( "area_weapon_offset_y", 0.0f );
	m_AreaWeaponInfo.m_vPos.z = rkLoader.LoadFloat_e( "area_weapon_offset_z", 0.0f );

	m_SkillAttackType = (SkillAttackType)rkLoader.LoadInt_e( "skill_attack_type", 0 );
	m_bDisableLoopMove = rkLoader.LoadBool_e( "disable_loop_move", 0 );
	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );
	LoadAttackAttribute_e( "pre_attack", m_PreAttackAttribute, rkLoader );

	m_fMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );
	m_fMaxMoveDist = rkLoader.LoadFloat_e( "max_range", 0.0f );

	m_fFlyTargetRange = rkLoader.LoadFloat_e( "fly_target_range", 0.0f );
	m_fFlyTargetAngle = rkLoader.LoadFloat_e( "fly_target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );

	m_fTargetGapRange	= rkLoader.LoadFloat_e( "target_gap_range", 0.0f );
	m_fCharHeightRate	= rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );

	LoadGrapplingTargetBuffList( rkLoader );

	m_bCheckWounedState = rkLoader.LoadBool_e( "check_traget_wounded", 0 );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
}

void ioFlyMoveAttackSkill::LoadGrapplingTargetBuffList( ioINILoader &rkLoader )
{
	int iCount = rkLoader.LoadInt_e( "grappling_target_buff_cnt", 0 );
	if ( iCount <= 0 )
		return;
	m_vGrapplingTargetBuff.clear();
	m_vGrapplingTargetBuff.reserve( iCount );
	char szBuf[MAX_PATH], szKey[MAX_PATH];
	for( int i=0; i<iCount; ++i )
	{
		wsprintf_e( szBuf, "grappling_target_buff%d", i+1 );
		rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
		ioHashString szBuffName = szKey;
		if( !szBuffName.IsEmpty() )
			m_vGrapplingTargetBuff.push_back( szBuffName );
	}
}

ioSkill* ioFlyMoveAttackSkill::Clone()
{
	return new ioFlyMoveAttackSkill( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioFlyMoveAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	Init();

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = dwCurTime + m_dwSkillProtectTime;
	
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
			
	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
	return true;
}

void ioFlyMoveAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_SkillState != SS_End )
	{
		if( m_dwCheckGaugeTime + m_dwTicTime <= dwCurTime )
		{
			m_dwCheckGaugeTime = dwCurTime;

			float fCurTicGauge = m_fGaugePerTic;
			if( fCurTicGauge != 0.0f )
			{
				if ( m_fCurExtraDecreaseGauge > 0.0f )
					fCurTicGauge += m_fCurExtraDecreaseGauge;

				if( pChar->IsNeedProcess() && pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
				{
					SetEndState( pChar );
					return;
				}
			}
		}
	}

	switch( m_SkillState )
	{
	case SS_None:
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				
				if ( m_SkillAttackType == SAT_Loop )
				{
					AddOwnerBuff( pChar );
					SetLoopState( pChar );
				}
				else if ( m_SkillAttackType == SAT_Pre_Move )
				{
					if( pChar->IsNeedProcess() )
						SetPreMoveState( pChar );
				}

				if ( pChar->IsNeedProcess() )
					CreateWeapon( pChar );
			}
		}
		break;
	case SS_Loop:
		{
			if ( m_dwLoopEndTime > 0 && m_dwLoopEndTime < dwCurTime )
			{
				SetEndState( pChar);
				return;
			}

			CheckDelayMoveAni( pChar );

			if ( !m_bNoInputKey && !CheckKeyInput( pChar ) )
				ProcessUpDownMove( pChar, pStage );
		}
		break;
	case SS_Action:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
				SetLoopState( pChar );
		}
		break;
	case SS_Pre_Move:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				SetEndState( pChar);

			if( !CheckMove( pChar, pStage ) )
				SetEndState( pChar);

			if ( m_fCurMoveDist >= m_fMaxMoveDist )
				SetEndState( pChar);
		}
		break;
	case SS_End:
		break;
	}
}

void ioFlyMoveAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	if ( !pChar )
		return;

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
	
	pChar->SetSkillMoveEnable( false, false );
	RemoveOwnerBuff( pChar );
	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if ( pTarget )
		RemoveGrapplingTargetBuff( pTarget, pChar );

	g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFlyMoveAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
	if( pZoneEffect )
	{
		ioBaseChar *pOwner = pWeapon->GetOwner();
		D3DXVECTOR3 vDir = pOwner->GetMoveDir();
		if( pOwner )
		{
			pZoneEffect->SetCollisionDir( vDir );
			if ( m_bAttachZoneEffectWeapon )
				pZoneEffect->SetZoneValue( true, true );
			else
				pZoneEffect->SetZoneValue( false, true );
		}
	}
}

void ioFlyMoveAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if ( !pOwner || !pStage )
		return;

	int nState;
	rkPacket >> nState;

	switch ( nState )
	{
	case SS_UpDown:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			rkPacket >> m_bSetUpKey;
			rkPacket >> m_bSetDownKey;
		}
		break;
	case SS_Action:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			SetAttackState( pOwner );
		}
		break;
	case SS_End:
		{
			SetEndState( pOwner );
		}
		break;
	case SS_CreateWeapon:
		{
			DWORD dwWeaponIdx;
			rkPacket >> dwWeaponIdx;

			pOwner->SetWeaponIndexBase( dwWeaponIdx );
			CreateWeapon( pOwner );
		}
		break;
	case SS_Pre_Move:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			rkPacket >> m_dwMoveAttackWeaponIndex;
			rkPacket >> m_vTargetDir;
			SetPreMoveState( pOwner );
		}
		break;
	case SS_Grappling:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			ioHashString szTargetName;
			rkPacket >> szTargetName;
			ioBaseChar *pTarget = pOwner->GetBaseChar( szTargetName );
			if ( !pTarget )
				return;

			m_szTargetName = szTargetName;
			SetLoopState( pOwner );
			AddGrapplingTargetBuff( pTarget, pOwner );
			if( pTarget && pTarget->IsNeedProcess() )
				pTarget->SetSkillGrapplingState( pOwner->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioFlyMoveAttackSkill::IsProtected( int iDefenseBreakType ) const
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

bool ioFlyMoveAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_None:
	case SS_Loop:
	case SS_Action:
	case SS_Pre_Move:
		return false;
	case SS_End:
		if( m_fSkillEndNextJumpAmt == 0.0f )
		{
			return true;
		}
		else
		{
			float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

			if( m_fSkillEndNextJumpAmt > 0.0f )
			{
				if( fHeightGap > FLOAT10 )
					return false;
			}
		}
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

bool ioFlyMoveAttackSkill::IsAttackEndState() const
{
	if( m_SkillState == SS_End )
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFlyMoveAttackSkill::LoadBuffList( ioINILoader &rkLoader )
{
	int i, iCount;
	ioBuffInfo tBuffInfo;
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	m_OwnerBuffList.clear();
	iCount = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( i=0; i<iCount; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );

		tBuffInfo.m_BuffName = szBuff;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

void ioFlyMoveAttackSkill::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar || m_OwnerBuffList.empty() )	
		return;

	int nCnt = m_OwnerBuffList.size();
	for( int i=0; i<nCnt; ++i )
		pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), "", this );
}

void ioFlyMoveAttackSkill::RemoveOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar || m_OwnerBuffList.empty() )	
		return;

	int nCnt = m_OwnerBuffList.size();
	for( int i=0; i<nCnt; ++i )
		pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName, true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFlyMoveAttackSkill::Init()
{
	m_bSetHandMesh = false;

	m_dwCurSkillProtectTime = 0;
	m_dwCurAttackTime = 0;
	m_dwMotionEndTime = 0;

	m_SkillState = SS_None;

	m_bSetUpKey	= false;
	m_bSetDownKey = false;
	m_bSetDelay = false;
	m_bSetMove = false;

	m_dwCheckGaugeTime = FRAMEGETTIME();
	m_fCurExtraDecreaseGauge = 0.0f;
	m_dwAreaWeaponIndex = 0;
	m_dwLoopEndTime = 0;
	m_dwMoveAttackWeaponIndex = 0;
	m_fCurMoveDist = 0.0f;
	m_vTargetDir = ioMath::VEC3_ZERO;
	m_szTargetName.Clear();
}

bool ioFlyMoveAttackSkill::CheckKeyInput( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	if ( CheckAttackInput( pChar ) )
	{
		SetAttackState( pChar );
		return true;
	}

	CheckUpDownMoveInput( pChar );
	return false;
}

bool ioFlyMoveAttackSkill::CheckAttackInput( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	if ( m_dwCurAttackTime + m_dwAttackTime < FRAMEGETTIME() && 
		 pChar->IsAttackKey() )
		return true;

	return false;
}

void ioFlyMoveAttackSkill::CheckUpDownMoveInput( ioBaseChar *pChar )
{
	if( !pChar || !pChar->IsNeedProcess() )	
		return;

	bool bChange = false;	
	if( pChar->IsJumpKeyDown() )
	{
		if( !m_bSetUpKey )	
			bChange = true;
		m_bSetUpKey		= true;
		m_bSetDownKey	= false;
	}
	else if( pChar->IsDefenseKeyDown() )
	{
		if( !m_bSetDownKey )	
			bChange = true;
		m_bSetUpKey		= false;
		m_bSetDownKey	= true;
	}
	else
	{
		if( m_bSetUpKey || m_bSetDownKey )	
			bChange = true;
		m_bSetUpKey		= false;
		m_bSetDownKey	= false;
	}

	if( bChange && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_UpDown;
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		kPacket << m_bSetUpKey;
		kPacket << m_bSetDownKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFlyMoveAttackSkill::CheckDelayMoveAni( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetMove )
		{	
			ioEntityGroup *pGrp = pOwner->GetGroup();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szMoveLoopAni, FLOAT100, FLOAT1, FLOAT1/m_fMoveLoopAniRate );

			m_bSetDelay = false;
			m_bSetMove = true;
		}
	}
	else
	{		
		if( !m_bSetDelay )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szDelayLoopAni, FLOAT100, FLOAT1, FLOAT1/m_fDelayLoopAniRate );

			m_bSetDelay = true;
			m_bSetMove = false;
		}
	}
}

void ioFlyMoveAttackSkill::ProcessUpDownMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	float fSpeed = m_fUpDownMoveSpeed;
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	fSpeed *= fTimePerSec;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	if( m_bSetUpKey )
		vPos.y += fSpeed;
	else if( m_bSetDownKey )
		vPos.y -= fSpeed;

	pStage->CheckFlyMapLimit( &vPos );
	pStage->CheckAxisLimit( &vPos );
	pChar->SetWorldPosition( vPos );

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if ( fMapHeight + m_fGaugeDecreaseHeight <= vPos.y )
		m_fCurExtraDecreaseGauge = m_fExtraDecreaseGauge;
	else
		m_fCurExtraDecreaseGauge = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFlyMoveAttackSkill::SetLoopState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	CheckDelayMoveAni( pChar );

	if ( !m_bDisableLoopMove )
		pChar->SetSkillMoveEnable( true, true );
	m_SkillState = SS_Loop;

	if ( m_dwLoopDuration > 0 )
		m_dwLoopEndTime = FRAMEGETTIME() + m_dwLoopDuration;
}

void ioFlyMoveAttackSkill::SetAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( m_AttackInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_AttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_AttackInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->RefreshFireTimeList( nAniID, m_AttackInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetReservedSliding( m_AttackInfo.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwCurAttackTime = dwCurTime + pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * fTimeRate;
	m_SkillState = SS_Action;

	pChar->SetSkillMoveEnable( false, false );

	m_bSetDelay = false;
	m_bSetMove = false;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Action;
		kPacket << pChar->GetWorldOrientation();
		kPacket << pChar->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFlyMoveAttackSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	RemoveOwnerBuff( pChar );
	m_SkillState = SS_End;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_End;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFlyMoveAttackSkill::CreateWeapon( ioBaseChar *pChar )
{
	if ( !pChar || !m_bCreateWeapon )
		return;
	
	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_CreateWeapon;
		kPacket << pChar->GetWeaponIndexBase();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DWORD dwWeaponIdx = pChar->GetWeaponIndexBase()+1;
	if ( m_AttachWeapon.m_iWeaponIdx != NO_WEAPON_ATTRIBUTE )
		pChar->SkillFireExplicit( m_AttachWeapon, GetName(), dwWeaponIdx++, pChar->GetMidPositionByRate() );
	
	for ( int i=0; i<(int)m_SubWeaponDataList.size(); i++ )
	{
		WeaponInfo sWeaponInfo = m_SubWeaponDataList[i].m_WeaponInfo;
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + pChar->GetWorldOrientation() * m_SubWeaponDataList[i].m_vPos;

		D3DXQUATERNION qtRot;
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(m_SubWeaponDataList[i].m_fAngle) );

		D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vDir = qtRot * vDir;
		D3DXVec3Normalize( &vDir, &vDir );

		pChar->SkillFireExplicit( sWeaponInfo, GetName(), dwWeaponIdx++, vPos, vDir );
	}

	pChar->SetWeaponIndexBase( dwWeaponIdx );

	if ( !m_AreaWeaponInfo.m_szAreaWeapon.IsEmpty() )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate() + pChar->GetWorldOrientation() * m_AreaWeaponInfo.m_vPos;
		D3DXQUATERNION qtRot = pChar->GetWorldOrientation();

		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),	pChar->GetCharName(), "", m_AreaWeaponInfo.m_szAreaWeapon, vPos, qtRot, m_AreaWeaponInfo.m_nCreateType );
		if ( pAreaWeapon )
			m_dwAreaWeaponIndex = pAreaWeapon->GetAreaWeaponIndex();
	}	
}

void ioFlyMoveAttackSkill::SetPreMoveState( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	
	m_SkillState = SS_Pre_Move;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_PreAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = m_PreAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_PreAttackAttribute.m_dwPreDelay;

	if( pChar->IsNeedProcess() )
	{
		m_dwMoveAttackWeaponIndex = pChar->IncreaseWeaponIndexBase();


		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pChar->SetTargetRotToDirKey( eNewDirKey );

		ioHashString szTarget;
		if ( m_SkillUseType == SUT_JUMP )
			szTarget = FindAttTarget( pChar, m_fFlyTargetAngle, m_fFlyTargetRange );
		else
			szTarget = FindAttTarget( pChar, m_fTargetAngle, m_fTargetRange );

		m_vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		if ( !szTarget.IsEmpty() )
		{
			ioBaseChar* pTarget = pChar->GetBaseChar( szTarget );
			if ( pTarget )
			{
				m_vTargetDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();		
				D3DXVec3Normalize( &m_vTargetDir, &m_vTargetDir );
				pChar->SetTargetRotToDir( m_vTargetDir, true );
			}
		}
	}
	
	pChar->RefreshFireTimeListWithBase( m_dwMoveAttackWeaponIndex,
		iAniID,
		m_PreAttackAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_PreAttackAttribute.m_dwEndDelay;
	SetExtraAniJump( pChar, iAniID, fTimeRate );
	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );
	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pChar->SetReservedSliding( m_PreAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	AddOwnerBuff( pChar );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Pre_Move;
		kPacket << pChar->GetTargetRot();
		kPacket << pChar->GetWorldPosition();
		kPacket << m_dwMoveAttackWeaponIndex;
		kPacket << m_vTargetDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioFlyMoveAttackSkill::CheckMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage ) 
		return false;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;

	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fMoveSpeed > 0.0f && m_vTargetDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vTargetDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;
			}
		}

		if( pChar->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
		{
			float fPreHeight = pChar->GetBottomHeight();

			pChar->Translate( vTotalMove );

			D3DXVECTOR3 vDiff = ioMath::VEC3_ZERO;
			vDiff = vTotalMove - vDiff;
			float fMoveDist = D3DXVec3Length( &vDiff );
			m_fCurMoveDist += fMoveDist;

			if( pChar->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pChar->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pChar->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,
						vCurPos.z,
						pChar,
						false,
						pChar->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pChar->SetWorldPosition( vCurPos );
					}
				}
			}
		}
		if ( bCol )
			return false;
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	float fPreHeight = pChar->GetBottomHeight();

	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pStage && pChar->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pChar->SetWorldPosition( vPos );
	}	

	return true;
}

ioHashString ioFlyMoveAttackSkill::FindAttTarget( ioBaseChar *pOwner, float fAngle, float fRange )
{
	ioHashString szTarget;
	if( !pOwner ) 
		return szTarget;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return szTarget;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( fAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = fRange * fRange;
	float fCurGap = -FLOAT1;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( pTarget == pOwner )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() ) 
			continue;

		if( m_bCheckWounedState && !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	return szTarget;
}

bool ioFlyMoveAttackSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if ( !pChar )
		return false;

	ioBaseChar *pTarget = pChar->GetBaseChar( szTargetName );
	if ( !pTarget )
		return false;

	if ( m_SkillState != SS_Pre_Move )
		return false;

	m_szTargetName = szTargetName;

	SetLoopState( pChar );

	AddGrapplingTargetBuff( pTarget, pChar );

	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
		bNPC = true;
	if( ToNpcChar( pChar ) )
		bAttackerNPC = true;
	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Grappling;
		kPacket << pChar->GetTargetRot();
		kPacket << pChar->GetWorldPosition();
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

D3DXVECTOR3 ioFlyMoveAttackSkill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fTargetGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fTargetGapRange > 0.0f )
			vPos += m_fTargetGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

void ioFlyMoveAttackSkill::AddGrapplingTargetBuff( ioBaseChar *pTarget, ioBaseChar *pChar )
{
	if ( !pTarget || !pChar )
		return;

	for( int i=0; i<(int)m_vGrapplingTargetBuff.size(); ++i )
		pTarget->AddNewBuff( m_vGrapplingTargetBuff[i], pChar->GetCharName(), "", this );
}

void ioFlyMoveAttackSkill::RemoveGrapplingTargetBuff( ioBaseChar *pTarget, ioBaseChar *pChar )
{
	if ( !pTarget || !pChar )
		return;

	int nCnt = m_OwnerBuffList.size();
	for( int i=0; i<(int)m_vGrapplingTargetBuff.size() ; ++i )
		pTarget->RemoveBuffOwnerName( m_vGrapplingTargetBuff[i], pChar->GetCharName() );
}