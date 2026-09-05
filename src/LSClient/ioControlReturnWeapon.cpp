#include "stdafx.h"

#include "ioControlReturnWeapon.h"

ActionStopType ioControlReturnWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioControlReturnWeapon::m_DieActionStopType = AST_NONE;

ioControlReturnWeapon::ioControlReturnWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );
	m_pThrowMesh = NULL;
	m_fCurMoveRange = 0.0f;
	m_fCurMoveDistance = 0.0f;
	m_ReturnState = TRS_NONE;
	m_dwStartReturnTime = 0;

	m_fDefaultHeightGap = 0.0f;
	m_fControlValue = m_fControlValue2 = 0.0f;
	m_fReturnHeightMoveValue = 0.f;
	
	m_bWallCollision = false;
	m_vPrePos = D3DXVECTOR3( 0.f, 0.f, 0.f);
}

ioControlReturnWeapon::~ioControlReturnWeapon()
{
	if( m_pThrowMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioControlReturnWeapon::~ioControlReturnWeapon - UnReleased Resouce" );
	}
}

bool ioControlReturnWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetThrowMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioControlReturnWeapon::SetThrowMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	DestroyResource();

	m_pThrowMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pThrowMesh ) return false;

	GetParentSceneNode()->AttachObject( m_pThrowMesh );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pThrowMesh->GetSkeletonName().IsEmpty() && m_pThrowMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pThrowMesh->UpdateBounds( true );

	AddEffectEvent( m_pThrowMesh );
	return true;
}

void ioControlReturnWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pThrowMesh )
	{
		g_EffectFactory.DestroyEffect( m_pThrowMesh );
		m_pThrowMesh = NULL;
	}
}

void ioControlReturnWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	// 공통
	m_fControlSpeed = rkLoader.LoadFloat_e( "control_speed", 0.0f );
	m_fReturnControlSpeed = rkLoader.LoadFloat_e( "return_control_speed", 0.0f );

	m_fStartAngle = rkLoader.LoadFloat_e( "start_angle", 0.0f );
	m_dwReturnDuration = rkLoader.LoadInt_e( "return_duration", 0 );
	m_fEndRange = rkLoader.LoadFloat_e( "return_end_range", 0.0f );

	m_MoveType = (ControlMoveType)rkLoader.LoadInt_e( "control_move_type", CMT_NO1 );

	m_bIgnoreCharMove = rkLoader.LoadBool_e( "ignore_char_move", false );

	// NO2
	m_fTargetOffset = rkLoader.LoadFloat_e( "target_offset", 0.0f );
	m_fTargetOffset2 = rkLoader.LoadFloat_e( "target_offset2", 0.0f );

	m_bRotateMoveDir = rkLoader.LoadBool_e( "rotate_move_dir", false );

	int iCnt = rkLoader.LoadInt_e( "change_dir_cnt", 0 );
	if( iCnt > 0 )
	{
		char szBuf[MAX_PATH] = "";

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
	}

	m_fReturnHeightMoveUpSpeed = rkLoader.LoadFloat_e( "return_height_move_up_speed", 0.0f );
	m_fReturnHeightMoveDownSpeed = rkLoader.LoadFloat_e( "return_height_move_down_speed", 0.0f );
	m_dwReturnHeightMoveTime = rkLoader.LoadInt_e( "return_height_move_time", 0 );
	m_bWallCollision = rkLoader.LoadBool_e( "wall_collision", false );
}

void ioControlReturnWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	SetPosition( vPos );

	if( GetOwner() )
	{
		m_fDefaultHeightGap = vPos.y - GetOwner()->GetWorldPosition().y;
	}

	m_vPrePos = vPos;
}

void ioControlReturnWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	m_vCurMoveDir = m_vMoveDir;

	m_vRollAxis	= D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );

	D3DXVec3Cross( &m_vCrossVec, &m_vMoveDir, &m_vRollAxis );
	D3DXVec3Normalize( &m_vCrossVec, &m_vCrossVec );

	if( m_fStartAngle != 0.0f )
	{
		D3DXQUATERNION qtRot2;
		D3DXQuaternionRotationAxis( &qtRot2, &m_vRollAxis, DEGtoRAD(m_fStartAngle) );

		m_vCurMoveDir = qtRot2 * m_vCurMoveDir;
	}

	m_vFirstMoveDir = m_vCurMoveDir;
}

void ioControlReturnWeapon::RotateMoveDir( float fAngleH, float fAngleV )
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

	SetMoveDir( m_vMoveDir );
}

void ioControlReturnWeapon::SetReturnWay( ioPlayStage *pStage )
{
	if( m_ReturnState == TRS_NONE )
	{
		m_ReturnState = TRS_RETRUN;
		m_dwStartReturnTime = FRAMEGETTIME();

		float fMaxRange = GetAttribute()->m_fMoveRange;
		m_fCurMoveRange = fMaxRange;
	}
}

void ioControlReturnWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( fTimePerSec == 0.0f )
	{
		fTimePerSec = 0.001f;
	}

	CheckChangeDir();

	D3DXVECTOR3 vMove;
	m_fCurMoveDistance = GetMoveSpeed() * fTimePerSec;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetDieState( pStage, false );
		return;
	}

	switch( m_ReturnState )
	{
	case TRS_NONE:
		if( CheckMaxRange( fTimePerSec ) )
		{
			SetReturnWay( pStage );
		}
		break;
	case TRS_RETRUN:
		if( CheckMaxRange( fTimePerSec ) )
		{
			SetDieState( pStage, false );
			return;
		}
		break;
	}

	// check movetype
	switch( m_MoveType )
	{
	case CMT_NO1:
		if( CheckOwnerControl1( fTimePerSec ) )
		{
			SetDieState( pStage, false );
			return;
		}
		break;
	case CMT_NO2:
		if( CheckOwnerControl2( fTimePerSec ) )
		{
			SetDieState( pStage, false );
			return;
		}
		break;
	}

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vCurMoveDir );

	if( m_bRotateMoveDir )
	{
		D3DXVECTOR3 vNowPos = GetPosition() - m_vPrePos;
		D3DXVec3Normalize( &vNowPos, &vNowPos );
		ioMath::CalcDirectionQuaternion( qtRot, -vNowPos );
	}

	SetOrientation( qtRot );

	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pThrowMesh )
		m_pThrowMesh->Update( fTimePerSec * FLOAT1000 );

	m_vPrePos = GetPosition();
}

