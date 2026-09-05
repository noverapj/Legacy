
#include "stdafx.h"

#include "ioBall.h"
#include "ioGaugeHelper.h"
#include "WeaponDefine.h"
#include "ioVoiceChat/Fourier.h"

#define MAX_MAP_CONTINUOUS_COUNT 10

ioBall::ioBall( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioPlayEntity( pGrp, pMode )
{
	m_iArrayIndex = 0;
	m_State = BS_DELAY;

	m_ArmorType = AT_UNARMOR;

	m_fGravityAmt = 0.0f;

	m_fCurMoveSpeed = 0.0f;

	m_dwCreateTime = FRAMEGETTIME();
	m_dwProcessTime = FRAMEGETTIME();
	m_dwBallTouchTime = 0;

	m_bMapCollisionPushed = false;

	m_iMapTranslateContinuousCount = 0;
	m_dwMapTranslateFrame = 0;

	m_iAnimationIndex = -1;

	Help::InitColorValue( m_OutLineColor, 0.0f, 0.0f, 0.0f, FLOAT1 );
	m_fOutLineSize = FLOAT1;
	m_bOutLineModified = false;
	
	m_bSendReposition = false;
	m_bGoalState = false;

	m_dwBallEffect = -1;
	m_dwShadowEffect = -1;

	m_LastAttacker.Clear();
	m_PreAttacker.Clear();
}

ioBall::~ioBall()
{
	ioPlayStage *pStage = NULL;
	if( m_pModeParent )
		pStage = m_pModeParent->GetCreator();

	if( pStage )
	{
		if( m_dwBallEffect != -1 )
		{
			ioMapEffect *pEff = pStage->FindMapEffect( m_dwBallEffect );
			if( pEff )
			{
				pEff->EndEffectForce();
			}
		}

		if( m_dwShadowEffect != -1 )
		{
			ioMapEffect *pEff = pStage->FindMapEffect( m_dwShadowEffect );
			if( pEff )
			{
				pEff->EndEffectForce();
			}
		}
	}
}

void ioBall::LoadProperty( ioINILoader &rkLoader )
{
	m_ArmorType	    = (ArmorType)rkLoader.LoadInt_e( "armor_type", AT_UNARMOR );	

	m_fMaxMoveSpeed = rkLoader.LoadFloat_e( "max_move_speed", 0.0f );
	m_fMaxBoundAmt = rkLoader.LoadFloat_e( "max_bound_power", 0.0f );

	m_fMoveSpeedAddRate = rkLoader.LoadFloat_e( "move_speed_add_rate", FLOAT1 );
	m_fBoundAmtAddRate = rkLoader.LoadFloat_e( "bound_amt_add_rate", FLOAT1 );

	m_fLimitBoundAmt = rkLoader.LoadFloat_e( "limit_bound_amt", 0.0f );
	
	m_fCharPushRate = rkLoader.LoadFloat_e( "char_push_rate", 0.0f );
	m_fPingPongPowerRate = rkLoader.LoadFloat_e( "pingpong_power_rate", FLOAT09 );
	m_fPingPongPowerRate = min( m_fPingPongPowerRate, FLOAT09 );	/// FLOAT1 이상이면 계속 튀어오르게된다.
	
	m_fFrictionRate = rkLoader.LoadFloat_e( "friction_rate", FLOAT1 );
	m_fAirFrictionRate = rkLoader.LoadFloat_e( "air_friction_rate", FLOAT1 );

	m_fEffectRate = rkLoader.LoadFloat_e( "ball_effect_rot_rate", FLOAT1 );

	m_fGoalAreaRange = rkLoader.LoadFloat_e( "goal_area_range", 0.0f );	
	m_fGoalSpeedRate = rkLoader.LoadFloat_e( "goal_speed_rate", FLOAT1 );
	m_fGoalBoundRate = rkLoader.LoadFloat_e( "goal_bound_rate", FLOAT1 );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	rkLoader.LoadString_e( "ball_name", "", szBuf, MAX_PATH );
	m_BallName = szBuf;
	rkLoader.LoadString_e( "ball_effect", "", szBuf, MAX_PATH );
	m_BallEffect = szBuf;
	rkLoader.LoadString_e( "move_effect", "", szBuf, MAX_PATH );
	m_MoveEffect = szBuf;
	rkLoader.LoadString_e( "ball_shadow_effect", "", szBuf, MAX_PATH );
	m_BallShadowEffect = szBuf;
	rkLoader.LoadString_e( "ball_wounded_effect", "", szBuf, MAX_PATH );
	m_WoundedEffect = szBuf;
	rkLoader.LoadString_e( "ball_timeout_effect", "", szBuf, MAX_PATH );
	m_TimeOutEffect = szBuf;

	rkLoader.LoadString_e( "ball_start_sound", "", szBuf, MAX_PATH );
	m_StartSound = szBuf;

	m_vMoveEffectList.clear();
	int iEffectCnt = rkLoader.LoadInt_e( "move_effect_cnt", 0 );
	if( iEffectCnt > 0 )
	{
		for( int i=0; i < iEffectCnt; ++i )
		{
			MoveEffectInfo eInfo;

			wsprintf_e( szKey, "move_effect%d_name", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			eInfo.m_EffectName = szBuf;

			wsprintf_e( szKey, "move_effect%d_rate", i+1 );
			eInfo.m_fCheckRate = rkLoader.LoadFloat( szKey, FLOAT1 );

			m_vMoveEffectList.push_back( eInfo );
		}
	}

	m_vSoundList.clear();
	int iSoundCnt = rkLoader.LoadInt_e( "wound_sound_cnt", 0 );
	if( iSoundCnt > 0 )
	{
		for( int i=0; i < iSoundCnt; ++i )
		{
			WoundSoundInfo eInfo;

			wsprintf_e( szKey, "wound_sound%d_name", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			eInfo.m_SoundName = szBuf;

			wsprintf_e( szKey, "wound_sound%d_rate", i+1 );
			eInfo.m_fCheckRate = rkLoader.LoadFloat( szKey, FLOAT1 );

			m_vSoundList.push_back( eInfo );
		}
	}

	m_iAnimationIndex = rkLoader.LoadInt_e( "animation_idx", -1 );
}

void ioBall::InitStructInfo( int iBallNum, int iIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale )
{
	m_iStructNum = iBallNum;
	m_iArrayIndex = iIndex;

	m_bGoalState = false;

	m_fCurMoveSpeed = 0.0f;
	m_fBoundPower = 0.0f;
	m_fGravityAmt = 0.0f;

	m_LastAttacker.Clear();
	m_PreAttacker.Clear();

	m_dwBallTouchTime = 0;

	m_State = BS_DELAY;

	SetWorldPosition( vPos );
	SetWorldScale( vScale );

	ioPlayStage *pStage = NULL;
	if( m_pModeParent )
		pStage = m_pModeParent->GetCreator();

	if( pStage )
	{
		if( !m_BallEffect.IsEmpty() )
		{
			ioMapEffect *pMapEffect = NULL;

			pMapEffect = pStage->CreateMapEffect( m_BallEffect, GetWorldPosition(), vScale );
			if( pMapEffect )
			{
				m_dwBallEffect = pMapEffect->GetUniqueID();
				SetBallEffectPosition();
			}
		}

		if( !m_BallShadowEffect.IsEmpty() )
		{
			ioMapEffect *pMapEffect = NULL;
			D3DXVECTOR3 vShadowScale( FLOAT1, FLOAT1, FLOAT1 );

			pMapEffect = pStage->CreateMapEffect( m_BallShadowEffect, GetWorldPosition(), vShadowScale );
			if( pMapEffect )
			{
				D3DXVECTOR3 vCurPos = GetWorldPosition();
				float fMapHeight = GetBottomHeight();
				vCurPos.y = fMapHeight;

				pMapEffect->SetWorldPosition( vCurPos );
				m_dwShadowEffect = pMapEffect->GetUniqueID();
			}
		}
	}

	if( m_TimeOutEffect.IsEmpty() )
		return;

	m_pGroup->EndEffect( m_TimeOutEffect, true );

	if( !m_StartSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_StartSound, DSBVOLUME_MAX, PSM_NONE );
	}

	if( m_iAnimationIndex >= 0 )
	{
		if( !m_pGroup )	return;
		if( !m_pGroup->HasSkeleton() )	return;

		if( COMPARE( m_iAnimationIndex, 0, m_pGroup->GetAnimationCnt() ) )
		{
			m_pGroup->SetLoopAni( m_iAnimationIndex, FLOAT100 );
		}
	}
}

void ioBall::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ProcessState( fTimePerSec );
	UpdateBallEffect( fTimePerSec );

	if( m_iMapTranslateContinuousCount < MAX_MAP_CONTINUOUS_COUNT )
	{
		if( m_dwMapTranslateFrame + 1 < g_FrameTimer.GetFrameCounter() )
		{
			m_iMapTranslateContinuousCount = 0;
		}
	}
}

void ioBall::UpdateAfter( float fFrameGap )
{
	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioPlayStage *pStage = NULL;
	if( m_pModeParent )
		pStage = m_pModeParent->GetCreator();

	if( pStage && m_dwShadowEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwShadowEffect );
		if( pEff )
		{
			D3DXVECTOR3 vCurPos = GetWorldPosition();
			float fMapHeight = GetBottomHeight();
			vCurPos.y = fMapHeight;

			pEff->SetWorldPosition( vCurPos );
		}
	}

	ioPlayEntity::UpdateAfter( fFrameGap );
}

