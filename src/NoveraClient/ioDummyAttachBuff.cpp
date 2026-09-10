
#include "stdafx.h"

#include "ioDummyAttachBuff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "DummyCharDefine.h"

ioDummyAttachBuff::ioDummyAttachBuff()
{
	m_DummyInfoList.clear();
	m_dwCheckDuration = 0;
	m_dwCheckAreaWeapon = 0;
	m_dwCheckDummyWeapon = 0;
	m_dwCheckHornSoundTime = 0;
	m_dwCheckAttackSoundTime = 0;
	m_bStartGauge = false;
}

ioDummyAttachBuff::ioDummyAttachBuff( const ioDummyAttachBuff &rhs )
	: ioBuff( rhs ),
	  m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
	  m_DummyCharList( rhs.m_DummyCharList ),
	  m_dwActiveDuration( rhs.m_dwActiveDuration ),
	  m_bSetActiveDuration( rhs.m_bSetActiveDuration ),
	  m_dwAreaWeaponTick( rhs.m_dwAreaWeaponTick ),
	  m_szAreaWeaponName( rhs.m_szAreaWeaponName ),
	  m_fAreaWeaponUseGauge( rhs.m_fAreaWeaponUseGauge ),
	  m_bSetDummyAttach( rhs.m_bSetDummyAttach ),
	  m_bSetAreaWeapon( rhs.m_bSetAreaWeapon ),
	  m_bSetDummyWeapon( rhs.m_bSetDummyWeapon ),
	  m_fDummyWeaponUseGauge( rhs.m_fDummyWeaponUseGauge ),
	  m_dwDummyWeaponTick( rhs.m_dwDummyWeaponTick ),
	  m_szHornSoundName( rhs.m_szHornSoundName ),
	  m_szAttackSoundName( rhs.m_szAttackSoundName ),
	  m_dwDelayHornSoundTime( rhs.m_dwDelayHornSoundTime ),
	  m_dwDelayAttackSoundTime( rhs.m_dwDelayAttackSoundTime )
{
	m_DummyInfoList.clear();
	m_dwCheckDuration = 0;
	m_dwCheckAreaWeapon = 0;
	m_dwCheckDummyWeapon = 0;
	m_dwCheckHornSoundTime = 0;
	m_dwCheckAttackSoundTime = 0;
	m_bStartGauge = false;
}

ioDummyAttachBuff::~ioDummyAttachBuff()
{
	m_vDestroyDummyCharList.clear();
	m_DummyCharList.clear();
	m_DummyInfoList.clear();
}

void ioDummyAttachBuff::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";

	ioBuff::LoadProperty( rkLoader );
	//시간 설정
	m_bSetActiveDuration = rkLoader.LoadBool_e( "set_duration", false );
	m_dwActiveDuration = rkLoader.LoadInt_e( "active_duration", 0);

	//사운드 설정
	rkLoader.LoadString_e( "horn_sound_name", "", szBuf, MAX_PATH );
	m_szHornSoundName = szBuf;
	rkLoader.LoadString_e( "attack_sound_name", "", szBuf, MAX_PATH );
	m_szAttackSoundName = szBuf;
	m_dwDelayHornSoundTime = rkLoader.LoadInt_e( "horn_sound_delay", 0);
	m_dwDelayAttackSoundTime = rkLoader.LoadInt_e( "attack_sound_delay", 0);

	//범위 무기 설정
	m_bSetAreaWeapon = rkLoader.LoadBool_e( "set_area_weapon_use",false );
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_szAreaWeaponName = szBuf;
	m_dwAreaWeaponTick = rkLoader.LoadInt_e( "area_weapon_tick", 0);
	m_fAreaWeaponUseGauge = rkLoader.LoadFloat_e( "area_weapon_use_gauge", 0.0f );

	//더미 무기 설정
	m_bSetDummyWeapon = rkLoader.LoadBool_e( "set_dummy_weapon_use",false );
	m_dwDummyWeaponTick = rkLoader.LoadInt_e( "dummy_weapon_tick", 0);
	m_fDummyWeaponUseGauge = rkLoader.LoadFloat_e( "dummy_weapon_use_gauge", 0.0f );
	m_bSetDummyAttach = rkLoader.LoadBool_e( "set_dummy_attach", false );

	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );
}

