
#include "stdafx.h"

#include "ioControlDummySkill3.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"

#include "DummyCharDefine.h"

#include "ioZoneEffectWeapon.h"

ioControlDummySkill3::ioControlDummySkill3()
{
	Init();
}

ioControlDummySkill3::ioControlDummySkill3( const ioControlDummySkill3 &rhs )
	: ioAttackSkill( rhs ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_dwTicTime( rhs.m_dwTicTime ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_DummyCharName( rhs.m_DummyCharName ),
	m_vDummyCharOffset( rhs.m_vDummyCharOffset ),
	m_SubDummyCharInfoList( rhs.m_SubDummyCharInfoList ),
	m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
	m_OwnerBuffList( rhs.m_OwnerBuffList )
{
	Init();
}

ioControlDummySkill3::~ioControlDummySkill3()
{
	m_vDestroyDummyCharList.clear();
}

void ioControlDummySkill3::Init()
{
	m_bSetHandMesh			= false;
	m_SkillState			= SS_PRE;
	m_dwFireStartTime		= 0;
	m_iDummyIndex			= -1;

	m_dwCheckTime			= 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	m_vCreatePos			= ioMath::VEC3_ZERO;

	m_vSubDmmyCharVec.clear();
}

void ioControlDummySkill3::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_bDisableGrowth		= rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic			= rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime				= (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_dwSkillProtectTime	= (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	
	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;

	m_vDummyCharOffset.x = rkLoader.LoadFloat_e( "dummy_char_offset_x", 0.0f );
	m_vDummyCharOffset.y = rkLoader.LoadFloat_e( "dummy_char_offset_y", 0.0f );
	m_vDummyCharOffset.z = rkLoader.LoadFloat_e( "dummy_char_offset_z", 0.0f );

	int iCnt = rkLoader.LoadInt_e( "sub_dummy_cnt", 0 );
	m_SubDummyCharInfoList.clear();
	for( int i = 0; i < iCnt; ++i )
	{
		SubDummyInfo Info;
		wsprintf_e( szKey, "sub_dummy%d_name", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.DummyName =  szBuf;

		wsprintf_e( szKey, "sub_dummy%d_offset_x", i + 1 );
		Info.vOffset.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "sub_dummy%d_offset_y", i + 1 );
		Info.vOffset.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "sub_dummy%d_offset_z", i + 1 );
		Info.vOffset.z = rkLoader.LoadFloat( szKey, 0.0f );

		m_SubDummyCharInfoList.push_back( Info );
	}

	iCnt = rkLoader.LoadInt_e( "destroy_dummy_cnt", 0 );
	m_vDestroyDummyCharList.clear();
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "destroy_dummy%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_vDestroyDummyCharList.push_back( szBuf );
	}

	LoadBuffList( rkLoader );
}

ioSkill* ioControlDummySkill3::Clone()
{
	return new ioControlDummySkill3( *this );
}

void ioControlDummySkill3::LoadBuffList( ioINILoader &rkLoader )
{
	int i, iCount;
	ioBuffInfo tBuffInfo;
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	m_OwnerBuffList.clear();
	iCount = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( i=0; i<iCount; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );

		tBuffInfo.m_BuffName = szBuff;

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

void ioControlDummySkill3::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar || m_OwnerBuffList.empty() )	
		return;

	int nCnt = m_OwnerBuffList.size();
	for( int i=0; i<nCnt; ++i )
		pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), "", this );
}

void ioControlDummySkill3::RemoveOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar || m_OwnerBuffList.empty() )	
		return;

	int nCnt = m_OwnerBuffList.size();
	for( int i=0; i<nCnt; ++i )
		pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName, true );
}

bool ioControlDummySkill3::OnSkillStart( ioBaseChar *pChar )
{
	Init();

	m_bSetHandMesh = false;
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_dwCheckTime			= FRAMEGETTIME();
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;
	m_bReduceSkillGuage		= false;

	// 성장관련 요소 추가.
	if( !m_bDisableGrowth )
	{
		ioPlayStage *pStage = pChar->GetCreator();
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue(pStage);

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );


			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue(pStage);			
		}
	}

	SetCameraBuff( pChar );
	return true;
}

