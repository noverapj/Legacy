

#include "stdafx.h"

#include "ioReturnMissileWeapon.h"

#include "ioBaseChar.h"
#include "ioUroborusGuideArrow.h"

ActionStopType ioReturnMissileWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioReturnMissileWeapon::m_DieActionStopType = AST_NONE;

ioReturnMissileWeapon::ioReturnMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_State = WS_NORMAL;
	m_pFireBall = NULL;

	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;
	m_fCurMoveRate = 0.0f;
	
	m_bFirst = true;
	m_bSetGraceClose = false;

	m_pReturnAttr = NULL;

	m_bArleadyCallWeapon = false;

	m_pGuideArrow = NULL;
}

ioReturnMissileWeapon::~ioReturnMissileWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioReturnMissileWeapon::~ioReturnMissileWeapon - UnReleased Resouce" );
	}

	SAFEDELETE( m_pGuideArrow );
}

bool ioReturnMissileWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioReturnMissileWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szBuf2[MAX_PATH] = "";
	
	m_bDisableOwnerScale = rkLoader.LoadBool_e( "disable_owner_scale", false );

	m_vStartOffSet.x = rkLoader.LoadFloat_e( "start_offset_x", 0.0f );
	m_vStartOffSet.y = rkLoader.LoadFloat_e( "start_offset_y", 0.0f );
	m_vStartOffSet.z = rkLoader.LoadFloat_e( "start_offset_z", 0.0f );

	LoadCallAttribute( rkLoader );
	LoadCallAttributeFireDirectionList( rkLoader );

	int iCnt = rkLoader.LoadInt_e( "change_dir_cnt", 0 );

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

	// return 
	m_iReturnType = rkLoader.LoadInt_e( "return_type", RT_NONE );
	m_fReturnMoveSpeed = rkLoader.LoadFloat_e( "return_move_speed", 0.0f );

	m_iReturnAttribute = rkLoader.LoadInt_e( "return_attack", 0 );

	rkLoader.LoadString_e( "return_weapon_effect", "", szBuf, MAX_PATH );
	m_ReturnEffect = szBuf;

	// Air
	int iAirCount = rkLoader.LoadInt_e( "return_target_air_buff_cnt", 0 );
	m_ReturnTargetAirBuffList.reserve( iAirCount );

	for( int i=0; i<iAirCount; i++ )
	{
		wsprintf_e( szBuf2, "return_target_air_buff%d", i+1 );
		rkLoader.LoadString( szBuf2, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;	
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_ReturnTargetAirBuffList.push_back( tBuffInfo );
			}
		}
	}

	// Normal
	int iCount = rkLoader.LoadInt_e( "return_target_buff_cnt", 0 );
	m_ReturnTargetBuffList.reserve( iCount );

	for( int i=0; i<iCount; i++ )
	{
		wsprintf_e( szBuf2, "return_target_buff%d", i+1 );
		rkLoader.LoadString( szBuf2, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;	
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_ReturnTargetBuffList.push_back( tBuffInfo );
			}
		}
	}

	// Down
	int iDownCount = rkLoader.LoadInt_e( "return_target_down_buff_cnt", 0 );
	m_ReturnTargetDownBuffList.reserve( iDownCount );

	for( int i=0; i<iDownCount; i++ )
	{
		wsprintf_e( szBuf2, "return_target_down_buff%d", i+1 );
		rkLoader.LoadString( szBuf2, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;	
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_ReturnTargetDownBuffList.push_back( tBuffInfo );
			}
		}
	}

	//
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		ioPlayStage *pStage = pOwner->GetCreator();

		SAFEDELETE( m_pGuideArrow );

		m_pGuideArrow = new ioUroborusGuideArrow( pStage );
		m_pGuideArrow->Initialize( rkLoader );
		m_pGuideArrow->SetMaxArrowRange( GetMaxRange() );
	}

	//////////////////////////////////////////////////////////////////////////
	//칼리태 추가
	m_bUseLengthMove = rkLoader.LoadBool_e( "use_length_move", false );
	m_fUseMaxLenthValue = rkLoader.LoadFloat_e( "use_length_move_value", 0.f );
	m_bNotUseBipeLink = rkLoader.LoadBool_e( "not_use_bipe_link", false );
	m_bUseEffectStartPosMode = rkLoader.LoadBool_e( "use_effect_start_mode", false );
}

