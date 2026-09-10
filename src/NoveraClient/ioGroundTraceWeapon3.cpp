
#include "stdafx.h"

#include "ioGroundTraceWeapon3.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioGroundTraceWeapon3::m_ActionStopType = AST_NONE;
ActionStopType ioGroundTraceWeapon3::m_DieActionStopType = AST_NONE;

ioGroundTraceWeapon3::ioGroundTraceWeapon3( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_ATTACK_OWNER );
	m_bResistance = false;

	m_GroundTraceState = GTS_MOVE_AIR;
	m_pWeaponMesh = NULL;

	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_bArleadyCallWeapon = false;
}

ioGroundTraceWeapon3::~ioGroundTraceWeapon3()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioWeapon::WeaponType ioGroundTraceWeapon3::GetType() const
{
	return WT_GROUND_TRACE3;
}

ActionStopType ioGroundTraceWeapon3::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioGroundTraceWeapon3::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioGroundTraceWeapon3::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetMesh( GetAttribute()->m_WeaponEffect );
	}
	return false;
}

void ioGroundTraceWeapon3::DestroyResource( ioPlayStage *pStage )
{
	if( m_pWeaponMesh )
	{
		g_EffectFactory.DestroyEffect( m_pWeaponMesh );
		m_pWeaponMesh = NULL;
	}
}

void ioGroundTraceWeapon3::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	
	m_fStartSpeed = rkLoader.LoadFloat_e( "start_speed", 0.0f );
	m_fMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );
	m_fTraceSpeed = rkLoader.LoadFloat_e( "trace_speed", 0.0f );
	m_dwTraceTime = rkLoader.LoadInt_e( "trace_time", 0 );

	m_fEnableMoveGap = rkLoader.LoadFloat_e( "enable_move_gap_height", FLOAT100 );
	m_fDefaultGapHeight = rkLoader.LoadFloat_e( "defalut_height_gap", 0 );
	
	m_fSensingRange = rkLoader.LoadFloat_e( "sensing_range", 0.0f );
	m_fSensingAngle = rkLoader.LoadFloat_e( "sensing_angle", 0.0f );

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
	
	LoadCallAttributeList( rkLoader );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioGroundTraceWeapon3::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

void ioGroundTraceWeapon3::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioGroundTraceWeapon3::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pWeaponMesh && m_pWeaponMesh->GetSkeletonName().IsEmpty() && m_pWeaponMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
}

void ioGroundTraceWeapon3::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;
	
	switch( m_GroundTraceState )
	{
	case GTS_MOVE_AIR:
		OnMoveAir( fTimePerSec, pStage );
		break;
	case GTS_MOVE_LAND:
		OnMoveLand( fTimePerSec, pStage );
		break;
	case GTS_TRACE:
		OnTraceLand( fTimePerSec, pStage );
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pWeaponMesh )
		m_pWeaponMesh->Update( fTimePerSec * FLOAT1000 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const ioOrientBox& ioGroundTraceWeapon3::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

bool ioGroundTraceWeapon3::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{	
	bool bCollision = false;
	if( pEntity->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pEntity) || ToPushStruct(pEntity) || ToBallStruct(pEntity) || ToDummyChar(pEntity) || ToWallZone(pEntity) )
			bCollision = true;
		else if( GetTeam() != pEntity->GetTeam() )
			bCollision = true;
		else if( GetOwner() != pEntity )
		{
			ioBaseChar *pChar = ToBaseChar(pEntity);
			if( pChar )
			{
				if( pChar->IsEnableTeamCollision( this ) )
					bCollision = true;
			}
		}
	}

	if( !bCollision )
		return false;

	if ( ToBaseChar(pEntity) ||	ToDummyChar(pEntity) )
	{
		CallAttribute();
		return true;
	}

	if( ToSymbolStruct(pEntity) || ToPushStruct(pEntity) || ToBallStruct(pEntity) || ToWallZone(pEntity) )
		return true;

	return false;
}

