

#include "stdafx.h"

#include "ioMissileWeapon.h"
#include "ioTargetWarpSkill.h"

ActionStopType ioMissileWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioMissileWeapon::m_DieActionStopType = AST_NONE;

ioMissileWeapon::ioMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall = NULL;

	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;
	m_fCurMoveRate = 0.0f;
	
	m_bFirst = true;
	m_bSetGraceClose = false;

	m_pSecondAttr = NULL;

	m_bArleadyCallWeapon = false;

	m_bCreateDummy = false;
	m_pTargetChar = NULL;
}

ioMissileWeapon::~ioMissileWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioMissileWeapon::~ioMissileWeapon - UnReleased Resouce" );
	}

	m_vSecondAttrList.clear();
}

bool ioMissileWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		ioHashString szWeaponEffect = GetAttribute()->m_WeaponEffect;
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			TeamType eCurTeamType = pOwner->GetTeam();
			TeamType eCurSingleTeamType = pOwner->GetSingleConvertTeam();
			TeamType eTeamType = TEAM_NONE;
			switch( eCurTeamType )
			{
			case TEAM_BLUE:
				eTeamType = TEAM_BLUE;
				break;
			case TEAM_RED:
				eTeamType = TEAM_RED;
				break;
			default:
				if( eCurSingleTeamType == TEAM_BLUE )
					eTeamType = TEAM_BLUE;
				else
					eTeamType = TEAM_RED;
				break;
			}

			if( !m_szWeaponEffect_Blue.IsEmpty() && eTeamType == TEAM_BLUE )
				szWeaponEffect = m_szWeaponEffect_Blue;
			else if( !m_szWeaponEffect_Red.IsEmpty() && eTeamType == TEAM_RED )
				szWeaponEffect = m_szWeaponEffect_Red;
		}
		return CreateFireBall( szWeaponEffect );
	}

	return false;
}

void ioMissileWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_bPassLinkSkeleton = rkLoader.LoadBool_e( "pass_link_skeleton", false );
	
	m_bDisableOwnerScale = rkLoader.LoadBool_e( "disable_owner_scale", false );

	m_vStartOffSet.x = rkLoader.LoadFloat_e( "start_offset_x", 0.0f );
	m_vStartOffSet.y = rkLoader.LoadFloat_e( "start_offset_y", 0.0f );
	m_vStartOffSet.z = rkLoader.LoadFloat_e( "start_offset_z", 0.0f );

	LoadCallAttribute( rkLoader );
	LoadCallAttributeFireDirectionList( rkLoader );

	m_bSetMoveDirFunc = rkLoader.LoadBool_e( "use_new_dir_fuc", false );
	m_bSetForceStartPos = rkLoader.LoadBool_e( "force_start_pos", false );

	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_szDummyName = szBuf;

	m_fDummyRange = rkLoader.LoadFloat_e( "dummy_range", 0.0f );

	rkLoader.LoadString_e( "weapon_effect_blue", "", szBuf, MAX_PATH );
	m_szWeaponEffect_Blue = szBuf;
	rkLoader.LoadString_e( "weapon_effect_red", "", szBuf, MAX_PATH );
	m_szWeaponEffect_Red = szBuf;

	int iCnt = rkLoader.LoadInt_e( "change_dir_cnt", 0 );
	if( iCnt <= 0 ) return;

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

void ioMissileWeapon::LoadCallAttribute( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_CallWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "call_attribute_index", 0 );		
	m_CallWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "call_attribute_resistance", 0 );		

	rkLoader.LoadString_e( "call_attribute_wound_ani", "", szBuf, MAX_PATH );
	m_CallWeaponInfo.m_WoundedAnimation = szBuf;		

	m_CallWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "call_attribute_wound_duration", 0 );
	m_CallWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "call_attribute_wound_loop_ani", false );	

}

void ioMissileWeapon::LoadCallAttributeFireDirectionList( ioINILoader &rkLoader )
{	

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	int iCnt = rkLoader.LoadInt_e( "call_attribute_fire_dir_cnt", 0 );

	if( iCnt <= 0 ) return;

	m_CallAttFireList.clear();
	m_CallAttFireList.reserve( iCnt );
	
	for( int i= 0; i < iCnt; ++i )
	{
		ChangeDirInfo kInfo;

		wsprintf_e( szBuf, "call_attribute_fire%d_range", i+1 );
		kInfo.m_fChangeRange = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "call_attribute_fire%d_speed", i+1 );
		kInfo.m_fChangeSpeed = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "call_attribute_fire%d_angle_h", i+1 );
		kInfo.m_fChangeAngleH = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "call_attribute_fire%d_angle_v", i+1 );
		kInfo.m_fChangeAngleV = rkLoader.LoadFloat( szBuf, 0.0f );

		m_CallAttFireList.push_back( kInfo );
	}

	std::sort( m_CallAttFireList.begin(), m_CallAttFireList.end(), ChangeDirSort() );
}


