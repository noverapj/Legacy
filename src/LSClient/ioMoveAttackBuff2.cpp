
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioMoveAttackBuff2.h"

#include "WeaponDefine.h"

ioMoveAttackBuff2::ioMoveAttackBuff2()
{
	Init();
}

ioMoveAttackBuff2::ioMoveAttackBuff2( const ioMoveAttackBuff2 &rhs )
	: ioBuff( rhs ),
      m_bEnableWeaponSkill( rhs.m_bEnableWeaponSkill ),
	  m_bEnableArmorSkill( rhs.m_bEnableArmorSkill ),
	  m_bEnableHelmetSkill( rhs.m_bEnableHelmetSkill ),
	  m_bEnableCloakSkill( rhs.m_bEnableCloakSkill ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_RemoveBuffList( rhs.m_RemoveBuffList ),
	  m_fStartJumpPower( rhs.m_fStartJumpPower ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_dwCameraEvent( rhs.m_dwCameraEvent ),
	  m_szCameraBuff( rhs.m_szCameraBuff ),
	  m_EnableInputKeyState( rhs.m_EnableInputKeyState ),
	  m_BuffEndState( rhs.m_BuffEndState ),
	  m_NormalAttack( rhs.m_NormalAttack ),
	  m_SubWeaponInfoList( rhs.m_SubWeaponInfoList ),
	  m_dwSubWeaponRotateSpeed( rhs.m_dwSubWeaponRotateSpeed ),
	  m_nMaxWeaponGrade( rhs.m_nMaxWeaponGrade ),
	  m_dwWeaponGradeTime( rhs.m_dwWeaponGradeTime )
{
	Init();
}

ioMoveAttackBuff2::~ioMoveAttackBuff2()
{	
	m_EnableInputKeyState.clear();
	m_BuffEndState.clear();
}

void ioMoveAttackBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH] = "";
	int nCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	char szRemoveBuf[MAX_PATH] = "";
	for (int i=0; i<nCnt; ++i)
	{
		wsprintf_e( szRemoveBuf, "remove_buff%d_name",i+1);
		rkLoader.LoadString( szRemoveBuf, "", szBuf, MAX_PATH );
		m_RemoveBuffList.push_back( szBuf );
	}

	m_bEnableWeaponSkill = rkLoader.LoadBool_e( "enable_weapon_skill", false );
	m_bEnableArmorSkill = rkLoader.LoadBool_e( "enable_armor_skill", false );
	m_bEnableHelmetSkill = rkLoader.LoadBool_e( "enable_helmet_skill", false );
	m_bEnableCloakSkill = rkLoader.LoadBool_e( "enable_cloak_skill", false );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	
	m_dwCameraEvent = rkLoader.LoadInt_e( "camera_event", 0 );

	rkLoader.LoadString_e( "camera_buff", "", szBuf, MAX_PATH );
	m_szCameraBuff = szBuf;

	//추가 읽기
	LoadExtraProperty( rkLoader );
}

ioBuff* ioMoveAttackBuff2::Clone()
{
	return new ioMoveAttackBuff2( *this );
}

void ioMoveAttackBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	Init();

	m_iMaxActiveCount = m_nMaxWeaponGrade;
	m_iCurActiveCount = m_nCurWeaponGrade+1;

	if ( pOwner->IsNeedProcess() )
	{
		SetCameraEvent( pOwner );
		SetCameraBuff( pOwner );
	}

	pOwner->SetCurMoveSpeed(0.0f);
	pOwner->SetJumpPower(0.0f);
	pOwner->SetGravityAmt( 0.0f );
	//공중에 있는 상태
	if ( pOwner->IsFloating() )
	{
		if ( m_fStartJumpPower > 0.0f )
			pOwner->SetSKillEndJumpState( m_fStartJumpPower, false, false, false, true );
	}

	CreateSubWeaponEffect( pOwner );
}

