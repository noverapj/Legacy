
#include "stdafx.h"

#include "ioAreaStructSkill.h"


#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioHackerItem.h"

ioAreaStructSkill::ioAreaStructSkill()
{
	m_dwCurMapEffect = -1;

	m_dwCircleEnd = 0;
	m_dwCreateTime = 0;
	m_dwFireMotionEndTime = 0;
	m_dwAreaWeaponIndex = 0;

	m_bSetHandMesh = false;

	m_dwAttackEndTime = 0;
	m_dwCurJumpReserveTime = 0;
	m_AreaWeaponIndexList.clear();
}

ioAreaStructSkill::ioAreaStructSkill( const ioAreaStructSkill &rhs )
: ioNormalSkill( rhs ),
m_fOffSet( rhs.m_fOffSet ),
m_fOffSet2( rhs.m_fOffSet2 ),
m_fYOffSet( rhs.m_fYOffSet ),
m_AreaWeaponName( rhs.m_AreaWeaponName ),
m_RedCircle( rhs.m_RedCircle ),
m_BlueCircle( rhs.m_BlueCircle ),
m_dwCircleDuration( rhs.m_dwCircleDuration ),
m_iBildStruct( rhs.m_iBildStruct ),
m_BildEffect( rhs.m_BildEffect ),
m_bEnableCheckHeight ( rhs.m_bEnableCheckHeight ),
m_fUpLimitHeight( rhs.m_fUpLimitHeight ),
m_fDownLimitHeight( rhs.m_fDownLimitHeight ),
m_iMaxStructCnt( rhs.m_iMaxStructCnt ),
m_vForceInfoList( rhs.m_vForceInfoList ),
m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
m_DummyCharList( rhs.m_DummyCharList ),
m_DummyCharEffect( rhs.m_DummyCharEffect ),
m_dwSkillJumpPower ( rhs.m_dwSkillJumpPower ),
m_dwJumpReserveTime( rhs.m_dwJumpReserveTime ),
m_AreaWeapnStyleList( rhs.m_AreaWeapnStyleList ),
m_bUseItemCircle( rhs.m_bUseItemCircle )
{
	m_dwCurMapEffect = -1;

	m_dwCircleEnd = 0;
	m_dwCreateTime = 0;
	m_dwAreaWeaponIndex = 0;
	m_dwFireMotionEndTime = 0;

	m_bSetHandMesh = false;

	m_dwAttackEndTime = 0;
	m_dwCurJumpReserveTime = 0;
	m_AreaWeaponIndexList.clear();
}

ioAreaStructSkill::~ioAreaStructSkill()
{
	m_vDestroyDummyCharList.clear();
	m_DummyCharList.clear();
	m_AreaWeaponIndexList.clear();
}

void ioAreaStructSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH],szKey[MAX_PATH];

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	m_dwCircleDuration = rkLoader.LoadInt_e( "circle_duration", 0 );

	m_fOffSet = rkLoader.LoadFloat_e( "circle_offset", 0.0f );
	m_fOffSet2 = rkLoader.LoadFloat_e( "circle_offset2", 0.0f );
	m_fYOffSet = rkLoader.LoadFloat_e( "circle_y_offset", 0.0f );

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedCircle = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueCircle = szBuf;


	rkLoader.LoadString_e( "bild_effect", "", szBuf, MAX_PATH );
	m_BildEffect = szBuf;
	m_iBildStruct = rkLoader.LoadInt_e( "bild_struct_num", 0 );

	m_bEnableCheckHeight = rkLoader.LoadBool_e( "enable_check_height", false );

	m_fUpLimitHeight = rkLoader.LoadFloat_e( "up_limit_height", 0.0f );
	m_fDownLimitHeight = rkLoader.LoadFloat_e( "down_limit_height", 0.0f );
	m_iMaxStructCnt = rkLoader.LoadInt_e( "max_struct_cnt", 0 );

	m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "skill_motion_force_cnt", 0 );
	if( iForceCnt > 0 )
	{
		m_vForceInfoList.reserve( iForceCnt );
	}

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "skill_motion_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "skill_motion_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		wsprintf_e( szBuf, "skill_motion_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szBuf, 0.0f );

		m_vForceInfoList.push_back( kInfo );
	}

	//
	rkLoader.LoadString_e( "dummy_char_effect", "", szBuf, MAX_PATH );
	m_DummyCharEffect = szBuf;

	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );


	//for mutant accessory
	m_dwSkillJumpPower	= rkLoader.LoadInt_e( "skill_jump_power", 0 );
	m_dwJumpReserveTime = rkLoader.LoadInt_e( "skill_jump_reserve_time", 0 );

	m_AreaWeapnStyleList.clear();
	int iAreaWeapnCount = rkLoader.LoadInt_e( "area_weapon_count", 0 );
	if ( iAreaWeapnCount != 0 )
		m_AreaWeapnStyleList.reserve( iAreaWeapnCount );
	for ( int i=0 ; i < iAreaWeapnCount ; ++i )
	{
		AreaWeaponStyle AreaStyle;
		wsprintf_e( szKey, "area_weapon%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		AreaStyle.szAreaWeaponName = szBuf;

		wsprintf_e( szBuf, "area_weapon%d_offset_x", i+1 );
		AreaStyle.vOffset.x = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "area_weapon%d_offset_y", i+1 );
		AreaStyle.vOffset.y = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "area_weapon%d_offset_z", i+1 );
		AreaStyle.vOffset.z = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "area_weapon%d_x_rot_angle", i+1 );
		AreaStyle.vRotAngle.x = rkLoader.LoadFloat( szBuf, 0.0f );
		
		wsprintf_e( szBuf, "area_weapon%d_y_rot_angle", i+1 );
		AreaStyle.vRotAngle.y = rkLoader.LoadFloat( szBuf, 0.0f );
		
		wsprintf_e( szBuf, "area_weapon%d_z_rot_angle", i+1 );
		AreaStyle.vRotAngle.z = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "area_weapon%d_change_height", i+1 );
		AreaStyle.bChangeHeight = rkLoader.LoadBool( szBuf, false );

		m_AreaWeapnStyleList.push_back( AreaStyle );
	}

	m_bUseItemCircle = rkLoader.LoadBool_e( "use_item_circle", false );
}

ioSkill* ioAreaStructSkill::Clone()
{
	return new ioAreaStructSkill( *this );
}

bool ioAreaStructSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

bool ioAreaStructSkill::OnSkillStart( ioBaseChar *pChar )
{

	if( !HasSkillMotion() )
		return false;

	if( m_AreaWeaponName.IsEmpty() && m_iBildStruct == 0 && m_DummyCharList.empty() && m_AreaWeapnStyleList.empty() )
		return false;

	m_State = SS_NONE;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_dwCurJumpReserveTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	pChar->SetSkillProtection();

	m_dwAttackEndTime = 0;

	//CreateMapEffect
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( 0.0f, 0.0f, 0.0f, true, true );
	}

	float fCurOffSet = GetOwnerItem()->GetMagicCircleOffSet();
	float fCurYOffSet = GetOwnerItem()->GetMagicCircleYOffSet();
	if( fCurOffSet == 0.0f )
		fCurOffSet = m_fOffSet;

	//아누비스때 추가
	if( m_fOffSet2 != 0.0f )
		fCurOffSet = m_fOffSet2;

	if( fCurYOffSet == 0.0f )
		fCurYOffSet = m_fYOffSet;

	if( fCurYOffSet != 0.0f )
		m_vCreatePos = pChar->GetMagicCirclePos( fCurOffSet, fCurYOffSet );
	else
		m_vCreatePos = pChar->GetMagicCirclePos( fCurOffSet, m_bCircleNoChangeY );

	if( m_bUseItemCircle )
	{
		ioHackerItem *pHacker = ToHackerItem( GetOwnerItem() );
		if ( pHacker )
			m_vCreatePos = pHacker->GetHackerCirclePos( pChar );

		if ( m_vCreatePos == ioMath::VEC3_ZERO )
		{
			D3DXVECTOR3 vLook = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vLook, &vLook );
			m_vCreatePos = pChar->GetWorldPosition() + (vLook * m_fOffSet);
		}
	}

	if( m_bEnableCheckHeight )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
		{
			float fHeight = pStage->GetMapHeightNoneEntity( m_vCreatePos.x, m_vCreatePos.z, m_vCreatePos.y );
			m_vCreatePos.y = fHeight;
		}
	}
	
	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCreatePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCreatePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCreatePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCreatePos );
	}

	if( pMapEffect )
	{
		DWORD dwPreDelayTime = GetPreDelayTime();

		m_dwCurMapEffect = pMapEffect->GetUniqueID();
		m_dwCircleEnd = FRAMEGETTIME() + dwPreDelayTime + m_dwCircleDuration;
	}

	CheckInitMoveSpeed( pChar );

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetActionState( pChar );
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioAreaStructSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwCircleEnd > 0 && m_dwCircleEnd < FRAMEGETTIME() )
	{
		//DestroyMapEffect
		pChar->EndMapEffect( m_dwCurMapEffect );
		pChar->DestroyMagicCircle();
		m_dwCircleEnd = 0;
	}

	switch( m_State )
	{
	case SS_NONE :
		{	
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				if( GetSkillUseType() == SUT_NORMAL && 0 < m_dwJumpReserveTime )
				{					
					m_dwPreDelayStartTime = 0;
					EndPreDelayEffect( pChar );
					SetPreActionState( pChar, pStage );
				}
				else
				{					
					m_dwPreDelayStartTime = 0;
					EndPreDelayEffect( pChar );        
					SetActionState( pChar );
				}
			}
		}
		break;
	case SS_PRE_ACTION : 
		{
			if( 0 < m_dwCurJumpReserveTime && m_dwCurJumpReserveTime < dwCurTime )
			{
				SetActionState( pChar );
			}
		}
		break;
	case SS_ACTION : 
		{
			CheckActionState( pChar, pStage );
		}
		break;
	}
}

void ioAreaStructSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	m_dwCurJumpReserveTime = 0;
	
	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	//AreaDestroy
	// 스위치 타입일 경우는 스킬 종료시 영역도 소멸
	if( IsCanSwitchSkill() )
	{
		if ( m_dwAreaWeaponIndex != 0 )
			g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
		if ( !m_AreaWeaponIndexList.empty() )
		{
			for( int i=0 ; i<(int)m_AreaWeaponIndexList.size() ; ++i )
				g_AreaWeaponMgr.DestroyAreaWeapon( m_AreaWeaponIndexList[i] );

			m_AreaWeaponIndexList.clear();
		}
	}

	//DestroyMapEffect
	pOwner->EndMapEffect( m_dwCurMapEffect );
	pOwner->DestroyMagicCircle();

	ioNormalSkill::OnSkillEnd( pOwner );
}

void ioAreaStructSkill::StartPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::StartPreDelayEffect( pChar );
}

void ioAreaStructSkill::EndPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::EndPreDelayEffect( pChar );
}

void ioAreaStructSkill::CreateArea( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( m_AreaWeaponName.IsEmpty() ) return;
	if( !pOwner ) return;

	if( pOwner->IsNeedProcess() )
	{
		ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(),
																"",
																GetName(),
																m_AreaWeaponName,
																m_vCreatePos,
																pOwner->GetTargetRot(),
																ioAreaWeapon::CT_NORMAL );
		if( pArea )
		{
			m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();

			//UPD로 생성 상황 동기화 맞춰줄것
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << SSC_AREA;
				kPacket << m_AreaWeaponName;
				kPacket << m_vCreatePos;
				kPacket << (int)ioAreaWeapon::CT_NORMAL;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioAreaStructSkill::CreateAreaWeapons( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( m_AreaWeapnStyleList.empty() )
		return;
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	for ( int i=0 ; i<(int)m_AreaWeapnStyleList.size() ; ++i )
		g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_AreaWeapnStyleList[i].szAreaWeaponName );
	m_AreaWeaponIndexList.clear();

	D3DXVECTOR3 vPos = m_vCreatePos;
	D3DXQUATERNION qtCharRot = pOwner->GetWorldOrientation();
	for ( int i=0 ; i<(int)m_AreaWeapnStyleList.size() ; ++i )
	{
		D3DXVECTOR3 vAreaWeaponPos = m_vCreatePos + qtCharRot * m_AreaWeapnStyleList[i].vOffset;
		if ( m_AreaWeapnStyleList[i].bChangeHeight )
		{
			float fHeight = pStage->GetMapHeightNoneEntity( vAreaWeaponPos.x, vAreaWeaponPos.z );
			if ( fHeight != 0.0f )
				vAreaWeaponPos.y = fHeight;
		}

		D3DXQUATERNION qtNewRot;
		float fCharYaw = RADtoDEG( ioMath::QuaterToYaw( qtCharRot ) );
		D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(m_AreaWeapnStyleList[i].vRotAngle.y + fCharYaw), DEGtoRAD(m_AreaWeapnStyleList[i].vRotAngle.x), DEGtoRAD(m_AreaWeapnStyleList[i].vRotAngle.z) );

		ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(),
			"",
			GetName(),
			m_AreaWeapnStyleList[i].szAreaWeaponName,
			vAreaWeaponPos,
			qtNewRot,
			ioAreaWeapon::CT_NORMAL );

		if( pArea )
			m_AreaWeaponIndexList.push_back( pArea->GetAreaWeaponIndex() );
	}

	if( !m_AreaWeaponIndexList.empty() )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_AREA_WEAPONS;
			kPacket << m_vCreatePos;
			kPacket << pOwner->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioAreaStructSkill::CreateStruct( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_iBildStruct == 0 )
		return;

	int iDestroyIndex = 0;
	ioPushStruct *pDelete = g_PushStructListMgr.CheckMaxStructCntByNum( pChar, m_iBildStruct, m_iMaxStructCnt );
	if( pDelete )
	{
		iDestroyIndex = pDelete->GetPushArrayIdx();
	}

	pChar->CreateMapEffectBySkill( m_BildEffect, m_vCreatePos );

	if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			ioHashString szItemName;
			if( GetOwnerItem() )
				szItemName = GetOwnerItem()->GetName();

			SP2Packet kPacket( CTPK_PUSHSTRUCT_CREATE );
			kPacket << iDestroyIndex;
			kPacket << m_iBildStruct;
			kPacket << m_vCreatePos;
			kPacket << pChar->GetTargetRot();
			kPacket << pChar->GetCharName();
			kPacket << szItemName;
			kPacket << 0;
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else
	{
		// 싱글모드 뿐만아니라 로비에서도 이쪽으로 오기때문에 주의가 필요함
		if( pDelete )
		{
			pDelete->StructDie( true );
		}

		DWORD dwSeed = timeGetTime();
		int iCurIndex = g_PushStructListMgr.GetCurLocalIndex() + 1;
		pStage->CreatePushStruct( m_iBildStruct,
								  iCurIndex,
								  m_vCreatePos,
								  pChar->GetTargetRot(),
								  pChar->GetCharName(),
								  0,
								  dwSeed,
								  0,
								  true );
	}
}

