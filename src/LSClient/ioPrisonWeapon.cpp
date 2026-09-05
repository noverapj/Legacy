#include "stdafx.h"

#include "ioPrisonWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioPrisonWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioPrisonWeapon::m_DieActionStopType = AST_NONE;

ioPrisonWeapon::ioPrisonWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );

	m_pFireMesh = NULL;

	ClearData();
}

ioPrisonWeapon::~ioPrisonWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "%s - UnReleased Resouce", __FUNCTION__ );
	}
}

void ioPrisonWeapon::ClearData()
{
	m_vPrisionPos	  = ioMath::VEC3_ZERO;
	m_vMoveDir		  = ioMath::VEC3_ZERO;
	m_vStartUpDownPos = ioMath::VEC3_ZERO;
	m_vCollisionDir   = D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );

	m_iUpDownIndex = 0;

	m_fCurMoveRange   = 0;
	m_fTotUpDownRange = 0;
	m_fCurUpDownRange = 0;

	m_State = WS_INIT;

	
	m_PrisonEffectID = -1;
	m_PrisonEffectID2 = -1;

	m_fMoveRange = 0;

	m_dwCurDelayTime      = 0;
	m_dwCurUpDownDuration = 0;
}

void ioPrisonWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "red_delay_effect", "", szBuf, MAX_PATH );
	m_szRedDealyEffect = szBuf;

	rkLoader.LoadString_e( "blue_delay_effect", "", szBuf, MAX_PATH );
	m_szBlueDealyEffect = szBuf;

	rkLoader.LoadString_e( "red_prison_effect", "", szBuf, MAX_PATH );
	m_szRedPrisonEffect = szBuf;

	rkLoader.LoadString_e( "blue_prison_effect", "", szBuf, MAX_PATH );
	m_szBluePrisonEffect = szBuf;

	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_szEndEffect = szBuf;

	m_fUpHeight		   = rkLoader.LoadFloat_e( "weapon_up_height", 0.0f );
	m_fUnderHeight	   = rkLoader.LoadFloat_e( "weapon_under_height", 0.0f );

	m_iUpDownInRange   = rkLoader.LoadFloat_e( "weapon_in_range", 0.0f );
	m_iUpDownOutRange  = rkLoader.LoadFloat_e( "weapon_out_range", 0.0f );

	m_dwDelayTime	   = rkLoader.LoadInt_e( "delay_time", 0 );

	m_dwUpDownDuration = rkLoader.LoadInt_e( "updown_time", 0 );
	
	LoadUpDownProperty( rkLoader );
}

void ioPrisonWeapon::LoadUpDownProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";

	int iUpDownCnt = rkLoader.LoadInt_e( "updown_cnt", 0 );
	m_vUpDownInfo.clear();

	for( int i = 0; i < iUpDownCnt; ++i )
	{
		UpDownInfo Info;

		wsprintf_e( szBuf, "updown_move%d_cnt", i + 1 );
		int iUpDownMoveCnt = rkLoader.LoadInt( szBuf, 0.0f );

		wsprintf_e( szBuf, "updown_move%d_height", i + 1 );
		Info.m_fMoveUpDownHeight = rkLoader.LoadFloat( szBuf, 0.0f );

		for( int j = 0; j < iUpDownMoveCnt; ++j )
		{
			UpDownMoveInfo MoveInfo;
			wsprintf_e( szBuf, "updown_move%d_speed%d", i + 1, j + 1 );
			MoveInfo.m_fMoveSpeed = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "updown_move%d_max_range%d", i + 1, j + 1 );
			MoveInfo.m_fMaxRnage = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "updown_move%d_min_range%d", i + 1, j + 1 );
			MoveInfo.m_fMinRnage = rkLoader.LoadFloat( szBuf, 0.0f );

			Info.m_vUpDownMoveInfo.push_back( MoveInfo );
		}
		m_vUpDownInfo.push_back( Info );
	}
}

bool ioPrisonWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetFireMesh( GetAttribute()->m_WeaponEffect );
	}
	return false;
}

bool ioPrisonWeapon::SetFireMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	
		return false;

	m_pFireMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireMesh )
		return false;

	GetParentSceneNode()->AttachObject( m_pFireMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pFireMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pFireMesh->GetCollisionBox(); 

	AddEffectEvent( m_pFireMesh );
	return true;
}

void ioPrisonWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireMesh && m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );	
}

void ioPrisonWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioPrisonWeapon::SetMoveRange( float fMoveRange )
{
	m_fMoveRange = fMoveRange;
}

void ioPrisonWeapon::SetPrisionPos( const D3DXVECTOR3 &vPos )
{
	m_vPrisionPos = vPos;
}