bool ioMoveAttackBuff2::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	Init();

	m_iMaxActiveCount = m_nMaxWeaponGrade;
	m_iCurActiveCount = m_nCurWeaponGrade+1;

	pOwner->SetCurMoveSpeed(0.0f);
	pOwner->SetJumpPower(0.0f);
	pOwner->SetGravityAmt( 0.0f );
	//공중에 있는 상태
	if ( pOwner->IsFloating() )
	{
		if ( m_fStartJumpPower > 0.0f )
			pOwner->SetSKillEndJumpState( m_fStartJumpPower, false, false, false, true );
	}

	CreateSubWeaponEffect( pOwner );

	return true;
}

void ioMoveAttackBuff2::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( CheckWoundState( m_pOwner ) || CheckBuffEndState( m_pOwner ) )
	{
		SetReserveEndBuff();
		return;
	}
	
	switch ( m_BuffState )
	{
	case BS_Delay:
		{
			m_nCurWeaponGrade = ( dwCurTime - m_dwBuffStartTime ) / m_dwWeaponGradeTime;
			m_nCurWeaponGrade = min( m_nCurWeaponGrade, m_nMaxWeaponGrade-1 );
			m_iCurActiveCount = m_nCurWeaponGrade+1;

			UpdateSubWeaponEffect( m_pOwner );
			CheckInputKey( m_pOwner );
		}
		break;
	case BS_Attack:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				SetReserveEndBuff();
				return;
			}
			else
			{
				if ( m_dwFireTime > 0 && m_dwFireTime < dwCurTime )
				{
					CreateSubWeapon( m_pOwner );
					m_dwFireTime = 0;
				}
			}
		}
		break;
	}
}

void ioMoveAttackBuff2::EndBuff()
{	
	ioBuff::EndBuff();
	
	if ( m_pOwner->IsNeedProcess() )
	{
		EndCameraBuff( m_pOwner );
		EndCameraEvent( m_pOwner );
	}
	
	DestroySubWeaponEffect( m_pOwner );

	//피격 상태가 아니면서
	if ( !m_bWound )
	{
		m_pOwner->SetCurMoveSpeed(0.0f);
		m_pOwner->SetJumpPower(0.0f);
		m_pOwner->SetGravityAmt( 0.0f );
		//공중에 있는 상태
		if ( m_pOwner->IsFloating() )
		{
			if ( m_fEndJumpPower > 0.0f )
				m_pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false );
		}
		else
			m_pOwner->SetState( CS_DELAY );
	}

	//버프 제거
	for ( int i=0; i<(int)m_RemoveBuffList.size(); ++i )
		m_pOwner->RemoveBuff( m_RemoveBuffList[i], true );
	m_RemoveBuffList.clear();

	ioItem *pItem = GetOwnerItem();
	if ( pItem )
		pItem->SetCurSkillGauge( 0.0f );
}

void ioMoveAttackBuff2::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int nState;
	rkPacket >> nState;

	switch ( nState )
	{
	case BS_Attack:
		{
			DWORD dwWeaponIdxBase;
			rkPacket >> dwWeaponIdxBase;

			pOwner->SetWeaponIndexBase( dwWeaponIdxBase );

			int nCurWeaponGrade;
			rkPacket >> nCurWeaponGrade;

			int nCurKey;
			rkPacket >> nCurKey;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			int nSubWeaponCnt;
			rkPacket >> nSubWeaponCnt;

			for ( int i=0; i<nSubWeaponCnt; i++ )
			{
				bool bLive;
				rkPacket >> bLive;

				if ( bLive )
				{
					int nIndex;
					rkPacket >> nIndex;

					D3DXVECTOR3 vEffectPos;
					rkPacket >> vEffectPos;

					D3DXQUATERNION qtEffectRot;
					rkPacket >> qtEffectRot;

					DWORD dwEffectID = m_SubWeaponEffectList[nIndex];
					ioMapEffect *pSubWeaponEffect = pStage->FindMapEffect( dwEffectID );
					if ( pSubWeaponEffect )
					{
						pSubWeaponEffect->SetWorldPosition( vEffectPos );
						pSubWeaponEffect->SetWorldOrientation( qtEffectRot );
					}
				}
			}

			SetAttackState( pOwner );
			m_nCurWeaponGrade = nCurWeaponGrade;
		}
		break;
	}
}