bool ioControlReturnWeapon::CheckOwnerControl1( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return false;

	if( fTimePerSec == 0.0f )
	{
		fTimePerSec = 0.001f;
	}

	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vPreCharPos = pOwner->GetPrePosition();

	D3DXVECTOR3 vPos = GetPosition();
	D3DXVECTOR3 vTargetPos = pOwner->GetMidPositionByRate();

	if( m_ReturnState == TRS_NONE )
	{
		D3DXVECTOR3 vDiff = vCharPos - vPreCharPos;
		D3DXVECTOR3 vFrontMove = m_fCurMoveDistance * m_vCurMoveDir;
		D3DXVECTOR3 vSideMove = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

		float fLength = m_fControlSpeed * fTimePerSec;

		// 수평
		D3DXVECTOR3 vCharXZ = vCharPos;
		vCharXZ.y = 0.0f;
		D3DXVec3Normalize( &vCharXZ, &vCharXZ );
		D3DXVECTOR3 vPreCharXZ = vPreCharPos;
		vPreCharXZ.y = 0.0f;
		D3DXVec3Normalize( &vPreCharXZ, &vPreCharXZ );

		D3DXVECTOR3 vXZDir = vDiff;
		vXZDir.y = 0.0f;

		float fLength2 = D3DXVec3Length( &vXZDir );
		D3DXVec3Normalize( &vXZDir, &vXZDir );

		if( vCharXZ != vPreCharXZ && m_vMoveDir != vXZDir )
		{
			float fDotValue = D3DXVec3Dot( &m_vCrossVec, &vXZDir );
			float fNewLength = fDotValue * fLength2;

			m_fControlValue += fNewLength;
		}

		if( fabs(m_fControlValue) > 0.001f && fLength <= fabs(m_fControlValue) )
		{
			if( m_fControlValue < 0.0f )
			{
				vSideMove += (fLength*fTimePerSec) * (-m_vCrossVec);
				m_fControlValue += fLength;
			}
			else if( m_fControlValue > 0.0f )
			{
				vSideMove += (fLength*fTimePerSec) * m_vCrossVec;
				m_fControlValue -= fLength;
			}
		}
		else if( fabs(m_fControlValue) > 0.001f )
		{
			if( m_fControlValue < 0.0f )
			{
				vSideMove += (fLength*fTimePerSec) * (-m_vCrossVec);
				m_fControlValue = 0.0f;
			}
			else if( m_fControlValue > 0.0f )
			{
				vSideMove += (fLength*fTimePerSec) * m_vCrossVec;
				m_fControlValue = 0.0f;
			}
		}

		if( !m_bIgnoreCharMove )
			vFrontMove += vSideMove;

		D3DXVec3Normalize( &vFrontMove, &vFrontMove );
		m_vCurMoveDir = vFrontMove;

		D3DXVECTOR3 vMove = m_fCurMoveDistance * m_vCurMoveDir;

		// 수직
		if( vCharPos.y != vPreCharPos.y )
		{
			float fGap = vCharPos.y - vPreCharPos.y;
			m_fControlValue2 += fGap;
		}

		if( fabs(m_fControlValue2) > 0.001f && fLength <= fabs(m_fControlValue2) )
		{
			if( m_fControlValue2 < 0.0f )
			{
				vMove += fLength * (-m_vRollAxis);
				m_fControlValue2 += fLength;
			}
			else if( m_fControlValue2 > 0.0f )
			{
				vMove += fLength * m_vRollAxis;
				m_fControlValue2 -= fLength;
			}
		}
		else if( fabs(m_fControlValue2) > 0.001f )
		{
			if( m_fControlValue2 < 0.0f )
			{
				vMove += m_fControlValue2 * m_vRollAxis;
				m_fControlValue2 = 0.0f;
			}
			else if( m_fControlValue2 > 0.0f )
			{
				vMove += m_fControlValue2 * m_vRollAxis;
				m_fControlValue2 = 0.0f;
			}
		}

		D3DXVec3Normalize( &vMove, &vMove );
		vMove *= m_fCurMoveDistance;

		GetParentSceneNode()->Translate( vMove );
	}
	else
	{
		D3DXVECTOR3 vDiff = vTargetPos - vPos;
		float fLength = D3DXVec3Length( &vDiff );
		if( fLength <= m_fEndRange )
			return true;

		D3DXVECTOR3 vCurDir = m_vCurMoveDir;
		float fTimeGap = fTimePerSec * FLOAT1000;
		float fMoveSpeed = GetMoveSpeed();

		float fCurLoopTimeGap;
		float fCurLoopTimePerSec;
		while( fTimeGap > 0.0f )
		{
			if( fTimeGap > FLOAT10 )
			{
				fCurLoopTimeGap = FLOAT10;
				fCurLoopTimePerSec = FLOAT10 / FLOAT1000;

				fTimeGap -= FLOAT10;
			}
			else
			{
				fCurLoopTimeGap = fTimeGap;
				fCurLoopTimePerSec = fTimeGap / FLOAT1000;

				fTimeGap = 0.0f;
			}

			vDiff = vTargetPos - GetPosition();
			D3DXVec3Normalize( &vDiff, &vDiff );

			D3DXVECTOR3 vCurXZDir = vCurDir;
			D3DXVECTOR3 vDiffXZDir = vDiff;
			vCurXZDir.y = 0.0f;
			vDiffXZDir.y = 0.0f;
			D3DXVec3Normalize( &vCurXZDir, &vCurXZDir );
			D3DXVec3Normalize( &vDiffXZDir, &vDiffXZDir );

			float fDotValue = D3DXVec3Dot( &vCurXZDir, &vDiffXZDir );
			if( COMPARE( fDotValue, -1.0001f, -0.9999f ) )
			{
				D3DXVECTOR3 vCrossVec;
				D3DXVec3Cross( &vCrossVec, &vCurXZDir, &m_vRollAxis );
				D3DXVec3Normalize( &vCrossVec, &vCrossVec );

				D3DXVECTOR3 vCurForce = vCurXZDir * fMoveSpeed * fCurLoopTimePerSec;
				vCurDir = vCurForce + (vCrossVec * m_fReturnControlSpeed * fCurLoopTimePerSec);

				D3DXVec3Normalize( &vCurDir, &vCurDir );
			}
			else
			{
				D3DXVECTOR3 vCurForce = vCurXZDir * fMoveSpeed * fCurLoopTimePerSec;
				vCurDir = vCurForce + (vDiffXZDir * m_fReturnControlSpeed * fCurLoopTimePerSec);

				D3DXVec3Normalize( &vCurDir, &vCurDir );
			}
		}

		D3DXVec3Normalize( &m_vCurMoveDir, &vCurDir );
		D3DXVECTOR3 vMove = vCurDir * m_fCurMoveDistance;

		// 수직
		DWORD dwCurTime = FRAMEGETTIME();
		if( m_dwReturnHeightMoveTime > 0 && m_dwReturnHeightMoveTime + m_dwStartReturnTime > dwCurTime )
		{
			float fHeightSpeed = m_fReturnHeightMoveUpSpeed * fTimePerSec;
			vMove += fHeightSpeed * m_vRollAxis;
			m_fReturnHeightMoveValue += fHeightSpeed;
		}
		else
		{
			float fReturnHeightSpeed = m_fReturnHeightMoveDownSpeed * fTimePerSec;
			float fHeightSpeed = m_fControlSpeed * fTimePerSec;
			if( vCharPos.y != vPreCharPos.y )
			{
				float fGap = vCharPos.y - vPreCharPos.y;
				m_fControlValue2 += fGap;
			}

			if( fabs(m_fReturnHeightMoveValue) > 0.001f && fReturnHeightSpeed <= fabs(m_fReturnHeightMoveValue) )
			{
				if( m_fReturnHeightMoveValue < 0.0f )
				{
					vMove += fReturnHeightSpeed * m_vRollAxis;
					m_fReturnHeightMoveValue += fReturnHeightSpeed;
				}
				else if( m_fReturnHeightMoveValue > 0.0f )
				{
					vMove -= fReturnHeightSpeed * m_vRollAxis;
					m_fReturnHeightMoveValue -= fReturnHeightSpeed;
				}
			}
			else if( fabs(m_fReturnHeightMoveValue) > 0.001f )
			{
				vMove -= m_fReturnHeightMoveValue * m_vRollAxis;
				m_fReturnHeightMoveValue = 0.0f;
			}
			else if( fabs(m_fControlValue2) > 0.001f && fHeightSpeed <= fabs(m_fControlValue2) )
			{
				if( m_fControlValue2 < 0.0f )
				{
					vMove += fHeightSpeed * (-m_vRollAxis);
					m_fControlValue2 += fHeightSpeed;
				}
				else if( m_fControlValue2 > 0.0f )
				{
					vMove += fHeightSpeed * m_vRollAxis;
					m_fControlValue2 -= fHeightSpeed;
				}
			}
			else if( fabs(m_fControlValue2) > 0.001f )
			{
				vMove += m_fControlValue2 * m_vRollAxis;
				m_fControlValue2 = 0.0f;
			}
		}		
		D3DXVec3Normalize( &vMove, &vMove );
		if( fLength <= m_fCurMoveDistance )
			vMove = vCurDir * fLength;
		else
			vMove *= m_fCurMoveDistance;

		GetParentSceneNode()->Translate( vMove );
	}

	return false;
}

