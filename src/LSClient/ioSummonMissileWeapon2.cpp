

#include "stdafx.h"

#include "ioSummonMissileWeapon2.h"
#include "ItemDefine.h"

ActionStopType ioSummonMissileWeapon2::m_ActionStopType = AST_NONE;
ActionStopType ioSummonMissileWeapon2::m_DieActionStopType = AST_NONE;

ioSummonMissileWeapon2::ioSummonMissileWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_WOUND_NO_SOUND | WP_DEF_NO_SOUND );

	m_pIceMesh = NULL;
	m_IceState = IS_MOVE;

	m_bFirst = true;
	m_bResistance = false;
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;
	m_dwCurRotateTime = 0;

	m_eDirKey = ioUserKeyInput::DKI_NONE;
	m_nCurChangeDirCnt = 0;
	m_bUseItemGauge = false;
}

ioSummonMissileWeapon2::~ioSummonMissileWeapon2()
{
	if( m_pIceMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioSummonMissileWeapon2::~ioSummonMissileWeapon2 - UnReleased Resouce" );
	}
}

bool ioSummonMissileWeapon2::InitResource()
{
	return true;
}

void ioSummonMissileWeapon2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedTeamEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueTeamEffect = szBuf;

	m_OwnerRotate = (OwnerRotType)rkLoader.LoadInt_e( "owner_rotate", ORT_NONE );

	m_dwMinRotateTime = (DWORD)rkLoader.LoadInt_e( "min_rotate_speed", 0 );
	m_dwMaxRotateTime = (DWORD)rkLoader.LoadInt_e( "max_rotate_speed", 0 );

	m_fMinMoveSpeed = rkLoader.LoadFloat_e( "min_move_speed", 0.0f );
	m_fMaxMoveSpeed = rkLoader.LoadFloat_e( "max_move_speed", 0.0f );

	m_fRotateStartRange = rkLoader.LoadFloat_e( "rotate_start_range", 0.0f );
	m_fRotateEndRange = rkLoader.LoadFloat_e( "rotate_end_range", 0.0f );

	m_bEndExplosion = rkLoader.LoadBool_e( "end_explosion", false );
	m_fEnableMoveGap = rkLoader.LoadFloat_e( "enable_move_gap_height", 0.0f );

	int iCnt = rkLoader.LoadInt_e( "change_dir_cnt", 0 );
	if( iCnt > 0 )
	{
		m_ChangeDirInfoList.clear();
		m_ChangeDirInfoList.reserve( iCnt );

		for( int i=0; i < iCnt; ++i )
		{
			ChangeDirInfo kInfo;

			wsprintf_e( szBuf, "change_range%d", i+1 );
			kInfo.m_fChangeRange = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "change_speed%d", i+1 );
			kInfo.m_fChangeSpeed = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "change_angle_h%d", i+1 );
			kInfo.m_fChangeAngleH = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "change_angle_v%d", i+1 );
			kInfo.m_fChangeAngleV = rkLoader.LoadFloat( szBuf, 0.0f );

			m_ChangeDirInfoList.push_back( kInfo );
		}

		std::sort( m_ChangeDirInfoList.begin(), m_ChangeDirInfoList.end(), ChangeDirSort() );
	}

	m_nMaxChangeDirCnt = rkLoader.LoadInt("max_rotate_cnt", 0);

	rkLoader.LoadString( "reflect_effect", "", szBuf,  MAX_PATH );
	m_stReflectEffect = szBuf;

	rkLoader.LoadString( "reflect_sound", "", szBuf,  MAX_PATH );
	m_szReserveSound = szBuf;

	m_fRotateDirAddRange = rkLoader.LoadFloat_e( "rotate_dir_add_range", 0.0f );
	m_bCheckOwnerState = rkLoader.LoadBool_e( "check_owner_state", false );
	m_bUseItemGauge = rkLoader.LoadBool_e( "use_item_gauge", false );
}

void ioSummonMissileWeapon2::SetWeaponIndex( DWORD dwIndex )
{
	m_dwWeaponIdx = dwIndex;

	// Set Seed
	m_Random.SetRandomSeed( m_dwWeaponIdx );
}