bool ioGroundTraceWeapon3::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	if( m_GroundTraceState != GTS_MOVE_AIR && m_GroundTraceState != GTS_MOVE_LAND && m_GroundTraceState != GTS_TRACE )
		return true;

	if( ( m_GroundTraceState == GTS_MOVE_LAND || m_GroundTraceState == GTS_TRACE ) && GetPiercingType() == CPT_ALL )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		if( m_GroundTraceState == GTS_MOVE_AIR )
		{
			m_vMoveDir.y = 0;
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
			SetMoveDir( m_vMoveDir );

			D3DXVECTOR3 vPos = GetPosition();
			float fHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
			vPos.y = fHeight + m_fDefaultGapHeight;
			SetPosition( vPos );

			m_GroundTraceState = GTS_MOVE_LAND;
		}
		else if( m_GroundTraceState == GTS_MOVE_LAND || m_GroundTraceState == GTS_TRACE )
			SetWeaponDead();

		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioGroundTraceWeapon3::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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
			m_WorldAttackBox.Transform( GetPosition(), vScale, ioMath::QUAT_IDENTITY );
			m_WorldAttackBoxForMap.Transform( GetPosition(), vScale, ioMath::QUAT_IDENTITY );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );
			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioGroundTraceWeapon3::SetMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	m_pWeaponMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pWeaponMesh )
		return false;

	GetParentSceneNode()->AttachObject( m_pWeaponMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pWeaponMesh->GetSkeletonName().IsEmpty() && m_pWeaponMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pWeaponMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pWeaponMesh->GetCollisionBox();

	AddEffectEvent( m_pWeaponMesh );
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioGroundTraceWeapon3::LoadCallAttributeList( ioINILoader &rkLoader )
{
	m_CallWeaponInfoList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "call_attribute_cnt", 0 );
	if( iCnt == 0 )
		return;

	m_CallWeaponInfoList.reserve( iCnt );

	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kCallAttribute;

		wsprintf_e( szKey, "call_attribute%d_index", i+1 );
		kCallAttribute.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_resistance", i+1 );
		kCallAttribute.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_time", i+1 );
		kCallAttribute.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCallAttribute.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_attribute%d_wound_duration", i+1 );
		kCallAttribute.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_loop_ani", i+1 );
		kCallAttribute.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_CallWeaponInfoList.push_back( kCallAttribute );
	}
}

void ioGroundTraceWeapon3::CallAttribute()
{
	if( m_bArleadyCallWeapon )
		return;

	if( m_CallWeaponInfoList.empty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurGenerateTime = FRAMEGETTIME();
	WeaponInfoList::iterator iter;
	for( iter = m_CallWeaponInfoList.begin(); iter != m_CallWeaponInfoList.end(); ++iter )
	{
		WeaponInfo pInfo = (*iter);
		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), pInfo, GetPosition(),	m_vMoveDir, dwCurGenerateTime + pInfo.m_dwCallTime, m_dwWeaponIdx );
	}

	m_bArleadyCallWeapon = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioGroundTraceWeapon3::CheckChangeDir()
{
	if( m_ChangeDirInfoList.empty() )
		return;
	
	float fChangeRange = m_ChangeDirInfoList[0].m_fChangeRange;

	if( m_fCurMoveRange >= fChangeRange )
	{
		if( m_ChangeDirInfoList[0].m_fChangeSpeed > 0.0f )
			m_fCurMoveSpeed = m_ChangeDirInfoList[0].m_fChangeSpeed;

		RotateMoveDir( m_ChangeDirInfoList[0].m_fChangeAngleH, m_ChangeDirInfoList[0].m_fChangeAngleV );
		m_ChangeDirInfoList.erase( m_ChangeDirInfoList.begin() );
	}
}

void ioGroundTraceWeapon3::RotateMoveDir( float fAngleH, float fAngleV )
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

float ioGroundTraceWeapon3::GetMoveSpeed()
{
	switch( m_GroundTraceState )
	{
	case GTS_MOVE_AIR:
		m_fCurMoveSpeed = m_fStartSpeed;
		break;
	case GTS_MOVE_LAND:
		m_fCurMoveSpeed = m_fMoveSpeed;
		break;
	case GTS_TRACE:
		m_fCurMoveSpeed = m_fTraceSpeed;
		break;
	}

	return m_fCurMoveSpeed;
}

