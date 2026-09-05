

#include "stdafx.h"

#include "ioMeshTrailDataManager.h"

#include "ioBiped.h"
#include "ioSkeleton.h"
#include "ioAnimation.h"
#include "ioAniTrack.h"
#include "ioMath.h"
#include "ioAnimationManager.h"
#include "ioSkeletonManager.h"

#include "ioMeshTrailer.h"

MeshTrailData::MeshTrailData()
{
	m_fAniEndTime = 1.0f;
}

MeshTrailData::~MeshTrailData()
{
}

void MeshTrailData::AddTrailPoint( float fTime,
								   const D3DXVECTOR3 &vStart,
								   const D3DXVECTOR3 &vEnd )
{
	TrailPoint kPoint;
	kPoint.vStart = vStart;
	kPoint.vEnd = vEnd;

	m_TrailPointList.insert( TrailPointList::value_type( fTime, kPoint ) );
}

void MeshTrailData::SetAniEndTime( float fEndTime )
{
	m_fAniEndTime = fEndTime;
}

void MeshTrailData::GetTrailPointRange( float fStartTime, float fEndTime, Vector3Vec &rkList )
{
	TrailPointList::iterator iStart = m_TrailPointList.upper_bound( fStartTime );
	if( iStart == m_TrailPointList.begin() )
	{
		rkList.push_back( iStart->second.vStart );
		rkList.push_back( iStart->second.vEnd );
	}
	else if( iStart == m_TrailPointList.end() )
	{
		--iStart;
		rkList.push_back( iStart->second.vStart );
		rkList.push_back( iStart->second.vEnd );
		return;
	}
	else
	{
		TrailPointList::iterator iBefore = iStart;
		--iBefore;

		float fBlend;
		fBlend = fStartTime - iBefore->first;
		fBlend /= iStart->first - iBefore->first;

		D3DXVECTOR3 vLerpPos;
		
		vLerpPos = ioMath::Lerp( fBlend, iBefore->second.vStart, iStart->second.vStart );
		rkList.push_back( vLerpPos );

		vLerpPos = ioMath::Lerp( fBlend, iBefore->second.vEnd, iStart->second.vEnd );
		rkList.push_back( vLerpPos );
	}

	TrailPointList::iterator iEnd = m_TrailPointList.upper_bound( fEndTime );
	for( TrailPointList::iterator iter=iStart ; iter!=iEnd ; ++iter )
	{
		rkList.push_back( iter->second.vStart );
		rkList.push_back( iter->second.vEnd );
	}

	if( iEnd != m_TrailPointList.begin() && iEnd != m_TrailPointList.end() )
	{
		TrailPointList::iterator iBefore = iEnd;
		--iBefore;

		float fBlend;
		fBlend = fEndTime - iBefore->first;
		fBlend /= iEnd->first - iBefore->first;

		D3DXVECTOR3 vLerpPos;
		vLerpPos = ioMath::Lerp( fBlend, iBefore->second.vStart, iEnd->second.vStart );
		rkList.push_back( vLerpPos );

		vLerpPos = ioMath::Lerp( fBlend, iBefore->second.vEnd, iEnd->second.vEnd );
		rkList.push_back( vLerpPos );
	}
}

//--------------------------------------------------------------------------------------------------

template<> ioMeshTrailDataManager* Singleton< ioMeshTrailDataManager >::ms_Singleton = 0;

ioMeshTrailDataManager::ioMeshTrailDataManager()
{
}

ioMeshTrailDataManager::~ioMeshTrailDataManager()
{
	ClearAllTrailData();
}

void ioMeshTrailDataManager::ClearAllTrailData()
{
	MeshTrailDataList::iterator iter;
	for( iter=m_MeshTrailDataList.begin() ; iter!=m_MeshTrailDataList.end() ; ++iter )
	{
		delete iter->second;
	}
	m_MeshTrailDataList.clear();
}

