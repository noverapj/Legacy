

#include "stdafx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioMovableObject.h"

#include "SkillTypeID.h"
#include "ioDummyCharSkill2.h"
#include "ioFlameWeapon.h"
#include "ioPlayStage.h"

#include "ioBaseChar.h"
#include "ioMapEffect.h"
#include "ioMagicStaffItem4.h"

#include "ioDummyChar.h"
#include "ioDummyCharManager.h"

ioDummyCharSkill2::ioDummyCharSkill2()
{
	m_dwMeteorFireTime = 0;
	m_dwCircleEnd = 0;

	m_fExplosionRange = 0.0f;

	m_dwCurMapEffect = -1;
}

ioDummyCharSkill2::ioDummyCharSkill2( const ioDummyCharSkill2 &rhs )
	: ioAttackSkill( rhs ),
	m_fCircleOffSet( rhs.m_fCircleOffSet ),
	m_dwCircleDuration( rhs.m_dwCircleDuration ),
	m_RedCircle( rhs.m_RedCircle ),
	m_BlueCircle( rhs.m_BlueCircle ),
	m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
	m_DummyCharList( rhs.m_DummyCharList )
{
	m_dwMeteorFireTime = 0;
	m_dwCircleEnd = 0;

	m_fExplosionRange = 0.0f;

	m_dwCurMapEffect = -1;

	m_bUsedMagicCircle = false;
}

ioDummyCharSkill2::~ioDummyCharSkill2()
{
	m_vDestroyDummyCharList.clear();
	m_DummyCharList.clear();
}

void ioDummyCharSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedCircle = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueCircle = szBuf;

	m_fCircleOffSet = rkLoader.LoadFloat_e( "circle_offset", 0.0f );

	m_dwCircleDuration = rkLoader.LoadInt_e( "circle_duration", 0 );

	//////////////////////////////////////////////////////////////////////////
	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );
}

ioSkill* ioDummyCharSkill2::Clone()
{
	return new ioDummyCharSkill2( *this );
}

void ioDummyCharSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioFlameWeapon *pFlame = ToFlameWeapon( pWeapon );

	if( pFlame && m_fExplosionRange > 0.0f )
	{
		pFlame->SetExplosionRange( m_fExplosionRange );
	}
}

bool ioDummyCharSkill2::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ioMagicStaffItem4 *pStaff = ToMagicStaffItem4( GetOwnerItem() );
	if( pStaff )
	{
		m_fExplosionRange = pStaff->GetCircleRadius();
	}

	//CreateMapEffect
	if( pChar->CheckMagicCircle() )
	{
		m_bUsedMagicCircle = true;
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( 0.0f, 0.0f, 0.0f, true, true );
	}

	float fCurOffSet = GetOwnerItem()->GetMagicCircleOffSet();
	if( fCurOffSet == 0.0f )
		fCurOffSet = m_fCircleOffSet;

	m_vCirclePos = pChar->GetMagicCirclePos( fCurOffSet, m_bCircleNoChangeY );

	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCirclePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCirclePos );
	}

	if( pMapEffect )
	{
		DWORD dwPreDelayTime = GetPreDelayTime();

		m_dwCurMapEffect = pMapEffect->GetUniqueID();
		m_dwCircleEnd = FRAMEGETTIME() + dwPreDelayTime + m_dwCircleDuration;
	}

	m_dwMeteorFireTime = 0;

	return true;
}


void ioDummyCharSkill2::CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage, const D3DXVECTOR3& vPos )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;
	if( !pStage ) return;

	if( m_DummyCharList.empty() )
		return;

	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );
	vDummyCharCreateInfoList vCreateList;
	vCreateList.clear();

	int iSize = m_DummyCharList.size();
	for( int i=0; i < iSize; ++i )
	{
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vSideDir;
		D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
		D3DXVec3Normalize( &vSideDir, &vSideDir );

		ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		/*float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
		float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;*/
		float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

		/*D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition() + (fOffset * vDir);
		vTargetPos += fSideOffset * vSideDir;*/

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
			pChar->GetCharName(),
			vPos,
			fStartAngle,
			0, true );

		if( pDummy )
			pDummy->SetControlSkillName( GetName() );

		DummyCharCreateInfo kInfo;
		kInfo.m_DummyCharName = szDummyCharName;
		kInfo.m_iDummyCharIdx = iCurIndex;
		kInfo.m_fDummyCharStartAngle = fStartAngle;
		kInfo.m_vTargetPos = vPos;

		vCreateList.push_back( kInfo );
	}

	int iCreateCnt = vCreateList.size();
	if( pChar->IsNeedSendNetwork() && iCreateCnt > 0 )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetTargetRot();

		kPacket << iCreateCnt;

		for( int i=0; i < iCreateCnt; ++i )
		{
			kPacket << vCreateList[i].m_DummyCharName;
			kPacket << vCreateList[i].m_iDummyCharIdx;
			kPacket << vCreateList[i].m_fDummyCharStartAngle;
			kPacket << vCreateList[i].m_vTargetPos;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyCharSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioAttackSkill::OnProcessState( pChar, pStage );

	if( pChar->IsNeedProcess() && m_dwMeteorFireTime > 0 && m_dwMeteorFireTime < FRAMEGETTIME() )
	{
		/*pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
			GetName(),
			m_dwWeaponBaseIndex,
			m_vCirclePos );*/
		CheckDummyChar( pChar, pStage, m_vCirclePos );
		

		m_dwMeteorFireTime = 0;
	}

	if( m_dwCircleEnd > 0 && m_dwCircleEnd < FRAMEGETTIME() )
	{
		//DestroyMapEffect
		pChar->EndMapEffect( m_dwCurMapEffect );
		pChar->DestroyMagicCircle();
		m_dwCircleEnd = 0;
	}
}

void ioDummyCharSkill2::AttackSkillFire( ioBaseChar *pChar )
{
	EnableRefreshFireTimeList( false );	// ioAttackSkill::AttackSkillFire() 이전에 해줘야함

	ioAttackSkill::AttackSkillFire( pChar );

	m_dwMeteorFireTime = FRAMEGETTIME() + GetAttribute().m_dwPreDelay;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID != -1 )
	{
		float fFireTime = pChar->GetFirstFireTime( szSkillMotion );
		m_dwMeteorFireTime += fFireTime * GetAttribute().m_fAttackAniRate;
	}
}

void ioDummyCharSkill2::OnSkillEnd( ioBaseChar *pChar )
{
	//DestroyMapEffect
	pChar->EndMapEffect( m_dwCurMapEffect );
	pChar->DestroyMagicCircle();

	ioAttackSkill::OnSkillEnd( pChar );
}



void ioDummyCharSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pChar->SetTargetRotToRotate( qtRot, true, false );

	int iCreateCnt;
	rkPacket >> iCreateCnt;

	for( int i=0; i < iCreateCnt; ++i )
	{
		int iCurIndex;
		ioHashString szDummyCharName;
		float fStartAngle;
		D3DXVECTOR3 vTargetPos;

		rkPacket >> szDummyCharName >> iCurIndex;
		rkPacket >> fStartAngle;
		rkPacket >> vTargetPos;

		//m_dwFireStartTime = 0;

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
			pChar->GetCharName(),
			vTargetPos,
			fStartAngle,
			0, false );

		if( pDummy )
			pDummy->SetControlSkillName( GetName() );
	}
}