void ioSummonMissileWeapon2::SetTeam( ioPlayStage *pStage, TeamType eType )
{
	if( !pStage ) return;

	ioWeapon::SetTeam( pStage, eType );

	DestroyResource();

	ioHashString szCurWeaponEffect;

	if( eType == TEAM_BLUE )
	{
		szCurWeaponEffect = m_BlueTeamEffect;
	}
	else if( eType == TEAM_RED )
	{
		szCurWeaponEffect = m_RedTeamEffect;
	}
	else //개인전
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->GetSingleConvertTeam() == TEAM_BLUE )
		{
			szCurWeaponEffect = m_BlueTeamEffect;
		}
		else
		{
			szCurWeaponEffect = m_RedTeamEffect;
		}
	}

	if( !SetIceMesh( szCurWeaponEffect ) )
		DestroyResource();
}

bool ioSummonMissileWeapon2::SetIceMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pIceMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pIceMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pIceMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pIceMesh->GetSkeletonName().IsEmpty() && m_pIceMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pIceMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pIceMesh->GetCollisionBox();

	m_vPrePos = GetPosition();

	m_fDefaultGapHeight = fabs( m_vPrePos.y - pOwner->GetWorldPosition().y );

	AddEffectEvent( m_pIceMesh );
	return true;
}

void ioSummonMissileWeapon2::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

D3DXVECTOR3 ioSummonMissileWeapon2::GetMoveDir()
{
	return m_vMoveDir;
}

void ioSummonMissileWeapon2::CheckChangeDir()
{
	if( m_ChangeDirInfoList.empty() ) return;

	float fChangeRange = m_ChangeDirInfoList[0].m_fChangeRange;

	if( m_fCurMoveRange >= fChangeRange )
	{
		if( m_ChangeDirInfoList[0].m_fChangeSpeed > 0.0f )
			m_fCurMoveSpeed = m_ChangeDirInfoList[0].m_fChangeSpeed;

		RotateMoveDir( m_ChangeDirInfoList[0].m_fChangeAngleH, m_ChangeDirInfoList[0].m_fChangeAngleV );
		m_ChangeDirInfoList.erase( m_ChangeDirInfoList.begin() );
	}
}

void ioSummonMissileWeapon2::RotateMoveDir( float fAngleH, float fAngleV )
{ 
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	m_vMoveDir = qtRot * m_vMoveDir;

	// horizon
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
	m_vMoveDir = qtRot * m_vMoveDir;
	 
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioSummonMissileWeapon2::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	//key setting
	if( GetOwner() && GetOwner()->IsNeedProcess() )
	{
		m_eDirKey = GetOwner()->CheckCurDirKey();
		m_OwnerState = GetOwner()->GetState();
	}

	if( m_pIceMesh && m_pIceMesh->GetSkeletonName().IsEmpty() && m_pIceMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );

	m_fDefaultGapHeight = 0.0f;
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		m_fDefaultGapHeight = fabs( vPos.y - pOwner->GetWorldPosition().y );
	}
}

void ioSummonMissileWeapon2::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	switch( m_IceState )
	{
	case IS_MOVE:
		CheckMoveKey( GetOwner() );
		OnFloating( fTimePerSec, pStage );
		CheckOwnerState( GetOwner() );
		break;
	case IS_EXPLOSION_WAIT:
		Explosion( pStage );
		break;
	case IS_EXPLOSION:
		{
			CheckCreateWeaponList();
			SetWeaponDead();
		}
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioSummonMissileWeapon2::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	if( m_IceState != IS_MOVE )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->IsNeedProcess() )
			SetExplosion( pStage, true );

		return true;
	}

	return false;
}

void ioSummonMissileWeapon2::CreateMapEffect( ioBaseChar* pOwner, const D3DXVECTOR3& vPos, const D3DXQUATERNION& qt )
{
	if( !pOwner )
		return;
	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_stReflectEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	if( pMapEffect )
	{
		pMapEffect->SetWorldOrientation( qt );
	}
}

