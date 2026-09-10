

#include "stdafx.h"

#include "ioFlamewaveWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioFlamewaveWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioFlamewaveWeapon::m_DieActionStopType = AST_NONE;

ioFlamewaveWeapon::ioFlamewaveWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MINE_EXPLOSION );

	m_dwMoveDelayEffectID = -1;
	m_dwEndEffectID = -1;

	m_pFireBall = NULL;
	m_fCurMoveRange = 0.0f;

	m_bResistance = false;
	m_bSetGraceClose = false;

	m_bFirst = true;
	m_bLink = false;
	m_bYRot = false;

	//
	m_bUseCustomRange = false;
	m_fCustomMaxRange = 0.0f;
}

ioFlamewaveWeapon::~ioFlamewaveWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioFlamewaveWeapon::~ioFlamewaveWeapon - UnReleased Resouce" );
	}
}

bool ioFlamewaveWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioFlamewaveWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "move_delay_effect", "", szBuf, MAX_PATH );
	m_szMoveDelayEffect = szBuf;
	m_FlameMoveType = (FlameMoveType)rkLoader.LoadInt_e( "flame_move_type", FMT_NORMAL );
	m_dwRotateTime = rkLoader.LoadInt_e( "rotate_speed", 0 );

	m_vStartOffSet.x = rkLoader.LoadFloat_e( "start_offset_x", 0.0f );
	m_vStartOffSet.y = rkLoader.LoadFloat_e( "start_offset_y", 0.0f );
	m_vStartOffSet.z = rkLoader.LoadFloat_e( "start_offset_z", 0.0f );

	m_bYRot = rkLoader.LoadBool_e( "YRot", false );
}

bool ioFlamewaveWeapon::CreateFireBall( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_dwMoveDelayEffectID = -1;
	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )
		return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );

	//
	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
	{
		ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
		if( pLinked )
		{
			m_bLink = true;

			vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
			vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
			vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();
			SetPosition( vLinkPosOffset );
		}
	}
	//
	AddEffectEvent( m_pFireBall );
	return true;
}
 
void ioFlamewaveWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vFirstMoveDir = vDir;
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}


void ioFlamewaveWeapon::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
	
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioFlamewaveWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( !pStage )
	{
		SetPosition( vPos );
		return;
	}

	D3DXVECTOR3 vStartPos;
	if( m_pFireBall && m_bLink )
	{
		vStartPos = GetPosition();
		vStartPos.y = pStage->GetMapHeight( vStartPos.x, vStartPos.z, GetOwner() ) - 3.0f;
		vStartPos.y = max( 0.0f, vStartPos.y );
		SetPosition( vStartPos );
		return;
	}

	switch( m_FlameMoveType )
	{
	case FMT_NORMAL:
		{
			vStartPos.x = vPos.x;
			vStartPos.y = pStage->GetMapHeight( vPos.x, vPos.z, GetOwner() ) - 3.0f;
			vStartPos.y = max( 0.0f, vStartPos.y );
			vStartPos.z = vPos.z;
		}
		break;
	case FMT_OWNER_RL:
		{
			vStartPos = vPos;

			ioBaseChar *pOwner = GetOwner();
			if( pOwner && m_vStartOffSet != ioMath::VEC3_ZERO )
			{
				D3DXQUATERNION qtRot = pOwner->GetTargetRot();
				D3DXVECTOR3 vOffSet = qtRot * m_vStartOffSet;

				vStartPos += vOffSet;
			}
		}
		break;
	case FMT_CUSTOM_RANGE:
		{
			vStartPos = vPos;
		}
		break;
	}

	SetPosition( vStartPos );
}

void ioFlamewaveWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( !CheckPiercing() )
		return;

	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
	{
		if( m_dwMoveDelayEffectID == -1 && !m_szMoveDelayEffect.IsEmpty() )
		{
			ioBaseChar *pOwner = GetOwner();
			if( pOwner )
			{
				D3DXVECTOR3 vScale = pOwner->GetWorldScale();
				ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szMoveDelayEffect, GetPosition(), vScale );
				if( pMapEffect )
				{
					m_dwMoveDelayEffectID = pMapEffect->GetUniqueID();
				}
			}
		}
		return;
	}

	if( m_FlameMoveType == FMT_CUSTOM_RANGE )
	{
		if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime < dwCurTime )
			m_bMoveDelay = false;
	}

	if( !m_bFirst )
	{
		switch( m_FlameMoveType )
		{
		case FMT_OWNER_RL:
			CheckOwnerRLMoveDir( fTimePerSec );
			break;
		}

		ProcessMove( fTimePerSec, pStage );
	}
	else
	{
		if( m_dwMoveDelayEffectID != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwMoveDelayEffectID );
			if( pMapEffect )
			{
				pMapEffect->EndEffectForce();
			}
		}
	}

	m_bFirst = false;
	
	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioFlamewaveWeapon::CheckOwnerRLMoveDir( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_dwMoveDelayTime > 0 && m_bMoveDelay )
	{
		m_bMoveDelay = false;

		D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		m_vMoveDir = vTargetDir;

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vTargetDir );
		SetOrientation( qtTargetRot );
		return;
	}

	if( m_dwRotateTime == 0 )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXVECTOR3 vCurMoveDir = m_vMoveDir;
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	D3DXQUATERNION	qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vTargetDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = (float)D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= 0.98f )
	{
		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vTargetDir );

		m_vMoveDir = qtTargetRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

		SetOrientation( qtTargetRot );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
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

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	SetOrientation( qtNewRot );
}

void ioFlamewaveWeapon::ProcessMove( float fTimePerSec, ioPlayStage *pStage )
{
	float fMoveDist = GetAttribute()->m_fMoveSpeed * fTimePerSec;

	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	m_fCurMoveRange += fMoveDist;

	float fMaxRange = 0;
	if( m_bUseCustomRange )
		fMaxRange = m_fCustomMaxRange;
	else
		fMaxRange = GetAttribute()->m_fMoveRange + m_fGrowthMoveRange;

	if( m_fCurMoveRange <= fMaxRange )
	{
		UpdateWorldAttackBox( fTimePerSec, false );
	}
	else
	{
		UpdateWorldAttackBox( fTimePerSec, true );

		SetGraceClose();

		if( m_dwEndEffectID = -1 && !GetWeaponEndEffect().IsEmpty() )
		{
			ioMapEffect *pMapEffect = pStage->CreateMapEffect( GetWeaponEndEffect(), GetPosition(), GetScale() );
			if( pMapEffect )
				m_dwEndEffectID = pMapEffect->GetUniqueID();
		}

		if( !m_pFireBall || !m_pFireBall->IsLive() )
			SetWeaponDead();
	}
}

void ioFlamewaveWeapon::SetCustomRange(bool b, float fLen)
{
	m_bUseCustomRange = b;
	m_fCustomMaxRange = fLen;
	m_FlameMoveType = FMT_CUSTOM_RANGE;
}

void ioFlamewaveWeapon::SetGraceClose()
{
	if( m_bSetGraceClose )	return;

	if( m_pFireBall )
	{
		if( m_bEndEffectGrace )
			m_pFireBall->EndEffectGrace();
		else
			m_pFireBall->EndEffectForce();
	}

	StopLoopSound();
	m_bSetGraceClose = true;
}

void ioFlamewaveWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

D3DXVECTOR3 ioFlamewaveWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

const ioOrientBox& ioFlamewaveWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioFlamewaveWeapon::GetType() const
{
	return WT_FLAME_WAVE;
}

ActionStopType ioFlamewaveWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioFlamewaveWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}
	

bool ioFlamewaveWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_dwMoveDelayTime > 0 && m_bMoveDelay )
		return false;

	if( !pEntity )
		return false;

	bool bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision) );

	CheckPiercing( bCollision );

	return bCollision;
}


void ioFlamewaveWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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
		if( m_bYRot )//Y축 고정일때( 제갈량 SD )
		{
			m_WorldAttackBox.Transform( GetPosition(),
				GetScale(),
				ioMath::QUAT_IDENTITY );

			m_WorldAttackBoxForMap.Transform( GetPosition(),
				GetScale(),
				ioMath::QUAT_IDENTITY );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(),
				GetScale(),
				GetOrientation() );

			m_WorldAttackBoxForMap.Transform( GetPosition(),
				GetScale(),
				GetOrientation() );
		}
	}
}
