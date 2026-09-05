

#ifndef _ioSubMesh_h_
#define _ioSubMesh_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioMesh;

class __EX ioSubMesh
{
	friend class ioMesh;
private:
	ioMesh *m_pParent;
	ioHashString m_MaterialName;

public:
	int m_iMinIndex;
	int m_iVertexCount;

	int m_iIndexStart;
	int m_iFaceCount;

public:
	inline void SetMaterialName( const char *szName ) { m_MaterialName = szName; }
	inline const ioHashString& GetMaterialName() const { return m_MaterialName; }
	
	int GetWeightedBipedCnt() const;
	DWORD GetMeshHash() const;

private:		// ioMesh만이 생성/소멸 할수 있다.
	ioSubMesh( ioMesh *pParent );
	~ioSubMesh();
};

#endif