bool ioControlReturnWeapon::CheckOwnerControl2( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return false;

	if( fTimePerSec == 0.0f )
	{
		fTimePerSec = 0.001f;
	}

	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vPreCharPos = pOwner->GetPrePosition();

	D3DXVECTOR3 vPos = GetPosition();

	D3DXVECTOR3 vOffSetVec;
	D3DXVec3Cross( &vOffSetVec, &m_vMoveDir, &m_vRollAxis );
	vOffSetVec.y = 0.0f;
	D3DXVec3Normalize( &vOffSetVec, &vOffSetVec );

	D3DXVECTOR3 vTargetPos = pOwner->GetMidPositionByRate();
	if( m_fTargetOffset != 0.0f )
		vTargetPos += m_fTargetOffset * vOffSetVec;

	if( m_fTargetOffset2 != 0.0f )
		vTargetPos += m_fTargetOffset2 * m_vMoveDir;

	if( m_ReturnState == TRS_NONE )
	{
		D3DXVECTOR3 vDiff = vCharPos - vPreCharPos;
		D3DXVECTOR3 vFrontMove = m_fCurMoveDistance * m_vCurMoveDir;
		D3DXVECTOR3 vSideMove = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

		float fLength = m_fControlSpeed * fTimePerSec;

		// 수평
		D3DXVECTOR3 vCharXZ = vCharPos;
		vCharXZ.y = 0.0f;
		D3DXVec3Normalize( &vCharXZ, &vCharXZ );
		D3DXVECTOR3 vPreCharXZ = vPreCharPos;
		vPreCharXZ.y = 0.0f;
		D3DXVec3Normalize( &vPreCharXZ, &vPreCharXZ );

		D3DXVECTOR3 vXZDir = vDiff;
		vXZDir.y = 0.0f;

		float fLength2 = D3DXVec3Length( &vXZDir );
		D3DXVec3Normalize( &vXZDir, &vXZDir );

		if( vCharXZ != vPreCharXZ && m_vMoveDir != vXZDir )
		{
			float fDotValue = D3DXVec3Dot( &m_vCrossVec, &vXZDir );
			float fNewLength = fDotValue * fLength2;

			m_fControlValue += fNewLength;
		}

		if( fabs(m_fControlValue) > 0.001f && fLength <= fabs(m_fControlValue) )
		{
			if( m_fControlValue < 0.0f )
			{
				vSideMove += (fLength*fTimePerSec) * (-m_vCrossVec);
				m_fControlValue += fLength;
			}
			else if( m_fControlValue > 0.0f )
			{
				vSideMove += (fLength*fTimePerSec) * m_vCrossVec;
				m_fControlValue -= fLength;
			}
		}
		else if( fabs(m_fControlValue) > 0.001f )
		{
			if( m_fControlValue < 0.0f )
			{
				vSideMove += (fLength*fTimePerSec) * (-m_vCrossVec);
				m_fControlValue = 0.0f;
			}
			else if( m_fControlValue > 0.0f )
			{
				vSideMove += (fLength*fTimePerSec) * m_vCrossVec;
				m_fControlValue = 0.0f;
			}
		}

		if( !m_bIgnoreCharMove )
			vFrontMove += vSideMove;

		D3DXVec3Normalize( &vFrontMove, &vFrontMove );
		m_vCurMoveDir = vFrontMove;

		D3DXVECTOR3 vMove = m_fCurMoveDistance * m_vCurMoveDir;

		// 수직
		if( vCharPos.y != vPreCharPos.y )
		{
			float fGap = vCharPos.y - vPreCharPos.y;
			m_fControlValue2 += fGap;
		}

		if( fabs(m_fControlValue2) > 0.001f && fLength <= fabs(m_fControlValue2) )
		{
			if( m_fControlValue2 < 0.0f )
			{
				vMove += fLength * (-m_vRollAxis);
				m_fControlValue2 += fLength;
			}
			else if( m_fControlValue2 > 0.0f )
			{
				vMove += fLength * m_vRollAxis;
				m_fControlValue2 -= fLength;
			}
		}
		else if( fabs(m_fControlValue2) > 0.001f )
		{
			if( m_fControlValue2 < 0.0f )
			{
				vMove += m_fControlValue2 * m_vRollAxis;
				m_fControlValue2 = 0.0f;
			}
			else if( m_fControlValue2 > 0.0f )
			{
				vMove += m_fControlValue2 * m_vRollAxis;
				m_fControlValue2 = 0.0f;
			}
		}

		D3DXVec3Normalize( &vMove, &vMove );
		vMove *= m_fCurMoveDistance;

		GetParentSceneNode()->Translate( vMove );
	}
	else
	{
		D3DXVECTOR3 vEnd = pOwner->GetMidPositionByRate() - vPos;
		float fEndLength = D3DXVec3Length( &vEnd );
		if( m_fEndRange > 0.0f && fEndLength <= m_fEndRange )
			return true;

		D3DXVECTOR3 vDiff = vTargetPos - vPos;
		float fLength = D3DXVec3Length( &vDiff );

		D3DXVECTOR3 vCurDir = m_vCurMoveDir;
		float fTimeGap = fTimePerSec * FLOAT1000;
		float fMoveSpeed = GetMoveSpeed();

		float fCurLoopTimeGap;
		float fCurLoopTimePerSec;
		while( fTimeGap > 0.0f )
		{
			if( fTimeGap > FLOAT10 )
			{
				fCurLoopTimeGap = FLOAT10;
				fCurLoopTimePerSec = FLOAT10 / FLOAT1000;

				fTimeGap -= FLOAT10;
			}
			else
			{
				fCurLoopTimeGap = fTimeGap;
				fCurLoopTimePerSec = fTimeGap / FLOAT1000;

				fTimeGap = 0.0f;
			}

			vDiff = vTargetPos - GetPosition();
			D3DXVec3Normalize( &vDiff, &vDiff );

			D3DXVECTOR3 vCurXZDir = vCurDir;
			D3DXVECTOR3 vDiffXZDir = vDiff;
			vCurXZDir.y = 0.0f;
			vDiffXZDir.y = 0.0f;
			D3DXVec3Normalize( &vCurXZDir, &vCurXZDir );
			D3DXVec3Normalize( &vDiffXZDir, &vDiffXZDir );

			float fDotValue = D3DXVec3Dot( &vCurXZDir, &vDiffXZDir );
			if( COMPARE( fDotValue, -1.0001f, -0.9999f ) )
			{
				D3DXVECTOR3 vCrossVec;
				D3DXVec3Cross( &vCrossVec, &vCurXZDir, &m_vRollAxis );
				D3DXVec3Normalize( &vCrossVec, &vCrossVec );

				D3DXVECTOR3 vCurForce = vCurXZDir * fMoveSpeed * fCurLoopTimePerSec;
				vCurDir = vCurForce + (vCrossVec * m_fReturnControlSpeed * fCurLoopTimePerSec);

				D3DXVec3Normalize( &vCurDir, &vCurDir );
			}
			else
			{
				D3DXVECTOR3 vCurForce = vCurXZDir * fMoveSpeed * fCurLoopTimePerSec;
				vCurDir = vCurForce + (vDiffXZDir * m_fReturnControlSpeed * fCurLoopTimePerSec);

				D3DXVec3Normalize( &vCurDir, &vCurDir );
			}
		}

		D3DXVec3Normalize( &m_vCurMoveDir, &vCurDir );
		D3DXVECTOR3 vMove = vCurDir * m_fCurMoveDistance;

		// 수직
		float fHeightSpeed = m_fControlSpeed * fTimePerSec;
		if( vCharPos.y != vPreCharPos.y )
		{
			float fGap = vCharPos.y - vPreCharPos.y;
			m_fControlValue2 += fGap;
		}

		if( fabs(m_fControlValue2) > 0.001f && fHeightSpeed <= fabs(m_fControlValue2) )
		{
			if( m_fControlValue2 < 0.0f )
			{
				vMove += fHeightSpeed * (-m_vRollAxis);
				m_fControlValue2 += fHeightSpeed;
			}
			else if( m_fControlValue2 > 0.0f )
			{
				vMove += fHeightSpeed * m_vRollAxis;
				m_fControlValue2 -= fHeightSpeed;
			}
		}
		else if( fabs(m_fControlValue2) > 0.001f )
		{
			if( m_fControlValue2 < 0.0f )
			{
				vMove += m_fControlValue2 * m_vRollAxis;
				m_fControlValue2 = 0.0f;
			}
			else if( m_fControlValue2 > 0.0f )
			{
				vMove += m_fControlValue2 * m_vRollAxis;
				m_fControlValue2 = 0.0f;
			}
		}

		D3DXVec3Normalize( &vMove, &vMove );
		if( fLength <= m_fCurMoveDistance )
			vMove = vCurDir * fLength;
		else
			vMove *= m_fCurMoveDistance;

		GetParentSceneNode()->Translate( vMove );
	}

	return false;
}