bool ioMoveAttackBuff2::CheckEnableUseSkill( int iSlot )
{
	switch( iSlot )
	{
	case ES_WEAPON:
		if( m_bEnableWeaponSkill )
			return true;
		break;
	case ES_ARMOR:
		if( m_bEnableArmorSkill )
			return true;
		break;
	case ES_HELMET:
		if( m_bEnableHelmetSkill )
			return true;
		break;
	case ES_CLOAK:
		if( m_bEnableCloakSkill )
			return true;
		break;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////

void ioMoveAttackBuff2::Init()
{
	m_dwMotionEndTime = 0;
	m_dwFireTime = 0;
	m_bWound = false;
	m_fCurSubWeaponAngle = 0.0f;
	m_nCurWeaponGrade = 0;
	m_BuffState = BS_Delay;
}

void ioMoveAttackBuff2::LoadExtraProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];		
	m_fStartJumpPower = rkLoader.LoadFloat_e( "start_jump_power", 0.0f );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );
	
	m_EnableInputKeyState.clear();
	int nMaxState = rkLoader.LoadInt_e( "max_enable_input_state", 0 );
	for ( int i=0; i<nMaxState; ++i )
	{
		wsprintf_e( szBuf, "enable_input_state%d", i+1 );
		int nState = rkLoader.LoadInt( szBuf, -1 );
		m_EnableInputKeyState.push_back( nState );
	}

	m_BuffEndState.clear();
	nMaxState = rkLoader.LoadInt_e( "max_buff_end_state", 0 );
	for ( int i=0; i<nMaxState; ++i )
	{
		wsprintf_e( szBuf, "buff_end_state%d", i+1 );
		int nState = rkLoader.LoadInt( szBuf, -1 );
		m_BuffEndState.push_back( nState );
	}

	LoadAttackAttribute( "normal_attack", m_NormalAttack, rkLoader );

	m_SubWeaponInfoList.clear();
	int nMaxSubCnt = rkLoader.LoadInt_e( "max_sub_weapon_cnt", 0 );
	for ( int i=0; i<nMaxSubCnt; i++ )
	{
		SubWeaponInfo sSubWeaponInfo;
		wsprintf_e( szKey, "sub_weapon%d_effect",i+1);
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sSubWeaponInfo.m_szWeaponEffect = szBuf;

		wsprintf_e( szKey, "sub_weapon%d_angle", i+1 );
		sSubWeaponInfo.m_fWeaponAngle = rkLoader.LoadFloat( szKey, 0.0f );

		D3DXVECTOR3 vPos;
		wsprintf_e( szKey, "sub_weapon%d_offset_x", i+1 );
		vPos.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "sub_weapon%d_offset_y", i+1 );
		vPos.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "sub_weapon%d_offset_z", i+1 );
		vPos.z = rkLoader.LoadFloat( szKey, 0.0f );
		sSubWeaponInfo.m_vWeaponPos = vPos;

		wsprintf_e( szKey, "sub_weapon%d_attack_type_count", i+1 );
		int nMaxAttackCnt = rkLoader.LoadInt( szKey, 0 );
		for ( int j=0; j<nMaxAttackCnt; j++ )
		{
			WeaponInfo sWeaponInfo;
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d", i+1, j+1 );
			sWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d_resist", i+1, j+1 );
			sWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d_wounded", i+1, j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			sWeaponInfo.m_WoundedAnimation = szBuf;
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d_wounded_time", i+1, j+1 );
			sWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d_wounded_loop_ani", i+1, j+1 );
			sWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

			sSubWeaponInfo.m_WeaponInfoList.push_back( sWeaponInfo );
		}

		m_SubWeaponInfoList.push_back( sSubWeaponInfo );
	}

	m_dwSubWeaponRotateSpeed = rkLoader.LoadInt_e( "sub_weapon_rotate_speed", 0 );
	m_nMaxWeaponGrade = rkLoader.LoadInt_e( "max_weapon_grade", 1 );
	m_dwWeaponGradeTime = rkLoader.LoadInt_e( "weapon_grade_time", 0 );
}

