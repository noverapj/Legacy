

#ifndef _ioCPU_h_
#define _ioCPU_h_

struct ioVtxNrm4;
struct JointWeight;

#define _CPU_FEATURE_MMX    0x0001
#define _CPU_FEATURE_SSE    0x0002
#define _CPU_FEATURE_SSE2   0x0004
#define _CPU_FEATURE_3DNOW  0x0008
#define _CPU_FEATURE_RDTSC  0x1000

class __EX ioCPU
{
public:
	static void Init( bool bForceGeneric = false );
	static void ShotDown();

public:
	static const char* GetSIMDProcessorName();
	static int GetCPUFeature();
	static int GetCPUClockSpeed();	// MHz ¥‹¿ß
	static int GetCPUCount();

public:
	static void TransformVertsAndNormals( void *verts,
										  const int numVerts,
										  const D3DXVECTOR4 *joints,
										  const ioVtxNrm4 *base,
										  const JointWeight *weights );

	static void FastMemcpy( void *dest, const void *src, const int count );

private:
	static int  IsCPUID();
	static void CheckCPUFeature();
	static void CheckClockSpeed();
};

class ioSIMDProcessor
{
public:
	virtual void TransformVertsAndNormals( void *verts,
										   const int numVerts,
										   const D3DXVECTOR4 *joints,
										   const ioVtxNrm4 *base,
										   const JointWeight *weights ) = 0;

public:
	virtual const char* GetName() const = 0;

public:
	ioSIMDProcessor(){}
	virtual ~ioSIMDProcessor(){}
};

#endif