void ioReturnMissileWeapon::LoadCallAttribute( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_CallWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "call_attribute_index", 0 );		
	m_CallWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "call_attribute_resistance", 0 );		

	rkLoader.LoadString_e( "call_attribute_wound_ani", "", szBuf, MAX_PATH );
	m_CallWeaponInfo.m_WoundedAnimation = szBuf;		

	m_CallWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "call_attribute_wound_duration", 0 );
	m_CallWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "call_attribute_wound_loop_ani", false );	
}

void ioReturnMissileWeapon::LoadCallAttributeFireDirectionList( ioINILoader &rkLoader )
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

bool ioReturnMissileWeapon::CreateFireBall( const ioHashString &szFileName )
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

	if( m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

bool ioReturnMissileWeapon::ChangeWeaponEffect( const ioHashString& szEffectName )
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

void ioReturnMissileWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	/*D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );*/

	D3DXQUATERNION qtRot;
	ioPlayEntityHelp::CheckOrientation( vDir, qtRot );
	SetOrientation( qtRot );
}

void ioReturnMissileWeapon::SetMoveSpeed( float fSpeed )
{
	m_fCurMoveSpeed = fSpeed;
}

void ioReturnMissileWeapon::CheckChangeDir()
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

void ioReturnMissileWeapon::RotateMoveDir( float fAngleH, float fAngleV )
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

	D3DXQUATERNION qtRotDir;
	ioPlayEntityHelp::CheckOrientation( m_vMoveDir, qtRotDir );
	SetOrientation( qtRotDir );
}

void ioReturnMissileWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() && !m_bNotUseBipeLink )
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

	if( m_bUseEffectStartPosMode )
		SetGuidArrowPosMode();
}

void ioReturnMissileWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;
	
	if( !m_bFirst )
	{
		switch( m_State )
		{
		case WS_NORMAL:
			ProcessNormal( fTimePerSec, pStage );

			if( m_bUseLengthMove && m_fCurMoveRange >= m_fUseMaxLenthValue )
				ChangeToReturnState();

			break;
		case WS_RETURN:
			ProcessReturn( fTimePerSec, pStage );
			break;
		}
	}

	m_bFirst = false;
	
	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioReturnMissileWeapon::ProcessNormal( float fTimePerSec, ioPlayStage *pStage )
{
	CheckChangeDir();

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	m_fCurMoveRange += fMoveDist;

	UpdateWorldAttackBox( fTimePerSec, false );

	ProcessGuidArrow();
}

void ioReturnMissileWeapon::ProcessReturn( float fTimePerSec, ioPlayStage *pStage )
{
	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	float fNewMoveRange = m_fCurMoveRange + fMoveDist;
	float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;

	if( fNewMoveRange >= fMaxRange )
	{
		fMoveDist = fNewMoveRange - fMaxRange;
		m_fCurMoveRange = fMaxRange;
	}
	else
	{
		m_fCurMoveRange = fNewMoveRange;
	}

	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	if( fMaxRange > 0.0f )
		m_fCurMoveRate = m_fCurMoveRange / fMaxRange;

	if( m_fCurMoveRange == fMaxRange )
	{
		UpdateWorldAttackBox( fTimePerSec, true );

		SAFEDELETE( m_pGuideArrow );
		m_pFireBall->EndEffectForce();

		SetWeaponDead();
	}
	else
	{
		UpdateWorldAttackBox( fTimePerSec, false );
	}

	ProcessGuidArrow();
}

void ioReturnMissileWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

D3DXVECTOR3 ioReturnMissileWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