void ioControlDummySkill3::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_PRE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetSkillActionState( pChar, pStage );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		if( 0 < m_dwFireMotionEndTime && m_dwFireMotionEndTime < dwCurTime )
		{	
			if( pChar->IsNeedProcess() )
				SetLoopState( pChar, pStage );
		}
		break;
	case SS_LOOP:
		{
			if( pChar->IsNeedProcess() )
			{
				if( !IsDummyCharEnable( pChar ) )
				{
					SetSkillEndState( pChar, pStage );
					return;
				}
				if( iTickGap >= iCurTicTime )
				{
					m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

					if( fCurTicGauge != 0.0f && m_bReduceSkillGuage )
					{
						if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
						{
							SetSkillEndState( pChar, pStage );
							return;
						}
					}
				}
			}

			ProcessLoopRotate( pChar );
			ProcessLoopPos( pChar );
		}
		break;
	case SS_END:
		break;
	}
}

void ioControlDummySkill3::SetSkillActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	AttackSkillFire( pChar );
	m_SkillState = SS_ACTION;
}

void ioControlDummySkill3::SetLoopState( ioBaseChar* pChar, ioPlayStage *pStage )
{
	m_SkillState		= SS_LOOP;
	m_dwFireStartTime	= 0;
	
	AddOwnerBuff( pChar );
	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	if( pChar->IsNeedProcess() )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << pChar->GetTargetRot();
			kPacket	<< pChar->GetWorldPosition();
			kPacket << (byte)SSS_LOOP;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		CreateDummyChar( pChar, pStage, g_DummyCharMgr.GetCurLocalIndex() );
		CreateSubDummyChar( pChar, pStage );
	}
}

void ioControlDummySkill3::SetSkillEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_SkillState = SS_END;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetTargetRot();
		kPacket	<< pChar->GetWorldPosition();
		kPacket << (byte)SSS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioControlDummySkill3::CreateDummyChar( ioBaseChar *pChar, ioPlayStage *pStage, int iCurIndex )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	if( m_DummyCharName.IsEmpty() )
		return;
	
	m_vCreatePos	= pChar->GetWorldPosition() + ( pChar->GetTargetRot() * m_vDummyCharOffset );
	m_iDummyIndex	= iCurIndex;

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, m_iDummyIndex, pChar->GetCharName(), m_vCreatePos, 0.0f, 0, true );
	if( pDummy )
	{
		pDummy->SetControlSkillName( GetName() );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetTargetRot();
		kPacket	<< pChar->GetWorldPosition();
		kPacket << (byte)SSS_CREATE_CONTROL_DUMMY;
		kPacket << m_iDummyIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioControlDummySkill3::CreateSubDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	if( m_SubDummyCharInfoList.empty() )
		return;
	
	if( pChar->IsNeedProcess() )
	{
		for( int i = 0 ; i < (int)m_SubDummyCharInfoList.size(); ++i )
		{			
			int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
			m_vSubDmmyCharVec.push_back( iCurIndex );
		}
	}

	for( int i = 0 ; i < (int)m_SubDummyCharInfoList.size(); ++i )
	{
		const SubDummyInfo& Info	= m_SubDummyCharInfoList[i];
		D3DXVECTOR3 vOffset			= pChar->GetWorldPosition() + ( pChar->GetTargetRot() * Info.vOffset );

		ioDummyChar *pDummy = pStage->CreateDummyChar( Info.DummyName, m_vSubDmmyCharVec[i], pChar->GetCharName(), vOffset, 0.0f, 0, true );
		if( pDummy )
		{
			pDummy->SetControlSkillName( GetName() );
		}
	}

	if( !m_vSubDmmyCharVec.empty() )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << pChar->GetTargetRot();
			kPacket	<< pChar->GetWorldPosition();
			kPacket << (byte)SSS_CREATE_SUB_DUMMY;
			kPacket << (int)m_vSubDmmyCharVec.size();
			for( int i = 0; i < (int)m_vSubDmmyCharVec.size(); ++i )
			{	
				kPacket << m_vSubDmmyCharVec[i];
			}
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioControlDummySkill3::ProcessLoopRotate( ioBaseChar *pChar )
{
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( !pDummy )
		return;

	if( !pChar->IsNeedProcess() )
		return;

	if( pChar->CheckCurDirKey() != ioUserKeyInput::DKI_NONE && pChar->CheckCurDirKey() != m_DirKeyInput )
	{
		m_DirKeyInput = pChar->CheckCurDirKey();

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << pChar->GetTargetRot();
			kPacket	<< pChar->GetWorldPosition();
			kPacket << (byte)SSS_DIR;
			kPacket << (int)m_DirKeyInput;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		pChar->SetTargetRotToDirKey( m_DirKeyInput );
	}
}

void ioControlDummySkill3::ProcessLoopPos( ioBaseChar *pChar )
{
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( !pDummy )
		return;

	pChar->SetWorldPosition( pDummy->GetWorldPosition() );
}

bool ioControlDummySkill3::IsDummyCharControl( ioBaseChar *pChar )
{
	if( !pChar || m_iDummyIndex == -1 )
		return false;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( !pDummy )
		return false;

	return true;
}

bool ioControlDummySkill3::IsDummyCharEnable( ioBaseChar *pChar )
{
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );

	if( !pDummy )	return false;

	if( pDummy->GetState() == DCS_DIE )
		return false;

	return true;
}

