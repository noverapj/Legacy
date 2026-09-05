

#include "stdafx.h"
#include "ioCPU.h"
#include "ioSIMDSSE.h"

#define SHUFFLE_PS( x, y, z, w )	(( (x) & 3 ) << 6 | ( (y) & 3 ) << 4 | ( (z) & 3 ) << 2 | ( (w) & 3 ))
#define R_SHUFFLE_PS( x, y, z, w )	(( (w) & 3 ) << 6 | ( (z) & 3 ) << 4 | ( (y) & 3 ) << 2 | ( (x) & 3 ))

#define SHUFFLE_PD( x, y )			(( (x) & 1 ) << 1 | ( (y) & 1 ))
#define R_SHUFFLE_PD( x, y )		(( (y) & 1 ) << 1 | ( (x) & 1 ))

#define SHUFFLE_D( x, y, z, w )		(( (x) & 3 ) << 6 | ( (y) & 3 ) << 4 | ( (z) & 3 ) << 2 | ( (w) & 3 ))
#define R_SHUFFLE_D( x, y, z, w )	(( (w) & 3 ) << 6 | ( (z) & 3 ) << 4 | ( (y) & 3 ) << 2 | ( (x) & 3 ))

#define assert_16_byte_aligned( pointer ) assert( (((UINT_PTR)(pointer))&15) == 0 );


// offsets for SIMD code
#define BASEVECTOR_SIZE (4*4) // sizeof( D3DXVECTOR4 )

#define JOINTWEIGHT_SIZE (3*4) // sizeof( JointWeight )
#define JOINTWEIGHT_WEIGHT_OFFSET (0*4) // offsetof( JointWeight, weight )
#define JOINTWEIGHT_JOINTMATOFFSET_OFFSET (1*4) // offsetof( JointWeight, jointMatOffset )
#define JOINTWEIGHT_NEXTVERTEXOFFSET_OFFSET (2*4) // offsetof( JointWeight, nextVertexOffset )

#define VERTEX_SIZE (4*3*2) // sizeof( ioVtxNrm )
#define VERTEX_POSITION_OFFSET (0*4) // offsetof( ioVtxNrm.vPos )
#define VERTEX_NORMAL_OFFSET (3*4) // offsetof( ioVtxNrm.vNrm )

ioSIMDSSE::ioSIMDSSE()
{
}

ioSIMDSSE::~ioSIMDSSE()
{

}

const char* ioSIMDSSE::GetName() const
{
	return "SSE";
}