const ioOrientBox& ioReturnMissileWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioReturnMissileWeapon::GetType() const
{
	return WT_RETURN_MISSILE_WEAPON;
}

ActionStopType ioReturnMissileWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioReturnMissileWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioReturnMissileWeapon::GetMoveSpeed()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
		return 0.0f;

	if( m_State == WS_RETURN )
	{
		m_fCurMoveSpeed = m_fReturnMoveSpeed;
	}
	else if( m_fCurMoveSpeed == 0.0f )
	{
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;
	}

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioReturnMissileWeapon::GetMaxRange()
{
	if( m_State == WS_RETURN )
		return m_fCurMaxMoveRange;

	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioReturnMissileWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioReturnMissileWeapon::CheckCurAttribute( const ioPlayEntity *pTarget )
{
	ioWeapon::CheckCurAttribute( pTarget );

	if( m_bResistance ) return;

	if( m_State == WS_RETURN )
	{
		m_pReturnAttr = g_WeaponMgr.FindAttribute( m_iReturnAttribute );
		return;
	}

	m_pReturnAttr = NULL;
	return;
}

const WeaponAttribute* ioReturnMissileWeapon::GetAttribute() const
{
	if( m_bResistance )
	{
		if( m_pResistanceAttr )
			return m_pResistanceAttr;
	}

	if( m_pReturnAttr )
		return m_pReturnAttr;

	return m_pConstAttr;
}

bool ioReturnMissileWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = CheckCollisionLine( pEntity, vColPoint );

	CheckPiercing( bCollision );

	if( bCollision )
		CallAttribute( pEntity );

	return bCollision;	
}

bool ioReturnMissileWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	if( GetPiercingType() == CPT_ALL || GetPiercingType() == CPT_ALL_LIMITE )
		return true;

	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( m_StartPos, vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

void ioReturnMissileWeapon::SetWeaponDead()
{
	CreateCollisionAreaWeapon();

	ioWeapon::SetWeaponDead();
}

bool ioReturnMissileWeapon::isCallAttribute()
{
	if(!m_CallAttFireList.empty())
		return true;

	return false;
}

void ioReturnMissileWeapon::CallAttribute( ioPlayEntity *pEntity )
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

const BuffInfoList& ioReturnMissileWeapon::GetTargetAirBuffList() const
{
	if( m_State != WS_RETURN )
		return GetAttribute()->m_TargetAirBuffList;

	return m_ReturnTargetAirBuffList;
}

const BuffInfoList& ioReturnMissileWeapon::GetTargetBuffList() const
{
	if( m_State != WS_RETURN )
		return GetAttribute()->m_TargetBuffList;

	return m_ReturnTargetBuffList;
}

const BuffInfoList& ioReturnMissileWeapon::GetTargetDownBuffList() const
{
	if( m_State != WS_RETURN )
		return GetAttribute()->m_TargetDownBuffList;

	return m_ReturnTargetDownBuffList;
}

void ioReturnMissileWeapon::ChangeToReturnState()
{
	m_State = WS_RETURN;

	m_fCurMaxMoveRange = m_fCurMoveRange;
	m_fCurMoveRange = 0.0f;

	ChangeWeaponEffect( m_ReturnEffect );

	m_vMoveDir = -m_vMoveDir;
	m_StartPos = GetPosition();

	D3DXQUATERNION qtRot;
	ioPlayEntityHelp::CheckOrientation( m_vMoveDir, qtRot );
	SetOrientation( qtRot );
	ClearCollisionList();
}

void ioReturnMissileWeapon::SetSkillDependencyAction()
{
	ChangeToReturnState();
}

void ioReturnMissileWeapon::ProcessGuidArrow()
{
	if( !m_pGuideArrow )
		return;

	m_pGuideArrow->Process( GetOwner(), GetPosition() );
}

void ioReturnMissileWeapon::SetGuidArrowPosMode()
{
	if( m_pGuideArrow )
		m_pGuideArrow->SetStartPosMode( true, GetPosition() );
}