D3DXVECTOR3 ioControlDummySkill3::GetDummyCharControlPos( ioBaseChar *pChar )
{
	if( !pChar || m_iDummyIndex == -1 )
		return m_vCreatePos;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( pDummy )
	{
		return pDummy->GetCameraPosition();
	}

	return m_vCreatePos;
}

void ioControlDummySkill3::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	RemoveOwnerBuff( pChar );
	EndCameraBuff( pChar );

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

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( pDummy )
	{
		if( pDummy->GetType() == DCT_OWNER_CONTROL2 )
		{
			ioOwnerControlDummy2 *pOwnerControl2 = ToOwnerControlDummy2( pDummy );
			if( pOwnerControl2 )
				pOwnerControl2->SetLostControl();
			else
				pDummy->SetDieState( true );
		}
		else
			pDummy->SetDieState( true );
	}

	for( int i = 0; i < (int)m_vSubDmmyCharVec.size(); ++i )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_vSubDmmyCharVec[i] );
		if( pDummy )
			pDummy->SetDieState( true );
	}

	pChar->SetCurItemSkillGauge( GetOwnerItem()->GetName(), 0.0f );

	Init();
}

void ioControlDummySkill3::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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
			pZone->SetZoneValue( false, true );
		}
	}

	SetChargingRateForWeapon( pWeapon );
}

void ioControlDummySkill3::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	D3DXVECTOR3 vPos;
	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	rkPacket >> vPos;

	pChar->SetWorldPosition( vPos );
	pChar->SetWorldOrientation( qtRot );

	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case SSS_LOOP:
		{
			SetLoopState( pChar, pStage );
		}
		break;
	case SSS_CREATE_CONTROL_DUMMY:
		{
			int iDummyIndex;
			rkPacket >> iDummyIndex;
			CreateDummyChar( pChar, pStage, iDummyIndex );
			g_DummyCharMgr.SetCurLocalIndex( iDummyIndex );
		}
		break;
	case SSS_CREATE_SUB_DUMMY:
		{
			int iSize;
			rkPacket >> iSize;
			for( int i = 0; i < iSize; ++i )
			{
				int iDummyIndex;
				rkPacket>> iDummyIndex;
				m_vSubDmmyCharVec.push_back( iDummyIndex );
				g_DummyCharMgr.SetCurLocalIndex( iDummyIndex );
			}
			CreateSubDummyChar( pChar, pStage );
		}
		break;
	case SSS_DIR:
		{
			int eDir;
			rkPacket >> eDir;
			m_DirKeyInput = (ioUserKeyInput::DirKeyInput)eDir;
			pChar->SetTargetRotToDirKey( m_DirKeyInput );
		}
		break;
	case SSS_END:
		{
			SetSkillEndState( pChar, pStage );
		}
		break;
	}
}

bool ioControlDummySkill3::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioControlDummySkill3::IsProtected( int iDefenseBreakType ) const
{
	if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
		return true;

	return ioSkill::IsProtected( iDefenseBreakType );
}

bool ioControlDummySkill3::IsEndState() const
{
	if( 0.0f < m_fSkillEndNextJumpAmt )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioControlDummySkill3::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( 0.0f < m_fSkillEndNextJumpAmt )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

bool ioControlDummySkill3::IsAttackEndState() const
{
	if( m_fSkillEndNextJumpAmt == 0.0f )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

void ioControlDummySkill3::StartReduceGuage()
{
	m_bReduceSkillGuage = true;
}

bool ioControlDummySkill3::IsNoDropState() const
{
	return true;
}

SkillEndJumpType ioControlDummySkill3::GetSkillEndNextJumpMustEnable()
{
	return SEJT_JUMP;
}