float ioGroundTraceWeapon3::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioGroundTraceWeapon3::SetTraceState( ioPlayStage *pStage, bool bSendNetwork )
{
	if ( !pStage )
		return;

	m_GroundTraceState = GTS_TRACE;
	m_dwCheckTraceTime = FRAMEGETTIME() + m_dwTraceTime;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE_WEAPON );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			kPacket << m_szTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGroundTraceWeapon3::SetTraceTarget( ioHashString &szName )
{
	m_szTargetName = szName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioGroundTraceWeapon3::OnMoveAir( float fTimePerSec, ioPlayStage *pStage )
{	
	if( m_fCurMoveRange >= GetMaxRange() + m_fGrowthMoveRange )
	{
		SetWeaponDead();
		return;
	}

	float fCurSpeed = GetMoveSpeed();

	CheckChangeDir();

	float fMoveDist = fCurSpeed * fTimePerSec;
	
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	m_fCurMoveRange += fMoveDist;
}

void ioGroundTraceWeapon3::OnMoveLand( float fTimePerSec, ioPlayStage *pStage )
{	
	if( m_fCurMoveRange >= GetMaxRange() + m_fGrowthMoveRange )
	{
		SetWeaponDead();
		return;
	}

	float fCurSpeed = GetMoveSpeed();
	float fMoveDist = fCurSpeed * fTimePerSec;

	D3DXVECTOR3 vPrePos = GetPosition();
	D3DXVECTOR3 vNewPos = vPrePos + ( m_vMoveDir * fMoveDist );

	float fHeight = pStage->GetMapHeightOnlyTerrain( vNewPos.x, vNewPos.z );
	if( fabs( vPrePos.y - fHeight ) < m_fEnableMoveGap )
		vNewPos.y = fHeight + m_fDefaultGapHeight;

	SetPosition( vNewPos );
	
	m_fCurMoveRange += fMoveDist;
	
	// Å¸°Ù °Ë»ö
	D3DXVECTOR3 vCurPos = GetPosition();

	float fSensingRangeSq = m_fSensingRange * m_fSensingRange;
	float fHalfValue = cosf( DEGtoRAD( m_fSensingAngle ) * FLOAT05 );

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar ) 
			continue;
		
		if( pChar->GetTeam() == GetTeam() ) 
			continue;

		if( pChar->GetCharName() == GetOwnerName() ) 
			continue;

		if ( !pChar->IsEnableTarget() )
			continue;

		D3DXVECTOR3 vTargetPos = pChar->GetMidPositionByRate();
		D3DXVECTOR3 vDiff = vCurPos - vTargetPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		if( m_fSensingAngle != 0.0f && GetOwner() )
		{
			D3DXVECTOR3 vDir = m_vMoveDir;
			D3DXVECTOR3 vDiff2 = vTargetPos - GetPosition();
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		if( fDiffRangeSq < fSensingRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			m_szTargetName = szTarget;
			SetTraceState( pStage, true );
		}
	}
}

void ioGroundTraceWeapon3::OnTraceLand( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCheckTraceTime > 0 && m_dwCheckTraceTime < dwCurTime )
	{
		SetWeaponDead();
		return;
	}

	float fCurSpeed = GetMoveSpeed();
	float fMoveDist = fCurSpeed * fTimePerSec;
	D3DXVECTOR3 vPrePos = GetPosition();

	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if ( pTarget && pTarget->IsEnableTarget() )
	{
		D3DXVECTOR3 vMoveDir = pTarget->GetMidPositionByRate() - vPrePos;
		vMoveDir.y = 0;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
		SetMoveDir( vMoveDir );
	}
	else
		m_szTargetName.Clear();
	
	D3DXVECTOR3 vNewPos = vPrePos + ( m_vMoveDir * fMoveDist );

	float fHeight = pStage->GetMapHeightOnlyTerrain( vNewPos.x, vNewPos.z );
	if( fabs( vPrePos.y - fHeight ) < m_fEnableMoveGap )
		vNewPos.y = fHeight + m_fDefaultGapHeight;

	SetPosition( vNewPos );
}