bool ioControlReturnWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	if( !m_bWallCollision )
	{
		const ioOrientBox &rkAttackBox = GetWorldAttackBox( true );
		if( pShape->TestIntersection( pShapeWorldMat, rkAttackBox ) )
		{
			pStage->CreateMapEffect( GetExplosionEffectName(), rkAttackBox.GetCenter(), GetScale() );

			PlayWoundedSound( NULL );

			ioBaseChar *pOwner = GetOwner();
			if( pOwner && pOwner->IsNeedProcess() )
				SetDieState( pStage, true );

			return true;
		}
	}
	else
	{
		int iFaceIndex = 0;
		D3DXVECTOR3 vNormal;
		// 1차 충돌 영역으로 체크
		if( pShape->FindIntersection( pShapeWorldMat, GetWorldAttackBox(true), &iFaceIndex ) )
		{
			pShape->GetTriNormalInModelSpace( iFaceIndex, vNormal );
			if( SetWallCollision( vNormal, pStage ) )
			{
				pStage->CreateMapEffect( GetExplosionEffectName(), GetWorldAttackBox( true ).GetCenter(), GetScale() );
				return true;
			}
		}
		else
		{
			// 2차 라인으로 체크
			D3DXVECTOR3 vEnd = GetPosition();
			D3DXVECTOR3 vOrgDiff = vEnd - m_vPrePos;
			float fOrgDiffSq = D3DXVec3LengthSq( &vOrgDiff );
			D3DXVECTOR3 vDir;
			D3DXVec3Normalize( &vDir, &vOrgDiff );
			D3DXVECTOR3 vColPoint;
			ioRay kLineRay( m_vPrePos, vDir );

			if( pShape->TestIntersection( pShapeWorldMat, kLineRay, &iFaceIndex, &vColPoint ) )
			{
				pShape->GetTriNormalInModelSpace( iFaceIndex, vNormal );
				if( SetWallCollision( vNormal, pStage ) )
				{
					pStage->CreateMapEffect( GetExplosionEffectName(), GetWorldAttackBox( true ).GetCenter(), GetScale() );
					return true;
				}
			}
		}
	}
	return false;
}

