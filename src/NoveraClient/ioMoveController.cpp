
#include "stdafx.h"

#include "ioMoveController.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"

ioMoveController::ioMoveController()
{
	m_dwCurrentTime = 0;
}

ioMoveController::~ioMoveController()
{
	MoveMap::iterator iCreate;
	for( iCreate = m_MoveMap.begin() ; iCreate != m_MoveMap.end() ; ++iCreate )
	{
		MoveList &kList = iCreate->second;
		kList.m_Move.clear();
	}
	m_CurMove.m_Move.clear();
}

void ioMoveController::LoadController( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "MoveController" );
	int iMaxController = rkLoader.LoadInt_e( "MaxController", 0 );
	for(int i = 0;i < iMaxController;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "Move%d", i + 1 );
		rkLoader.SetTitle( szKey );

		MoveList kMoveList;
		kMoveList.m_fKeepRange = rkLoader.LoadFloat_e( "KeepRange", 0.0f );
		kMoveList.m_fMoveRange = rkLoader.LoadFloat_e( "TargetRange", 600.0f );
		kMoveList.m_fLimitHPRateMin = rkLoader.LoadFloat_e( "LimitHPRateMin", 0.0f );
		kMoveList.m_fLimitHPRateMax = rkLoader.LoadFloat_e( "LimitHPRateMax", 1.1f );

		int iMaxMove = rkLoader.LoadInt_e( "MaxKey", 0 );
		for(int j = 0;j < iMaxMove;j++)
		{
			MoveData kData;

			sprintf_e( szKey, "Key%d", j + 1 );
			kData.m_wKeyData  = rkLoader.LoadInt( szKey, 0 );
			sprintf_e( szKey, "Key%d_Delay", j + 1 );
			kData.m_dwDelayTime = rkLoader.LoadInt( szKey, 0 );

			kMoveList.m_Move.push_back( kData );
		}
		m_MoveMap.insert( MoveMap::value_type( i + 1, kMoveList ) );
	}
}

void ioMoveController::ClearCurrentData()
{
	m_dwCurrentTime = 0;
	m_CurMove.m_Move.clear();
}

void ioMoveController::AttachMoveData( MoveList &rkList )
{
	// Move Copy
	m_CurMove.m_fMoveRange = rkList.m_fMoveRange;
	m_CurMove.m_fKeepRange = rkList.m_fKeepRange;
	m_CurMove.m_Move.clear();
	for(int i = 0;i < (int)rkList.m_Move.size();i++)
	{
		MoveData &rkData = rkList.m_Move[i];
		m_CurMove.m_Move.push_back( rkData );
	}
	m_dwCurrentTime = FRAMEGETTIME();
}

bool ioMoveController::Attach( ioNpcChar *pOwner, ioBaseChar *pTarget, float fDistSq )
{
	if( pOwner == NULL ) return false;

	DWORDVec vMoveID;
	MoveMap::iterator iCreate;
	for( iCreate = m_MoveMap.begin() ; iCreate != m_MoveMap.end() ; ++iCreate )
	{
		MoveList &rkList = iCreate->second;

		// 내 HP 체크
		float fMinHP = pOwner->GetHP().m_fMaxValue * rkList.m_fLimitHPRateMin;
		float fMaxHP = pOwner->GetHP().m_fMaxValue * rkList.m_fLimitHPRateMax;
		if( !COMPARE( pOwner->GetHP().m_fCurValue, fMinHP, fMaxHP ) ) continue;
		
		// 거리 체크
		float fMoveRange = rkList.m_fMoveRange * rkList.m_fMoveRange;
		if( fDistSq > fMoveRange ) continue;

		vMoveID.push_back( iCreate->first );
	}

	if( !vMoveID.empty() )
	{
		int r = rand()%vMoveID.size();
		MoveMap::iterator iter = m_MoveMap.find( vMoveID[r] );
		if( iter != m_MoveMap.end() )
		{
			AttachMoveData( iter->second );
			return true;
		}
	}
	return false;
}

bool ioMoveController::IsMoveRangeCheck( float fRange )
{
	if( m_CurMove.m_Move.empty() ) return false;
	if( m_CurMove.m_fKeepRange == 0.0f ) return false; 

	float fKeepRange = m_CurMove.m_fKeepRange * m_CurMove.m_fKeepRange;
	float fMoveRange = (m_CurMove.m_fKeepRange + 100) * (m_CurMove.m_fKeepRange + 100);
	if( COMPARE( fRange, fKeepRange, fMoveRange ) )
	{
		return true;
	}

	return false;
}

bool ioMoveController::OnMoveAction( ioNpcChar *pOwner, ioBaseChar *pTarget, D3DXVECTOR3 &vMovePos, float fAngle, float fRange )
{
	if( m_CurMove.m_Move.empty() ) return false;
	if( IsMoveRangeCheck( fRange ) ) return false;

	MoveData &rkData = m_CurMove.m_Move[0];
	{
		if( pTarget )
		{
			float fKeepRange = m_CurMove.m_fKeepRange * m_CurMove.m_fKeepRange;
			if( fRange < fKeepRange )
			{
				// 반대 방향으로 이동
				fAngle += 180.0f;
			}
		}

		int iDirection = ( (int)fAngle % 360 ) / 45;
		pOwner->SetDirectionKey( iDirection, rkData.m_wKeyData );

		if( pTarget )
		{
			if( pOwner->GetState() == CS_DELAY )
			{
				pOwner->SetAutoTargetExplicit( pTarget->GetCharName(), pOwner->GetMoveDir(), 200, ATT_NORMAL, false, false );
			}
		}

		if( FRAMEGETTIME() - m_dwCurrentTime > rkData.m_dwDelayTime )
		{
			m_dwCurrentTime = FRAMEGETTIME();
			m_CurMove.m_Move.erase( m_CurMove.m_Move.begin() );
		}
	}			
	return true;
}

bool ioMoveController::OnMoveAIAction( ioNpcChar *pOwner, ioBaseChar *pTarget, D3DXVECTOR3 &vMovePos, float fAngle, float fRange )
{
	if( m_CurMove.m_Move.empty() ) return false;
	if( IsMoveRangeCheck( fRange ) ) return false;

	MoveData &rkData = m_CurMove.m_Move[0];
	{
// 		if( pTarget )
// 		{
// 			float fKeepRange = m_CurMove.m_fKeepRange * m_CurMove.m_fKeepRange;
// 			if( fRange < fKeepRange )
// 			{
// 				// 반대 방향으로 이동
// 				fAngle += 180.0f;
// 			}
// 		}

		int iDirection = ( (int)fAngle % 360 ) / 45;
		pOwner->SetDirectionKey( iDirection, rkData.m_wKeyData );

		if( FRAMEGETTIME() - m_dwCurrentTime > rkData.m_dwDelayTime )
		{
			m_dwCurrentTime = FRAMEGETTIME();
			m_CurMove.m_Move.erase( m_CurMove.m_Move.begin() );
		}
	}			
	return true;
}

bool ioMoveController::IsMove()
{
	if( m_CurMove.m_Move.empty() )
		return false;
	return true;
}