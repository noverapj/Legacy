

#include "stdafx.h"

#include "ioThrowReturnWeapon.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ActionStopType ioThrowReturnWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioThrowReturnWeapon::m_DieActionStopType = AST_NONE;

ioThrowReturnWeapon::ioThrowReturnWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );
	m_pThrowMesh = NULL;
	m_fCurMoveRange = 0.0f;
	m_fCurMoveDistance = 0.0f;
	m_ReturnState = TRS_NONE;
	m_dwStartReturnTime = 0;

	m_pSecondAttr = NULL;
}

ioThrowReturnWeapon::~ioThrowReturnWeapon()
{
	if( m_pThrowMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioFloatThrowWeapon::~ioFloatThrowWeapon - UnReleased Resouce" );
	}
}

bool ioThrowReturnWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetThrowMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioThrowReturnWeapon::SetThrowMesh( const ioHashString &szFileName )
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

void ioThrowReturnWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pThrowMesh )
	{
		g_EffectFactory.DestroyEffect( m_pThrowMesh );
		m_pThrowMesh = NULL;
	}
}

void ioThrowReturnWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	m_dwReturnDelay = rkLoader.LoadInt_e( "return_delay", 0 );
}

void ioThrowReturnWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	SetPosition( vPos );
}

void ioThrowReturnWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );

	m_vRollAxis	= D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );
}

void ioThrowReturnWeapon::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;

	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioThrowReturnWeapon::SetReturnWay()
{
	if( m_ReturnState == TRS_NONE )
	{
		m_ReturnState = TRS_RETRUN;
		m_dwStartReturnTime = FRAMEGETTIME();
	}
}

void ioThrowReturnWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	D3DXVECTOR3 vMove;
	m_fCurMoveDistance = GetMoveSpeed() * fTimePerSec;

	switch( m_ReturnState )
	{
	case TRS_NONE:
		if( CheckMaxRange() )
			SetReturnWay();

		vMove = m_vMoveDir * m_fCurMoveDistance;
		break;
	case TRS_RETRUN:
		if( CheckMaxRange() )
		{
			ioBaseChar *pOwner = GetOwner();
			if( pOwner )
			{
				pOwner->SetThrowSkillEnd();
			}

			SetWeaponDead();
		}
		
		vMove = (-m_vMoveDir) * m_fCurMoveDistance;
		break;
	}

	if( m_pThrowMesh )
	{
		GetParentSceneNode()->Rotate( m_vRollAxis, 1600.0f * fTimePerSec );
	}

	GetParentSceneNode()->Translate( vMove );
	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pThrowMesh )
	{
		m_pThrowMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

bool ioThrowReturnWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	// 
	const ioOrientBox &rkAttackBox = GetWorldAttackBox( true );
	if( pShape->TestIntersection( pShapeWorldMat, rkAttackBox ) )
	{
		pStage->CreateMapEffect( GetExplosionEffectName(), rkAttackBox.GetCenter(), GetScale() );

		PlayWoundedSound( NULL );
		SetReturnWay();
		return true;
	}

	return false;
}

bool ioThrowReturnWeapon::CheckMaxRange()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_ReturnState == TRS_NONE )
	{
		float fMaxRange = GetAttribute()->m_fMoveRange + m_fGrowthMoveRange;

		if( m_fCurMoveRange <= fMaxRange )
		{
			m_fCurMoveRange += m_fCurMoveDistance;

			if( m_fCurMoveRange >= fMaxRange )
			{
				m_fCurMoveDistance -= m_fCurMoveRange - fMaxRange;
				m_fCurMoveRange = fMaxRange;
				return true;
			}
		}
		else
		{
			m_fCurMoveRange -= m_fCurMoveRange - fMaxRange;
			m_fCurMoveDistance = 0.0f;
			return true;
		}
	}
	else
	{
		if( m_dwStartReturnTime > 0 && m_dwStartReturnTime+m_dwReturnDelay < dwCurTime )
		{
			m_dwStartReturnTime = 0;
			ClearCollisionList();
		}

		if( m_fCurMoveRange > 0.0f )
		{
			m_fCurMoveRange -= m_fCurMoveDistance;
			if( m_fCurMoveRange <= 0.0f )
			{
				m_fCurMoveDistance += m_fCurMoveRange;
				m_fCurMoveRange = 0.0f;
				return true;
			}
		}
		else
		{
			m_fCurMoveRange = 0.0f;
			m_fCurMoveDistance = 0.0f;
			return true;
		}
	}

	return false;
}

const ioOrientBox& ioThrowReturnWeapon::GetLocalAttackBox() const
{
	if( m_pThrowMesh )
		return m_pThrowMesh->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioThrowReturnWeapon::GetType() const
{
	return WT_THROW_RETURN;
}

ActionStopType ioThrowReturnWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioThrowReturnWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioThrowReturnWeapon::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed;
}

void ioThrowReturnWeapon::SetSecondAttribute( int iAttributeIdx )
{
	if( iAttributeIdx == 0 ) return;

	m_pSecondAttr = g_WeaponMgr.FindAttribute( iAttributeIdx );
}

const WeaponAttribute* ioThrowReturnWeapon::GetAttribute() const
{
	if( !m_pSecondAttr )
		return m_pConstAttr;

	if( m_ReturnState == TRS_RETRUN )
		return m_pSecondAttr;

	return m_pConstAttr;
}

void ioThrowReturnWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_ReturnState != TRS_NONE ) 
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

		SetReturnWay();
		return;
	}
}