

#include "stdafx.h"
#include "CPUSpeedCheck.h"

#define CLASSICAL_CPU_FREQ_LOOP		10000000
#define RDTSC_INSTRUCTION			_asm _emit 0x0f _asm _emit 0x31
#define	CPUSPEED_I32TO64(x, y)		(((__int64) x << 32) + y)

//------------------------------------------------------------------
typedef	void (*DELAY_FUNC)(unsigned int uiMS);

__int64	__cdecl GetCyclesDifference(DELAY_FUNC DelayFunction, unsigned int uiParameter)
{
	unsigned int edx1, eax1;
	unsigned int edx2, eax2;
		
	// Calculate the frequency of the CPU instructions.
	__try {
		_asm {
			push uiParameter		; push parameter param
			mov ebx, DelayFunction	; store func in ebx

			RDTSC_INSTRUCTION

			mov esi, eax			; esi = eax
			mov edi, edx			; edi = edx

			call ebx				; call the delay functions

			RDTSC_INSTRUCTION

			pop ebx

			mov edx2, edx			; edx2 = edx
			mov eax2, eax			; eax2 = eax

			mov edx1, edi			; edx2 = edi
			mov eax1, esi			; eax2 = esi
		}
	}

	// A generic catch-all just to be sure...
	__except (1) {
		return -1;
	}

	return (CPUSPEED_I32TO64 (edx2, eax2) - CPUSPEED_I32TO64 (edx1, eax1));
}

void Delay( unsigned int uiMS )
{
	LARGE_INTEGER Frequency, StartCounter, EndCounter;
	__int64 x;

	// Get the frequency of the high performance counter.
	if (!QueryPerformanceFrequency (&Frequency)) return;
	x = Frequency.QuadPart / 1000 * uiMS;

	// Get the starting position of the counter.
	QueryPerformanceCounter (&StartCounter);

	do {
		// Get the ending position of the counter.	
		QueryPerformanceCounter (&EndCounter);
	} while (EndCounter.QuadPart - StartCounter.QuadPart < x);
}

void DelayOverhead( unsigned int uiMS )
{
	LARGE_INTEGER Frequency, StartCounter, EndCounter;
	__int64 x;

	// Get the frequency of the high performance counter.
	if (!QueryPerformanceFrequency (&Frequency)) return;
	x = Frequency.QuadPart / 1000 * uiMS;

	// Get the starting position of the counter.
	QueryPerformanceCounter (&StartCounter);
	
	do {
		// Get the ending position of the counter.	
		QueryPerformanceCounter (&EndCounter);
	} while (EndCounter.QuadPart - StartCounter.QuadPart == x);
}

//------------------------------------------------------------------

int CheckCPUClockSpeed()
{
	unsigned int uiRepetitions = 1;
	unsigned int uiMSecPerRepetition = 50;
	__int64	i64Total = 0, i64Overhead = 0;

	for( unsigned int nCounter=0; nCounter < uiRepetitions; nCounter++ )
	{
		i64Total += GetCyclesDifference( Delay, uiMSecPerRepetition);
		i64Overhead += GetCyclesDifference( DelayOverhead, uiMSecPerRepetition);
	}

	// Calculate the MHz speed.
	i64Total -= i64Overhead;
	i64Total /= uiRepetitions;
	i64Total /= uiMSecPerRepetition;
	i64Total /= 1000;

	return (int)i64Total;
}

int CheckClassicalCPUClockSpeed( int iCPUFamily )
{
	LARGE_INTEGER liStart, liEnd, liCountsPerSecond;

	// Attempt to get a starting tick count.
	QueryPerformanceCounter (&liStart);

	__try 
	{
		_asm 
		{
			mov eax, 0x80000000
			mov ebx, CLASSICAL_CPU_FREQ_LOOP
			Timer_Loop: 
			bsf ecx,eax
			dec ebx
			jnz Timer_Loop
		}	
	}
	__except (1)	// A generic catch-all just to be sure...
	{
		return 0;
	}

	// Attempt to get a starting tick count.
	QueryPerformanceCounter (&liEnd);

	// Get the difference...  NB: This is in seconds....
	QueryPerformanceFrequency (&liCountsPerSecond);

	double dDifference = 0.0f;
	dDifference = (((double) liEnd.QuadPart - (double) liStart.QuadPart) / (double) liCountsPerSecond.QuadPart);

	// Calculate the clock speed.
	double dFrequency = 0.0f;
	switch( iCPUFamily )
	{
	case 3:	// 80386 processors....  Loop time is 115 cycles!
		dFrequency = (((CLASSICAL_CPU_FREQ_LOOP * 115) / dDifference) / 1048576);
		break;
	case 4:	// 80486 processors....  Loop time is 47 cycles!
		dFrequency = (((CLASSICAL_CPU_FREQ_LOOP * 47) / dDifference) / 1048576);
		break;
	case 5:	// Pentium processors....  Loop time is 43 cycles!
		dFrequency = (((CLASSICAL_CPU_FREQ_LOOP * 43) / dDifference) / 1048576);
		break;
	}

	return (int)dFrequency;
}