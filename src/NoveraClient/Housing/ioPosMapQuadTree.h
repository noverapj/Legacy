#pragma once
class ioPlayStage;

class ioPosMapQuadTree
{
public:
	enum FaceType
	{
		FT_1_4	= 0,
		FT_2_4	= 1,
		FT_3_4	= 2,
		FT_4_4	= 3,
		FT_MAX	= 4,
		FT_NONE	= 5,
	};

	struct FacePos
	{
		int m_XPos;
		int m_ZPos;

		FacePos()
		{
			m_XPos = m_ZPos = -1;
		}
		FacePos( int XPos, int ZPos )
		{
			m_XPos = XPos;
			m_ZPos = ZPos;
		}
	};

protected:
	ioPosMapQuadTree*	m_pChild[FT_MAX];
	FacePos				m_FacePos;
	int					m_nSquared;
	int					m_nTileMax;

	D3DXQUATERNION		m_CollRot;
	ioOrientBox			m_CollBox;
	D3DXVECTOR3			m_vWorldPos;

public:
	void Build( const D3DXVECTOR3& vPos, int nFaceType, int nFaceMax, int nSquared, int nBlockMax );
	void AddChild( FaceType eType, const D3DXVECTOR3& vPos, int nFaceMax, ioPosMapQuadTree* pParents, int nSquared );

public:
	void SetCollBoxRotaion( const D3DXQUATERNION& qtRot );
	void UpdateCollBox( int nSquared );
	bool TestIntersection( const ioRay& kYRay );

	bool FindBlockIndex( ioRay& kYRay, int& XPos, int& ZPos );
	bool FindBlockIndexList( const ioRay& kYRay, IntVec& vList );

public:
	bool IsRoot();
	bool IsLeaf();

public:
	void RenderCollision( ioPlayStage* pStage, ioCamera* pCamera );

public:
	ioPosMapQuadTree();
	~ioPosMapQuadTree();
};