

#include "stdafx.h"

#include "ioDummyCharSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"
#include "ioZoneEffectWeapon.h"

ioDummyCharSkill::ioDummyCharSkill()
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
	m_bSkillMotionLoop = false;
	m_bSkillMotionLoopCheck = false;
}

ioDummyCharSkill::ioDummyCharSkill( const ioDummyCharSkill &rhs )
: ioAttackSkill( rhs ),
 m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
 m_DummyCharList( rhs.m_DummyCharList ),
 m_OwnerBuffList( rhs.m_OwnerBuffList ),
 m_bSkillMotionLoop( rhs.m_bSkillMotionLoop )
{
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
	m_bSkillMotionLoopCheck = false;
}

ioDummyCharSkill::~ioDummyCharSkill()
{
	m_vDestroyDummyCharList.clear();
	m_DummyCharList.clear();
}

void ioDummyCharSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	//
	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );

	m_bSkillMotionLoop = rkLoader.LoadBool_e( "skill_motion_loop", false );

	//
	LoadOwnerBuffList( rkLoader );
}

void ioDummyCharSkill::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

ioSkill* ioDummyCharSkill::Clone()
{
	return new ioDummyCharSkill( *this );
}

bool ioDummyCharSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	SetCameraBuff( pChar );

	if(m_bSkillMotionLoop)
		m_bSkillMotionLoopCheck = true;

	return true;
}

void ioDummyCharSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	SetSkillActionState( pChar );
}

void ioDummyCharSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetAttackTimeRate();
	m_dwFireStartTime = FRAMEGETTIME();
	m_SkillState = SS_ACTION;
}

void ioDummyCharSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				if(!m_bSkillMotionLoopCheck)
					m_dwPreDelayStartTime = 0;

				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );
			CheckDummyChar( pChar, pStage );
			//SkillMotionEndByBuff(pChar, m_OwnerBuffList[1].m_BuffName);  이 코드 사용했을때 다른 더미 스킬에 사이드 이펙트(크래시)가 나타나 주석처리 : SYH
		}
		break;
	}
}

void ioDummyCharSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	
	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}


	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;

	m_bSkillMotionLoopCheck = false;
	m_dwPreDelayStartTime = 0;
	EndCameraBuff( pChar );
}

bool ioDummyCharSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioDummyCharSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, true );

		}
	}

	SetChargingRateForWeapon( pWeapon );
}

void ioDummyCharSkill::CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;
	if( !pStage ) return;

	if( m_DummyCharList.empty() )
		return;

	if( m_dwFireStartTime == 0 ) return;
	if( m_dwFireStartTime >= FRAMEGETTIME() )
		return;

	m_dwFireStartTime = 0;

	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	//
	vDummyCharCreateInfoList vCreateList;
	vCreateList.clear();

	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	int iSize = m_DummyCharList.size();
	for( int i=0; i < iSize; ++i )
	{
		ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
		float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
		float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

		D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition() + (fOffset * vDir);
		vTargetPos += fSideOffset * vSideDir;

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
													   pChar->GetCharName(),
													   vTargetPos,
													   fStartAngle,
													   0, true );

		if( pDummy )
			pDummy->SetControlSkillName( GetName() );

		DummyCharCreateInfo kInfo;
		kInfo.m_DummyCharName = szDummyCharName;
		kInfo.m_iDummyCharIdx = iCurIndex;
		kInfo.m_fDummyCharStartAngle = fStartAngle;
		kInfo.m_vTargetPos = vTargetPos;

		vCreateList.push_back( kInfo );
	}

	SetOwnerBuffList( pChar, pStage );

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

void ioDummyCharSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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

		m_dwFireStartTime = 0;

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
													   pChar->GetCharName(),
													   vTargetPos,
													   fStartAngle,
													   0, false );

		g_DummyCharMgr.SetCurLocalIndex( iCurIndex );

		if( pDummy )
			pDummy->SetControlSkillName( GetName() );
	}
}

