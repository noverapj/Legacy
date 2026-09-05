

#ifndef _ioEdgeRender_h_
#define _ioEdgeRender_h_

#include "ioRenderable.h"

class ioEntityGroup;
class ioMaterial;
class ioMeshData;
class ioCamera;

class ioStream;
class ioBinaryStream;

class __EX ioEdgeRender : public ioRenderable
{
public:
	enum
	{
		EMPTY_FACE_SLOT = 0xFFFF
	};

protected:
	class Edge
	{
	private:
		WORD m_wVertexIndex[2];
		WORD m_wFaceIndex[2];

	public:
		inline void SetVertexIndex( int iCorner, WORD wIndex ) { m_wVertexIndex[iCorner] = wIndex; }
		inline void SetVertexIndices( WORD wA, WORD wB )
		{
			m_wVertexIndex[0] = wA;
			m_wVertexIndex[1] = wB;
		}

		inline void SetFaceIndex( int iCorner, WORD wIndex ) { m_wFaceIndex[iCorner] = wIndex; }
		inline void SetFaceIndices( WORD wA, WORD wB )
		{
			m_wFaceIndex[0] = wA;
			m_wFaceIndex[1] = wB;
		}

		inline WORD GetVertexIndex( int iCorner ) const	{ return m_wVertexIndex[iCorner]; }
		inline WORD GetFaceIndex( int iCorner ) const {	return m_wFaceIndex[iCorner]; }
	};

	typedef std::vector< Edge > EdgeList;

protected:
	ioEntityGroup *m_pOwner;
	ioMeshData *m_pMeshData;
	ioMaterial *m_pMaterial;
	float	m_fCreaseAngle;

	ioHashString m_FileName;
	D3DCOLORVALUE m_EdgeColor;
	int m_iEdgeThickness;

	Vector3Vec m_vVtxList;
	Vector3Vec m_vNrmList;
	Vector3Vec m_vFaceNormals;

	EdgeList m_vEdgeList;
	EdgeList m_vBoundCreaseList;

	Vector3Vec m_vCachedVtxList;
	Vector3Vec m_vCachedNormalList;

	WORD *m_pBoundCreaseIndices;
	int   m_iBoundCreaseIndexCount;

	WORD *m_pRenderEdgeIndices;
	int   m_iRenderEdgeIndexCount;

	int m_iCurIndexStart;
	int m_iPreIndexEnd;

	ResourceLoadState m_LoadState;
	bool m_bEnable;
	bool m_bUpdateBufferOK;

public:
	void SetOwner( ioEntityGroup *pOwner );
	void SetFileName( const ioHashString &rkName );
	void SetEdgeThickness( int iThickness );
	void SetCreaseAngle( float fCreaseAngle );
	void SetEdgeColor( float fRed, float fGreen, float fBlue, float fAlpha = 1.0f );
	void SetEdgeColor( const D3DCOLORVALUE &rkColor );
	
public:
	void SetLoadState( ResourceLoadState eState ) { m_LoadState = eState; }

	bool LoadEdgeFile( const char *szFullName );
	void OptimizeEdgeList();

protected:
	bool IsEdgeFile( const char *szToken );
	bool LoadFileInternal( ioBinaryStream *pStream );

public:
	inline void Enable( bool bEnable ) { m_bEnable = bEnable; }
	inline bool IsEnable() const { return m_bEnable; }

	void NotifyCurrentCamera( ioCamera *pCamera );

protected:
	bool BuildRenderingData();
	void BuildMaterial();
	void DetectEdges( const D3DXVECTOR3 &vCamPos );

public:
	virtual ioMaterial* GetMaterial() const;
	virtual float GetSquaredViewDepth( const ioCamera *pCamera );
	virtual void GetRenderOperation( ioRenderOperation &rkRenderOp );
	virtual void GetWorldTransform( D3DXMATRIX *pMatWorld ) const;

public:
	inline const ioHashString& GetFileName() const { return m_FileName; }
	inline float GetCreaseAngle() const { return m_fCreaseAngle; }
	inline const D3DCOLORVALUE& GetEdgeColor() const { return m_EdgeColor; }
	inline int GetEdgeThickness() const { return m_iEdgeThickness; }
	
	inline ResourceLoadState GetLoadState() const { return m_LoadState; }
	inline bool IsLoaded() const { return ( m_pMeshData != NULL ); }

	inline bool IsUpdateBufferOK() const { return m_bUpdateBufferOK; }

public:
	ioEdgeRender();
	virtual ~ioEdgeRender();
};

#endif