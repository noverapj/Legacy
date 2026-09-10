

#include "stdafx.h"
#include "ioBombMissileWeapon2.h"

#include "ioRocketlauncherItem.h"
#include "ioNpcChar.h"

ActionStopType ioBombMissileWeapon2::m_ActionStopType = AST_NONE;
ActionStopType ioBombMissileWeapon2::m_DieActionStopType = AST_NONE;
ioBombMissileWeapon2::ioBombMissileWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER |
					WP_MINE_EXPLOSION |
					WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_pBombMesh = NULL;
	m_BombState = BS_FLOATING;

	m_bFirst = true;
	m_bResistance = false;
	m_fCurMoveSpeed = 0.0f;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_vAttackFuncTerm.clear();

	m_dwAttackKeyStartTime = 0;

	m_bExcutePressFunc = false;

	m_pTraceTarget = NULL;
	m_fEnemyDistance = 0.0f;
}

ioBombMissileWeapon2::~ioBombMissileWeapon2()
{
	if( m_pBombMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioBombMissileWeapon2::~ioBombMissileWeapon2 - UnReleased Resouce" );
	}
	if( m_pTraceTarget )
		m_pTraceTarget = NULL;
}

bool ioBombMissileWeapon2::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetBombMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioBombMissileWeapon2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	m_dwLiveTime = (DWORD)rkLoader.LoadInt_e( "live_time", 0 );
	m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "rotate_speed", 0 );

	m_bDisableUpDown = rkLoader.LoadBool_e( "disable_updown", false );
	m_bDisableAutoMove = rkLoader.LoadBool_e( "disable_automove", false );
	m_bPlayWoundSound = rkLoader.LoadBool_e( "play_wound_sound", false );
	m_bDisableMapCollision = rkLoader.LoadBool_e( "disable_map_collision", false );

	// 공격키 사용 여부
	m_bDisableAttackKey = rkLoader.LoadBool_e( "disable_attack_key", true );
	// 공격키 허용이면... 공격키에 대한 기능이 2개가 있어 누르는 시간 텀 설정하게 함 (설정한 텀 이상 일때 해당 기능이 동작)
	int iAttackFuncCount = rkLoader.LoadInt_e( "attack_key_func_count", 0 );
	DWORD dwAttackFuncTerm = 0;
	for( int i = 0; i < iAttackFuncCount; i++)
	{
		FunctionTermInfo info;
		wsprintf_e( szBuf, "attack_key_func%d_term", i+1 );
		info.m_dwAttackFuncTerm = (DWORD)rkLoader.LoadInt( szBuf, 0 );
		wsprintf_e( szBuf, "attack_key_func%d_press", i+1 );
		info.m_bPress = rkLoader.LoadBool( szBuf, false );

		m_vAttackFuncTerm.push_back( info );
	}

	m_bEnableTrace			= rkLoader.LoadBool_e( "enable_trace", false );
	m_fTraceMaxDistance		= rkLoader.LoadFloat_e( "trace_max_distance", 0.0f );
}

bool ioBombMissileWeapon2::SetBombMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pBombMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pBombMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pBombMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pBombMesh->GetSkeletonName().IsEmpty() && m_pBombMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
	{
		ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
		if( pLinked )
		{
			vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
			vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
			vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();
			SetPosition( vLinkPosOffset );
		}
	}

	m_pBombMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pBombMesh->GetCollisionBox();

	AddEffectEvent( m_pBombMesh );
	return true;
}

void ioBombMissileWeapon2::DestroyResource( ioPlayStage *pStage )
{
	if( m_pBombMesh )
	{
		g_EffectFactory.DestroyEffect( m_pBombMesh );
		m_pBombMesh = NULL;
	}
}

const ioOrientBox& ioBombMissileWeapon2::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioBombMissileWeapon2::GetType() const
{
	return WT_BOMB_MISSILE2;
}

ActionStopType ioBombMissileWeapon2::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioBombMissileWeapon2::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioBombMissileWeapon2::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioBombMissileWeapon2::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pBombMesh && m_pBombMesh->GetSkeletonName().IsEmpty() && m_pBombMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
}