MeshTrailData* ioMeshTrailDataManager::GenerateTrailData( const ioHashString &rkSkeleton,
														  const ioHashString &rkMeshName,
														  const ioMeshTrailer *pTrailer,
														  int iCtrlPointIdx,
														  float fSampleTick,
														  float fStartTime )
{
	char szBuf[MAX_PATH];
	wsprintf( szBuf, "%s_%s_%s_%d", rkMeshName.c_str(),
									pTrailer->GetTrailAniName().c_str(),
									rkSkeleton.c_str(),								 
									iCtrlPointIdx );

	ioHashString szDataName( szBuf );

	MeshTrailDataList::iterator iter = m_MeshTrailDataList.find( szDataName );
	if( iter != m_MeshTrailDataList.end() )
		return iter->second;

	ioSkeleton *pSkeleton = LoadTrailSkeleton( rkSkeleton, pTrailer );
	if( !pSkeleton )	return NULL;

	ioAnimation *pTrailAni = g_AnimationMgr.CreateInstance( pTrailer->GetTrailAniName() );
	if( !pTrailAni )
	{
		SAFEDELETE( pSkeleton );
		return NULL;
	}

	pTrailAni->BuildAniTrackList( pSkeleton );

	MeshTrailData *pNewData = new MeshTrailData;
	m_MeshTrailDataList.insert( MeshTrailDataList::value_type( szDataName, pNewData ) );

	ioBiped *pStartBiped = pSkeleton->GetBiped( pTrailer->GetStartBiped() );
	ioBiped *pEndBiped = pSkeleton->GetBiped( pTrailer->GetEndBiped() );

	D3DXVECTOR3 vStartPoint, vEndPoint;
	vStartPoint = pTrailer->GetStartPoint();
	vEndPoint   = pTrailer->GetEndPoint();

	D3DXVECTOR3 vTrans;
	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vCurStart, vCurEnd;

	float fFullTime = pTrailAni->GetFullTime();
	for( float fCurTime = fStartTime; fCurTime<fFullTime ; fCurTime += fSampleTick )
	{
		pSkeleton->ClearBlend();

		int iTotalTrack = pTrailAni->GetTrackCount();
		for( int i=0 ; i<iTotalTrack ; i++ )
		{
			ioAniTrack *pTrack = pTrailAni->GetAniTrack( i );
			if( pTrack )
			{
				pTrack->GetInterpolatedFrame( fCurTime, vTrans, qtRot );
				
				pSkeleton->BlendBiped( pTrack->GetBipedID(),
									   pTrack->GetWeight(),
									   vTrans,
									   qtRot );

			}
		}

		pSkeleton->LockBlend();
		pSkeleton->UpdateSkeleton( fSampleTick );

		D3DXVec3TransformCoord( &vCurStart, &vStartPoint, pStartBiped->GetBipedSpaceFullXform() );
		D3DXVec3TransformCoord( &vCurEnd, &vEndPoint, pEndBiped->GetBipedSpaceFullXform() );

		pNewData->AddTrailPoint( fCurTime, vCurStart, vCurEnd );
	}

	pNewData->SetAniEndTime( pTrailAni->GetFullTime() );

	SAFEDELETE( pSkeleton );
	SAFEDELETE( pTrailAni );

	return pNewData;
}

ioSkeleton* ioMeshTrailDataManager::LoadTrailSkeleton( const ioHashString &rkSkeleton,
													   const ioMeshTrailer *pTrailer )
{
	ioSkeleton *pSkeleton = ioSkeletonManager::GetSingleton().CreateInstance( rkSkeleton );
	if( !pSkeleton )	return NULL;

	if( !pSkeleton->GetBiped( pTrailer->GetStartBiped() ) )
	{
		delete pSkeleton;
		return NULL;
	}

	if( !pSkeleton->GetBiped( pTrailer->GetEndBiped() ) )
	{
		delete pSkeleton;
		return NULL;
	}

	return pSkeleton;
}

ioMeshTrailDataManager& ioMeshTrailDataManager::GetSingleton()
{
	return Singleton<ioMeshTrailDataManager>::GetSingleton();
}