bool ioMoveAttackBuff2::CheckWoundState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	CharState eState = pOwner->GetState();
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && pOwner->GetBlowFrozenState() )
			m_bWound = true;
		else if( eState == CS_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			m_bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			m_bWound = true;
		else if( eState == CS_ETC_STATE && pOwner->IsApplyDownState(false) )
			m_bWound = true;
		else if( eState == CS_STOP_MOTION && pOwner->GetBlowStopMotionState() )
			m_bWound = true;
		else if( eState == CS_ICE_STATE && pOwner->GetBlowStopMotionState() )
			m_bWound = true;
	}

	if( m_bEnableWoundCancel )
	{
		switch( eState )
		{
		case CS_BLOW_WOUNDED:
		case CS_BOUND_BLOW_WOUNDED:
		case CS_WOUNDED:
		case CS_DROP_ZONE_DOWN:
			m_bWound = true;
			break;
		}

		if( m_bWound )
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioMoveAttackBuff2::CheckEnableKey( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	int nOwnerState = (int)pOwner->GetState();
	for ( int i=0; i<(int)m_EnableInputKeyState.size(); ++i )
	{
		if ( nOwnerState == m_EnableInputKeyState[i] )
			return true;
	}
	
	return false;
}

bool ioMoveAttackBuff2::CheckBuffEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	int nOwnerState = (int)pOwner->GetState();
	for ( int i=0; i<(int)m_BuffEndState.size(); ++i )
	{
		if ( nOwnerState == m_BuffEndState[i] )
		{
			m_bWound = true;
			return true;
		}
	}

	return false;
}

void ioMoveAttackBuff2::CheckInputKey(  ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() || !CheckEnableKey( pOwner ) )
		return;
	
	if ( CheckAttackKey( pOwner ) )
		SetAttackState( pOwner );
}