void ioBall::ProcessCurMoveSpeed( float fTimePerSec )
{
	if( !m_pModeParent ) return;
	if( m_fCurMoveSpeed == 0.0f )
		return;

	if( m_fCurMoveSpeed < 0.0f )
	{
		m_fCurMoveSpeed = 0.0f;
		return;
	}

	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( pStage )
	{
		float fPreSpeed = m_fCurMoveSpeed;

		float fFrictionAmt = GetCurFriction() * fTimePerSec;
		m_fCurMoveSpeed -= fFrictionAmt;
		m_fCurMoveSpeed = max( 0.0f, m_fCurMoveSpeed );

		if( m_fMaxMoveSpeed > 0.0f )
		{
			float fPreRate = fPreSpeed / m_fMaxMoveSpeed;
			float fNewRate = m_fCurMoveSpeed / m_fMaxMoveSpeed;

			CheckMoveEffect( fPreRate, fNewRate );
		}
	}
}

float ioBall::GetCurFriction()
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( pStage )
	{
		float fFriction = pStage->GetRoomFriction();

		if( IsFloating() )
			fFriction *= m_fAirFrictionRate;
		else
			fFriction *= m_fFrictionRate;

		return fFriction;
	}

	return FLOAT1;
}

bool ioBall::IsFloating()
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( pStage )
	{
		D3DXVECTOR3 vCurPos = GetWorldPosition();
		float fMapHeight = GetBottomHeight();

		float fFriction = pStage->GetRoomFriction();
		if( vCurPos.y > fMapHeight )
			return true;
	}

	return false;
}

void ioBall::ProcessMoveState( float fTimePerSec )
{
	if( m_State != BS_MOVE )	return;

	D3DXVECTOR3 vMoveAmt = m_vMoveDir * ( m_fCurMoveSpeed * fTimePerSec );
	D3DXVECTOR3 vNewPos  = GetWorldPosition() + vMoveAmt;

	SetWorldPosition( vNewPos );
}

