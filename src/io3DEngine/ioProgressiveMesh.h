

#ifndef _ioProgressiveMesh_h_
#define _ioProgressiveMesh_h_

class __EX ioProgressiveMesh
{
public:
	enum VertexReduceMethod
	{
		VRM_CONSTANT,
		VRM_RATE,
	};

	struct LODUsage 
	{
		WORD *pIndex;
		int  iIndexCount;
	};
	typedef std::vector< LODUsage > LODUsageList;

protected:
	class PMTriangle;
	class PMVertex;

public:
	class PMFaceVertex
	{
	public:
		PMVertex *m_pCommonVertex;
		int m_iRealIndex;
	};

protected:
	class PMTriangle
	{
	public:
		PMFaceVertex *m_pVertex[3];
		D3DXVECTOR3 m_vNormal;
		int	 m_iIndex;
		bool m_bRemoved;

	public:
		void SetDetails( int iIndex, PMFaceVertex *v0, PMFaceVertex *v1, PMFaceVertex *v2 );
		void ReplaceVertex( PMFaceVertex *pOld, PMFaceVertex *pNew );
		void ComputeNormal();
		void NotifyRemoved();

	public:
		bool HasCommonVertex( PMVertex *v ) const;
		bool HasFaceVertex( PMFaceVertex *v ) const;
		PMFaceVertex* GetFaceVertexFromCommon( PMVertex *pCommonVert );

	public:
		PMTriangle();
		~PMTriangle();
	};

	class PMVertex
	{
	public:
		typedef std::set< PMVertex* > NeighborList;
		typedef std::set< PMTriangle* > FaceList;

	public:
		D3DXVECTOR3 m_vPosition;
		int m_iIndex;

		NeighborList m_Neighbors;
		FaceList m_Face;

		float m_fCollapseCost;
		PMVertex *m_pCollapseTo;

		bool m_bRemoved;
		bool m_bToBeRemoved;
		bool m_bSeam;

	public:
		void SetDetails( const D3DXVECTOR3 &vPos, int iIndex );
		void RemoveIfNonNeighbor( PMVertex *pVert );
		void NotifyRemoved();

	public:
		bool IsBorder();
		bool IsManifoldEdgeWith( PMVertex* pVert );

	public:
		PMVertex();
		~PMVertex();
	};

protected:
	typedef std::vector<PMTriangle> TriangleList;
	typedef std::vector<PMFaceVertex> FaceVertexList;
	typedef std::vector<PMVertex> CommonVertexList;
	typedef std::vector<float> WorstCostList;

	struct PMWorkingData
	{
		TriangleList m_TriList;
		FaceVertexList m_FaceVertList;
		CommonVertexList m_VertList;
	};

protected:
	const D3DXVECTOR3 *m_pVertex;
	int m_iVertexCount;

	const WORD *m_pIndex;
	int m_iIndexCount;
	int m_iIndexOffset;

	int m_iNumCommonVertices;
	int m_iCurNumIndices;

	PMWorkingData m_WorkingData;
	WorstCostList m_WorstCosts;

public:
	void SetMeshInfo( const D3DXVECTOR3 *pVertex,
					  int iVertexCount,
					  const WORD *pIndex,
					  int iIndexCount,
					  int iIndexOffset );

	void Build( VertexReduceMethod eMethod, float fReduceValue, LODUsage &rkResult );

protected:
	void InitialiseEdgeCollapseCosts();
	void BuildWorkingData();

	float ComputeEdgeCollapseCost( PMVertex *pSrc, PMVertex *pDest );
	float ComputeEdgeCostAtVertexForBuffer( int iVert );
	void  ComputeEdgeCostAtVertex( int iVert );
	void  ComputeAllCosts();

	void BakeNewLOD( WORD *pIndexData );
	void Collapse( PMVertex *pSrc );
	int  GetNextCollapser();

public:
	ioProgressiveMesh();
	virtual ~ioProgressiveMesh();
};

#endif
