#include "stdafx.h"
#include "cEncrypt.h"
#include "minilzo.h"
#include "Encoder.h"



cEncrypt G_ENCRYPT;

#if defined(__LZO_STRICT_16BIT)
#define IN_LEN      (8*1024u)
#elif defined(LZO_ARCH_I086) && !defined(LZO_HAVE_MM_HUGE_ARRAY)
#define IN_LEN      (60*1024u)
#else
#define IN_LEN      (128*1024ul)
#endif
#define OUT_LEN     (IN_LEN + IN_LEN / 16 + 64 + 3)

static unsigned char __LZO_MMODEL in  [ IN_LEN ];
static unsigned char __LZO_MMODEL out [ OUT_LEN ];


/* Work-memory needed for compression. Allocate memory in units
 * of 'lzo_align_t' (instead of 'char') to make sure it is properly aligned.
 */

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);

lzo_uint in_len;
lzo_uint out_len;
lzo_uint new_len;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEncoder::CEncoder(void)
{
	Init();
}


CEncoder::~CEncoder(void)
{
	Destroy();
}

void CEncoder::Init()
{
	
}

void CEncoder::Destroy()
{
}

BOOL CEncoder::Startup()
{
	if (lzo_init() != LZO_E_OK)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CEncoder::Pack(unsigned char* src, const int srcLength, unsigned char* dst, unsigned int& dstLength)
{
	dstLength = G_ENCRYPT.Compress(reinterpret_cast<uint8*>(src), srcLength, reinterpret_cast<uint8*>(dst));
	return TRUE;
}

BOOL CEncoder::Unpack(unsigned char* src, const int srcLength, unsigned char* dst, unsigned int& dstLength)
{
	dstLength = G_ENCRYPT.Decompress(reinterpret_cast<uint8*>(src), srcLength, reinterpret_cast<uint8*>(dst));
	return TRUE;
}

BOOL CEncoder::Compress(unsigned char* src, const int srcLength, unsigned char* dst, unsigned int& dstLength)
{
	lzo_uint outLength = 0;
	int result = lzo1x_1_compress(src, srcLength, dst, &outLength, wrkmem);
	if (result == LZO_E_OK)
	{
		dstLength = outLength;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CEncoder::Decompress(unsigned char* src, const int srcLength, unsigned char* dst, unsigned int& dstLength)
{
	lzo_uint outLength = srcLength;
	int result = lzo1x_decompress(src, srcLength, dst, &outLength, NULL);
	if ((result == LZO_E_OK) && (outLength == srcLength))
	{
		dstLength = outLength;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}