void ioAreaStructSkill::SetActionState( ioBaseChar *pChar )
{
	m_State = SS_ACTION;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )
		return;

	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pChar->SetReservedSliding( m_vForceInfoList, iCurSkillAniID, fTimeRate, 0 );
	
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	m_dwCreateTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwCreateTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = FRAMEGETTIME();
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	if( m_dwCreateTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	//
	SetExtraAniJump( pChar, iCurSkillAniID, fTimeRate );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioAreaStructSkill::SetPreActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{	
	m_State = SS_PRE_ACTION;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwCurJumpReserveTime = dwCurTime;
	m_dwCurJumpReserveTime += m_dwJumpReserveTime;

	pChar->SetJumpPower( m_dwSkillJumpPower );
	SetSkillUseType( SUT_JUMP );

}

void ioAreaStructSkill::CheckActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{   	
	DWORD dwCurTime = FRAMEGETTIME();

	CheckExtraAniJump( pChar );

	if( m_dwCreateTime != 0 && m_dwCreateTime < dwCurTime )
	{
		CreateArea( pChar, pStage );
		CreateAreaWeapons( pChar, pStage );
		CreateStruct( pChar, pStage );
		CheckDummyChar( pChar, pStage );

		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;

		m_dwCreateTime = 0;
	}
}

void ioAreaStructSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SSC_AREA:
		{
			ioHashString szAreaWeaponName;
			rkPacket >> szAreaWeaponName;

			int iCreateType;
			D3DXVECTOR3 vPos;
			rkPacket >> vPos >> iCreateType;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
																	"",
																	GetName(),
																	szAreaWeaponName,
																	vPos,
																	qtRot,
																	iCreateType );
			if( pArea )
			{
				m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
			}
		}
		break;
	case SSC_DUMMY:
		{
			g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotToRotate( qtRot, true, false );

			int iCreateCnt;
			rkPacket >> iCreateCnt;

			for( int i=0; i < iCreateCnt; ++i )
			{
				int iCurIndex;
				ioHashString szDummyCharName;
				float fStartAngle;
				D3DXVECTOR3 vTargetPos;

				rkPacket >> szDummyCharName >> iCurIndex;
				rkPacket >> fStartAngle;
				rkPacket >> vTargetPos;

				ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
															   pChar->GetCharName(),
															   vTargetPos,
															   fStartAngle,
															   0, false );

				if( pDummy )
					pDummy->SetControlSkillName( GetName() );
			}
		}
		break;
	case SSC_AREA_WEAPONS:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtCharRot;
			rkPacket >> vPos >> qtCharRot;

			if ( m_AreaWeapnStyleList.empty() )
				return;

			for ( int i=0 ; i<(int)m_AreaWeapnStyleList.size() ; ++i )
			{
				D3DXVECTOR3 vAreaWeaponPos = m_vCreatePos + qtCharRot * m_AreaWeapnStyleList[i].vOffset;
				if ( m_AreaWeapnStyleList[i].bChangeHeight )
				{
					float fHeight = pStage->GetMapHeightNoneEntity( vAreaWeaponPos.x, vAreaWeaponPos.z );
					if ( fHeight != 0.0f )
						vAreaWeaponPos.y = fHeight;
				}

				D3DXQUATERNION qtNewRot;
				float fCharYaw = RADtoDEG( ioMath::QuaterToYaw( qtCharRot ) );
				D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(m_AreaWeapnStyleList[i].vRotAngle.y + fCharYaw), DEGtoRAD(m_AreaWeapnStyleList[i].vRotAngle.x), DEGtoRAD(m_AreaWeapnStyleList[i].vRotAngle.z) );

				ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
					"",
					GetName(),
					m_AreaWeapnStyleList[i].szAreaWeaponName,
					vAreaWeaponPos,
					qtNewRot,
					ioAreaWeapon::CT_NORMAL );

				if( pArea )
					m_AreaWeaponIndexList.push_back( pArea->GetAreaWeaponIndex() );
			}
		}
		break;
	}
}

