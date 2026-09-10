
#include "stdafx.h"

#include "ioDummyCrane.h"
#include "ioDummySubCrane.h"

ioDummyCrane::ioDummyCrane( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_DummyCraneState = DAS_Start;
	m_bSetRise = false;
	m_bSetUpKey	= false;
	m_bSetDownKey = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_nSkillActiveCurCnt = 0;
	m_dwCurReserveEndTime = 0;
}

ioDummyCrane::~ioDummyCrane()
{
}

void ioDummyCrane::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	
	rkLoader.LoadString_e( "delay_ani_name", "", szBuf, MAX_PATH );
	m_szDelayAniName = szBuf;
	rkLoader.LoadString_e( "move_ani_name", "", szBuf, MAX_PATH );
	m_szMoveAniName = szBuf;
	rkLoader.LoadString_e( "rise_ani_name", "", szBuf, MAX_PATH );
	m_szRiseAniName = szBuf;

	rkLoader.LoadString_e( "action_ani_name", "", szBuf, MAX_PATH );
	m_szActionAniName = szBuf;
	
	rkLoader.LoadString_e( "sub_dummy_name", "", szBuf, MAX_PATH );
	m_szSubDummyName = szBuf;

	m_vSubDummyOffset.x = rkLoader.LoadFloat_e( "sub_dummy_offset_x", 0.0f );
	m_vSubDummyOffset.y = rkLoader.LoadFloat_e( "sub_dummy_offset_y", 0.0f );
	m_vSubDummyOffset.z = rkLoader.LoadFloat_e( "sub_dummy_offset_z", 0.0f );

	m_fMinDownHeight = rkLoader.LoadFloat_e( "min_down_height", 0.0f );

	rkLoader.LoadString_e( "grap_buff", "", szBuf, MAX_PATH );
	m_szGrapBuff = szBuf;

	m_nSkillActiveMaxCnt = rkLoader.LoadInt_e( "skill_active_max_cnt", 0 );

	m_dwReserveEndTime = rkLoader.LoadInt_e( "reserve_end_time", 0 );
}

void ioDummyCrane::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	if ( m_dwCurReserveEndTime > 0 && m_dwCurReserveEndTime < FRAMEGETTIME() )
	{
		SetDieState( true );
		return;
	}

	switch( m_DummyCraneState )
	{
	case DAS_Start:
		{
			if( !m_DummyEffect.IsEmpty() )
			{
				m_pGroup->AttachEffect( m_DummyEffect, NULL );
				m_bMoveEffectCreated = true;
			}

			SetState( m_szDelayAniName, DAS_Delay );

			CreateSubDummy();
		}
		break;
	case DAS_Delay:
		ProcessDelayState();
		break;
	case DAS_Move:
		ProcessMoveState();
		break;
	case DAS_Rise:
		ProcessRiseState();
		break;
	}
	
	if ( m_DummyCraneState != DAS_Sub_Action )
	{
		CheckRiseInputKey();
		CheckMove( fTimePerSec );
		CheckRise( fTimePerSec );
	}
}

void ioDummyCrane::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	if( !pStage ) 
		return;	
	
	int iType;
	rkPacket >> iType;	

	switch ( iType )
	{
	case DAS_Delay:
		SetDelayState( pOwner );
		break;
	case DAS_Move:
		SetMoveState( pOwner );
		break;
	case DAS_Rise:
		SetRiseState( true, pOwner );
		break;
	case DAS_Change_Key:
		{
			rkPacket >> m_bSetUpKey;
			rkPacket >> m_bSetDownKey;

			int nKey;
			rkPacket >> nKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nKey;

			D3DXVECTOR3 vNewPos;
			rkPacket >> vNewPos;

			D3DXQUATERNION qtNewRot;
			rkPacket >> qtNewRot;

			SetWorldPosition( vNewPos );
			SetWorldOrientation( qtNewRot );
		}
		break;
	case DAS_Create_Sub_Dummy:
		{
			int nCurIndex;
			rkPacket >> nCurIndex;
			
			g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_szSubDummyName );

			D3DXVECTOR3 vPos = GetWorldPosition() + GetWorldOrientation() * m_vSubDummyOffset;

			ioDummyChar *pDummy = pStage->CreateDummyChar( m_szSubDummyName, nCurIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
			if ( pDummy )
			{
				pDummy->SetWorldOrientationAndDir( GetWorldOrientation() );
				pDummy->SetWorldPosition( vPos );
			}
		}
		break;
	case DAS_Sub_Action:
		{
			D3DXVECTOR3 vNewPos;
			rkPacket >> vNewPos;

			D3DXQUATERNION qtNewRot;
			rkPacket >> qtNewRot;

			SetWorldPosition( vNewPos );
			SetWorldOrientation( qtNewRot );

			SetActionState( pOwner );
		}
		break;
	case DAS_End:
		SetDieState( true );
		break;
	}
}