ioBuff* ioDummyAttachBuff::Clone()
{
	return new ioDummyAttachBuff( *this );
}

void ioDummyAttachBuff::StartBuff( ioBaseChar *pOwner )
{
	m_DummyInfoList.clear();

	ioBuff::StartBuff( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCheckDuration = dwCurTime;
	m_DummyInfoList.clear();
	m_dwCheckAreaWeapon = 0;
	m_dwCheckDummyWeapon = 0;
	m_dwCheckHornSoundTime = 0;
	m_dwDelayAttackSoundTime = 0;
	m_bStartGauge = false;
	CreateDummyChar();
}

bool ioDummyAttachBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_DummyInfoList.clear();

	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwCheckDuration = dwCurTime;
	m_DummyInfoList.clear();
	m_dwCheckAreaWeapon = 0;
	m_dwCheckDummyWeapon = 0;
	m_dwCheckHornSoundTime = 0;
	m_dwDelayAttackSoundTime = 0;
	m_bStartGauge = false;

	return true;
}

void ioDummyAttachBuff::ProcessBuff( float fTimePerSec )
{	
	if ( m_bSetActiveDuration && ( m_dwCheckDuration + m_dwActiveDuration < FRAMEGETTIME()) )
	{
		m_pOwner->CheckUseBuffSkill( m_Name, false );
		SetReserveEndBuff();
		return;
	}

	//종료 조건 설정
	if( (m_bStartGauge && !CheckSwitchType( fTimePerSec )) || !CheckDummy() )
	{
		m_pOwner->CheckUseBuffSkill( m_Name, false );
		SetReserveEndBuff();
		return;
	}		

	//더미 목표 위치 설정	
	ProcessDummyAttach();
	//키입력
	ProcessKeyInput();
}

void ioDummyAttachBuff::EndBuff()
{	
	ioBuff::EndBuff();

	m_DummyInfoList.clear();

	SetSkillGauge( 0.0f );
}

void ioDummyAttachBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case BS_CreateAreaWeapon:
		{
			for (int i=0; i<(int)m_DummyInfoList.size(); ++i)
			{	
				ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyChar( pOwner, m_DummyInfoList[i].nIndex );

				if ( pDummyChar )
				{
					D3DXVECTOR3 vPos = pDummyChar->GetWorldPosition();
					D3DXQUATERNION qtRot = pDummyChar->GetWorldOrientation();

					g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", GetName(),	m_szAreaWeaponName,	vPos, qtRot, ioAreaWeapon::CT_NORMAL );
				}		
			}					

			pOwner->ReduceCurSkillGaugeBySkill( m_ItemName, m_fAreaWeaponUseGauge );
			
			if ( !m_bStartGauge )
			{
				m_bStartGauge = true;
				m_dwCheckTime = FRAMEGETTIME();
			}
		}
		break;
	case BS_CreateDummy:
		{
			if( m_DummyCharList.empty() )
				return;

			//기존 더미 제거 (추가 생성 금지)
			g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );
			
			//방향 설정
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			//방향에 대한 우측 방향 설정
			D3DXVECTOR3 vSideDir;
			D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
			D3DXVec3Normalize( &vSideDir, &vSideDir );

			int iCurIndex;
			int iSize = m_DummyCharList.size();
			for( int i=0; i < iSize; ++i )
			{
				//인덱스 얻기
				rkPacket >> iCurIndex;

				//더미 이름 얻기
				ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
				
				//오프셋 설정
				float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
				float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
				float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

				//생성 위치
				D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition() + (fOffset * vDir);
				vTargetPos += fSideOffset * vSideDir;
				
				//더미 생성
				ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
					pOwner->GetCharName(),
					vTargetPos,
					fStartAngle,
					0, true );

				//더미에 스킬 이름 설정
				if( pDummy && pOwner->GetCurActiveSkill() != NULL )
					pDummy->SetControlSkillName( pOwner->GetCurActiveSkill()->GetName() );

				//특정 더미면
				ioOwnerAttach4Dummy *pOwnerAttach4Dummy = ToOwnerAttach4Dummy(pDummy);
				if ( pOwnerAttach4Dummy)
				{
					//맨 앞 더미일때
					if ( i == 0 )
					{
						pOwnerAttach4Dummy->SetHead( true );
					}
				}
				AddDummyInfo( iCurIndex );	
			}
		}
		break;
	case BS_CreateDummyWeapon:
		{
			CreateDummyWeapon();
		}
		break;
	case BS_PlayHornSound:
		{
			if( !m_szHornSoundName.IsEmpty() )
			{
				pOwner->PlaySound( m_szHornSoundName );
			}
		}
		break;
	case BS_PlayAttackSound:
		{
			if( !m_szAttackSoundName.IsEmpty() )
			{
				pOwner->PlaySound( m_szAttackSoundName );
			}
		}
		break;
	}
}