void ioBall::ProcessState( float fTimePerSec )
{
	if( !m_pModeParent ) return;
	
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage ) return;

	if( m_bSendReposition ) return;	// 드랍존 상태면 처리하지 않음

	DWORD dwCurTime = FRAMEGETTIME();

	// 속도 갱신
	ProcessCurMoveSpeed( fTimePerSec );
	
	// 수평 이동 처리
	ProcessMoveState( fTimePerSec );

	// 수직 처리
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	float fMapHeight = GetBottomHeight();

	DWORD dwGapTime = dwCurTime - m_dwProcessTime;
	do 
	{
		DWORD dwCurGap = min( dwGapTime, 10 );
		if( dwGapTime >= 10 )
		{
			dwGapTime -= 10;
		}
		else
		{
			dwCurGap = dwGapTime;
			dwGapTime = 0;
		}

		float fNewTimePerSec = (float)dwCurGap / FLOAT1000;

		if( m_fBoundPower > 0.0f )
		{
			vCurPos.y += m_fBoundPower * fNewTimePerSec;
		}

		if( vCurPos.y > fMapHeight )
		{
			m_fGravityAmt += pStage->GetRoomGravity() * fNewTimePerSec;
			vCurPos.y -= m_fGravityAmt * fNewTimePerSec;
		}

		if( m_fBoundPower <= m_fGravityAmt && vCurPos.y <= fMapHeight )
		{
			if( m_fGravityAmt > 0.0f )
			{
				float fGapAmt = (m_fGravityAmt-m_fBoundPower) * m_fPingPongPowerRate;
				if( fGapAmt > m_fLimitBoundAmt )
				{
					m_fBoundPower = fGapAmt;
				}
				else
				{
					m_fBoundPower = 0.0f;
				}
			}

			m_fGravityAmt = 0.0f;
			vCurPos.y = fMapHeight;
		}

		SetWorldPosition( vCurPos );

		if( vCurPos.y == fMapHeight && m_fBoundPower == 0.0f && m_fGravityAmt == 0.0f )
			break;

	} while( dwGapTime > 0 );

	SetBallEffectPosition();

	if( m_fCurMoveSpeed > 0.0f && m_State != BS_MOVE )
	{
		SetMoveState();
	}
	else if( m_fCurMoveSpeed == 0.0f && m_State != BS_DELAY )
	{
		SetDelayState();
	}

	// 드랍존 체크
	DropZoneCheck();

	// 골 체크
	dwGapTime = dwCurTime - m_dwProcessTime;
	if( !m_bSendReposition )
	{
		do 
		{
			DWORD dwCurGap = min( dwGapTime, 10 );
			if( dwGapTime >= 10 )
			{
				dwGapTime -= 10;
			}
			else
			{
				dwCurGap = dwGapTime;
				dwGapTime = 0;
			}

			float fNewTimePerSec = (float)dwCurGap / FLOAT1000;

			if( GoalCheck( fNewTimePerSec ) )
				break;

		} while( dwGapTime > 0 );
	}

	// Process시간 갱신
	m_dwProcessTime = dwCurTime;
}

ApplyWeaponExp ioBall::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioBallStruct::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_NO_EXPLOSION;
	}

	if( !pAttacker->IsOwnerChar() )
		return AWE_NO_EXPLOSION;

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	ioPlayStage *pStage = NULL;
	if( m_pModeParent && m_pModeParent->GetCreator() )
		pStage = m_pModeParent->GetCreator();

	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);
	float fDamage = pWeapon->GetDamage( this );
	float fPushPower = pWeapon->GetBallPushPower( fDamage );
	float fBlowPower = pWeapon->GetBallBlowPower( fDamage );

	bool bTimeOut = false;
	if( m_pModeParent && m_pModeParent->IsZeroHP() )
		bTimeOut = true;

	if( IsFloating() )
	{
		fPushPower = pWeapon->GetBallAirPushPower( fDamage );
		fBlowPower = pWeapon->GetBallAirBlowPower( fDamage );
	}

	// 속도, 띄움처리
	int iWoundSoundIndex = -1;
	{
		int iOwnerTeamCnt, iEnemyTeamCnt;
		iOwnerTeamCnt = iEnemyTeamCnt = 1;

		TeamType eWeaponTeam = pWeapon->GetTeam();
		switch( eWeaponTeam )
		{
		case TEAM_RED:
			{
				if( pStage )
				{
					iOwnerTeamCnt = pStage->GetRedMember();
					iEnemyTeamCnt = pStage->GetBlueMember();
				}
			}
			break;
		case TEAM_BLUE:
			{
				if( pStage )
				{
					iOwnerTeamCnt = pStage->GetBlueMember();
					iEnemyTeamCnt = pStage->GetRedMember();
				}
			}
			break;
		}

		float fNewBound = g_MathManager.CalcBallDamageBoundAmt( fDamage, fBlowPower, iOwnerTeamCnt, iEnemyTeamCnt, bTimeOut );
		if( fNewBound > 0.0f )
		{
			if( m_fBoundPower > 0.0f )
			{
				m_fGravityAmt *= m_fBoundAmtAddRate;
				m_fBoundPower *= m_fBoundAmtAddRate;
			}

			m_fBoundPower += fNewBound;
		}

		m_fBoundPower = max( 0.0f, m_fBoundPower );
		m_fBoundPower = min( m_fBoundPower, m_fMaxBoundAmt );

		float fWeaponSpeedAmt = g_MathManager.CalcBallDamageSpeedAmt( fDamage, fPushPower, iOwnerTeamCnt, iEnemyTeamCnt, bTimeOut );
		
		vAttackDir.y = 0.0f;
		D3DXVec3Normalize( &vAttackDir, &vAttackDir );
		D3DXVECTOR3 vWeaponSpeedAmt = vAttackDir * fWeaponSpeedAmt;

		float fPreSpeed = m_fCurMoveSpeed;
		D3DXVECTOR3 vNewSpeedAmt;
		if( m_fCurMoveSpeed > 0.0f )
		{
			vNewSpeedAmt = m_vMoveDir * (m_fCurMoveSpeed * m_fMoveSpeedAddRate);
			vNewSpeedAmt = vNewSpeedAmt + vWeaponSpeedAmt;
		}
		else
		{
			vNewSpeedAmt = vWeaponSpeedAmt;
		}

		m_fCurMoveSpeed = D3DXVec3Length( &vNewSpeedAmt );
		m_fCurMoveSpeed = min( m_fCurMoveSpeed, m_fMaxMoveSpeed );

		if( m_fMaxMoveSpeed > 0.0f )
		{
			float fPreRate = fPreSpeed / m_fMaxMoveSpeed;
			float fNewRate = m_fCurMoveSpeed / m_fMaxMoveSpeed;

			CheckMoveEffect( fPreRate, fNewRate );
			iWoundSoundIndex = CheckWoundSound( fNewRate );
		}

		D3DXVec3Normalize( &m_vMoveDir, &vNewSpeedAmt );
	}

	int iShakeCamera = pWeapon->GetShakeCamera();
	ActionStopType eActionStop = pWeapon->GetActionStopType();

	if( eActionStop == AST_BOTH || eActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
		pWeapon->SetActionStopDuration( ioBaseChar::CalcActionStopDelay( fDamage, false ),
										ioBaseChar::GetActionStopLooseRate( false ) );
	}

	if( m_fCurMoveSpeed > 0.0f )
	{
		if( m_State != BS_MOVE )
			SetMoveState();
	}
	else
	{
		if( m_State != BS_DELAY )
			SetDelayState();
	}

	if( m_LastAttacker != pAttacker->GetCharName() )
	{
		m_dwBallTouchTime = FRAMEGETTIME();
		m_PreAttacker = m_LastAttacker;
	}
	
	m_LastAttacker = pAttacker->GetCharName();

	ioFootballMode *pFootball = ToFootballMode(m_pModeParent);
	if( pFootball )
	{
		pFootball->BallTouchContribute( m_LastAttacker );
	}

	CheckGoalSaveTouch( pAttacker, vAttackDir );

	if( !m_LastAttacker.IsEmpty() && fDamage > 0.0f )
	{
		m_DamageTable.AddDamage( m_LastAttacker, fDamage );

		g_AwardMgr.AddHit( m_LastAttacker );
	}

	if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		SP2Packet kPacket( CUPK_BALL_INFO );
		kPacket << m_iArrayIndex;
		kPacket << BALL_KICK;
		kPacket << (int)m_State;
		kPacket << pAttacker->GetCharName();
		kPacket << GetWorldPosition();
		kPacket << m_vMoveDir;
		kPacket << vAttackDir;
		kPacket << m_fBoundPower;
		kPacket << m_fGravityAmt;
		kPacket << m_fCurMoveSpeed;
		kPacket << fDamage;
		kPacket << (int)eActionStop;
		kPacket << iShakeCamera;
		kPacket << iWoundSoundIndex;
		kPacket << pWeapon->GetWeaponIndex();
		P2PNetwork::SendToAllPlayingUser( kPacket );

		g_AbuseMgr.AddDamage( (int)fDamage );

		if( m_pModeParent && m_pModeParent->GetCreator() )
		{
			m_pModeParent->GetCreator()->NotifyBallStructSendInfo();

			if( !m_WoundedEffect.IsEmpty() )
			{
				D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
				m_pModeParent->GetCreator()->CreateMapEffect( m_WoundedEffect, GetMidPositionByRate(), vScale );
			}
		}
	}

	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	return AWE_NO_EXPLOSION;
}