bool ioDummyCrane::IsActivateWeapon()
{
	return true;
}

ApplyWeaponExp ioDummyCrane::ApplyWeapon( ioWeapon *pWeapon )
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
		SetDieState( true );
	}

	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	return AWE_EXPLOSION;
}

void ioDummyCrane::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
}

void ioDummyCrane::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ProcessEnd();

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioDummyCrane::IsNeedCheckMapCollision() const
{
	return !m_bDisableMapCollision;
}

int ioDummyCrane::DontMoveEntityLevel() const
{
	return 1;
}

int ioDummyCrane::GetSkillMaxActiveCnt()
{
	return m_nSkillActiveMaxCnt;
}

int ioDummyCrane::GetSkillCurActiveCnt()
{
	return m_nSkillActiveCurCnt;
}

void ioDummyCrane::SetState( ioHashString szAniName, DummyCraneActionState eState )
{
	if ( !m_pGroup || szAniName.IsEmpty() )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( szAniName );
	if( iAniID == -1 )
		return;
	
	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT100 );

	m_DummyCraneState = eState;
}

void ioDummyCrane::SetDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	SetState( m_szDelayAniName, DAS_Delay );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DAS_Delay;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyCrane::SetMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	SetState( m_szMoveAniName, DAS_Move );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DAS_Move;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyCrane::SetRiseState( bool bRise, ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_bSetRise = bRise;

	if ( bRise )
	{
		SetState( m_szRiseAniName, DAS_Rise );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DAS_Rise;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioDummyCrane::SetActionState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	bool bFirstAttack = true;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin(); iter!=rkCharList.end(); ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )	
			continue;

		if( pTarget == pOwner )
			continue;

		if ( pTarget->GetTeam() == pOwner->GetTeam() )
			continue;

		ioBuff *pFindBuff = pTarget->GetBuff( m_szGrapBuff, pOwner->GetCharName() );
		if ( pFindBuff )
		{
			pFindBuff->SetReserveEndBuff();
			bFirstAttack = false;
		}
	}

	if ( bFirstAttack && m_nSkillActiveCurCnt < m_nSkillActiveMaxCnt )
	{
		SetState( m_szActionAniName, DAS_Sub_Action );

		ioDummySubCrane *pDummySubCrane = ToDummySubCrane( g_DummyCharMgr.FindDummyCharToName( pOwner, m_szSubDummyName ) );
		if ( pDummySubCrane )
			pDummySubCrane->SetAttackState();

		m_nSkillActiveCurCnt++;
		if ( m_nSkillActiveCurCnt >= m_nSkillActiveMaxCnt )
			m_dwCurReserveEndTime = FRAMEGETTIME() + m_dwReserveEndTime;
	}
	else if ( bFirstAttack && m_nSkillActiveCurCnt >= m_nSkillActiveMaxCnt )
	{
		SetDieState( true );
		return;
	}
	else if ( !bFirstAttack && m_nSkillActiveCurCnt >= m_nSkillActiveMaxCnt )
	{
		SetDieState( true );
		return;
	}

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DAS_Sub_Action;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyCrane::ProcessDelayState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	if ( pOwner->IsSettedDirection() )
		SetMoveState( pOwner );
	else if ( pOwner->IsAttackKey() )
		SetActionState( pOwner );
}

void ioDummyCrane::ProcessMoveState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( !pOwner->IsSettedDirection() )
		SetDelayState( pOwner );
}

void ioDummyCrane::ProcessRiseState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() || m_bSetRise )
		return;

	if ( pOwner->IsSettedDirection() )
		SetMoveState( pOwner );
	else
		SetDelayState( pOwner );
}

void ioDummyCrane::ProcessEnd()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_szSubDummyName );	

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin(); iter!=rkCharList.end(); ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )	
			continue;

		if( pTarget == pOwner )
			continue;

		if ( pTarget->GetTeam() == pOwner->GetTeam() )
			continue;

		ioBuff *pFindBuff = pTarget->GetBuff( m_szGrapBuff, pOwner->GetCharName() );
		if ( pFindBuff )
			pFindBuff->SetReserveEndBuff();
	}
}