bool ioControlReturnWeapon::SetWallCollision( const D3DXVECTOR3 &vFaceNrm, ioPlayStage *pStage )
{
	float fCheck = D3DXVec3Dot( &m_vMoveDir, &vFaceNrm );
	if( fCheck >= 0.0f )
	{
		return false;	// 나가는 방향
	}
	D3DXVECTOR3 vReflect = m_vMoveDir - 2.0f * D3DXVec3Dot( &m_vMoveDir, &vFaceNrm ) * vFaceNrm;
	D3DXVec3Normalize( &m_vMoveDir, &vReflect );
	m_vCurMoveDir = m_vMoveDir;

	SetReturnWay( pStage );
	return true;
}

bool ioControlReturnWeapon::CheckMaxRange( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( fTimePerSec == 0.0f )
	{
		fTimePerSec = 0.001f;
	}

	if( m_ReturnState == TRS_NONE )
	{
		float fCurMoveDistance = GetMoveSpeed() * fTimePerSec;
		float fMaxRange = GetAttribute()->m_fMoveRange;

		float fDotValue = D3DXVec3Dot( &m_vCurMoveDir, &m_vFirstMoveDir );
		if( fDotValue < 0.001f && fDotValue > -0.001f )
			fDotValue = FLOAT1;

		float fCurRange = fCurMoveDistance * fDotValue;
		if( m_fCurMoveRange <= fMaxRange )
		{
			m_fCurMoveRange += fabs(fCurRange);
		}
		else
		{
			m_fCurMoveDistance = 0.0f;
			return true;
		}
	}
	else if( m_ReturnState == TRS_RETRUN )
	{
		if( m_dwStartReturnTime+m_dwReturnDuration < dwCurTime )
			return true;
	}

	return false;
}

