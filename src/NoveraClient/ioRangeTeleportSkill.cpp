

#include "stdafx.h"

#include "ioRangeTeleportSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"


#include "WeaponDefine.h"

ioRangeTeleportSkill::ioRangeTeleportSkill()
{
	m_dwFireMotionEndTime = 0;

	m_fBackRange  = 0.0f;
	m_fHeightRange= 0.0f;
}

ioRangeTeleportSkill::ioRangeTeleportSkill( const ioRangeTeleportSkill &rhs )
: ioRangeSkill( rhs ),
	m_fBackRange( rhs.m_fBackRange ),
	m_fHeightRange( rhs.m_fHeightRange )
{
	m_dwFireMotionEndTime = 0;
}

ioRangeTeleportSkill::~ioRangeTeleportSkill()
{
}

void ioRangeTeleportSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioRangeSkill::LoadProperty( rkLoader );

    m_fBackRange	= rkLoader.LoadFloat_e( "back_move_range", 0.0f );
	m_fHeightRange	= rkLoader.LoadFloat_e( "back_move_height_range", 0.0f );
}

ioSkill* ioRangeTeleportSkill::Clone()
{
	return new ioRangeTeleportSkill( *this );
}

bool ioRangeTeleportSkill::IsEndState() const
{
	if( m_RangeState == RS_END )
		return true;

	return false;
}

void ioRangeTeleportSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_RangeState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;			
			SetTeleportList( pChar, pStage );

			if(	GetSkillMotionName().IsEmpty() )
			{
				SetEndState( pChar );
			}
			else
			{
				SetActionState( pChar );
			}
		}
		break;
	case RS_ACTION:
		if( m_dwMotionStartTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{			
			SetEndState( pChar );
		}
		break;
	}
}

void ioRangeTeleportSkill::SetTeleportList( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "ioRangeTeleportSkill::SetTeleportList() - Owner is NULL" );
		return;
	}

	if( !pOwner->IsNeedProcess() )
		return;

	float fPrevDistSq = m_fSkillRange;
	ioBaseChar *pTargetChar = pOwner;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = (*iter);

		if( !pChar || IsCollisioned( pChar ) )
			continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;

		if( pOwner->GetCharName() == pChar->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pChar->GetTeam() )
			continue;

		if( pChar->IsSystemState() )
			continue;
			
		if( !pChar->IsEnableTarget() )
			continue;

		if( pChar->IsPrisonerMode() )
			continue;

		if( pChar->IsApplyDownState(false) )
			continue;
		
		D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		if( fDistSq < fPrevDistSq )	// Enemy
		{
			pTargetChar = pChar;
			fPrevDistSq = fDistSq;
		}
	}

	pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge( pOwner->GetCharName() ) );
	m_bReduceGauge = true;

	if( pOwner && pTargetChar )
	{
		D3DXVECTOR3 vOffSet( 0.0f, m_fHeightRange, 0.0f );
		D3DXVECTOR3 vTargetPos = pTargetChar->GetWorldPosition() + vOffSet;
		D3DXQUATERNION qtTargetOrientation = pTargetChar->GetWorldOrientation();
		D3DXVECTOR3 vNewPos = vTargetPos - ( ( qtTargetOrientation * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) ) * m_fBackRange );

		pOwner->SetWorldPosition( vNewPos );
		pOwner->SetTargetRotAndMoveDirChange( qtTargetOrientation );

		if( pStage )
			pStage->SetCameraToOwnerExplicit( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioRangeTeleportSkill::ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	D3DXVECTOR3 vTargetPos;
	D3DXQUATERNION qtTargetOrientation;
	rkPacket >> vTargetPos >> qtTargetOrientation;
	if( pOwner )
	{
		pOwner->SetWorldPosition( vTargetPos );
		pOwner->SetTargetRotAndMoveDirChange( qtTargetOrientation );
	}
}