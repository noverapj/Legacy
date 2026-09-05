

#ifndef _ioOpcodeManagerImpl_h_
#define _ioOpcodeManagerImpl_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioOpcodeManager;
class ioOpcodeShape;

class ioStream;
class ioBinaryStream;

class ioOpcodeManagerImpl : public ioOpcodeManager, public Singleton< ioOpcodeManagerImpl >
{
private:
	D3DXVECTOR3	*m_pVertex;
	WORD		*m_pFace;

	int	m_iVertexCnt;
	int m_iFaceCnt;

protected:
	virtual ioResource* CreateImpl( const ioHashString &name, bool bThread );
	virtual void ReLoadImpl( const ioHashString &name );

public:
	virtual ioOpcodeShape* CreateShape( const ioHashString &name );
	virtual ioOpcodeShape* GetShapeByName( const ioHashString &name );

protected:
	bool LoadFile( const ioHashString &name, ioOpcodeShape *pShape );
	bool LoadFileInternal( ioOpcodeShape *pShape, ioBinaryStream *pStream );

	bool IsOpcodeShapeFile( const char *szToken );
	bool IsNormalMeshFile( const char *szToken );
	bool LoadOpcodeShapeMesh( ioBinaryStream *pStream );
	bool LoadOpcodeShapeNormalMesh( ioBinaryStream *pStream );

	void BuildOpcodeShape( ioOpcodeShape *pShape );
	void ClearLoadHelper();

public:
	static ioOpcodeManagerImpl& GetSingleton();

public:
	ioOpcodeManagerImpl();
	virtual ~ioOpcodeManagerImpl();
};

#endif

