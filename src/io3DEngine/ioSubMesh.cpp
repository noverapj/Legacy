

#include "stdafx.h"
#include "ioSubMesh.h"
#include "ioMesh.h"

ioSubMesh::ioSubMesh( ioMesh *pParent ) : m_pParent( pParent )
{
	m_iMinIndex    = 0;
	m_iVertexCount = 0;

	m_iIndexStart = 0;
	m_iFaceCount  = 0;
}

ioSubMesh::~ioSubMesh()
{
}

int ioSubMesh::GetWeightedBipedCnt() const
{
	return m_pParent->GetWeightedBipedCnt();
}

DWORD ioSubMesh::GetMeshHash() const
{
	return m_pParent->GetName().GetHashCode();
}