void ioSummonMissileWeapon2::CheckMoveKey( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_nCurChangeDirCnt >= m_nMaxChangeDirCnt )
		return;

	ioSummonChargeItem3 *pSummonChargeItem3 = ToSummonChargeItem3( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( m_bUseItemGauge && ( !pSummonChargeItem3 || !pSummonChargeItem3->IsEnableGauge() ) )
		return;

	D3DXQUATERNION qtRotate;
	int nCurKey = pOwner->CheckCurDirKey();
	if( m_OwnerState != pOwner->GetState() && pOwner->GetState() == CS_JUMP )
	{
		m_nCurChangeDirCnt++;
		m_OwnerState = CS_JUMP;
		m_vMoveDir = D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );
		ioMath::CalcDirectionQuaternion( qtRotate, -m_vMoveDir );
		SetOrientation( qtRotate );

		CreateMapEffect( pOwner, GetPosition(), qtRotate );
		if( m_bUseItemGauge )
			pSummonChargeItem3->CheckDecreaseGauge();

		g_SoundMgr.PlaySound( m_szReserveSound, GetParentSceneNode() );
		
		if( pOwner->IsNeedProcess() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << SSC_CHANGE_DIR;
			kPacket << GetPosition();
			kPacket << m_vMoveDir;
			kPacket << m_nCurChangeDirCnt;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	m_OwnerState = pOwner->GetState();

	if( nCurKey == ioUserKeyInput::DKI_NONE || nCurKey == m_eDirKey )
		return;

	m_nCurChangeDirCnt++;

	m_eDirKey = (ioUserKeyInput::DirKeyInput)nCurKey;

	float fYawD = 180.0f + 45.0f * ( nCurKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	m_vMoveDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);

	ioMath::CalcDirectionQuaternion( qtRotate, -m_vMoveDir );
	SetOrientation( qtRotate );
	CreateMapEffect( pOwner, GetPosition(), qtRotate );
	if( m_bUseItemGauge )
		pSummonChargeItem3->CheckDecreaseGauge();

	g_SoundMgr.PlaySound( m_szReserveSound, GetParentSceneNode() );
	
	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << SSC_CHANGE_DIR;
		kPacket << GetPosition();
		kPacket << m_vMoveDir;
		kPacket << m_nCurChangeDirCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSummonMissileWeapon2::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bFirst )
	{
		CheckChangeDir();
	}

	float fCurSpeed = GetMoveSpeed();
	float fMoveDist = fCurSpeed * fTimePerSec;

	m_vPrePos = GetPosition();
	D3DXVECTOR3 vNewPos = m_vPrePos + (m_vMoveDir * fMoveDist);
	SetPosition( vNewPos );
	m_fCurMoveRange += fMoveDist;

	float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;
	if( m_fCurMoveRange >= fMaxRange )
	{
		if( m_bEndExplosion )
		{
			Explosion( pStage );
			return;
		}
		else
		{
			SetWeaponDead();
		}
	}

	m_bFirst = false;

	if( m_pIceMesh )
	{
		m_pIceMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioSummonMissileWeapon2::DestroyResource( ioPlayStage *pStage )
{
	if( m_pIceMesh )
	{
		g_EffectFactory.DestroyEffect( m_pIceMesh );
		m_pIceMesh = NULL;
	}
}

const ioOrientBox& ioSummonMissileWeapon2::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioSummonMissileWeapon2::GetType() const
{
	return WT_SUMMON_MISSILE2;
}

ActionStopType ioSummonMissileWeapon2::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioSummonMissileWeapon2::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioSummonMissileWeapon2::GetMoveSpeed()
{
	return m_fMaxMoveSpeed;
}

DWORD ioSummonMissileWeapon2::GetRotateSpeed()
{
	return m_dwMaxRotateTime;
}

float ioSummonMissileWeapon2::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate + m_nCurChangeDirCnt * m_fRotateDirAddRange;
}

void ioSummonMissileWeapon2::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioSummonMissileWeapon2::SetExplosion( ioPlayStage *pStage, bool bSendNet )
{
	Explosion( pStage );

	if( bSendNet )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << SSC_EXPLOER;
			kPacket << GetPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioSummonMissileWeapon2::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	D3DXQUATERNION qtRotate;
	int iSyncType;
	rkPacket >> iSyncType;

	D3DXVECTOR3 vPos;

	switch( iSyncType )
	{
	case SSC_EXPLOER:
		rkPacket >> vPos;
		SetPosition( vPos );
		Explosion( pStage );
		break;
	case SSC_CHANGE_DIR:
		rkPacket >> vPos;
		rkPacket >> m_vMoveDir;
		rkPacket >> m_nCurChangeDirCnt;
		SetPosition( vPos );
		ioMath::CalcDirectionQuaternion( qtRotate, -m_vMoveDir );
		SetOrientation( qtRotate );
		CreateMapEffect( GetOwner(), GetPosition(), qtRotate );
		g_SoundMgr.PlaySound( m_szReserveSound, GetParentSceneNode() );
		break;
	}
}

void ioSummonMissileWeapon2::Explosion( ioPlayStage *pStage )
{
	if( m_IceState == IS_EXPLOSION )
		return;

	m_IceState = IS_EXPLOSION;

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos =  GetPosition();
		ioMapEffect *pIceBoom = pStage->CreateMapEffect( m_BoomEffect, vPos, ioMath::UNIT_ALL );
		if( pIceBoom && pIceBoom->GetEffect() )
		{
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
			pIceBoom->SetWorldOrientation( qtRot );

			pIceBoom->GetEffect()->UpdateBounds( true );
			m_LocalAttackBox = pIceBoom->GetEffect()->GetCollisionBox();

			UpdateWorldAttackBox( 0.0f, false );
		}

		ioWeapon::PlayExplosionSound();
	}
}

