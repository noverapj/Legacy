

#include "stdafx.h"

#include "ioCPU.h"
#include "ioSIMDGeneric.h"
#include "ioSIMDSSE.h"

#include "CPUSpeedCheck.h"
#include "FastAssemFunc.h"

// These are the bit flags that get set on calling cpuid
// with register eax set to 1

#define _RDTSC_FEATURE_BIT		0x00000010
#define _MMX_FEATURE_BIT        0x00800000
#define _SSE_FEATURE_BIT        0x02000000
#define _SSE2_FEATURE_BIT       0x04000000

// This bit is set when cpuid is called with
// register set to 80000001h (only applicable to AMD)
#define _3DNOW_FEATURE_BIT      0x80000000

int iCPUFeature = 0;
int iCPUFamily  = 0;
int iCPUClockSpeed = 0;
int iCPUCount = 1;

ioSIMDProcessor *pSimdProcessor = NULL;

int ioCPU::IsCPUID()
{
    __try {
        _asm {
            xor eax, eax
            cpuid
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
    return 1;
}

void ioCPU::CheckCPUFeature()
{
    DWORD dwStandard = 0;
    DWORD dwFeature = 0;
    DWORD dwMax = 0;
    DWORD dwExt = 0;
	
    union 
	{
        char cBuf[12+1];
        struct 
		{
            DWORD dw0;
            DWORD dw1;
            DWORD dw2;
        }s;
    }Ident;

    if (!IsCPUID())
		return;

    _asm {
        push ebx
        push ecx
        push edx

        // get the vendor string
        xor eax, eax
        cpuid
        mov dwMax, eax
        mov Ident.s.dw0, ebx
        mov Ident.s.dw1, edx
        mov Ident.s.dw2, ecx

        // get the Standard bits
        mov eax, 1
        cpuid
        mov dwStandard, eax
        mov dwFeature, edx

        // get AMD-specials
        mov eax, 80000000h
        cpuid
        cmp eax, 80000000h
        jc notamd
        mov eax, 80000001h
        cpuid
        mov dwExt, edx

notamd:
        pop ecx
        pop ebx
        pop edx
    }

	iCPUFeature = 0;
	if( dwFeature & _RDTSC_FEATURE_BIT )
		iCPUFeature |= _CPU_FEATURE_RDTSC;

    if( dwFeature & _MMX_FEATURE_BIT )
        iCPUFeature |= _CPU_FEATURE_MMX;

    if( dwExt & _3DNOW_FEATURE_BIT )
        iCPUFeature |= _CPU_FEATURE_3DNOW;

    if( dwFeature & _SSE_FEATURE_BIT )
        iCPUFeature |= _CPU_FEATURE_SSE;

	if( dwFeature & _SSE2_FEATURE_BIT ) 
        iCPUFeature |= _CPU_FEATURE_SSE2;

	iCPUFamily = ( dwStandard >> 8 ) & 0xF;
}

void ioCPU::Init( bool bForceGeneric )
{
	if( pSimdProcessor )
		return;

	CheckCPUFeature();
	CheckClockSpeed();

	SYSTEM_INFO kSystemInfo;
	GetSystemInfo( &kSystemInfo );
	iCPUCount = kSystemInfo.dwNumberOfProcessors;

	if( bForceGeneric )
	{
		pSimdProcessor = new ioSIMDGeneric;
	}
	else
	{
		if( iCPUFeature & _CPU_FEATURE_SSE )
		{
			pSimdProcessor = new ioSIMDSSE;
		}
		else
		{
			pSimdProcessor = new ioSIMDGeneric;
		}
	}
}

void ioCPU::CheckClockSpeed()
{
	if( iCPUFeature & _CPU_FEATURE_RDTSC )
	{
		iCPUClockSpeed = CheckCPUClockSpeed();
	}
	else
	{
		iCPUClockSpeed = CheckClassicalCPUClockSpeed( iCPUFamily );
	}
}

void ioCPU::ShotDown()
{
	SAFEDELETE( pSimdProcessor );
}

const char* ioCPU::GetSIMDProcessorName()
{
	if( pSimdProcessor )
		pSimdProcessor->GetName();

	return "Not Initialized";
}

int ioCPU::GetCPUFeature()
{
	return iCPUFeature;
}

int ioCPU::GetCPUClockSpeed()
{
	return iCPUClockSpeed;
}

int ioCPU::GetCPUCount()
{
	return iCPUCount;
}

void ioCPU::TransformVertsAndNormals( void *verts,
									  const int numVerts,
									  const D3DXVECTOR4 *joints,
									  const ioVtxNrm4 *base,
									  const JointWeight *weights )
{
	if( pSimdProcessor )
	{
		pSimdProcessor->TransformVertsAndNormals( verts, numVerts, joints, base, weights );
	}
}

void ioCPU::FastMemcpy( void *dest, const void *src, const int count )
{
	if( iCPUFeature & _CPU_FEATURE_SSE )
		SSEMemcpy( dest, src, count );
	else if( iCPUFeature & _CPU_FEATURE_MMX )
		MMXMemcpy( dest, src, count );
	else
		memcpy( dest, src, count );
}