void ioSIMDSSE::TransformVertsAndNormals( void *verts,
										  const int numVerts,
										  const D3DXVECTOR4 *joints,
										  const ioVtxNrm4 *base,
										  const JointWeight *weights )
{
	assert_16_byte_aligned( joints );
	assert_16_byte_aligned( base );

	__asm
	{
		mov eax, numVerts
		test eax, eax
		jz done

		imul eax, VERTEX_SIZE
		mov ecx, verts
		mov edx, weights
		mov esi, base
		mov edi, joints
		add ecx, eax
		neg eax

	// accumulate weighted matrix
	// accumulated  = matrix0 * weight0;
	// accumulated += matrix1 * weight1;
	// accumulated += matrix2 * weight2;
	// xmm0 = m0, m1, m2, t0
	// xmm1 = m3, m4, m5, t1
	// xmm2 = m6, m7, m8, t2
	loopVert:
		movss xmm0, [edx+JOINTWEIGHT_WEIGHT_OFFSET]
		mov ebx, dword ptr [edx+JOINTWEIGHT_JOINTMATOFFSET_OFFSET]
		shufps xmm0, xmm0, R_SHUFFLE_PS( 0, 0, 0, 0 )
		add edx, JOINTWEIGHT_SIZE
		movaps xmm1, xmm0
		add esi, 2*BASEVECTOR_SIZE
		movaps xmm2, xmm0
		cmp dword ptr [edx-JOINTWEIGHT_SIZE+JOINTWEIGHT_NEXTVERTEXOFFSET_OFFSET], JOINTWEIGHT_SIZE
		mulps xmm0, [edi+ebx+ 0] // xmm0 = m0, m1, m2, t0
		mulps xmm1, [edi+ebx+16] // xmm1 = m3, m4, m5, t1
		mulps xmm2, [edi+ebx+32] // xmm2 = m6, m7, m8, t2
		je doneWeight
	
	loopWeight:
		movss xmm3, [edx+JOINTWEIGHT_WEIGHT_OFFSET]
		mov ebx, dword ptr [edx+JOINTWEIGHT_JOINTMATOFFSET_OFFSET]
		shufps xmm3, xmm3, R_SHUFFLE_PS( 0, 0, 0, 0 )
		add edx, JOINTWEIGHT_SIZE
		movaps xmm4, xmm3
		movaps xmm5, xmm3
		mulps xmm3, [edi+ebx+ 0] // xmm3 = m0, m1, m2, t0
		mulps xmm4, [edi+ebx+16] // xmm4 = m3, m4, m5, t1
		mulps xmm5, [edi+ebx+32] // xmm5 = m6, m7, m8, t2
		cmp dword ptr [edx-JOINTWEIGHT_SIZE+JOINTWEIGHT_NEXTVERTEXOFFSET_OFFSET], JOINTWEIGHT_SIZE
		addps xmm0, xmm3
		addps xmm1, xmm4
		addps xmm2, xmm5
		jne loopWeight

	doneWeight:	// accumulate weighted matrix end

		add eax, VERTEX_SIZE

		// transform vertex
		movaps xmm3, [esi-2*BASEVECTOR_SIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3		 // xmm6 = m0, m1, m2, t0
		unpcklps xmm6, xmm4		 // xmm6 = m0, m3, m1, m4
		unpckhps xmm3, xmm4		 // xmm3 = m2, m5, t0, t1
		addps xmm6, xmm3		 // xmm6 = m0+m2, m3+m5, m1+t0, m4+t1

		movaps xmm7, xmm5		 // xmm7 = m6, m7, m8, t2
		movlhps xmm5, xmm6		 // xmm5 = m6, m7, m0+m2, m3+m5
		movhlps xmm6, xmm7		 // xmm6 = m8, t2, m1+t0, m4+t1
		addps xmm6, xmm5		 // xmm6 = m6+m8, m7+t2, m0+m1+m2+t0, m3+m4+m5+t1

		// copy x, y
		movhps [ecx+eax-VERTEX_SIZE+VERTEX_POSITION_OFFSET+0], xmm6

		movaps xmm7, xmm6
		shufps xmm7, xmm7, R_SHUFFLE_PS( 1, 0, 2, 3 )	 // xmm7 = m7+t2, m6+m8
		addss xmm7, xmm6 // xmm7 = m6+m8+m7+t2

		// copy z
		movss [ecx+eax-VERTEX_SIZE+VERTEX_POSITION_OFFSET+8], xmm7

		// transform normal
		movaps xmm3, [esi-1*BASEVECTOR_SIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0		// nrm.x * _11, nrm.y * _21, nrm.z * _31, nrm.w * _41
		mulps xmm4, xmm1		// nrm.x * _12, nrm.y * _22, nrm.z * _32, nrm.w * _42
		mulps xmm5, xmm2		// nrm.x * _13, nrm.y * _23, nrm.z * _33, nrm.w * _43

		movaps xmm6, xmm3		// xmm6 = m0, m1, m2, t0
		unpcklps xmm6, xmm4		// xmm6 = m0, m3, m1, m4
		unpckhps xmm3, xmm4		// xmm3 = m2, m5, t0, t1
		addps xmm6, xmm3		// xmm6 = m0+m2, m3+m5, m1+t0, m4+t1

		movaps xmm7, xmm5		// xmm7 = m6, m7, m8, t2
		movlhps xmm5, xmm6		// xmm5 = m6, m7, m0+m2, m3+m5
		movhlps xmm6, xmm7		// xmm6 = m8, t2, m1+t0, m4+t1
		addps xmm6, xmm5		// xmm6 = m6+m8, m7+t2, vNrm.x, vNrm.y

		movaps xmm7, xmm6
		shufps xmm7, xmm7, R_SHUFFLE_PS( 1, 0, 2, 3 ) // xmm7 = m7+t2, m6+m8, vNrm.x, vNrm.y
		addss xmm7, xmm6		// xmm7 = vNrm.z, m6+m8, vNrm.x, vNrm.y

		// normalize
		movaps xmm5, xmm7		// xmm5 = vNrm.z, xx, vNrm.x, vNrm.y
		mulps xmm7, xmm7		// xmm7 = z*z, xx, x*x, y*y

		movhlps xmm3, xmm7 // xmm3 = x*x, y*y, xx, xx

		movaps xmm4, xmm7
		shufps xmm4, xmm4, R_SHUFFLE_PS( 3, 2, 1, 0 )	// xmm4 = y*y, x*x, xx, z*z

		addps xmm3, xmm7	// xmm3 = x*x + z*z, xx, xx, xx
		addps xmm3, xmm4	// xmm3 = x*x + y*y + z*z, xx, xx, xx

		shufps xmm3, xmm3, R_SHUFFLE_PS( 0, 0, 0, 0 )
		rsqrtps xmm3, xmm3

		mulps xmm5, xmm3

		movhps [ecx+eax-VERTEX_SIZE+VERTEX_NORMAL_OFFSET+0], xmm5
		movss  [ecx+eax-VERTEX_SIZE+VERTEX_NORMAL_OFFSET+8], xmm5

		jl loopVert

	done:
	}
}
