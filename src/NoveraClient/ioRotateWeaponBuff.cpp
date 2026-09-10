#include "StdAfx.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"

#include "ioRotateWeaponBuff.h"

ioRotateWeaponBuff::ioRotateWeaponBuff(void)
{
}

ioRotateWeaponBuff::ioRotateWeaponBuff( const ioRotateWeaponBuff &rhs )
: ioBuff( rhs ),
m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
m_bEscapeCatchState( rhs.m_bEscapeCatchState ),
m_AttributeList( rhs.m_AttributeList )
{

}

ioRotateWeaponBuff::~ioRotateWeaponBuff(void)
{
}

void ioRotateWeaponBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );
	m_bEscapeCatchState = rkLoader.LoadBool_e( "escape_catch_state", false );
	
	LoadCallAttributeList( rkLoader );
}

void ioRotateWeaponBuff::LoadCallAttributeList( ioINILoader &rkLoader )
{
	m_AttributeList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "attribute_cnt", 0 );
	if( iCnt == 0 )
		return;

	m_AttributeList.reserve( iCnt );

	for( int i=0; i<iCnt; ++i )
	{
		CallAttribute kCallAttribute;

		wsprintf_e( szKey, "attribute%d_index", i+1 );
		kCallAttribute.m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attribute%d_resistance", i+1 );
		kCallAttribute.m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCallAttribute.m_WeaponInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attribute%d_wound_duration", i+1 );
		kCallAttribute.m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attribute%d_wound_loop_ani", i+1 );
		kCallAttribute.m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "attribute%d_time", i+1 );
		kCallAttribute.m_dwAttributeCallTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attribute%d_randomtime_min", i+1 );
		kCallAttribute.m_iRandomMinTime = rkLoader.LoadInt( szKey, -1);

		wsprintf_e( szKey, "attribute%d_randomtime_max", i+1 );
		kCallAttribute.m_iRandomMaxTime = rkLoader.LoadInt( szKey, -1);

		kCallAttribute.m_iRandomMaxTime = max( kCallAttribute.m_iRandomMinTime, kCallAttribute.m_iRandomMaxTime);

		wsprintf_e( szKey, "attribute%d_angle", i+1 );
		kCallAttribute.m_fAttributeCallAngle = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "attribute%d_range", i+1 );
		kCallAttribute.m_fAttributeCallRange = rkLoader.LoadFloat( szKey, 0.0f );

		m_AttributeList.push_back( kCallAttribute );
	}

	std::sort( m_AttributeList.begin(), m_AttributeList.end(), CallAttributeSort() );
}

ioBuff* ioRotateWeaponBuff::Clone()
{
	return new ioRotateWeaponBuff( *this );
}

void ioRotateWeaponBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	if( m_bEscapeCatchState && pOwner && pOwner->IsCatchMode() )
	{
		pOwner->SetEscapeCatchState();
	}
	
	CreateWeapon();
	
}

bool ioRotateWeaponBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;
	
	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	if( m_bEscapeCatchState && pOwner && pOwner->IsCatchMode() )
	{
		pOwner->SetEscapeCatchState();
	}
	
	return true;
}

void ioRotateWeaponBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
		ProcessDelay( fTimePerSec );
		break;
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

}

void ioRotateWeaponBuff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	ioBuff::EndBuff();
}

void ioRotateWeaponBuff::CreateWeapon()
{
	if( !m_pOwner ) return;

	ioPlayStage* pStage = m_pOwner->GetCreator();

	auto Iter = m_AttributeList.begin();
	for ( ; Iter != m_AttributeList.end(); (Iter)++ )
	{
		CallAttribute &kCallAttribute = *Iter;

		float fAngle = kCallAttribute.m_fAttributeCallAngle;
		float fRange = kCallAttribute.m_fAttributeCallRange;

		D3DXVECTOR3 vPos;
		if( fAngle > 0.0f && fRange > 0.0f )
		{
			vPos = m_pOwner->GetMidPositionByRate();
			D3DXVECTOR3 vDir = m_pOwner->GetMoveDir();
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXQUATERNION	qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );

			D3DXVECTOR3 vAxis = ioMath::UNIT_Y;
			D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fAngle) );
			vDir = qtNewRot * vDir;
			D3DXVec3Normalize( &vDir, &vDir );
			vPos += vDir * fRange;
		}
		else
		{
			vPos = m_pOwner->GetMidPositionByRate();
		}
		ioBaseChar *pCreater = m_pOwner->GetBaseChar( m_CreateChar );
		if ( !pCreater )
			return;

		DWORD dwWeaponIndex = pCreater->IncreaseWeaponIndexBase();
		
		ioWeapon *pWeapon = pCreater->DummyExplicit( kCallAttribute.m_WeaponInfo,
			vPos,
			dwWeaponIndex,
			false,
			m_CreateChar,
			m_Name,
			m_bUseCallAttWeaponCol );

		ioRotationWeapon2 *pRotationWeapon2 = ToRotationWeapon2( pWeapon );
		if ( pRotationWeapon2 )
		{
			pRotationWeapon2->SetPassTargetName( m_OwnerName );
			pRotationWeapon2->SetStartPosition( vPos, pStage );
		}
	}
	
}