bool ioAreaStructSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioAreaStructSkill::IsAttackEndState() const
{
	switch( m_State )
	{
		case SS_NONE:
		case SS_PRE_ACTION:
			return false;

		case SS_ACTION:
			if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
			{
				return true;
			}
	}

	return false;
}

bool ioAreaStructSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	switch( m_State )
	{
	case SS_NONE:
	case SS_PRE_ACTION:
		return false;

	case SS_ACTION:
		if( m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
			return true;
	}

	return false;
}

void ioAreaStructSkill::CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;
	if( !pStage ) return;

	if( m_DummyCharList.empty() )
		return;

	pChar->CreateMapEffectBySkill( m_DummyCharEffect, m_vCreatePos );

	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	//
	vDummyCharCreateInfoList vCreateList;
	vCreateList.clear();

	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	int iSize = m_DummyCharList.size();
	for( int i=0; i < iSize; ++i )
	{
		ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
		float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
		float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

		D3DXVECTOR3 vTargetPos = m_vCreatePos + (fOffset * vDir);
		vTargetPos += fSideOffset * vSideDir;

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
													   pChar->GetCharName(),
													   vTargetPos,
													   fStartAngle,
													   0, true );

		if( pDummy )
			pDummy->SetControlSkillName( GetName() );

		DummyCharCreateInfo kInfo;
		kInfo.m_DummyCharName = szDummyCharName;
		kInfo.m_iDummyCharIdx = iCurIndex;
		kInfo.m_fDummyCharStartAngle = fStartAngle;
		kInfo.m_vTargetPos = vTargetPos;

		vCreateList.push_back( kInfo );
	}

	int iCreateCnt = vCreateList.size();
	if( pChar->IsNeedSendNetwork() && iCreateCnt > 0 )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_DUMMY;
		kPacket << pChar->GetTargetRot();

		kPacket << iCreateCnt;

		for( int i=0; i < iCreateCnt; ++i )
		{
			kPacket << vCreateList[i].m_DummyCharName;
			kPacket << vCreateList[i].m_iDummyCharIdx;
			kPacket << vCreateList[i].m_fDummyCharStartAngle;
			kPacket << vCreateList[i].m_vTargetPos;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