void ioBall::SendStructInfo()
{
	if( g_MyInfo.GetPublicID() != m_LastAttacker )
		return;

	SP2Packet kPacket( CUPK_BALL_INFO );
	kPacket << m_iArrayIndex;
	kPacket << BALL_INFO;
	kPacket << (int)m_State;
	kPacket << GetWorldPosition();
	kPacket << m_vMoveDir;
	kPacket << m_fBoundPower;
	kPacket << m_fGravityAmt;
	kPacket << m_fCurMoveSpeed;
	P2PNetwork::SendToAllPlayingUser( kPacket );

	if( m_pModeParent && m_pModeParent->GetCreator() )
		m_pModeParent->GetCreator()->NotifyBallStructSendInfo();
}

void ioBall::GetBallStructInfo( SP2Packet &rkPacket )
{
	rkPacket << m_iArrayIndex;
	rkPacket << BALL_INFO;
	rkPacket << (int)m_State;
	rkPacket << GetWorldPosition();
	rkPacket << m_vMoveDir;
	rkPacket << m_fBoundPower;
	rkPacket << m_fGravityAmt;
	rkPacket << m_fCurMoveSpeed;
}

void ioBall::OnStructInfo( SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	int iState;
	rkPacket >> iState;

	switch( iType )
	{
	case BALL_KICK:
		{
			ioHashString szAttacker;
			rkPacket >> szAttacker;

			D3DXVECTOR3 vPos, vAttackDir;
			rkPacket >> vPos;

			SetWorldPosition( vPos );
			SetBallEffectPosition();

			float fPreSpeed = m_fCurMoveSpeed;

			rkPacket >> m_vMoveDir >> vAttackDir;
			rkPacket >> m_fBoundPower;
			rkPacket >> m_fGravityAmt;
			rkPacket >> m_fCurMoveSpeed;

			if( m_fMaxMoveSpeed > 0.0f )
			{
				float fPreRate = fPreSpeed / m_fMaxMoveSpeed;
				float fNewRate = m_fCurMoveSpeed / m_fMaxMoveSpeed;

				CheckMoveEffect( fPreRate, fNewRate );
			}

			float fDamage;
			rkPacket >> fDamage;
			int iActionStop, iShakeCamera, iWoundSoundIndex;
			rkPacket >> iActionStop >> iShakeCamera >> iWoundSoundIndex;

			g_AbuseMgr.AddDamage( (int)fDamage );

			ioBaseChar *pAttacker = NULL;
			ioPlayStage *pStage = NULL;
			if( m_pModeParent )
			{
				pStage = m_pModeParent->GetCreator();

				if( pStage )
					pAttacker = pStage->GetBaseChar(szAttacker);
			}

			if( pAttacker )
			{
				if( m_PreAttacker != m_LastAttacker )
				{
					m_dwBallTouchTime = FRAMEGETTIME();
					m_PreAttacker = m_LastAttacker;
				}
					
				m_LastAttacker = szAttacker;

				ioFootballMode *pFootball = ToFootballMode(m_pModeParent);
				if( pFootball )
				{
					pFootball->BallTouchContribute( m_LastAttacker );
				}

				if( iActionStop == AST_BOTH || iActionStop == AST_ATTACKER )
					pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );

				if( !m_WoundedEffect.IsEmpty() && pStage )
				{
					D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
					pStage->CreateMapEffect( m_WoundedEffect, GetMidPositionByRate(), vScale );
				}
			}

			if( !m_LastAttacker.IsEmpty() && fDamage > 0.0f )
			{
				m_DamageTable.AddDamage( m_LastAttacker, fDamage );

				g_AwardMgr.AddHit( m_LastAttacker );
				CheckGoalSaveTouch( pAttacker, vAttackDir );
			}

			// Sound
			if( iWoundSoundIndex != -1 )
			{
				if( COMPARE( iWoundSoundIndex, 0, (int)m_vSoundList.size() ) )
				{
					g_SoundMgr.PlaySound( m_vSoundList[iWoundSoundIndex].m_SoundName, m_pGroup->GetParentSceneNode() );
				}
			}

			int iWeaponIndex = 0;
			rkPacket >> iWeaponIndex;

			ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pAttacker, iWeaponIndex );
			if( pWeapon && pAttacker )
			{
				int iDummyIndex = pWeapon->GetWeaponByDummyChar();
				g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
			}
		}
		break;
	case BALL_INFO:
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		SetWorldPosition( vPos );
		SetBallEffectPosition();

		float fPreSpeed = m_fCurMoveSpeed;

		rkPacket >> m_vMoveDir;
		rkPacket >> m_fBoundPower;
		rkPacket >> m_fGravityAmt;
		rkPacket >> m_fCurMoveSpeed;

		if( m_fMaxMoveSpeed > 0.0f )
		{
			float fPreRate = fPreSpeed / m_fMaxMoveSpeed;
			float fNewRate = m_fCurMoveSpeed / m_fMaxMoveSpeed;

			CheckMoveEffect( fPreRate, fNewRate );
		}
		break;
	}

	// 상태 전환
	if( iState == BS_DELAY && m_State != BS_DELAY )
		SetDelayState();
	else if( iState == BS_MOVE && m_State != BS_MOVE )
		SetMoveState();
}

