

#ifndef _ioMeshTrailDataManager_h_
#define _ioMeshTrailDataManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioVectorIterator.h"

class ioSkeleton;
class ioMeshTrailer;

class MeshTrailData
{
public:
	struct TrailPoint
	{
		D3DXVECTOR3 vStart;
		D3DXVECTOR3 vEnd;
	};

	typedef std::map< float, TrailPoint > TrailPointList;

private:
	TrailPointList m_TrailPointList;
	float m_fAniEndTime;

public:
	void AddTrailPoint( float fTime,
						const D3DXVECTOR3 &vStart,
						const D3DXVECTOR3 &vEnd );

public:
	void SetAniEndTime( float fEndTime );
	inline float GetAniEndTime() const { return m_fAniEndTime; }

	void GetTrailPointRange( float fStartTime, float fEndTime, Vector3Vec &rkList );

public:
	MeshTrailData();
	~MeshTrailData();
};

class __EX ioMeshTrailDataManager : public Singleton< ioMeshTrailDataManager >
{
protected:
	typedef std::map< ioHashString, MeshTrailData* > MeshTrailDataList;
	MeshTrailDataList m_MeshTrailDataList;

public:
	MeshTrailData* GenerateTrailData( const ioHashString &rkSkeleton,
									  const ioHashString &rkMeshName,
									  const ioMeshTrailer *pTrailer,
									  int iCtrlPointIdx,
									  float fSampleTick,
									  float fStartTime );

protected:
	ioSkeleton* LoadTrailSkeleton( const ioHashString &rkSkeleton,
								   const ioMeshTrailer *pTrailer );

public:
	void ClearAllTrailData();

public:
	static ioMeshTrailDataManager& GetSingleton();

public:
	ioMeshTrailDataManager();
	virtual ~ioMeshTrailDataManager();
};

#endif
