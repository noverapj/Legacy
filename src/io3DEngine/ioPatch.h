

#ifndef _ioPatch_h_
#define _ioPatch_h_

class ioRay;
class ioLandScape;
class ioPlane;
class ioAxisAlignBox;
class ioSphere;
class ioMovableObject;

#define D3DFVF_PATCHVERTEX D3DFVF_XYZ

typedef struct tagError
{
	D3DXVECTOR3	vCorrect;
	D3DXVECTOR3 vReal;
	float		fDiff;

	tagError()
	{
		fDiff = 0.0f;
	}
}Errored;;

class __EX ioPatch
{
public:
	enum VISFLAGS
	{
		VIS_BASE = 1,
		VIS_LAYER_MAX = 16,
		VIS_LAYERALL = 0xffff,
	};

	enum
	{
		MAX_SUBDIV = 4,
		EMPTY_LEVEL = MAX_SUBDIV + 1,
		SOURCE_WIDTH = 17,
		SOURCE_HEIGHT = 17,
		MAX_VERTICES = SOURCE_WIDTH * SOURCE_HEIGHT,
		MAX_INDICES = MAX_VERTICES*3
	};

protected:
	ioLandScape	*m_pLandScape;

	ioAxisAlignBox m_WorldAABB;
	ioSphere	   m_WorldSphere;

	int		m_iGridX, m_iGridY;
	int		m_iHMapX, m_iHMapY;

	D3DXVECTOR3	*m_pVertex;
	D3DXVECTOR3 *m_pNormal;
	WORD		*m_pIndex;
	WORD		*m_pIndexMap;

	int		m_iCurVertex;
	int		m_iCurIndex;

	DWORD	m_dwVisibleLayers;

	D3DXVECTOR3		m_vPosition;
	D3DXVECTOR3		m_vScale;

	int		m_iNewSelfLv, m_iSelfLv, m_iOldRealSelfLv, m_iNewRealSelfLv, m_iRealSelfLv;
	int		m_iLeftLv, m_iRightLv, m_iBottomLv, m_iTopLv;

	Errored	m_Error[ MAX_SUBDIV+2 ];	// lv0 + 1 ~ 5 + lv6(null)
	float	m_fCurError;

	const ioPatch	*m_pLeft;
	const ioPatch	*m_pRight;
	const ioPatch	*m_pBottom;
	const ioPatch	*m_pTop;

	ioPatch	*m_pPrev;
	ioPatch *m_pNext;
	
	float	m_fCameraDistSq;

	bool	m_bVisible;
	bool	m_bInited;
	bool	m_bActiveFlag;
	bool	m_bForceBufferCreate;
	bool	m_bNotLOD;

public:
	virtual void Init();
	virtual void DeInit();

public:
	void SetVisible( bool bVisible );
	void UpdateCameraDistanceSq( const D3DXVECTOR3 &vCameraPos );

	inline bool IsVisible() const { return m_bVisible; }
	inline void SetPosition( const D3DXVECTOR3 &vPos ) { m_vPosition = vPos; }
	inline const ioAxisAlignBox& GetWorldAABB() const { return m_WorldAABB; }
	inline const ioSphere& GetWorldSphere() const { return m_WorldSphere; }
	inline float GetCameraDistanceSq() const { return m_fCameraDistSq; }

public:
	inline bool IsInited() const { return m_bInited; }

	inline void SetScale( const D3DXVECTOR3 &vScale ){ m_vScale = vScale; }
	inline void SetActiveFlag( bool bActive ) { m_bActiveFlag = bActive; }
	inline void SetVisibleLayersFlag( DWORD dwFlags ) { m_dwVisibleLayers = dwFlags; }
	inline void SetNotLOD( bool bNotLOD ) { m_bNotLOD = bNotLOD; }