bool ioMoveAttackBuff2::CheckAttackKey( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( pOwner->IsAttackKey() )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////

void ioMoveAttackBuff2::SetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	pOwner->SetState( CS_BUFF_ACTION );
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_NormalAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_NormalAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_NormalAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_NormalAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	//pOwner->RefreshFireTimeList( iAniID, m_NormalAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_BuffState = BS_Attack;
	
	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << BS_Attack;
			kPacket << pOwner->GetWeaponIndexBase();
			kPacket << m_nCurWeaponGrade;
			kPacket << (int)eNewDirKey;
			kPacket << (int)m_SubWeaponEffectList.size();

			for ( int i=0; i<(int)m_SubWeaponEffectList.size(); i++ )
			{
				DWORD dwEffectID = m_SubWeaponEffectList[i];
				ioMapEffect *pSubWeaponEffect = pStage->FindMapEffect( dwEffectID );
				if ( pSubWeaponEffect )
				{
					kPacket << true;

					kPacket << i;
					kPacket << pSubWeaponEffect->GetWorldPosition();
					kPacket << pSubWeaponEffect->GetWorldOrientation();
				}
				else 
					kPacket << false;
			}
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void ioMoveAttackBuff2::CreateSubWeaponEffect( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	for ( int i=0; i<(int)m_SubWeaponInfoList.size(); i++ )
	{
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + pOwner->GetWorldOrientation() * m_SubWeaponInfoList[i].m_vWeaponPos;
		DWORD dwEffectID = CreateEffect( pOwner, m_SubWeaponInfoList[i].m_szWeaponEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
		m_SubWeaponEffectList.push_back( dwEffectID );
	}
}

void ioMoveAttackBuff2::DestroySubWeaponEffect( ioBaseChar *pOwner )
{
	if ( !pOwner || m_SubWeaponEffectList.empty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	for ( int i=0; i<(int)m_SubWeaponEffectList.size(); i++ )
	{
		DWORD dwEffectID = m_SubWeaponEffectList[i];
		DestroyEffect( pOwner, dwEffectID );
	}
	m_SubWeaponEffectList.clear();
}

void ioMoveAttackBuff2::UpdateSubWeaponEffect( ioBaseChar *pOwner )
{
	if ( !pOwner || m_SubWeaponEffectList.empty() || m_dwSubWeaponRotateSpeed == 0 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	int nEffectCnt = (int)m_SubWeaponEffectList.size();
	if ( nEffectCnt > 0 && nEffectCnt <= (int)m_SubWeaponInfoList.size() )
	{
		for ( int i=0; i<nEffectCnt; i++ )
		{
			DWORD dwEffectID = m_SubWeaponEffectList[i];
			ioMapEffect *pSubWeaponEffect = pStage->FindMapEffect( dwEffectID );
			if ( pSubWeaponEffect )
			{
				float fFrameGap = g_FrameTimer.GetCurFrameGap();
				float fRate = fFrameGap / m_dwSubWeaponRotateSpeed;
				float fRotateAngle = 360.0f * fRate;

				m_fCurSubWeaponAngle += fRotateAngle;
				if ( m_fCurSubWeaponAngle >= 360.0f )
				{
					m_fCurSubWeaponAngle -= 360.0f;
					m_fCurSubWeaponAngle = max( m_fCurSubWeaponAngle, 0.0f );
				}

				float fYawR = DEGtoRAD( m_fCurSubWeaponAngle );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				D3DXVECTOR3 vNewPos = pOwner->GetMidPositionByRate() + qtRotate * m_SubWeaponInfoList[i].m_vWeaponPos;

				D3DXQUATERNION qtWeaponRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtWeaponRotate, DEGtoRAD( m_SubWeaponInfoList[i].m_fWeaponAngle ), 0.0f, 0.0f );
				D3DXQuaternionMultiply( &qtRotate, &qtRotate, &qtWeaponRotate );

				pSubWeaponEffect->SetWorldPosition( vNewPos );
				pSubWeaponEffect->SetWorldOrientation( qtRotate );
			}
		}
	}
}

DWORD ioMoveAttackBuff2::CreateEffect( ioBaseChar *pOwner, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pOwner || szEffect.IsEmpty() )
		return -1;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

void ioMoveAttackBuff2::DestroyEffect( ioBaseChar *pOwner, DWORD& dwEffectID )
{
	if ( !pOwner || dwEffectID == -1 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

//////////////////////////////////////////////////////////////////////////////////////

void ioMoveAttackBuff2::CreateSubWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	DWORD dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();

	int nEffectCnt = (int)m_SubWeaponEffectList.size();
	if ( nEffectCnt > 0 && nEffectCnt <= (int)m_SubWeaponInfoList.size() )
	{
		for ( int i=0; i<nEffectCnt; i++ )
		{
			DWORD dwEffectID = m_SubWeaponEffectList[i];
			ioMapEffect *pSubWeaponEffect = pStage->FindMapEffect( dwEffectID );
			if ( pSubWeaponEffect )
			{
				WeaponInfo sWeaponInfo = m_SubWeaponInfoList[i].m_WeaponInfoList[m_nCurWeaponGrade];

				FireTime kFireTime;
				kFireTime.dwStart = FRAMEGETTIME();
				kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
				kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
				kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
				kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
				kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;
				kFireTime.dwWeaponIdx = dwWeaponIdx++;
				kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

				D3DXVECTOR3 vPos = pSubWeaponEffect->GetWorldPosition();
				D3DXVECTOR3 vDir = pSubWeaponEffect->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVec3Normalize( &vDir, &vDir );

				pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			}
		}
	}

	DestroySubWeaponEffect( pOwner );
}

//////////////////////////////////////////////////////////////////////////////////
//카메라 설정
void ioMoveAttackBuff2::SetCameraBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_szCameraBuff.IsEmpty() )
		pOwner->ReserveAddNewBuff( m_szCameraBuff, "", "", NULL );
}

void ioMoveAttackBuff2::EndCameraBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->RemoveBuff( m_szCameraBuff );
}

void ioMoveAttackBuff2::SetCameraEvent( ioBaseChar *pOwner )
{
	if ( !pOwner)
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	g_CameraEventMgr.CreateCE( m_dwCameraEvent, pStage );
}

void ioMoveAttackBuff2::EndCameraEvent( ioBaseChar *pOwner )
{
	ioCameraEvent* pCamera = g_CameraEventMgr.GetCurCameraEvent();
	if( pCamera && pCamera->GetCECode() == m_dwCameraEvent )
	{
		g_CameraEventMgr.NotifyCEDestroyed();
	}
}