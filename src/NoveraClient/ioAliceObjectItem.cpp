

#include "stdafx.h"

#include "ioAliceItem.h"
#include "ioAliceObjectItem.h"
#include "ioIljimaeJump.h"

#include "ioBaseChar.h"
#include "WeaponAttribute.h"

#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioAliceObjectItem::ioAliceObjectItem()
{
	m_iWeaponWay		= 0;
	m_iCurCombo			= 0;
	m_bReserveRelease	= false;
}

ioAliceObjectItem::ioAliceObjectItem( const ioAliceObjectItem &rhs )
	: ioObjectItem( rhs ),	
	m_NormalBulletSetList( rhs.m_NormalBulletSetList ),
	m_DashBulletSetList( rhs.m_DashBulletSetList ),
	m_JumpBulletSetList( rhs.m_JumpBulletSetList ),
	m_DashJumpBulletSetList( rhs.m_DashJumpBulletSetList )
{
	m_iWeaponWay		= 0;
	m_iCurCombo			= 0;
	m_bReserveRelease	= false;
}

ioAliceObjectItem::~ioAliceObjectItem()
{
}

void ioAliceObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	LoadBulletProperty( rkLoader, "normal_bullet", m_NormalBulletSetList );
	LoadBulletProperty( rkLoader, "dash_bullet", m_DashBulletSetList );
	LoadBulletProperty( rkLoader, "jump_bullet", m_JumpBulletSetList );
	LoadBulletProperty( rkLoader, "dash_jump_bullet", m_DashJumpBulletSetList );
}

ioItem* ioAliceObjectItem::Clone()
{
	return new ioAliceObjectItem( *this );
}

void ioAliceObjectItem::LoadBulletProperty( ioINILoader &rkLoader, const ioHashString& szTitle, BulletSetList& rkList )
{
	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_max_cnt", szTitle.c_str() );
	int iMax = rkLoader.LoadInt( szKey, 0 );
	for( int i = 0; i < iMax; ++i )
	{
		BulletSet kSet;
		wsprintf_e( szKey, "%s%d_max", szTitle.c_str(), i + 1 );
		kSet.m_iMaxWeaponWay = rkLoader.LoadInt( szKey, 0 );

		for( int j = 0; j < kSet.m_iMaxWeaponWay; ++j )
		{
			BulletInfo kInfo;

			wsprintf_e( szKey, "%s%d_angle_h%d", szTitle.c_str(), i + 1, j + 1 );
			kInfo.m_fYawAngle = rkLoader.LoadFloat( szKey, 0.0f );

			wsprintf_e( szKey, "%s%d_angle_v%d", szTitle.c_str(), i + 1, j + 1 );
			kInfo.m_fPitchAngle = rkLoader.LoadFloat( szKey, 0.0f );

			wsprintf_e( szKey, "%s%d_range_offset%d", szTitle.c_str(), i + 1, j + 1 );
			kInfo.m_fRangeOffSet = rkLoader.LoadFloat( szKey, 0.0f );

			wsprintf_e( szKey, "%s%d_height_offset%d", szTitle.c_str(), i + 1, j + 1 );
			kInfo.m_fHeightOffSet = rkLoader.LoadFloat( szKey, 0.0f );

			kSet.m_BulletElmentList.push_back( kInfo );
		}

		rkList.push_back( kSet );
	}
}

ioObjectItem::ObjectSubType ioAliceObjectItem::GetObjectSubType() const
{
	return OST_ALICE;
}

bool ioAliceObjectItem::IsReleaseItem( const ioBaseChar* pOwner ) const
{
	if( pOwner )
	{
		ioWeaponItem *pItem = pOwner->GetWeapon();
		ioAliceItem* pAlice = ToAliceItem( pItem );
		if( pAlice )
		{
			if( m_bReserveRelease )
				return true;

			if( pAlice->GetCurBullet() > 0 )
				return false;
		}
	}

	return true;
}

bool ioAliceObjectItem::IsOneTimeWeaponItem() const
{
	return false;
}

void ioAliceObjectItem::OnEquiped( ioBaseChar *pOwner )
{
	ioObjectItem::OnEquiped( pOwner );

	m_iWeaponWay		= 0;
	m_iCurCombo			= 0;
	m_bReserveRelease	= false;
}

void ioAliceObjectItem::OnReleased( ioBaseChar *pOwner )
{
	ioObjectItem::OnReleased( pOwner );

	m_iWeaponWay		= 0;
	m_iCurCombo			= 0;
	m_bReserveRelease	= false;
}

void ioAliceObjectItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ioObjectItem::SetNormalAttack( pOwner, iCurCombo );

	m_iWeaponWay	= 0;
	m_iCurCombo		= iCurCombo;
}

void ioAliceObjectItem::NotifyDashAttack( ioBaseChar *pOwner )
{
	m_iCurCombo  = 0;
	m_iWeaponWay = 0;
}