const ioOrientBox& ioControlReturnWeapon::GetLocalAttackBox() const
{
	if( m_pThrowMesh )
		return m_pThrowMesh->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioControlReturnWeapon::GetType() const
{
	return WT_CONTROL_RETURN;
}

ActionStopType ioControlReturnWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioControlReturnWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioControlReturnWeapon::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed;
}

void ioControlReturnWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
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
	}
		
	if( !bCollision )
		return;

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
		ToDummyChar(pTarget) || ToWallZone(pTarget) ||
		GetPiercingType() == CPT_NONE )
	{
		CheckCurAttribute( pTarget );

		ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
		pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

		AddCollisionEntity( pTarget );

		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->IsNeedProcess() )
			SetDieState( pStage, true );

		return;
	}
}

void ioControlReturnWeapon::SetDieState( ioPlayStage *pStage, bool bSendNet )
{
	if( !pStage ) return;

	if( bSendNet )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << CSS_DIE;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	if( !GetWeaponEndEffect().IsEmpty() )
		pStage->CreateMapEffect( GetWeaponEndEffect(), GetPosition(), GetScale() );

	SetWeaponDead();
}

void ioControlReturnWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iSycType;
	rkPacket >> iSycType;

	switch( iSycType )
	{
	case CSS_RETURN:
		SetReturnWay( pStage );
		break;
	case CSS_DIE:
		SetDieState( pStage, false );
		break;
	}
}

