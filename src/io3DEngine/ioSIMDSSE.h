

#ifndef _ioSIMDSSE_h_
#define _ioSIMDSSE_h_

class ioSIMDSSE : public ioSIMDProcessor
{
public:
	virtual void TransformVertsAndNormals( void *verts,
										   const int numVerts,
										   const D3DXVECTOR4 *joints,
										   const ioVtxNrm4 *base,
										   const JointWeight *weights );

	virtual const char* GetName() const;

public:
	ioSIMDSSE();
	virtual ~ioSIMDSSE();
};

#endif