bool ioBall::IsNeedCheckMapCollision() const
{
	if( m_iMapTranslateContinuousCount >= MAX_MAP_CONTINUOUS_COUNT )
		return false;

	return true;
}

ioWorldEventReceiver* ioBall::GetWorldEventReceiver()
{
	return dynamic_cast<ioWorldEventReceiver*>(this);
}

void ioBall::OnTelepotation( const D3DXVECTOR3 &vTargetPos )
{
	SetWorldPosition( vTargetPos );
}

void ioBall::OnFireZoneDamage( float fDamage, const ioHashString &szBuff )
{
}

void ioBall::OnPoisonZoneDamage()
{
}

bool ioBall::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon ) return false;

	if( !IsCollisionAvailableRange( pWeapon ) )
		return false;

	if( pWeapon->IsAlreadyCollisioned( this ) )
		return false;

	if( m_bSendReposition )
		return false;

	return true;
}

int ioBall::DontMoveEntityLevel() const
{
	if( m_State == BS_MOVE )
		return 2;

	return 2;
}

ioPlayEntity::PlaySubType ioBall::GetSubType() const
{
	return PST_BALL;
}

ArmorType ioBall::GetArmorType() const
{
	return m_ArmorType;
}

void ioBall::NeedUpdateBottomHeight()
{
	ioPlayEntity::NeedUpdateBottomHeight();
}

void ioBall::Translate( const D3DXVECTOR3 &vMove )
{
	ioPlayEntity::Translate( vMove );
}

void ioBall::MapCollisionPushingTranslate( const D3DXVECTOR3 &vMoveAmt )
{
	m_iMapTranslateContinuousCount++;
	m_dwMapTranslateFrame = g_FrameTimer.GetFrameCounter();

	if( m_iMapTranslateContinuousCount < MAX_MAP_CONTINUOUS_COUNT )
	{
		D3DXVECTOR3 vPushAmt( vMoveAmt.x, 0.0f, vMoveAmt.z );	// 좌우 이동만 처리
		ioPlayEntity::MapCollisionPushingTranslate( vPushAmt );
	}

	m_bMapCollisionPushed = true;
}

void ioBall::DontMoveCollisionPushingTranslate( ioGameEntity *pPusher, const D3DXVECTOR3 &vMoveAmt )
{
	if( !IsMapCollisionPushed() )
	{
		ioPlayEntity::DontMoveCollisionPushingTranslate( pPusher, vMoveAmt );
	}
}

bool ioBall::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	ioOrientBox kEntityBox;
	kEntityBox.SetByAxisBox( GetWorldAxisBox() );

	if( !pShape->FindIntersection( pShapeWorldMat, kEntityBox, NULL ) )
		return false;

	ioAxisAlignBox kAxisBox = GetWorldAxisBox();

	D3DXVECTOR3 vDiff( 0.0f, 0.0f, 0.0f );
	vDiff.y = ( kAxisBox.GetMaxPos().y - kAxisBox.GetMinPos().y ) * 0.25f;
	kAxisBox.SetMinPos( kAxisBox.GetMinPos() + vDiff );

	Vector3Vec vPushNrmList;
	const IntVec &rkFaceList = pShape->GetFindIntersectionFaceList();
	vPushNrmList.reserve( rkFaceList.size() );

	int iFaceCount = rkFaceList.size();
	for( int i=0 ; i<iFaceCount ; i++ )
	{
		D3DXVECTOR3 vPlaneNrm;
		pShape->GetTriNormalInModelSpace( rkFaceList[i], vPlaneNrm );

		if( D3DXVec3Dot( &ioMath::UNIT_Y, &vPlaneNrm ) > FLOAT09 )
			continue;

		bool bSameNrmExist = false;
		int iPushNrmCount = vPushNrmList.size();
		for( int j=0 ; j<iPushNrmCount ; j++ )
		{
			if( ioMath::IsEqual( vPlaneNrm, vPushNrmList[j] ) )
			{
				bSameNrmExist = true;
				break;
			}
		}

		if( !bSameNrmExist )
		{
			vPushNrmList.push_back( vPlaneNrm );
		}
	}

	if( vPushNrmList.empty() )
		return false;

	D3DXVECTOR3 vPushDir = vPushNrmList.front();
	for( i=1 ; i<(int)vPushNrmList.size() ; i++ )
	{
		vPushDir += vPushNrmList[i];
		D3DXVec3Normalize( &vPushDir, &vPushDir );
	}

	// 일단 수평만
	vPushDir.y = 0.0f;
	D3DXVec3Normalize( &vPushDir, &vPushDir );

	// 각도 체크
	float fCheck = D3DXVec3Dot( &m_vMoveDir, &vPushDir );
	if( fCheck >= 0.0f )
		return false;	// 나가는 방향

	if( g_MyInfo.GetPublicID() != m_LastAttacker )
	{
		ioOrientBox kMovingBox;
		kMovingBox.SetByAxisBox( kAxisBox );

		int iMoveCount = 0;
		D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );

		while( iMoveCount < 20 )	// 무한루프 방지
		{
			kMovingBox.SetCenter( kMovingBox.GetCenter() + vPushDir );
			vMoveAmt += vPushDir;

			if( !pShape->TestIntersection( pShapeWorldMat, kMovingBox ) )
				break;

			iMoveCount++;
		}

		Translate( vMoveAmt );
		return true;
	}

	// 방향 바꾸기
	D3DXVECTOR3 vReflect = m_vMoveDir - 2.0f * D3DXVec3Dot( &m_vMoveDir, &vPushDir ) * vPushDir;
	D3DXVec3Normalize( &m_vMoveDir, &vReflect );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetWorldOrientation( qtRot );
	SetBallEffectOrientation();

	// 동기화
	SendStructInfo();
	return true;
}

float ioBall::CalcCollisionAmt( const D3DXVECTOR3 &vMoveAmt )
{
	D3DXVECTOR3 vDir = vMoveAmt;
	vDir.y = 0.0f;

	float fAmt = D3DXVec3Length( &vDir );
	if( fAmt > 6.0f )
		fAmt = 6.0f;
	else if( fAmt < 3.0f )
		fAmt = 3.0f;

	fAmt *= m_fCharPushRate;

	return fAmt;
}

