#include "StdAfx.h"
#include "ioBuffDummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioBuffDummy::ioBuffDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_vTarget.clear();
}


ioBuffDummy::~ioBuffDummy(void)
{
	DelAllTarget();
}

void ioBuffDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fTargetUnderHeight = rkLoader.LoadFloat_e( "target_under_height", 0.0f );
	m_iTargetCount = rkLoader.LoadInt_e( "target_count", 0 );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	int iCnt = rkLoader.LoadInt_e( "buff_count", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf( szKey, "target_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vBuffName.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "guide_effect", "", szBuf, MAX_PATH );
	m_szGuideEffect = szBuf;

	m_iGuideMaxCount = rkLoader.LoadInt_e( "guide_max_effect", 0 );
}

void ioBuffDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() )
		SendDieState( true );

	//ProcessPos( fTimePerSec );
	//ProcessEffectUpdate( fTimePerSec );
}

void ioBuffDummy::ProcessStartState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );
		return;
	}
}

void ioBuffDummy::ProcessDelayState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioBaseChar *pOwner = GetOwner();
	if( pOwner && pOwner->IsNeedProcess() )
	{
		CheckTarget();
		FindTarget();
	}

	ProcessGuideEffect();
}

void ioBuffDummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SYNC_ADD_TARGET:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;
			AddTarget( szTargetName, false );
		}
		return;
	case SYNC_DEL_TARGET:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;
			DelTarget( szTargetName, false );
		}
		return;
	default:
		ioDummyChar::ApplyDummyCharSyncByType( iType, rkPacket, pStage );
	}
}

void ioBuffDummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );

	DelAllTarget();
}

void ioBuffDummy::CheckTarget()
{
	ioHashStringVec vDelList;
	vTargetInfo::iterator iter = m_vTarget.begin();
	while( iter!=m_vTarget.end() )
	{
		if( !EnableTarget( (*iter).target_name ) )
		{
			vDelList.push_back( (*iter).target_name );
		}

		++iter;
	}

	for each( ioHashString szTargetName in vDelList )
	{
		DelTarget( szTargetName );
	}
}

void ioBuffDummy::FindTarget()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( m_iTargetCount <= (int)m_vTarget.size() )
		return;

	BaseCharList vTargetList;
	D3DXVECTOR3 vPos = GetWorldPosition();
	pOwner->GetTargetCharListByCylinder( vPos, m_fTargetRange, m_fTargetAngle, m_fTargetUpHeight, m_fTargetUnderHeight, vTargetList );

	if( vTargetList.empty() )
		return;

	BaseCharList::const_iterator iter = vTargetList.begin();
	while( iter!=vTargetList.end() )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )
		{
			iter = vTargetList.erase( iter );
			continue;
		}

		if( IsAlreadyTarget(pTarget->GetCharName()) )
		{
			iter = vTargetList.erase( iter );
			continue;
		}

		if( !pStage->IsCollisionCheckByMode() )
		{
			iter = vTargetList.erase( iter );
			continue;
		}

		if( pOwner->GetCharName() == pTarget->GetCharName() )
		{
			iter = vTargetList.erase( iter );
			continue;
		}

		if( pOwner->GetTeam() == pTarget->GetTeam() )
		{
			iter = vTargetList.erase( iter );
			continue;
		}

		if( pTarget->IsSystemState() )
		{
			iter = vTargetList.erase( iter );
			continue;
		}

		if( !pTarget->IsEnableTarget() ) 	
		{
			iter = vTargetList.erase( iter );
			continue;
		}

		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
		{
			iter = vTargetList.erase( iter );
			continue;
		}

		++iter;
	}

	auto SortTarget = [vPos](ioBaseChar *lhs, ioBaseChar *rhs)->bool
	{
		D3DXVECTOR3 vDiff1 = lhs->GetWorldPosition() - vPos;
		D3DXVECTOR3 vDiff2 = rhs->GetWorldPosition() - vPos;
		float distance1 = D3DXVec3Length( &vDiff1 );
		float distance2 = D3DXVec3Length( &vDiff2 );
		return distance1 < distance2;
	};

	std::sort( vTargetList.begin(), vTargetList.end(), SortTarget );
	
	for each( ioBaseChar *pTarget in vTargetList )
	{
		if( pTarget )
			AddTarget( pTarget->GetCharName() );

		if( m_iTargetCount <= (int)m_vTarget.size() )
			break;
	}
}

bool ioBuffDummy::EnableTarget( ioHashString szTargetName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	BaseCharList vTargetList;
	D3DXVECTOR3 vPos = GetWorldPosition();
	pOwner->GetTargetCharListByCylinder( vPos, m_fTargetRange, m_fTargetAngle, m_fTargetUpHeight, m_fTargetUnderHeight, vTargetList );

	auto FindTarget = [szTargetName](ioBaseChar *pChar)->bool
	{
		if( pChar && pChar->GetCharName() == szTargetName )
			return true;

		return false;
	};
	
	BaseCharList::iterator iter = std::find_if( vTargetList.begin(), vTargetList.end(), FindTarget );
	if( iter != vTargetList.end() )
		return true;

	return false;
}