bool ioSummonMissileWeapon2::IsFloating() const
{
	if( m_IceState == IS_MOVE )
		return true;

	return false;
}

bool ioSummonMissileWeapon2::IsExplosion() const
{
	if( m_IceState == IS_EXPLOSION )
		return true;

	return false;
}

void ioSummonMissileWeapon2::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_IceState != IS_MOVE )
		return;

	ioBaseChar *pChar = ToBaseChar(pTarget);
	if( pChar && !pChar->IsNeedProcess() )
		return;

	bool bCollision = false;
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget) || ToWallZone(pTarget) )
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

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) || ToDummyChar(pTarget) || ToWallZone(pTarget) )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->IsNeedProcess() )
			SetExplosion( pStage, true );

		return;
	}
	else if( GetPiercingType() == CPT_NONE )
	{
		ioBaseChar *pChar = ToBaseChar(pTarget);
		if( pChar )
		{
			SetExplosion( pStage, true );
		}

		return;
	}

	CheckCurAttribute( pTarget );

	ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
	pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

	AddCollisionEntity( pTarget );

	ioWeapon::PlayExplosionSound();
}

bool ioSummonMissileWeapon2::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	CheckPiercing( bCollision );

	return bCollision;
}

void ioSummonMissileWeapon2::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

bool ioSummonMissileWeapon2::CheckCollisionLine( ioPlayStage *pStage )
{
	if( !pStage ) return false;

	D3DXVECTOR3 vPos = GetPosition();

	if( m_vPrePos == vPos )
		return false;

	bool bMapCollision = pStage->CheckCollisionLine( m_vPrePos, vPos, true );

	if( !bMapCollision )
	{
		Explosion( pStage );
		return true;
	}

	return false;
}

void ioSummonMissileWeapon2::SetExplosionWait()
{
	if( m_IceState != IS_MOVE )
		return;

	m_IceState = IS_EXPLOSION_WAIT;
}

void ioSummonMissileWeapon2::CheckOwnerState( ioBaseChar *pOnwer )
{
	if ( !m_bCheckOwnerState )
		return;

	if ( !pOnwer || !m_bLive )
		return;

	if ( pOnwer->IsWoundedState())
	{
		SetWeaponDead();
		return;
	}
}