	inline bool GetActiveFlag() const { return m_bActiveFlag; }
	inline DWORD GetVisibleLayersFlag() const { return m_dwVisibleLayers; }
	inline bool IsNotLOD() const { return m_bNotLOD; }

public:
	void UpdateProjectionError( const D3DXMATRIX *pMat );
	float GetProjectedError( int iLevel ) const { return m_Error[iLevel].fDiff; }

public:
	virtual bool UpdateLevel();
	virtual bool UpdateLevelSecond();
	virtual bool UpdateLevelThird();
	virtual int FillVertices( D3DXVECTOR3 *pBuf );
	int FillNormalVertices( D3DXVECTOR3 *pBuf );

	void CreateLevel( int iCenterLv, int iLeftLv, int iRightLv, int iBottomLv, int iTopLv );

	void SetRealLevel( int iLevel );
	void SetLevel( int iLevel ) { m_iNewSelfLv = iLevel; }

	inline int GetCurrentLevel() const { return m_iSelfLv; }
	inline int GetNewLevel() const { return m_iNewSelfLv; }

	int GetNewLevelSafe() const;
	float GetExactLevel() const;

	inline int GetRealLevel() const { return m_iRealSelfLv; }

	inline int GetNumVertex() const { return m_iCurVertex; }
	inline int GetNumIndex() const { return m_iCurIndex; }
	inline const D3DXVECTOR3* GetVertexP() const { return m_pVertex; }
	inline const WORD* GetIndexP() const { return m_pIndex; }
	inline int GetGridX() const { return m_iGridX; }
	inline int GetGridY() const { return m_iGridY; }

public:
	void SetNeighbor( const ioPatch *pLeft,
					  const ioPatch *pRight,
					  const ioPatch *pBottom,
					  const ioPatch *pTop );

	void SetHeightMapPos( int nX, int nY );
	void CalculateCollisionObject();
	void CalErrors();
	void CalError( int iLevel );

	inline float GetCurError() const { return m_fCurError; }
	inline const ioPatch* GetLeft() const { return m_pLeft; }
	inline const ioPatch* GetRight() const { return m_pRight; }
	inline const ioPatch* GetBottom() const { return m_pBottom; }
	inline const ioPatch* GetTop() const { return m_pTop; }
	inline class ioPatch* GetNextActive()  {  return m_pNext;  }

	inline int GetPowerTwo( int nVal ) { return 1<<nVal; }

	WORD GetIndex( int nX, int nY );
	float GetHeight( int nX, int nY ) const;
	void GetVertex( int nX, int nY, D3DXVECTOR3& vPos ) const;
	void GetNormal( int nX, int nY, D3DXVECTOR3& vNrm ) const;

protected:
/*
  HARDCODE: 링크드 리스트를 만들어 쓰는 이유
  1. 일렬로된 순서가 중요하다. 위에서부터 아래로 훑어내려가야함( 그래서 std::set을 안씀 )
  2. std::list를 쓸 경우 remove시에 search를 해야한다.
*/
	void AddToActiveList();
	void ReMoveFromActiveList();

	void AddIndex( WORD wIdx );
	void AddLastIndexAgain();

	void AddLevel4PatchTriangles( bool bLeft, bool bRight, bool bBottom, bool bTop );
	void AddTriangleRow( int nY, int nPow, bool bLeft, bool bRight, bool bBottom, bool bTop );
	void AddTriangleTopRow( int nPow, bool bLeft, bool bRight );
	void AddTriangleBottomRow( int nPow, bool bLeft, bool bRight );

	void ReduceShapeTo( int iLevel, D3DXVECTOR3 *pVtx );
	void MakeBordersSimpler( D3DXVECTOR3 *pVtx );
	void MakeSimpler( int iLevel, D3DXVECTOR3 *pVtx );
	void CalculateInbetweenVertex( int x, int y, int xc, int yc, int pow, D3DXVECTOR3 *pVtx );
	int CalculateDiagonalVertices( int x, int y, int xc, int yc,
		                           int pow,
								   D3DXVECTOR3 *pVtx,
								   float &fHeightLeftTop, int &idxLT,
								   float &fHeightRightBottom, int &idxRB );	

public:
	ioPatch( ioLandScape *pLand, int nX, int nY );
	virtual ~ioPatch();
};

#endif