void ioDummyCrane::CheckMove( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	bool bMove = false;
	D3DXVECTOR3 vCurDir = ioMath::VEC3_ZERO;
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		switch( m_CurDirKey )
		{
		case ioUserKeyInput::DKI_UP:
			vCurDir	+= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_DOWN:
			vCurDir	-= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_LEFT:
			vCurDir	-= ioMath::UNIT_X;
			break;
		case ioUserKeyInput::DKI_RIGHT:
			vCurDir	+= ioMath::UNIT_X;
			break;
		case ioUserKeyInput::DKI_LEFTDOWN:
			vCurDir	-= ioMath::UNIT_X;	vCurDir	-= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_LEFTUP:
			vCurDir	-= ioMath::UNIT_X;	vCurDir	+= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_RIGHTDOWN:
			vCurDir	+= ioMath::UNIT_X;	vCurDir	-= ioMath::UNIT_Z;
			break;
		case ioUserKeyInput::DKI_RIGHTUP:
			vCurDir	+= ioMath::UNIT_X;	vCurDir	+= ioMath::UNIT_Z;
			break;
		};
		bMove = true;
	}
	
	if( !bMove )
		return;
	
	D3DXVec3Normalize( &vCurDir, &vCurDir );
	D3DXVECTOR3 vTotalMove = ioMath::VEC3_ZERO;

	float fMoveSpeed = m_fMoveSpeed * fTimePerSec;
	vTotalMove = vCurDir * fMoveSpeed;	
	
	// 더미 회전
	if( vCurDir != ioMath::VEC3_ZERO )
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vCurDir );
		SetWorldOrientation( qtRot );
	}

	bool bCol = false;
	if( pStage->ReCalculateMoveVectorByTerrain( this, &vTotalMove, bCol ) )
		Translate( vTotalMove );

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	SetWorldPosition( vCurPos );
}

void ioDummyCrane::CheckRise( float fTimePerSec )
{	
	if ( !m_bSetUpKey && !m_bSetDownKey )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	float fMapHeight = GetBottomHeight();

	float fUpDownSpeed = m_fUpDownSpeed * fTimePerSec;
	D3DXVECTOR3 vCurPos = GetWorldPosition();	

	if( m_bSetUpKey )
		vCurPos.y += fUpDownSpeed;

	if( fMapHeight > 0.0f && m_bSetDownKey )
		vCurPos.y -= fUpDownSpeed;

	ioOrientBox kBox = GetWorldCollisionBox();
	float fDownHeight = vCurPos.y - kBox.GetExtents(1);
	if( fMapHeight > 0.0f && ( fDownHeight < fMapHeight + m_fMinDownHeight ) )
		vCurPos.y = fMapHeight + kBox.GetExtents(1) + m_fMinDownHeight;

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	SetWorldPosition( vCurPos );
}

void ioDummyCrane::CheckRiseInputKey()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	bool bSend = false;
	if( pOwner->IsJumpKeyDown() )
	{
		if( !m_bSetUpKey )
		{
			SetRiseState( true, pOwner );
			bSend = true;
		}

		m_bSetUpKey	= true;
		m_bSetDownKey = false;
	}
	else if( pOwner->IsDefenseKeyDown() )
	{
		if( !m_bSetDownKey )
		{
			SetRiseState( true, pOwner );
			bSend = true;	
		}

		m_bSetUpKey	= false;
		m_bSetDownKey = true;
	}
	else
	{
		if( m_bSetUpKey || m_bSetDownKey )
		{
			SetRiseState( false, pOwner );
			bSend = true;			
		}

		m_bSetUpKey	= false;
		m_bSetDownKey = false;
	}

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_CurDirKey = eNewDirKey;
		bSend = true;	
	}

	if ( bSend && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DAS_Change_Key;
		kPacket << m_bSetUpKey;
		kPacket << m_bSetDownKey;
		kPacket << (int)m_CurDirKey;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyCrane::CreateSubDummy()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_szSubDummyName );

	int nCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	D3DXVECTOR3 vPos = GetWorldPosition() + GetWorldOrientation() * m_vSubDummyOffset;

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szSubDummyName, nCurIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
	if ( pDummy )
	{
		pDummy->SetWorldOrientationAndDir( GetWorldOrientation() );
		pDummy->SetWorldPosition( vPos );
	}

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DAS_Create_Sub_Dummy;
		kPacket << nCurIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyCrane::ResetCraneState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	bool bGrap = false;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin(); iter!=rkCharList.end(); ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )	
			continue;

		if( pTarget == pOwner )
			continue;

		if ( pTarget->GetTeam() == pOwner->GetTeam() )
			continue;

		ioBuff *pFindBuff = pTarget->GetBuff( m_szGrapBuff, pOwner->GetCharName() );
		if ( pFindBuff )
			bGrap = true;
	}

	if ( !bGrap && m_nSkillActiveCurCnt >= m_nSkillActiveMaxCnt )
	{
		SetDieState( true );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DAS_End;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	if ( pOwner->IsSettedDirection() )
		SetMoveState( pOwner );
	else
		SetDelayState( pOwner );
}