bool ioMissileWeapon::CreateFireBall( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );

	if( m_bDisableOwnerScale )
		GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( !m_bPassLinkSkeleton && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_StartPos = GetPosition();

	AddEffectEvent( m_pFireBall );
	return true;
}

bool ioMissileWeapon::ChangeWeaponEffect( const ioHashString& szEffectName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	if( szEffectName.IsEmpty() )
		return false;

	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}

	m_pFireBall = g_EffectFactory.CreateEffect( szEffectName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );

	AddEffectEvent( m_pFireBall );
	return true;
}

void ioMissileWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	if( m_bSetMoveDirFunc )
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vDir );
		SetOrientation( qtRot );
	}
	else
	{
		D3DXQUATERNION qtRot;
		ioPlayEntityHelp::CheckOrientation( vDir, qtRot );
		SetOrientation( qtRot );
	}
}

void ioMissileWeapon::SetMoveSpeed( float fSpeed )
{
	m_fCurMoveSpeed = fSpeed;
}

void ioMissileWeapon::CheckChangeDir()
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

void ioMissileWeapon::RotateMoveDir( float fAngleH, float fAngleV )
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

	if( m_bSetMoveDirFunc )
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
		SetOrientation( qtRot );
	}
	else
	{
		D3DXQUATERNION qtRot;
		ioPlayEntityHelp::CheckOrientation( m_vMoveDir, qtRot );
		SetOrientation( qtRot );
	}
}

void ioMissileWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if ( m_bSetForceStartPos )
	{
		SetPosition( vPos );
		m_StartPos = GetPosition();
		return;
	}
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() && !m_bReserveCallWeapon )
		return;

	ioBaseChar *pOwner = GetOwner();
	D3DXVECTOR3 vNewPos = vPos;
	if( pOwner && m_vStartOffSet != ioMath::VEC3_ZERO )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vOffSet = qtRot * m_vStartOffSet;

		vNewPos += vOffSet;
	}

	SetPosition( vNewPos );
	m_StartPos = GetPosition();
}

void ioMissileWeapon::SetGraceClose()
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

void ioMissileWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;
	
	if( !m_bFirst )
	{
		CheckChangeDir();

		float fMoveDist = GetMoveSpeed() * fTimePerSec;
		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
		m_fCurMoveRange += fMoveDist;

		float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;
		if( fMaxRange > 0.0f )
			m_fCurMoveRate = m_fCurMoveRange / fMaxRange;

		if( m_fCurMoveRange < fMaxRange )
		{
			UpdateWorldAttackBox( fTimePerSec, false );
		}
		else
		{
			UpdateWorldAttackBox( fTimePerSec, true );

			SetGraceClose();

			if( !m_pFireBall || !m_pFireBall->IsLive() )
			{
				SetWeaponDead();
			}
		}
	}

	m_bFirst = false;
	
	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioMissileWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

D3DXVECTOR3 ioMissileWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

const ioOrientBox& ioMissileWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioMissileWeapon::GetType() const
{
	return WT_MISSILE;
}

ActionStopType ioMissileWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioMissileWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioMissileWeapon::GetMoveSpeed()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
		return 0.0f;

	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioMissileWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioMissileWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();

	float fScaleRate = FLOAT1 + (GetAttribute()->m_fScaleRate * m_fCurMoveRate );
	float fExtentsX = m_WorldAttackBox.GetExtents(0) * fScaleRate;
	float fExtentsY = m_WorldAttackBox.GetExtents(1) * fScaleRate;

	m_WorldAttackBox.SetExtents( 0, fExtentsX );
	m_WorldAttackBox.SetExtents( 1, fExtentsY );

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap = m_WorldAttackBox;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		if( m_bDisableOwnerScale )
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetParentSceneNode()->GetDerivedOrientation() );

			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetParentSceneNode()->GetDerivedOrientation() );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(),
										GetScale(),
										GetParentSceneNode()->GetDerivedOrientation() );

			m_WorldAttackBoxForMap.Transform( GetPosition(),
											  GetScale(),
											  GetParentSceneNode()->GetDerivedOrientation() );
		}
	}
}

void ioMissileWeapon::SetSecondAttribute( SecondAttributeList vSecondList )
{
	int iSize = vSecondList.size();
	for( int i=0; i < iSize; ++i )
	{
		SecondAttribute kSecondAttr = vSecondList[i];
		m_vSecondAttrList.push_back( kSecondAttr );
	}
}

