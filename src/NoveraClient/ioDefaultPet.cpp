
#include "stdafx.h"

#include "ioDefaultPet.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioDefaultPet::ioDefaultPet( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioPet( pGrp, pMode )
{	
	m_PetState = DPS_None;
	m_bPreForceHide = false;
}

ioDefaultPet::~ioDefaultPet()
{
}

//데이터 읽기
void ioDefaultPet::LoadProperty( ioINILoader &rkLoader )
{
	ioPet::LoadProperty( rkLoader );

	m_vAttachOffset.x = rkLoader.LoadFloat_e( "attach_offset_x", 0.0f );
	m_vAttachOffset.y = rkLoader.LoadFloat_e( "attach_offset_y", 0.0f );
	m_vAttachOffset.z = rkLoader.LoadFloat_e( "attach_offset_z", 0.0f );
}

//동기화 처리
void ioDefaultPet::ApplyPetSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	//동기화
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//펫 상태 설정 (캐릭터의 스테이트를 통해)
void ioDefaultPet::SetPetState( int eCharState )
{
	//이전 상태 확인
	if ( m_PreOwnerState == eCharState )
		return;

	if ( eCharState == CS_SPECIAL_MOTION )
		m_PetState = DPS_Special_Loop;
	else
		m_PetState = DPS_Owner_Attach;
	
	//이전 상태를 교체
	m_PreOwnerState = (CharState)eCharState;

	//애니메이션 교체
	SetStateAni( eCharState );
}

//시작 상태
bool ioDefaultPet::OnStartState()
{
	SetStateAni( CS_DELAY );
	m_PetState = DPS_Owner_Attach;

	return true;
}

//엔드 상태
void ioDefaultPet::OnDeadState()
{
	ioPet::OnDeadState();
}

//루프
void ioDefaultPet::OnProcessState()
{
	CheckDie();
	CheckVisible();

	switch( m_PetState )
	{
	case DPS_Special_Loop:
		break;
	case DPS_Owner_Attach:
		OwnerAttachMove();
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//엔드 확인
void ioDefaultPet::CheckDie()
{
	if ( !GetOwner() )
		OnDeadState();
}

//하이드 확인
void ioDefaultPet::CheckVisible()
{
	if ( !m_pOwner || !m_pPlayStage )
		return;

	//강제 하이드 확인
	if ( m_bPreForceHide != m_bForceHide )
	{
		m_pGroup->SetVisible( !m_bForceHide );
		m_bPreForceHide = m_bForceHide;
	}
	
	//강제 하이드면 종료
	if ( m_bForceHide )
		return;		
	
	bool bHide = false;
	if ( m_pOwner->IsHideState() )
		bHide = true;

	//하이드 상태 확인
	if ( m_bPreHide != bHide )
	{
		m_pGroup->SetVisible( !bHide );
		m_bPreHide = bHide;
	}

	//하이드면 종료
	if ( bHide )
		return;		

	int nOwnerAlphaRate = m_pOwner->GetAlphaRate();
	if ( m_nPreAlphaRate != nOwnerAlphaRate )
	{
		m_pGroup->SetAlphaRate( nOwnerAlphaRate );
		m_nPreAlphaRate = nOwnerAlphaRate;

		if ( nOwnerAlphaRate > 250 )
			m_pGroup->SetVisible( true );
		else if ( nOwnerAlphaRate < 5 )
			m_pGroup->SetVisible( false );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//이동 설정
void ioDefaultPet::OwnerAttachMove()
{
	if ( !m_pOwner )
		return;

	D3DXQUATERNION qtRot = m_pOwner->GetTargetRot();
	D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate() + ( qtRot * m_vAttachOffset * m_pOwner->GetWorldScale() );
	SetWorldPosition( vPos );
	SetWorldOrientation( qtRot );
}