void ioControlReturnWeapon::PlayExplosionSound()
{
	g_SoundMgr.PlaySound( GetAttribute()->m_ExplosionSound, GetParentSceneNode() );
}

void ioControlReturnWeapon::PlayWoundedSound( ioPlayEntity *pEntity )
{
	if( HasPeculiarity( WP_WOUND_NO_SOUND ) )
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
}

void ioControlReturnWeapon::PlayDefensedSound( ioPlayEntity *pEntity )
{
	if( HasPeculiarity( WP_DEF_NO_SOUND ) )
		return;

	const ioHashString &szSound = GetAttribute()->m_DefensedSound;

	if( ToBaseChar( pEntity ) )
	{
		std::string szNewSound = ToBaseChar(pEntity)->GetDefensedSoundName( szSound );
		g_SoundMgr.PlaySound( ioHashString(szNewSound.c_str()), GetParentSceneNode() );
	}
	else
	{
		g_SoundMgr.PlaySound( szSound, GetParentSceneNode() );
	}
}

void ioControlReturnWeapon::CheckChangeDir()
{
	if( m_ChangeDirInfoList.empty() ) return;

	float fChangeRange = m_ChangeDirInfoList[0].m_fChangeRange;

	if( m_fCurMoveRange >= fChangeRange )
	{
		RotateMoveDir( m_ChangeDirInfoList[0].m_fChangeAngleH, m_ChangeDirInfoList[0].m_fChangeAngleV );
		m_ChangeDirInfoList.erase( m_ChangeDirInfoList.begin() );
	}
}