//추가한 함수//////////////////////////////////////////////////////////////////
//키입력
void ioDummyAttachBuff::ProcessKeyInput()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	if( pOwner->IsAttackKey() )
	{
		//범위 무기 생성
		if ( m_bSetAreaWeapon )
		{
			CreateAreaWeapon();
		}	
		//더미 무기 생성
		if ( m_bSetDummyWeapon )
		{
			CreateDummyWeapon();
		}		
		//공격 사운드 재생
		PlayAttackSound();
	}

	//사운드 재생
	if ( pOwner->IsDefenseKey() )
	{
		PlayHornSound();
	}
}

//소리 재생
void ioDummyAttachBuff::PlayHornSound()
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwCheckHornSoundTime + m_dwDelayHornSoundTime > dwCurTime )
		return;

	if( !m_szHornSoundName.IsEmpty() )
	{
		m_dwCheckHornSoundTime = dwCurTime;
		pOwner->PlaySound( m_szHornSoundName );
	}

	//패킷전송
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_PlayHornSound;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyAttachBuff::PlayAttackSound()
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwCheckAttackSoundTime + m_dwDelayAttackSoundTime > dwCurTime )
		return;

	if( !m_szAttackSoundName.IsEmpty() )
	{
		m_dwCheckAttackSoundTime = dwCurTime;
		pOwner->PlaySound( m_szAttackSoundName );
	}

	//패킷전송
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_PlayAttackSound;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//스킬게이지 설정
void ioDummyAttachBuff::SetSkillGauge( float fSkillGauge )
{
	//버프의 주인 얻기
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	//주인의 해당 아이템의 스킬 게이지를 재설정
	pOwner->SetCurItemSkillGauge( m_ItemName, fSkillGauge );
}