void ioMissileWeapon::CheckCurAttribute( const ioPlayEntity *pTarget )
{
	ioWeapon::CheckCurAttribute( pTarget );

	if( m_bResistance ) return;

	int iSize = m_vSecondAttrList.size();
	if( iSize > 0 )
	{
		for( int i=0; i < iSize-1; ++i )
		{
			if( m_fCurMoveRange < m_vSecondAttrList[i].m_fLimiteValue )
			{
				m_pSecondAttr = g_WeaponMgr.FindAttribute( m_vSecondAttrList[i].m_iAttributeIndex );
				return;
			}
		}

		m_pSecondAttr = g_WeaponMgr.FindAttribute( m_vSecondAttrList[iSize-1].m_iAttributeIndex );
		return;
	}

	m_pSecondAttr = NULL;
	return;
}

const WeaponAttribute* ioMissileWeapon::GetAttribute() const
{
	if( m_bResistance )
	{
		if( m_pResistanceAttr )
			return m_pResistanceAttr;
	}

	if( m_pSecondAttr )
		return m_pSecondAttr;

	return m_pConstAttr;
}

bool ioMissileWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = CheckCollisionLine( pEntity, vColPoint );

	CheckPiercing( bCollision );

	if( bCollision )
	{
		CallAttribute( pEntity );
		m_bCreateDummy = true;
	}

	return bCollision;	
}

bool ioMissileWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	if( GetPiercingType() == CPT_ALL || GetPiercingType() == CPT_ALL_LIMITE )
		return true;

	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	m_pTargetChar = pTargetChar;
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( m_StartPos, vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

void ioMissileWeapon::SetWeaponDead()
{
	CreateCollisionAreaWeapon();
	CreateDummy();

	ioWeapon::SetWeaponDead();

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioTargetWarpSkill *pTargetWarpSkill = ToTargetWarpSkill( pOwner->GetCurActiveSkill() );
	if( m_pTargetChar && pTargetWarpSkill )
	{
		if( pTargetWarpSkill->GetName() == GetWeaponBySkillName() )
			pTargetWarpSkill->SetWarpBackMotionState( pOwner, m_pTargetChar );
	}
	m_pTargetChar = NULL;
}

bool ioMissileWeapon::isCallAttribute()
{
	if(!m_CallAttFireList.empty())
		return true;

	return false;
}

void ioMissileWeapon::CallAttribute( ioPlayEntity *pEntity )
{
	if( m_bArleadyCallWeapon )
		return;	
	
	if( m_CallAttFireList.empty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;	

	ioBaseChar* pTarget = ToBaseChar(pEntity);
	if( !pTarget )	
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwCurGenerateTime = dwCurTime;
		
	ChangeDirInfoList::iterator iter;
	
	exReserveWeaponCollisionInfo pInfo;
	pInfo.m_szTarget = pTarget->GetCharName();

	for( iter = m_CallAttFireList.begin(); iter != m_CallAttFireList.end(); ++iter )
	{
		ChangeDirInfo Info = (*iter);

		D3DXVECTOR3 vDir = m_vMoveDir;
		D3DXVec3Normalize(&vDir, &vDir);
		float fAngleH = ioMath::ArrangeHead( Info.m_fChangeAngleH );
		float fAngleV = ioMath::ArrangeHead( Info.m_fChangeAngleV );

		D3DXQUATERNION qtRot;
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

		// vertical
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
		vDir = qtRot * vDir;

		// horizon
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
		vDir = qtRot * vDir;

		D3DXVECTOR3	vPos = GetPosition();

		g_WeaponMgr.SetReserveCallWeaponCollisionInfo( GetOwnerName(), 
			m_CallWeaponInfo,
			vPos,
			-vDir,
			dwCurGenerateTime,
			m_dwWeaponIdx,
			pInfo
		);
		
		dwCurGenerateTime += 10;		
	}

	m_bArleadyCallWeapon = true;	
}

void ioMissileWeapon::SetCreateDummy( bool bCreate )
{
	m_bCreateDummy = bCreate;
}

void ioMissileWeapon::CreateDummy()
{
	if ( m_szDummyName.IsEmpty() || !m_bCreateDummy )
		return;

	ioBaseChar* pOwner = GetOwner();
	if ( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_szDummyName );

	D3DXVECTOR3 vPos = GetPosition();
	if ( vPos.y <= 0.0f )
		return;
	
	float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );
	if ( fMapHeight > vPos.y )
		vPos.y = fMapHeight;
	
	
	D3DXVECTOR3 vDist = vPos - pOwner->GetWorldPosition();
	if ( D3DXVec3LengthSq( &vDist ) > m_fDummyRange * m_fDummyRange )
		return;

	int nCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyName, nCurIndex, pOwner->GetCharName(),	vPos, 0.0f, 0, true );
	if ( pDummy )
	{
		D3DXVECTOR3 vMoveDir = GetOrientation() * -ioMath::UNIT_Z;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
		
		D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
		D3DXVec3Normalize( &vXZDir, &vXZDir );
		
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vXZDir );
		pDummy->SetWorldOrientationAndDir( qtRot );
		pDummy->SetWorldPosition( vPos );
	}
}