

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ItemDefine.h"
#include "SkillDefine.h"

#include "ioPushDirectionBuff.h"

ioPushDirectionBuff::ioPushDirectionBuff()
{
}

ioPushDirectionBuff::ioPushDirectionBuff( const ioPushDirectionBuff &rhs )
	: ioBuff( rhs ),
	m_fAngle( rhs.m_fAngle ),
	m_bLinerPower( rhs.m_bLinerPower ),
	m_fPushPower( rhs.m_fPushPower ),
	m_fBlowPower( rhs.m_fBlowPower )
{
}

ioPushDirectionBuff::~ioPushDirectionBuff()
{
}

void ioPushDirectionBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_fAngle = rkLoader.LoadFloat_e( "push_angle", 0.0f );
	m_bLinerPower = rkLoader.LoadBool_e( "push_power_linear", true );
	m_fPushPower = rkLoader.LoadFloat_e( "push_power", 0.0f );
	m_fBlowPower = rkLoader.LoadFloat_e( "blow_power", 0.0f );
}

ioBuff* ioPushDirectionBuff::Clone()
{
	return new ioPushDirectionBuff( *this );
}

void ioPushDirectionBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioPushDirectionBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}


void ioPushDirectionBuff::SetAction()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	ioBaseChar *pCreater = GetCreator();
	if( !pCreater )
	{
		SetReserveEndBuff();
		return;
	}

	ioAttackableItem *pItem = pCreater->GetPriorityItem( SPT_ATTACK );

	ioSkill *pCurSkill = pCreater->GetCurActiveSkill();
	ioChainTraceSkill *pChainTraceSkill = ToChainTrace( pCurSkill );
	ioCircleMoveAttackSkill7 *pCircleMoveAttackSkill7 = ToCircleMoveAttackSkill7( pCurSkill );
	ioSuccubusItem *pSuccubusItem = ToSuccubusItem( pItem );
	ioMidnightItem* pMidnightItem = ToMidnightItem( pItem );
	ioFantasticWhipAttack* pFantastic = ToFantasticWhipAttack( pCurSkill );

	D3DXVECTOR3 vPushDir =	pCreater->GetMoveDir();
	ioUserKeyInput::DirKeyInput curKey = ioUserKeyInput::DKI_NONE;

	if( pChainTraceSkill )
	{
		curKey = pChainTraceSkill->GetCurKey();
	}
	else if( pCircleMoveAttackSkill7 )
	{
		curKey = pCircleMoveAttackSkill7->GetCurKey();
	}
	else if( pSuccubusItem )
	{
		curKey = pSuccubusItem->GetCurKey();
	}
	else if( pMidnightItem )
	{
		curKey = ioUserKeyInput::DKI_NONE;
	}
	else if( pFantastic )
	{
		curKey = ioUserKeyInput::DKI_NONE;
	}
	else 
	{
		SetReserveEndBuff();
		return;
	}


	D3DXVECTOR3 vAxis1, vAxis2;
	D3DXVec3Cross( &vAxis1, &ioMath::UNIT_Y, &vPushDir );
	D3DXVec3Normalize(&vAxis1,&vAxis1);

	D3DXVec3Cross( &vAxis2, &vPushDir, &vAxis1 );
	D3DXVec3Normalize(&vAxis2,&vAxis2);

	D3DXQUATERNION qtRot;
	if( curKey == ioUserKeyInput::DKI_NONE )
	{
		if( vAxis2 == ioMath::VEC3_ZERO )
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(m_fAngle) );
		else
			D3DXQuaternionRotationAxis( &qtRot, &vAxis2, DEGtoRAD(m_fAngle) );

		vPushDir = qtRot * vPushDir;
		D3DXVec3Normalize(&vPushDir,&vPushDir);
	}
	else
	{
		float fYawD = 180.0f + 45.0f * ( curKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
		D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
		vPushDir = vMoveDir;
	}

	pOwner->SetForcePower( vPushDir, m_fPushPower, FLOAT1, m_bLinerPower );
	pOwner->SetJumpPower( m_fBlowPower );
}


void ioPushDirectionBuff::ProcessBuff( float fTimePerSec )
{
	ioBuff::ProcessBuff( fTimePerSec );
}


void ioPushDirectionBuff::EndBuff()
{
	ioBuff::EndBuff();
}