bool ioBuffDummy::IsAlreadyTarget( ioHashString szTargetName )
{
	auto FindTarget = [szTargetName](TargetInfo info)->bool
	{
		return info.target_name == szTargetName;
	};

	vTargetInfo::iterator iter = std::find_if( m_vTarget.begin(), m_vTarget.end(), FindTarget );
	if( iter != m_vTarget.end() )
		return true;

	return false;
}

void ioBuffDummy::AddTarget( ioHashString szTargetName, bool bSend /* = true */ )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( szTargetName );
	if( pTarget )
	{
		for each( ioHashString szBuffName in m_vBuffName )
		{
			if( !pTarget->HasBuff(szBuffName) )
			{
				pTarget->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );
			}
		}

		m_vTarget.push_back( TargetInfo(szTargetName) );

		if( bSend && pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << SYNC_ADD_TARGET;
			kPacket << szTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioBuffDummy::DelTarget( ioHashString szTargetName, bool bSend /* = true */ )
{
	if( m_vTarget.empty() )
		return;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )
		return;

	auto FindTarget = [szTargetName](TargetInfo info)->bool
	{
		return info.target_name == szTargetName;
	};

	vTargetInfo::iterator iter = std::find_if( m_vTarget.begin(), m_vTarget.end(), FindTarget );
	if( iter != m_vTarget.end() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
		if( pTarget )
		{
			for each( ioHashString szBuffName in m_vBuffName )
			{
				pTarget->RemoveBuff( szBuffName );
			}
		}

		DestroyGuideEffect( *iter );
		m_vTarget.erase(iter);

		ioBaseChar *pOwner = GetOwner();
		if( bSend && pOwner && pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << SYNC_DEL_TARGET;
			kPacket << szTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioBuffDummy::DelAllTarget()
{
	if( m_vTarget.empty() )
		return;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )
		return;

	vTargetInfo::iterator iter = m_vTarget.begin();
	while( iter!=m_vTarget.end() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( (*iter).target_name );
		if( pTarget )
		{
			for each( ioHashString szBuffName in m_vBuffName )
			{
				pTarget->RemoveBuff( szBuffName );
			}
		}
		DestroyGuideEffect( *iter );
		iter = m_vTarget.erase(iter);
	}

	m_vTarget.clear();
}

void ioBuffDummy::ProcessGuideEffect()
{
	int iCnt = m_vTarget.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( CheckGuideEffectCreated( m_vTarget[i] ) )
		{
			ArrangeGuideEffect( m_vTarget[i] );
		}
	}
}

void ioBuffDummy::DestroyGuideEffect( TargetInfo &info )
{
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )	return;
	for( int i=0; i<info.GetEffectCount(); ++i )
	{
		UniqueObjID dwEffectID = info.GetEffectID(i);
		if( dwEffectID == 0 )
			continue;
		ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffectID );
		if( pMapEffect )
			pMapEffect->EndEffectForce();
	}
	info.ClearEffectID();
}

bool ioBuffDummy::CheckGuideEffectCreated( TargetInfo &info )
{
	if( info.GetEffectCount() > 0 )
		return true;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )
		return false;

	for( int i=0 ; i<m_iGuideMaxCount ; i++ )
	{
		ioMapEffect *pEffect = pStage->CreateMapEffect( m_szGuideEffect, ioMath::UNIT_ALL );
		if( pEffect )
		{
			info.AddEffectID( pEffect->GetUniqueID() );
		}
	}

	if( info.GetEffectCount() > 0 )
		return true;

	return false;
}

void ioBuffDummy::ArrangeGuideEffect( TargetInfo &info )
{
	if( m_iGuideMaxCount <= 0 )
		return;

	if( m_vTarget.empty() )
		return;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pStage )
		return;

	ioBaseChar *pTarget = pStage->GetBaseChar( info.target_name );
	if( !pTarget )
		return;

	D3DXVECTOR3 vStartPos = GetMidPositionByRate();
	D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
	D3DXVECTOR3 vDiff = vTargetPos - vStartPos;

	float fTargetLength = D3DXVec3Length( &vDiff );
	D3DXVec3Normalize( &vDiff, &vDiff );

	// Update Arrow Pos and Rotation
	D3DXVECTOR3 vCurPos = vStartPos;

	float fGapLength = min( m_fTargetRange, fTargetLength ) / m_iGuideMaxCount;

	float fCurRate = FLOAT1;
	if( m_fTargetRange > 0.0f )
		fCurRate = fTargetLength / m_fTargetRange;

	float fCurScale = max( 0.1f, fCurRate );

	for( int i=0; i<info.GetEffectCount(); ++i )
	{
		UniqueObjID dwEffectID = info.GetEffectID(i);
		if( dwEffectID == 0 )
			continue;

		vCurPos += vDiff * fGapLength;

		ioMapEffect *pMapEff = pStage->FindMapEffect( dwEffectID );
		if( pMapEff )
		{
			pMapEff->SetWorldPosition( vCurPos );

			ioEffect *pEff = pMapEff->GetEffect();
			if( pEff )
			{
				D3DXVECTOR3 vScale( FLOAT1, FLOAT1, fCurScale );
				pEff->GetParentNode()->SetScale( vScale );

				D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDiff );
				pEff->GetParentNode()->SetOrientation( qtRot );
			}
		}
	}
}