void ioAliceObjectItem::NotifyJumpAttack( ioBaseChar *pOwner, int iCurCombo )
{
	m_iWeaponWay	= 0;
	m_iCurCombo		= iCurCombo;
}

void ioAliceObjectItem::NotifyDashJumpAttack( ioBaseChar *pOwner, int iCurCombo )
{
	m_iCurCombo  = 0;
	m_iWeaponWay = iCurCombo;
}

void ioAliceObjectItem::OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	ioObjectItem::OnProcessState( pStage, pOwner );

	pOwner->CheckExtraAniJump();
}

bool ioAliceObjectItem::CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	bool bRet = ioObjectItem::CheckReleaseProcess( pStage, pOwner );
	if( bRet )
		return true;

	return CheckReleaseAliceGun( pStage, pOwner );
}

bool ioAliceObjectItem::CheckReleaseByNormalAttack( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	return IsReleaseItem( pOwner );
}

bool ioAliceObjectItem::CheckReleaseAliceGun( ioPlayStage *pStage, ioBaseChar *pOwner, bool bDefenseKeyCheck /*= true */ )
{
	if( !pOwner )
		return false;

	bool bDefense = false;
	if( bDefenseKeyCheck )
	{
		bDefense = pOwner->IsDefenseKey();

		if( bDefense )
			m_bReserveRelease = bDefense;
	}

	ioWeaponItem *pItem = pOwner->GetWeapon();
	ioAliceItem* pAlice = ToAliceItem( pItem );

	//자신을 생성한 무기가 없는 상태에서는 무조건 릴리즈 되어야 함
	if( !pAlice )
		return true;

	if( IsReleaseItem( pOwner ) || bDefense )
	{
		if( pOwner->GetState() == CS_OBJECT_EQUIP )
			return false;

		if( pOwner->GetState() == CS_JUMP )
			return false;

		if( pOwner->GetState() == CS_ATTACK )
			return false;

		if( pOwner->GetState() == CS_DASH_ATTACK )
			return false;
		
		if( pOwner->GetState() == CS_NEW_JUMP_ATTACK )
			return false;

		return true;
	}

	return false;
}

bool ioAliceObjectItem::IsEnableDash() const
{
	return true;
}

bool ioAliceObjectItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	if( IsReleaseItem( pOwner ) )
		return false;

	return true;
}

void ioAliceObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioObjectItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioMissileWeapon* pMissile = ToMissileWeapon( pWeapon );	
	if( pMissile )
	{
		ioBaseChar* pOwner = pMissile->GetOwner();
		if( pOwner )
		{
			BulletSet* pSet		= NULL;
			CharState eState	= pOwner->GetState();
			switch( eState )
			{
			case CS_ATTACK:
				{
					if( COMPARE( m_iCurCombo, 0, (int)m_NormalBulletSetList.size() ) )
					{
						pSet = &m_NormalBulletSetList[m_iCurCombo];
					}
				}
				break;
			case CS_DASH_ATTACK:
				{
					if( COMPARE( m_iCurCombo, 0, (int)m_DashBulletSetList.size() ) )
					{
						pSet = &m_DashBulletSetList[m_iCurCombo];
					}
				}
				break;
			case CS_JUMP:
				{
					if( COMPARE( m_iCurCombo, 0, (int)m_JumpBulletSetList.size() ) )
					{
						pSet = &m_JumpBulletSetList[m_iCurCombo];
					}
				}
				break;
			case CS_NEW_JUMP_ATTACK:
				{
					if( COMPARE( m_iCurCombo, 0, (int)m_DashJumpBulletSetList.size() ) )
					{
						pSet = &m_DashJumpBulletSetList[m_iCurCombo];
					}
				}
				break;
			}			

			if( pSet && COMPARE( m_iWeaponWay, 0, (int)pSet->m_BulletElmentList.size() ) )
			{
				float fCurYawAngle		= 0.0f;
				float fCurPitchAngle	= 0.0f;

				fCurYawAngle	= pSet->m_BulletElmentList[m_iWeaponWay].m_fYawAngle;
				fCurPitchAngle	= pSet->m_BulletElmentList[m_iWeaponWay].m_fPitchAngle;

				fCurYawAngle	= ioMath::ArrangeHead( fCurYawAngle );
				fCurPitchAngle	= ioMath::ArrangeHead( fCurPitchAngle );

				D3DXQUATERNION qtRotate = pWeapon->GetOwner()->GetTargetRot();
				D3DXVECTOR3 vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

				D3DXQUATERNION qtRot;
				D3DXVECTOR3 vAxis;
				D3DXVec3Cross( &vAxis, &vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

				// vertical
				D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fCurYawAngle) );
				vMoveDir = qtRot * vMoveDir;

				// horizon
				D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fCurPitchAngle) );
				vMoveDir = qtRot * vMoveDir;	
				pWeapon->SetMoveDir( vMoveDir );
				m_iWeaponWay++;
			}
		}
	}
}