void ioBall::EntityCollision( ioPlayStage *pStage, const D3DXVECTOR3 &vMoveAmt )
{
	if( m_fCharPushRate == 0.0f ) return;

	D3DXVECTOR3 vSpeedAmt = m_vMoveDir * m_fCurMoveSpeed;
	D3DXVECTOR3 vDir = vMoveAmt;
	vDir.y = 0.0f;

	float fAmt = CalcCollisionAmt( vMoveAmt );

	D3DXVec3Normalize( &vDir, &vDir );
	vDir *= fAmt;

	vSpeedAmt = vSpeedAmt + vDir;

	m_fCurMoveSpeed = D3DXVec3Length( &vSpeedAmt );
	m_fCurMoveSpeed = min( m_fCurMoveSpeed, m_fMaxMoveSpeed );

	D3DXVec3Normalize( &m_vMoveDir, &vSpeedAmt );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetWorldOrientation( qtRot );
	SetBallEffectOrientation();

	if( m_fCurMoveSpeed > 0.0f )
	{
		if( m_State != BS_MOVE )
			SetMoveState();
	}
	else
	{
		if( m_State != BS_DELAY )
			SetDelayState();
	}
}

void ioBall::UpdateBottomHeight() const
{
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	m_fCurBottomHeight = m_pModeParent->GetMapHeight( vCurPos.x, vCurPos.z, this );
}

void ioBall::SetMoveState()
{
	m_State = BS_MOVE;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetWorldOrientation( qtRot );
	SetBallEffectOrientation();
}

void ioBall::SetDelayState()
{
	m_State = BS_DELAY;
	m_fCurMoveSpeed = 0.0f;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetWorldOrientation( qtRot );
}

bool ioBall::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	return false;
}

void ioBall::DropZoneCheck()
{
	if( m_bSendReposition ) return;

	D3DXVECTOR3 vPos = GetWorldPosition();

	if( vPos.y > 0.0f )
		return;

	if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		if( g_MyInfo.GetPublicID() == m_LastAttacker )
		{
			// Reposition 요청
			m_bSendReposition = true;

			SP2Packet kPacket( CTPK_BALLSTRUCT_REPOSITION );
			kPacket << m_iArrayIndex;
			TCPNetwork::SendToServer( kPacket );
		}
	}
}

void ioBall::TranslateByWorldEntity( const D3DXVECTOR3 &vMove )
{
	D3DXVECTOR3 vPos;
	const ioOrientBox &rkBox = GetWorldCollisionBox();

	m_pGroup->GetParentNode()->Translate( vMove );
	NeedUpdateBound();
	NeedUpdateBottomHeight();
}

void ioBall::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
	D3DXVECTOR3 vPos;
	const ioOrientBox &rkBox = GetWorldCollisionBox();

	m_pGroup->GetParentNode()->Translate( vMove );
	NeedUpdateBound();
	NeedUpdateBottomHeight();
}

void ioBall::ApplyOutLineChange( ChangeOutLineType eType, const D3DCOLORVALUE &rkColor, float fSize )
{
	RemoveChangeOutLineRecord( eType );

	OutLineRecord kRecord;
	kRecord.m_Type   = eType;
	kRecord.m_Color  = rkColor;
	kRecord.m_fThick = fSize;
	m_OutLineChangeList.push_front( kRecord );

	ChangeOutLine( rkColor, fSize );
}

void ioBall::RestoreOutLineChange( ChangeOutLineType eType )
{
	RemoveChangeOutLineRecord( eType );

	if( !m_OutLineChangeList.empty() )
	{
		const OutLineRecord &rkRecord = m_OutLineChangeList.front();
		ChangeOutLine( rkRecord.m_Color, rkRecord.m_fThick );
	}
	else
	{
		// 최초 상태로 원상복구
		RestoreOutLine();
	}
}

bool ioBall::HasOutLineType( ChangeOutLineType eType )
{
	OutLineRecordList::iterator iter;
	for( iter=m_OutLineChangeList.begin() ; iter!=m_OutLineChangeList.end() ; ++iter )
	{
		if( iter->m_Type == eType )
			return true;
	}

	return false;
}