void ioBombMissileWeapon2::PlayWoundedSound( ioPlayEntity *pEntity )
{
	if( HasPeculiarity( WP_WOUND_NO_SOUND ) && !m_bPlayWoundSound )
		return;

	const ioHashString &szSound = GetAttribute()->m_WoundedSound;

	if( ToBaseChar( pEntity ) )
	{
		std::string szNewSound = ToBaseChar(pEntity)->GetWoundedSoundName( szSound );
		g_SoundMgr.PlaySound( ioHashString(szNewSound.c_str()), GetParentSceneNode() );
	}
	else
	{
		g_SoundMgr.PlaySound( szSound, GetParentSceneNode() );
	}

	StopLoopSound();
}

void ioBombMissileWeapon2::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_BombState )
	{
	case BS_FLOATING:
		{
			DWORD dwLooseTime = 0;
			if( GetOwner() )
				dwLooseTime = GetOwner()->GetActionStopCurAdd();

			if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < dwCurTime )
				Explosion( pStage );
			else
				OnFloating( fTimePerSec, pStage );
		}
		break;
	case BS_EXPLOSION:
		m_BombState = BS_EXPLOSION_END;
		break;
	case BS_EXPLOSION_END:
		SetWeaponDead();
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioBombMissileWeapon2::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	if( m_BombState != BS_FLOATING )
		return true;

	if( m_bDisableMapCollision )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		Explosion( pStage );
		return true;
	}

	return false;
}

void ioBombMissileWeapon2::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bFirst )
	{
		CheckMoveKey();
		if( !m_pTraceTarget )
			CheckMoveDir( fTimePerSec );

		CheckTraceDir( fTimePerSec );

		float fCurSpeed = GetMoveSpeed();
		float fMoveDist = fCurSpeed * fTimePerSec;
		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	}

	m_bFirst = false;
	
	if( m_pBombMesh )
	{
		m_pBombMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioBombMissileWeapon2::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();
	m_WorldAttackBoxForMap = m_WorldAttackBox;

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		if( m_fOwnerChargeScaleRate > FLOAT1 )
		{
			D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
			m_WorldAttackBox.Transform( GetPosition(), vScale, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), vScale, GetOrientation() );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		}
	}
}

bool ioBombMissileWeapon2::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

void ioBombMissileWeapon2::Explosion( ioPlayStage *pStage )
{
	if( m_BombState == BS_EXPLOSION || m_BombState == BS_EXPLOSION_END )
		return;

	m_BombState = BS_EXPLOSION;
	float fHalfRange = m_fExplosionRange;
	m_LocalAttackBox.SetExtents( 0, fHalfRange );
	m_LocalAttackBox.SetExtents( 1, fHalfRange );
	m_LocalAttackBox.SetExtents( 2, fHalfRange );
	UpdateWorldAttackBox( 0.0f, false );

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos =  GetPosition();
		D3DXVECTOR3 vScale = ioMath::UNIT_ALL;

		if( m_fOwnerChargeExplosionRate > FLOAT1 )
		{
			vScale = D3DXVECTOR3( m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate );
		}

		pStage->CreateMapEffect( m_BoomEffect, vPos, vScale );

		ioWeapon::PlayExplosionSound();
	}
}

bool ioBombMissileWeapon2::IsFloating() const
{
	if( m_BombState == BS_FLOATING )
		return true;

	return false;
}

bool ioBombMissileWeapon2::IsExplosion() const
{
	if( m_BombState == BS_EXPLOSION || m_BombState == BS_EXPLOSION_END )
		return true;

	return false;
}

void ioBombMissileWeapon2::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_BombState != BS_FLOATING )
		return;

	bool bCollision = false;
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget) || ToWallZone(pTarget))
		{
			bCollision = true;
		}
		else if( GetTeam() != pTarget->GetTeam() )
		{
			bCollision = true;
		}
		else if( GetOwner() != pTarget )
		{
			ioBaseChar *pChar = ToBaseChar(pTarget);
			if( pChar )
			{
				if( pChar->IsEnableTeamCollision( this ) )
				{
					bCollision = true;
				}
			}
		}
	}
	
	if( !bCollision )
		return;

	ioRocketLauncherItem* pRocketLauncherItem2 = ToRocketLauncherItem( GetOwner()->GetPriorityItem( SPT_ATTACK ) );
	if( pRocketLauncherItem2 )
	{
		ioDummyChar* pTargetDummy = ToDummyChar(pTarget);
		if ( pTargetDummy && pTargetDummy->GetOwnerName() != g_MyInfo.GetPublicID() )
			return;
		ioBaseChar *pChar = ToBaseChar(pTarget);
		ioNpcChar *pNpcChar = ToNpcChar(pTarget);
		if( pChar && pChar->GetCharName() != g_MyInfo.GetPublicID() && !pNpcChar )
			return;
	}

	if( !m_bDisableMapCollision )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget ) || ToWallZone(pTarget) ||
			GetPiercingType() == CPT_NONE )
		{
			Explosion( pStage );
			return;
		}
	}

	CheckCurAttribute( pTarget );

	ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
	pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

	AddCollisionEntity( pTarget );
}