D3DXVECTOR3 ioPrisonWeapon::GetMoveDir()
{
	return m_vMoveDir;
}

D3DXVECTOR3 ioPrisonWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	if( pEntity )
	{
		D3DXVECTOR3 vMoveDir = m_vPrisionPos - pEntity->GetWorldPosition();
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
		return vMoveDir;
	}
	
	return ioWeapon::GetAttackDir( pEntity );
}

ioWeapon::WeaponType ioPrisonWeapon::GetType() const
{
	return WT_PRISON_WEAPON;
}

ActionStopType ioPrisonWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioPrisonWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioPrisonWeapon::CreateDealyEffect( float fTimePerSec, ioPlayStage *pStage )
{
	if( !pStage )
		return;
	
	if( !GetOwner() )
		return;

	ioMapEffect* pMapEffect = NULL;
	if( GetOwner()->GetTeam() == TEAM_BLUE )
		pMapEffect = pStage->CreateMapEffect( m_szBlueDealyEffect, m_vPrisionPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	else
		pMapEffect = pStage->CreateMapEffect( m_szRedDealyEffect, m_vPrisionPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );

	if( pMapEffect )
		m_PrisonEffectID2 = pMapEffect->GetUniqueID();
}

void ioPrisonWeapon::CreatePrisonEffect( float fTimePerSec, ioPlayStage *pStage )
{
	if( !pStage )
		return;
	
	if( !GetOwner() )
		return;

	ioMapEffect* pMapEffect = NULL;
	if( GetOwner()->GetTeam() == TEAM_BLUE )
		pMapEffect = pStage->CreateMapEffect( m_szBluePrisonEffect, GetPosition(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	else
		pMapEffect = pStage->CreateMapEffect( m_szRedPrisonEffect, GetPosition(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );

	if( pMapEffect )
		m_PrisonEffectID = pMapEffect->GetUniqueID();
}

void ioPrisonWeapon::CreateEndEffect( float fTimePerSec, ioPlayStage *pStage )
{	
	if( !pStage )
		return;
	
	pStage->CreateMapEffect( m_szEndEffect, GetPosition(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
}

void ioPrisonWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case WS_INIT:
		{
			m_State = WS_MOVE;
		}
		break;
	case WS_MOVE:
		{
			ProcessMove( fTimePerSec, pStage );
		}
		break;
	case WM_DELAY:
		{
			if( m_dwCurDelayTime == 0 )
			{
				m_dwCurDelayTime = dwCurTime;
				CreateDealyEffect( fTimePerSec, pStage );
			}
			else if( m_dwCurDelayTime + m_dwDelayTime < dwCurTime )
			{
				m_State = WS_PRISON_CREATE;
				if( m_pFireMesh )
				{
					g_EffectFactory.DestroyEffect( m_pFireMesh );
					m_pFireMesh = NULL;
				}
			}
		}
		break;
	case WS_PRISON_CREATE:
		{
			CreatePrisonEffect( fTimePerSec, pStage );

			SetPosition( m_vPrisionPos );
			CreateAttachAreaWeapon();

			m_State = WS_PRISON_UPDOWN;	
			m_dwCurUpDownDuration = dwCurTime;
		}
		break;
	case WS_PRISON_UPDOWN:
		{
			if( m_dwCurUpDownDuration + m_dwUpDownDuration < dwCurTime )
			{
				CreateEndEffect( fTimePerSec, pStage );
				m_State = WS_END;
			}
			else
			{
				ProcessUpDown( fTimePerSec, pStage );
				UpdateAttachAreaWeapon( pStage );
				CheckTestCollisionBox();
				ProcessPrisionEffect( fTimePerSec, pStage );
			}
		}
		break;
	default:
		{
			SetWeaponDead();
		}
		break;
	}

	if( m_pFireMesh )
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioPrisonWeapon::ProcessMove( float fTimePerSec, ioPlayStage *pStage )
{
	if( fTimePerSec == 0.0f )
		fTimePerSec = 0.001f;
	
	float fMoveDist = GetAttribute()->m_fMoveSpeed * fTimePerSec;
	D3DXVECTOR3 vMove = GetMoveDir() * fMoveDist;	
	m_fCurMoveRange += fMoveDist;
	
	if( m_fCurMoveRange < m_fMoveRange )
	{			
		GetParentSceneNode()->Translate( vMove );
	}
	else
	{
		m_fCurMoveRange = 0;
		m_State = WM_DELAY;
	}
}

void ioPrisonWeapon::ProcessUpDown( float fTimePerSec, ioPlayStage *pStage )
{		
	float fMaxRange  = 0.0f;
	float fMoveSpeed = 0.0f;
	
	if( m_iUpDownIndex < m_vUpDownInfo.size() )
	{
		fMaxRange = m_vUpDownInfo[m_iUpDownIndex].m_fMoveUpDownHeight;

		UpDownMoveInfoVec vMoveInfo = m_vUpDownInfo[m_iUpDownIndex].m_vUpDownMoveInfo;
		UpDownMoveInfoVec::const_iterator iter = vMoveInfo.begin();
		for( int i = 0; iter != vMoveInfo.end(); ++iter, ++i )
		{
			const UpDownMoveInfo& rkMoveInfo = *iter;
			if( rkMoveInfo.m_fMinRnage <= m_fCurUpDownRange && m_fCurUpDownRange <= rkMoveInfo.m_fMaxRnage )
			{
				fMoveSpeed = rkMoveInfo.m_fMoveSpeed;				
			}
		}
	}
	else
	{
		CreateEndEffect( fTimePerSec, pStage );
		m_State = WS_END;
		return;
	}

	if( fTimePerSec == 0.0f )
		fTimePerSec = 0.001f;
	
	D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );

	m_fCurUpDownRange += fabs(fMoveSpeed) * fTimePerSec;	
	m_fTotUpDownRange += fMoveSpeed * fTimePerSec;
	D3DXVECTOR3 vMove = vDir * (fMoveSpeed * fTimePerSec);

	if( m_fCurUpDownRange < fMaxRange )
	{
		GetParentSceneNode()->Translate( vMove );
	}
	else
	{		
		m_iUpDownIndex++;
		m_fCurUpDownRange = 0;
	}
}

void ioPrisonWeapon::ProcessPrisionEffect( float fTimePerSec, ioPlayStage *pStage )
{
	ioMapEffect* pEffect = pStage->FindMapEffect( m_PrisonEffectID );
	if( !pEffect )
		return;

	pEffect->SetWorldPosition( GetPosition() );
}

void ioPrisonWeapon::CheckTestCollisionBox()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vTop, vBottom, vCenter;
	vTop = vBottom = GetPosition();
	vTop.y += m_fUpHeight;
	vBottom.y -= m_fUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, m_iUpDownOutRange );

#ifndef SHIPPING	
	D3DXVECTOR3 vMinPos, vMaxPos;
	vMaxPos = vMinPos = GetPosition();

	vMaxPos.x += m_iUpDownOutRange;
	vMaxPos.y += m_fUpHeight;
	vMaxPos.z += m_iUpDownOutRange;

	vMinPos.x -= m_iUpDownOutRange;
	vMinPos.y -= m_fUnderHeight;
	vMinPos.z -= m_iUpDownOutRange;

	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMinPos, vMaxPos );
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif
}

bool ioPrisonWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_State != WS_PRISON_UPDOWN )	
		return false;

	if( !pEntity )
		return false;
	
	if( pEntity->GetSubType() != ioPlayEntity::PST_CHAR )
		return false;

	ioBaseChar* pChar = ToBaseChar( pEntity );
	if( !pChar )	
		return false;

	D3DXVECTOR3 vTop, vBottom, vCenter;
	vTop = vBottom = GetPosition();
	vTop.y += m_fUpHeight;
	vBottom.y -= m_fUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, m_iUpDownOutRange );
	vCenter = kCylinder.GetCenter();

	D3DXVECTOR3 vCollisionPoint;
	if( !pEntity->CheckCylinderPoint( kCylinder, vCenter, vCollisionPoint ) )
		return false;
	
	// 수평 거리 체크
	D3DXVECTOR3 vDiff = vCollisionPoint - vCenter;
	vDiff.y = 0.0f;

	float fTargetDist = D3DXVec3Length( &vDiff );

	if( !COMPARE( fTargetDist, m_iUpDownInRange, m_iUpDownOutRange ) )			
		return false;	
	
	// 높이 체크
	if( vCollisionPoint.y >= vBottom.y && vCollisionPoint.y <= vTop.y )
	{		
		return true;
	}

	return false;
}

void ioPrisonWeapon::SetWeaponDead()
{
	DestroyAttachAreaWeapon();
	m_bLive = false;	
}

void ioPrisonWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( pStage )
	{
		ioMapEffect* pPrisonEffect = pStage->FindMapEffect( m_PrisonEffectID );
		if( pPrisonEffect )
			pPrisonEffect->EndEffectForce();

		pPrisonEffect = pStage->FindMapEffect( m_PrisonEffectID2 );
		if( pPrisonEffect )
			pPrisonEffect->EndEffectForce();
	}

	ClearData();
}