void ioBall::RemoveChangeOutLineRecord( ChangeOutLineType eType )
{
	OutLineRecordList::iterator iter = m_OutLineChangeList.begin();
	while( iter != m_OutLineChangeList.end() )
	{
		if( iter->m_Type == eType )
		{
			iter = m_OutLineChangeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioBall::ChangeOutLine( const D3DCOLORVALUE &rkOutLineColor, float fOutLineSize )
{
	m_OutLineColor = rkOutLineColor;
	m_fOutLineSize = fOutLineSize;
	m_bOutLineModified = true;

	ApplyModifiedOutLine();
}

void ioBall::RestoreOutLine()
{
	if( !m_bOutLineModified )	return;

	int iNumEntity = m_pGroup->GetNumEntities();
	for( int i=0 ; i<iNumEntity ; i++ )
	{
		ioEntity *pEntity = m_pGroup->GetEntity( i );

		int iNumSubEntity = pEntity->GetNumSubEntity();
		for( int j=0 ; j<iNumSubEntity ; j++ )
		{
			ioSubEntity *pSubEntity = pEntity->GetSubEntity( j );
			if( !pSubEntity )	continue;

			ioTechnique *pCloneTech = NULL;
			ioMaterial *pClone = pSubEntity->GetMaterial();
			if( pClone )
			{
				pCloneTech = pClone->GetBestTechnique();
			}

			ioTechnique *pSrcTech = NULL;
			ioMaterial *pSrc = ioMaterialManager::GetSingleton().GetMaterial( pSubEntity->GetMaterialName() );
			if( pSrc )
			{
				pSrcTech = pSrc->GetBestTechnique();
			}

			if( !pSrcTech || !pCloneTech )
				continue;

			ioPass *pClonePass = pCloneTech->GetPassByShaderGrp( SGT_OUTLINE );
			ioPass *pSrcPass = pSrcTech->GetPassByShaderGrp( SGT_OUTLINE );

			if( !pClonePass || !pSrcPass )
				continue;

			ioShaderCustomArg *pArg = pSrcPass->GetCustomArg( "OutLineColor" );
			if( pArg )
			{
				pClonePass->AddCustomParameter( *pArg );
			}

			pArg = pSrcPass->GetCustomArg( "OutLineThickness" );
			if( pArg )
			{
				pClonePass->AddCustomParameter( *pArg );
			}

			if( !Setting::UseObjectEdge() )
			{
				pClonePass->SetPassEnable( false );
			}
		}
	}

	m_bOutLineModified = false;
}

void ioBall::ApplyModifiedOutLine()
{
	if( !m_bOutLineModified )	return;

	ioShaderCustomArg kLineColor( "OutLineColor" );
	kLineColor.SetValue( m_OutLineColor );

	ioShaderCustomArg kLineThickness( "OutLineThickness" );
	kLineThickness.SetValue( m_fOutLineSize );

	int iNumEntity = m_pGroup->GetNumEntities();
	for( int i=0; i<iNumEntity; i++ )
	{
		ioEntity *pEntity = m_pGroup->GetEntity( i );

		int iNumSubEntity = pEntity->GetNumSubEntity();
		for( int j=0 ; j<iNumSubEntity ; j++ )
		{
			ioSubEntity *pSubEntity = pEntity->GetSubEntity( j );

			ioMaterial *pMaterial = pSubEntity->GetMaterial();
			if( !pMaterial ) continue;

			ioTechnique *pTech = pMaterial->GetBestTechnique();
			if( !pTech ) continue;

			ioPass *pOutLine = pTech->GetPassByShaderGrp( SGT_OUTLINE );
			if( pOutLine )
			{
				pOutLine->SetPassEnable( true );
				pOutLine->AddCustomParameter( kLineColor );
				pOutLine->AddCustomParameter( kLineThickness );
			}
		}
	}
}

void ioBall::OnStructReposition( SP2Packet &rkPacket )
{
	D3DXVECTOR3 vPos;
	rkPacket >> vPos;

	float fPreSpeed = m_fCurMoveSpeed;

	m_fCurMoveSpeed = 0.0f;
	m_fBoundPower = 0.0f;
	m_fGravityAmt = 0.0f;

	if( m_fMaxMoveSpeed > 0.0f )
	{
		float fPreRate = fPreSpeed / m_fMaxMoveSpeed;
		float fNewRate = m_fCurMoveSpeed / m_fMaxMoveSpeed;

		CheckMoveEffect( fPreRate, fNewRate );
	}

	m_State = BS_DELAY;
	m_bSendReposition = false;

	SetWorldPosition( vPos );
}

DWORD ioBall::GetEventGapTime()
{
	return (FRAMEGETTIME() - m_dwCreateTime);
}


void ioBall::SetEndFocusEffect( const ioHashString &szEndFocusEffect )
{
	if( !szEndFocusEffect.IsEmpty() )
	{
		if( m_pGroup )
			m_pGroup->AttachEffect( szEndFocusEffect, NULL );
	}
}

bool ioBall::GoalCheck( float fTimePerSec )
{
	if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		if( g_MyInfo.GetPublicID() != m_LastAttacker )
			return false;
	}
	else
	{
		return false;
	}

	if( m_bGoalState )
		return false;

	ioFootballMode *pFootball = ToFootballMode(m_pModeParent);
	if( !pFootball ) return false;

	if( pFootball->GetModeState() != ioPlayMode::MS_PLAY )
	{
		return false;
	}

	if( !pFootball->GetCreator() )
	{
		return false;
	}

	ioBaseChar *pAttacker = pFootball->GetCreator()->GetBaseChar(m_LastAttacker);
	if( !pAttacker )
	{
		return false;
	}

	GoalGate kRedGoalGate, kBlueGoalGate;

	pFootball->GetGoalGate( TEAM_RED, kRedGoalGate );
	pFootball->GetGoalGate( TEAM_BLUE, kBlueGoalGate );

	bool bRedGoal = false;
	bool bBlueGoal = false;

	// RedGoalGate, Blue Goal
	ioOrientBox kGateBox = kRedGoalGate.m_GateBox;
	if( ioMath::TestIntersection( GetWorldCollisionBox(), kGateBox, NULL ) )
	{
		if( D3DXVec3Dot( &m_vMoveDir, &kRedGoalGate.m_vGateDir ) < 0.0f )
		{
			bBlueGoal = true;
		}
	}

	// BlueGoalGate, Red Goal
	kGateBox = kBlueGoalGate.m_GateBox;
	if( ioMath::TestIntersection( GetWorldCollisionBox(), kGateBox, NULL ) )
	{
		if( D3DXVec3Dot( &m_vMoveDir, &kBlueGoalGate.m_vGateDir ) < 0.0f )
		{
			bRedGoal = true;
		}
	}

	if( !bRedGoal && !bBlueGoal )
	{
		return false;
	}

	if( bRedGoal && bBlueGoal )
	{
		LOG.PrintTimeAndLog( 0, "ioBall::GoalCheck - Error: Both Goal" );
		return false;
	}

	if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		m_bGoalState = true;

		m_fCurMoveSpeed *= m_fGoalSpeedRate;
		m_fBoundPower *= m_fGoalBoundRate;

		TeamType eGoalTeam = TEAM_NONE;
		if( bRedGoal )
			eGoalTeam = TEAM_RED;
		else if( bBlueGoal )
			eGoalTeam = TEAM_BLUE;

		SP2Packet kPacket( CTPK_FOOTBALL_GOAL );
		kPacket << (int)eGoalTeam;
		kPacket << m_LastAttacker;
		kPacket << m_PreAttacker;
		TCPNetwork::SendToServer( kPacket );

		return true;
	}

	return false;
}

void ioBall::SetBallEffectPosition()
{
	ioPlayStage *pStage = NULL;
	if( m_pModeParent )
	{
		pStage = m_pModeParent->GetCreator();
	}

	if( !pStage ) return;

	if( m_dwBallEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwBallEffect );
		if( pEff )
		{
			D3DXVECTOR3 vPos = GetWorldPosition();
			vPos.y = GetWorldCollisionBox().GetCenter().y;

			pEff->SetWorldPosition( vPos );
		}
	}
}

void ioBall::SetBallEffectOrientation()
{
	ioPlayStage *pStage = NULL;
	if( m_pModeParent )
	{
		pStage = m_pModeParent->GetCreator();
	}

	if( !pStage ) return;

	if( m_dwBallEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwBallEffect );
		if( pEff )
		{
			pEff->SetWorldOrientation( GetWorldOrientation() );
		}
	}
}