bool ioDummyCharSkill::SetOwnerBuffList( ioBaseChar *pChar, ioPlayStage* pStage )
{
	if( !pChar->IsNeedProcess() ) return false;
	if( m_OwnerBuffList.empty() ) return false;

	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
	if( pUpInfo )
	{		
		fValue = pUpInfo->GetValue(pStage);
	}

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i].m_BuffName;

		if( pChar->IsHasCrown() && m_bDisableKingSkill )
		{
			continue;
		}
		else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
			continue;


		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuff( szBuffName, pChar->GetCharName(), szItemName, this );
			if( pBuff )
			{				
				pBuff->SetExtraDurationByGrowth( fValue );

				bResult = true;
				vBuffList.push_back( szBuffName );
			}
		}
	}

	if( bResult )
	{
		int iCurBuffCnt = vBuffList.size();
		if( pChar->IsNeedSendNetwork() )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff2 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << pChar->GetCharName();
			kPacket << szItemName;
			kPacket << pChar->GetRandomSeed();
			kPacket << true;					// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << fValue;
			kPacket << 0.0f;
			kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << iCurBuffCnt;

			for( i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return bResult;
}

bool ioDummyCharSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if( !m_bSkillMotionLoop && !m_bSkillMotionLoopCheck )
	{
		if( m_dwPreDelayStartTime > 0 )
			return false;
		if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
			return false;
	}
	
	if( m_dwPreDelayStartTime > 0 && m_bSkillMotionLoopCheck )
		return false;

	return true;
}

void ioDummyCharSkill::SkillMotionEndByBuff( ioBaseChar* pOwner, ioHashString& szBuffName )
{
	if(!pOwner->HasBuff(szBuffName))
	{
		m_bSkillMotionLoopCheck = false;
		m_dwPreDelayStartTime = 0;
	}
}


//////////////////////////////////////////////////////////////////////////
// ioNpcSpawnSkill
//////////////////////////////////////////////////////////////////////////

ioNpcSpawnSkill::ioNpcSpawnSkill()
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
}

ioNpcSpawnSkill::ioNpcSpawnSkill( const ioNpcSpawnSkill &rhs )
	: ioAttackSkill( rhs ) , m_vecNpc( rhs.m_vecNpc )
{
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
}

ioNpcSpawnSkill::~ioNpcSpawnSkill()
{
	m_vecNpc.clear();
}

void ioNpcSpawnSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	int nMax = rkLoader.LoadInt_e( "max_npc", 0 );

	for( int i=0; i < nMax; ++i )
	{
		stNpc stSpawnNpc;

		wsprintf_e( szBuf, "npc%d_code", i+1 );
		stSpawnNpc.dwNpcCode = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "npc%d_start_time", i+1 );
		stSpawnNpc.dwStartTime = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "npc%d_offset", i+1 );
		stSpawnNpc.fOffset = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "npc%d_side_offset", i+1 );
		stSpawnNpc.fSideOffset = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "npc%d_start_angle", i+1 );
		stSpawnNpc.fStartAngle = rkLoader.LoadFloat( szBuf, 0.0f );

		m_vecNpc.push_back( stSpawnNpc );
	}
}

ioSkill* ioNpcSpawnSkill::Clone()
{
	return new ioNpcSpawnSkill( *this );
}


bool ioNpcSpawnSkill::OnSkillStart( ioBaseChar *pChar )
{
	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	return true;
}


void ioNpcSpawnSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			//TODO 여기에서 서버에 npc 소환 패킷 날린다.   npc 의 오너이면서 로컬일때만 날릴 것
			CheckAniRotate( pChar );  
			SendSpawnNpcMsg( pChar, pStage );
		}
		break;
	}
}

void ioNpcSpawnSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}


	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
}

bool ioNpcSpawnSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioNpcSpawnSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	SetSkillActionState( pChar );
}

void ioNpcSpawnSkill::SetSkillActionState( ioBaseChar *pChar )
{
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetAttackTimeRate();
	m_dwFireStartTime = FRAMEGETTIME();
	m_SkillState = SS_ACTION;
}


void ioNpcSpawnSkill::SendSpawnNpcMsg( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;
	if( !pStage ) return;

	if( m_vecNpc.empty() )
		return;

	if( !pChar->IsNeedSendNetwork() )
		return;

	if( m_dwFireStartTime == 0 ) return;
	if( m_dwFireStartTime >= FRAMEGETTIME() )
		return;

	m_dwFireStartTime = 0;

	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	int iSize = m_vecNpc.size();

	SP2Packet kPacket( CTPK_NPC_SPAWN_SKILL );
	kPacket << iSize;

	for( int i=0; i < iSize; ++i )
	{
		float fOffset = m_vecNpc[i].fOffset;
		float fSideOffset = m_vecNpc[i].fSideOffset;
		float fStartAngle = m_vecNpc[i].fStartAngle;

		D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition() + (fOffset * vDir);
		vTargetPos += fSideOffset * vSideDir;

		kPacket << m_vecNpc[i].dwNpcCode;
		kPacket << (int)pChar->GetTeam();
		kPacket << m_vecNpc[i].dwStartTime;
		kPacket << vTargetPos.x;
		kPacket << vTargetPos.z;
	}
	TCPNetwork::SendToServer( kPacket );
}