bool ioBombMissileWeapon2::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_BombState != BS_EXPLOSION && m_BombState != BS_EXPLOSION_END )
		return false;

	float fTargetDist = -FLOAT1;
	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();
	D3DXVECTOR3 vDiff = pEntity->GetWorldPosition() - vCenter;
	float fLength = D3DXVec3Length( &vDiff );
	
	float fCurExplosionRange = m_fExplosionRange;
	if( m_fOwnerChargeExplosionRate > FLOAT1 )
	{
		fCurExplosionRange = m_fExplosionRange * m_fOwnerChargeExplosionRate;
	}

	if( !pEntity->CheckSphereDistance( vCenter, fCurExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, fCurExplosionRange ) )
	{
		// 맞은 객체가 UDP 패킷 보내기
		ioBaseChar* pMyChar = ToBaseChar( pEntity );
		ioNpcChar* pNpcChar = ToNpcChar( pEntity );
		if( ToRocketLauncherItem( GetOwner()->GetPriorityItem( SPT_ATTACK ) ) )
		{
			if( pMyChar && pMyChar->GetCharName() == g_MyInfo.GetPublicID() && !pNpcChar )
			{
				SP2Packet kPacket( CUPK_WEAPON_SYNC );
				kPacket << GetOwnerName();
				kPacket << m_dwWeaponIdx;
				kPacket << SSC_CHANGE_DEAD;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		
		return true;
	}

	return false;
}

DWORD ioBombMissileWeapon2::GetCollisionEnableTime() const
{
	if( m_BombState == BS_EXPLOSION || m_BombState == BS_EXPLOSION_END )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

float ioBombMissileWeapon2::GetMoveSpeed()
{
	if( m_bDisableAutoMove && !m_bSetUpKey && !m_bSetDownKey && m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return 0.f;	
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

void ioBombMissileWeapon2::CheckMoveKey()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	bool bChange = false;
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChange = true;
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;
	}

	if( !m_bDisableUpDown )
	{
		if( !m_bSetDownKey && m_bSetUpKey != pOwner->IsJumpKeyDown() )
		{
			bChange = true;
			m_bTargetRot = false;
			m_bSetUpKey = pOwner->IsJumpKeyDown();
		}

		if( !m_bSetUpKey && m_bSetDownKey != pOwner->IsDefenseKeyDown() )
		{
			bChange = true;
			m_bTargetRot = false;
			m_bSetDownKey = pOwner->IsDefenseKeyDown();
		}
	}

	if( !m_bDisableAttackKey )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if( pOwner->IsAttackKeyDown() )
		{
			if( m_dwAttackKeyStartTime == 0 )
				m_dwAttackKeyStartTime = dwCurTime;
			else
			{
				DWORD dwAttackKeyTerm1 = dwCurTime - m_dwAttackKeyStartTime;
				int i = 0;
				for(i = 0; i < (int)m_vAttackFuncTerm.size() - 1; i++)
				{
					if( m_vAttackFuncTerm[ i ].m_bPress )
					{
						if( dwAttackKeyTerm1 >= m_vAttackFuncTerm[ i ].m_dwAttackFuncTerm && dwAttackKeyTerm1 < m_vAttackFuncTerm[ i + 1 ].m_dwAttackFuncTerm )
						{
							CheckFunction( i );
							m_bExcutePressFunc = true;
						}
					}
				}
				if( dwAttackKeyTerm1 >= m_vAttackFuncTerm[ i ].m_dwAttackFuncTerm && m_vAttackFuncTerm[ i ].m_bPress )
				{
					CheckFunction( i );
					m_bExcutePressFunc = true;
				}
			}
		}
		if( pOwner->IsAttackKeyRelease() )
		{
			if( !m_bExcutePressFunc )
			{
				// 마지막 요소 로직은 조금 로직이 다르기때문에 for문 루프에서 제외
				int j = 0;
				DWORD dwAttackKeyTerm2 = dwCurTime - m_dwAttackKeyStartTime;
				for(j = 0; j < (int)m_vAttackFuncTerm.size() - 1; j++)
				{
					if( !m_vAttackFuncTerm[ j ].m_bPress )
					{
						if( dwAttackKeyTerm2 >= m_vAttackFuncTerm[ j ].m_dwAttackFuncTerm && dwAttackKeyTerm2 < m_vAttackFuncTerm[ j + 1 ].m_dwAttackFuncTerm )
							CheckFunction( j );
					}
				}
				if( dwAttackKeyTerm2 >= m_vAttackFuncTerm[ j ].m_dwAttackFuncTerm && !m_vAttackFuncTerm[ j ].m_bPress )
					CheckFunction( j );
			}

			// 어택 키 뗄때 초기화 할 것들
			if( m_bExcutePressFunc )
			{
				m_pTraceTarget = NULL;
				m_bExcutePressFunc = false;
				m_fEnemyDistance = 0.0f;
			}

			m_dwAttackKeyStartTime = 0;
		}
	}

	if( bChange && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << SSC_CHANGE_MOVE;
		kPacket << (int)m_CurDirKey;
		kPacket << m_bSetUpKey;
		kPacket << m_bSetDownKey;
		kPacket << m_vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBombMissileWeapon2::CheckFunction( int iFunc )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_dwAttackKeyStartTime = 0;

	 switch( iFunc )
	 {
	 case 0:
		 {
			DWORD dwCurTime = FRAMEGETTIME();
			if( !m_pTraceTarget )
			{
				ioPlayStage* pPlayStage = pOwner->GetCreator();
				const BaseCharList &rkTargetList = pPlayStage->GetBaseCharList();
				for( int i = 0; i < (int)rkTargetList.size(); i++ )
				{
					if( GetOwner() == rkTargetList[ i ] || GetOwner()->GetTeam() == rkTargetList[ i ]->GetTeam() )
						continue;
					D3DXVECTOR3 vDiff = rkTargetList[ i ]->GetWorldPosition() - GetPosition();
					float fDistance = D3DXVec3Length( &vDiff );

					// 유도 기능 적과의 거리 제한
					if( m_fTraceMaxDistance > 0.0f )
					{
						if( m_fTraceMaxDistance < fDistance )
							continue;
					}
					
					if( fDistance < m_fEnemyDistance || m_fEnemyDistance == 0.0f )
					{
						m_fEnemyDistance = fDistance;
						m_pTraceTarget = rkTargetList[ i ];
					}
				}
			}
		 }
		 break;
	 //case 1:
		// {
		//	 
		// }
		// break;
	 }
}

void ioBombMissileWeapon2::CheckMoveDir( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;
	if( m_bTargetRot )
		return;
	if( m_CurDirKey == ioUserKeyInput::DKI_NONE && !m_bSetUpKey && !m_bSetDownKey )
		return;

	D3DXVECTOR3 vCurMoveDir = m_vMoveDir;
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	D3DXVECTOR3 vTargetDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	}
	if( m_bSetUpKey )
	{
		vTargetDir += ioMath::UNIT_Y;
	}
	else if( m_bSetDownKey )
	{
		vTargetDir += (-ioMath::UNIT_Y);
	}

	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = (float)D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );
	if( fDotValue >= FLOAT1 || m_bDisableAutoMove )
	{
		m_bTargetRot = true;

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vTargetDir );

		m_vMoveDir = qtTargetRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

		SetOrientation( qtTargetRot );
		return;
	}
	if( m_dwRotateSpeed == 0 )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -FLOAT1 )
	{
		D3DXVECTOR3 vTmpAxis;

		if( vCurMoveDir != ioMath::UNIT_Y )
		{
			D3DXVec3Cross( &vTmpAxis, &ioMath::UNIT_Y, &vCurMoveDir );
			D3DXVec3Normalize( &vTmpAxis, &vTmpAxis );
		}
		else
		{
			D3DXVec3Cross( &vTmpAxis, &ioMath::UNIT_X, &vCurMoveDir );
			D3DXVec3Normalize( &vTmpAxis, &vTmpAxis );
		}

		D3DXVec3Cross( &vAxis, &vTmpAxis, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &vAxis );
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vCurMoveDir, &vTargetDir );
		D3DXVec3Normalize( &vAxis, &vAxis );
	}

	float fCurRotateAngle = fRotateAngle;
	if( fGapAngle < fRotateAngle )
		fCurRotateAngle = fGapAngle;

	D3DXQUATERNION qtNewRot;
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	m_vMoveDir = vNewDir;
	GetParentSceneNode()->Rotate( vAxis, fCurRotateAngle );
}