void ioBall::UpdateBallEffect( float fTimePerSec )
{
	ioPlayStage *pStage = NULL;
	if( m_pModeParent )
	{
		pStage = m_pModeParent->GetCreator();
	}

	if( !pStage ) return;
	if( m_dwBallEffect == -1 ) return;

	ioMapEffect *pEff = pStage->FindMapEffect( m_dwBallEffect );
	if( !pEff ) return;

	D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );

	if( m_fCurMoveSpeed > 0.0f )
	{
		float fRotateAngle = (m_fCurMoveSpeed * m_fEffectRate) * fTimePerSec;

		pEff->Rotate( m_vRollAxis, fRotateAngle );
	}

	pEff->Update( FLOAT1000 * fTimePerSec );
}

bool ioBall::FillAllDamagedList( SP2Packet &rkPacket )
{
	m_DamageTable.AttachAllDamage();

	int iDamageCnt = m_DamageTable.GetDamageTableCnt();
	if( iDamageCnt > 0 )
	{
		rkPacket << iDamageCnt;
		for( int i=0 ; i<iDamageCnt ; i++ )
		{
			ioHashString rkAttacker;
			int iTotalDamage = 0;

			m_DamageTable.GetAllDamage( i, rkAttacker, iTotalDamage );
			rkPacket << rkAttacker;
			rkPacket << iTotalDamage;
		}

		m_DamageTable.ClearTable();
		return true;
	}
	else
	{
		rkPacket << 0;
	}

	return false;
}

void ioBall::CheckBallTouchTime()
{
	if( m_dwBallTouchTime == 0 )
	{
		if( !m_PreAttacker.IsEmpty() )
			m_PreAttacker.Clear();

		if( !m_LastAttacker.IsEmpty() )
			m_LastAttacker.Clear();

		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwBallTouchTime+10000 < dwCurTime )
	{
		m_PreAttacker.Clear();
		m_LastAttacker.Clear();

		m_dwBallTouchTime = 0;
	}
}

void ioBall::SetGoalState( bool bGoal )
{
	m_bGoalState = bGoal;

	if( m_bGoalState )
	{
		m_fCurMoveSpeed *= m_fGoalSpeedRate;
		m_fBoundPower *= m_fGoalBoundRate;
	}
}

void ioBall::ClearSendData()
{
	m_bGoalState = false;
}

void ioBall::CheckGoalSaveTouch( ioBaseChar *pChar, const D3DXVECTOR3 &vDir )
{
	if( !pChar ) return;

	ioFootballMode *pFootball = ToFootballMode(m_pModeParent);
	if( !pFootball ) return;

	if( pFootball->GetModeState() != ioPlayMode::MS_PLAY )
		return;

	if( !pFootball->GetCreator() )
		return;

	GoalGate kGoalGate;

	if( pChar->GetTeam() == TEAM_RED )
		pFootball->GetGoalGate( TEAM_RED, kGoalGate );
	else if( pChar->GetTeam() == TEAM_BLUE )
		pFootball->GetGoalGate( TEAM_BLUE, kGoalGate );
	else
		return;

	ioOrientBox kGateBox = kGoalGate.m_GateBox;
	D3DXVECTOR3 vCenterPos = kGateBox.GetCenter();
	vCenterPos.y -= kGateBox.GetExtents(1);

	D3DXVECTOR3 vDiff = GetWorldPosition() - vCenterPos;

	float fDist = D3DXVec3Length( &vDiff );

	if( fDist < m_fGoalAreaRange )
	{
		if( D3DXVec3Dot( &vDir, &kGoalGate.m_vGateDir ) >= 0.0f )
		{
			pFootball->SaveTouchContribute( m_LastAttacker );
			g_AwardMgr.AddGoalSave( pChar->GetCharName(), 1, 1 );

			if( pChar->IsOwnerChar() )
				g_QuestMgr.QuestCompleteTerm( QC_SOCCER_SAVE, 1 );
		}
	}
}

void ioBall::SetZeroHP()
{
	if( m_TimeOutEffect.IsEmpty() )
		return;

	m_pGroup->EndEffect( m_TimeOutEffect, true );
	m_pGroup->AttachEffect( m_TimeOutEffect, NULL );
}

void ioBall::CheckMoveEffect( float fPreRate, float fNewRate )
{
	if( m_vMoveEffectList.empty() ) return;

	float fCurRate = 0.0f;

	int iCnt = m_vMoveEffectList.size();
	int iPreRateIndex = -1;
	int iNewRateIndex = -1;

	for( int i=0; i < iCnt; ++i )
	{
		float fEffectRate = m_vMoveEffectList[i].m_fCheckRate;

		if( fPreRate > 0.0f && COMPARE( fPreRate, fCurRate, fEffectRate ) )
			iPreRateIndex = i;

		if( fNewRate > 0.0f && COMPARE( fNewRate, fCurRate, fEffectRate ) )
			iNewRateIndex = i;

		fCurRate = fEffectRate;
	}

	if( iPreRateIndex == -1 && iNewRateIndex == -1 )
		return;

	// 범위 변화 없는 경우
	if( iPreRateIndex == iNewRateIndex )
		return;

	// 정지상태에서 움직이는 경우
	if( iPreRateIndex == -1 && iNewRateIndex != -1 )
	{
		m_pGroup->AttachEffect( m_vMoveEffectList[iNewRateIndex].m_EffectName, NULL );
		return;
	}

	// 움직이다 정지하는 경우
	if( iPreRateIndex != -1 && iNewRateIndex == -1 )
	{
		m_pGroup->EndEffect( m_vMoveEffectList[iPreRateIndex].m_EffectName, true );
		return;
	}

	// 범위가 바뀌는 경우
	if( iPreRateIndex != iNewRateIndex )
	{
		m_pGroup->EndEffect( m_vMoveEffectList[iPreRateIndex].m_EffectName, true );
		m_pGroup->AttachEffect( m_vMoveEffectList[iNewRateIndex].m_EffectName, NULL );
		return;
	}
}

int ioBall::CheckWoundSound( float fRate )
{
	if( m_vSoundList.empty() ) return -1;

	float fCurRate = 0.0f;

	int iCnt = m_vSoundList.size();
	int iNewRateIndex = -1;

	for( int i=0; i < iCnt; ++i )
	{
		float fSoundRate = m_vSoundList[i].m_fCheckRate;

		if( fRate > 0.0f && COMPARE( fRate, fCurRate, fSoundRate ) )
			iNewRateIndex = i;

		fCurRate = fSoundRate;
	}

	if( iNewRateIndex == -1 )
		return -1;

	if( !m_vSoundList[iNewRateIndex].m_SoundName.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_vSoundList[iNewRateIndex].m_SoundName, m_pGroup->GetParentSceneNode() );

		return iNewRateIndex;
	}

	return -1;
}