//범위 무기 생성
void ioDummyAttachBuff::CreateAreaWeapon()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwCheckAreaWeapon + m_dwAreaWeaponTick > dwCurTime )
		return;

	//더미의 위치 기준으로 범위 무기 생성
	for (int i=0; i<(int)m_DummyInfoList.size(); ++i)
	{	
		ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyChar( pOwner, m_DummyInfoList[i].nIndex );

		if ( pDummyChar )
		{
			D3DXVECTOR3 vPos = pDummyChar->GetWorldPosition();
			D3DXQUATERNION qtRot = pDummyChar->GetWorldOrientation();

			g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", GetName(),	m_szAreaWeaponName,	vPos, qtRot, ioAreaWeapon::CT_NORMAL );
		}		
	}	

	m_dwCheckAreaWeapon = dwCurTime;

	if ( !m_bStartGauge )
	{
		m_bStartGauge = true;
		m_dwCheckTime = FRAMEGETTIME();
	}
		
	pOwner->ReduceCurSkillGaugeBySkill( m_ItemName, m_fAreaWeaponUseGauge );

	//패킷전송
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_CreateAreaWeapon;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//더미 캐릭터 생성
void ioDummyAttachBuff::CreateDummyChar()
{
	//버프의 주인 얻기
	ioBaseChar *pOwner = GetOwner();
	ioPlayStage *pStage = pOwner->GetCreator();

	if( !pOwner || !pOwner->IsNeedProcess() || !pStage) 
		return;

	if( m_DummyCharList.empty() )
		return;

	//기존 더미 제거 (추가 생성 금지)
	g_DummyCharMgr.DestroyDummyCharByNameList( pOwner, m_vDestroyDummyCharList );
	
	//방향 설정
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	//방향에 대한 우측 방향 설정
	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	int iSize = m_DummyCharList.size();
	for( int i=0; i < iSize; ++i )
	{
		//더미 이름 얻기
		ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;

		//더미 고유 인덱스 번호
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();

		//오프셋 설정
		float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
		float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
		float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

		//생성 위치
		D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition() + (fOffset * vDir);
		vTargetPos += fSideOffset * vSideDir;

		//더미 생성
		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
			pOwner->GetCharName(),
			vTargetPos,
			fStartAngle,
			0, true );

		//더미에 스킬 이름 설정
		if( pDummy && pOwner->GetCurActiveSkill() != NULL )
			pDummy->SetControlSkillName( pOwner->GetCurActiveSkill()->GetName() );

		//특정 더미면
		ioOwnerAttach4Dummy *pOwnerAttach4Dummy = ToOwnerAttach4Dummy(pDummy);
		if ( pOwnerAttach4Dummy)
		{
			//맨 앞 더미일때
			if ( i == 0 )
			{
				pOwnerAttach4Dummy->SetHead( true );
			}
		}
		AddDummyInfo( iCurIndex );			
	}
	
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_CreateDummy;
		for (int i=0; i<(int)m_DummyInfoList.size(); ++i)
		{
			kPacket << m_DummyInfoList[i].nIndex;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//더미 확인
bool ioDummyAttachBuff::CheckDummy()
{
	ioBaseChar *pOwner = GetOwner();

	if ( !pOwner )
		return false;

	for (int i=0; i<(int)m_DummyInfoList.size(); ++i)
	{
		ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyChar( pOwner, m_DummyInfoList[i].nIndex );

		if ( !pDummyChar )
			return false;
	}
	return true;
}

//더미 위치 설정
void ioDummyAttachBuff::ProcessDummyAttach()
{
	if ( !m_pOwner )
		return;

	int nCnt = (int)m_DummyInfoList.size();
	for (int i=0; i<nCnt; ++i)
	{
		ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyChar( m_pOwner, m_DummyInfoList[i].nIndex );

		if ( !pDummyChar )
			continue;

		m_DummyInfoList[i].vPos = pDummyChar->GetWorldPosition();

		ioOwnerAttach4Dummy *pOwnerAttach4Dummy = ToOwnerAttach4Dummy( pDummyChar );
		if ( pOwnerAttach4Dummy)
		{
			if ( m_bSetDummyAttach )
			{
				if ( pOwnerAttach4Dummy->GetHead() )
				{
					pOwnerAttach4Dummy->SetTargetPos( m_pOwner->GetWorldPosition() );
				}
				else 
				{
					pOwnerAttach4Dummy->SetTargetPos( m_DummyInfoList[i-1].vPos );
				}
			}
			else
			{
				pOwnerAttach4Dummy->SetDontMove( true );
			}			
		}
	}
}

//더미 정보 추가
void ioDummyAttachBuff::AddDummyInfo(int nIndex)
{
	DummyInfo CreateDummyInfo;
	CreateDummyInfo.nIndex = nIndex;
	CreateDummyInfo.vPos = ioMath::VEC3_ZERO;
	m_DummyInfoList.push_back( CreateDummyInfo );
}

//더미의 무기 발사
void ioDummyAttachBuff::CreateDummyWeapon()
{
	ioBaseChar *pOwner = GetOwner();

	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwCheckDummyWeapon + m_dwDummyWeaponTick > dwCurTime )
		return;

	for (int i=0; i<(int)m_DummyInfoList.size(); ++i)
	{
		ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyChar( pOwner, m_DummyInfoList[i].nIndex );
		ioOwnerAttach4Dummy *pOwnerAttachDummy = ToOwnerAttach4Dummy( pDummyChar );
		//더미 발사 설정
		if ( pOwnerAttachDummy )
		{
			pOwnerAttachDummy->SetFire( true );
		}
	}

	//게이지 소모
	pOwner->ReduceCurSkillGaugeBySkill( m_ItemName, m_fDummyWeaponUseGauge );

	if ( !m_bStartGauge )
	{
		m_bStartGauge = true;
		m_dwCheckTime = FRAMEGETTIME();
	}

	m_dwCheckDummyWeapon = dwCurTime;

	//패킷 전송
	if ( pOwner->IsNeedProcess() && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_CreateDummyWeapon;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}