void ioBombMissileWeapon2::CheckTraceDir( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	bool bChange = false;
	if( m_bEnableTrace && m_pTraceTarget )
	{
		if( m_CurDirKey == ioUserKeyInput::DKI_NONE && !m_bSetUpKey && !m_bSetDownKey )
		{
			D3DXVECTOR3 vDiff = m_pTraceTarget->GetWorldPosition() - GetPosition();
			D3DXVec3Normalize( &vDiff, &vDiff );

			float fTimeGap = fTimePerSec * FLOAT1000;
			float fCurLoopTimePerSec = fTimeGap / FLOAT1000;
			D3DXVECTOR3 vPreDir = m_vMoveDir;
			D3DXVECTOR3 vCurForce = m_vMoveDir * m_fCurMoveSpeed * fCurLoopTimePerSec;
			m_vMoveDir = vCurForce + (vDiff * m_dwRotateSpeed * fCurLoopTimePerSec);

			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

			float fCos = D3DXVec3Dot( &vPreDir, &m_vMoveDir );
			// 0도 일때
			if( fCos >= FLOAT1 )
			{
				// 처리 필요없음
			}
			else if( fCos <= -FLOAT1 ) // 180도 일때
			{
				D3DXVECTOR3 vAxis1, vAxis2;
				if( m_vMoveDir != ioMath::UNIT_Y )
				{
					D3DXVec3Cross( &vAxis1, &m_vMoveDir, &ioMath::UNIT_Y );
					D3DXVec3Normalize( &vAxis1, &-vAxis1 );
				}
				else
				{
					D3DXVec3Cross( &vAxis1, &m_vMoveDir, &ioMath::UNIT_X );
					D3DXVec3Normalize( &vAxis1, &-vAxis1 );
				}

				D3DXVec3Cross( &vAxis2, &m_vMoveDir, &vAxis1 );
				D3DXVec3Normalize( &vAxis2, &-vAxis2 );
				GetParentSceneNode()->Rotate( vAxis2, FLOAT1 );
			}
			else // 그외
			{
				float fGapAngle = RADtoDEG( acos( fCos ) );

				D3DXVECTOR3 vAxis;
				D3DXVec3Cross( &vAxis, &vDiff, &m_vMoveDir );
				D3DXVec3Normalize( &vAxis, &-vAxis );
				GetParentSceneNode()->Rotate( vAxis, fGapAngle );
			}

			bChange = true;
		}
	}

	if( bChange && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << SSC_CHANGE_MOVE;
		kPacket << (int)m_CurDirKey;
		kPacket << m_bSetUpKey;
		kPacket << m_bSetDownKey;
		kPacket << m_vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBombMissileWeapon2::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_CHANGE_MOVE:
		{
			int iDirKey;
			D3DXVECTOR3 vMoveDir;

			rkPacket >> iDirKey;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDirKey;

			rkPacket >> m_bSetUpKey;
			rkPacket >> m_bSetDownKey;
			rkPacket >> vMoveDir;

			m_bTargetRot = false;
			SetMoveDir( vMoveDir );
		}
		break;
	case SSC_CHANGE_DEAD:
		{
			if( m_bLive )
				Explosion( pStage );
		}
		break;
	}
}

void ioBombMissileWeapon2::SetLiveTime( DWORD dwTime )
{
	if( m_dwLiveTime == 0 )
		m_dwLiveTime = dwTime;

	return;
}