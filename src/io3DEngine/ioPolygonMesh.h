

#ifndef _ioPolygonMesh_h_
#define _ioPolygonMesh_h_

#include "io3DCommon.h"

class ioPolygon
{
public:
	Vector3Vec vPoint;

public:
	inline void Resize( int iSize ) { vPoint.resize( iSize ); }
	inline void AddPoint( const D3DXVECTOR3 &vPos ) { vPoint.push_back( vPos ); }
	inline void Reserve( int iSize ) { vPoint.reserve(iSize); }

	inline void Clear() { vPoint.erase( vPoint.begin(), vPoint.end() ); }
	inline bool IsEmpty() const { return vPoint.empty(); }

	inline int Size() const { return vPoint.size(); }
};

class __EX ioPolygonMesh
{
public:
	typedef std::list< ioPolygon > PolygonList;

private:
	PolygonList m_vPolygonList;

public:
	inline void AddPolygon( const ioPolygon &kPolygon ) { m_vPolygonList.push_back( kPolygon ); }
	inline void ClearPolygonList() { m_vPolygonList.clear(); }
	inline void PopBack() { m_vPolygonList.pop_back(); }

	void SwapPolygonWithLast( int iPoly );

public:
	inline const ioPolygon& GetBack() const { return m_vPolygonList.back(); }
	inline int GetNumPolygon() const { return m_vPolygonList.size(); }

	const ioPolygon& GetPolygon( int iIndex ) const;
	void GetAllVtxList( Vector3Vec &vVtxList );

public:
	ioPolygonMesh();
	ioPolygonMesh( const ioPolygonMesh &kPolyMesh );
	~ioPolygonMesh();
};

#endif