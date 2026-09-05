

#include "stdafx.h"

#include "ioWaveFile.h"

ioWaveFile::ioWaveFile()
{
	m_pwfx    = NULL;
    m_hmmio   = NULL;
    m_dwSize  = 0;
}

ioWaveFile::~ioWaveFile()
{
    Close();
	SAFEDELETEARRAY( m_pwfx );
}

HRESULT ioWaveFile::Open( LPCSTR szFileName )
{
    HRESULT hr;

	SAFEDELETEARRAY( m_pwfx );

	char szNameTemp[MAX_PATH];
	strcpy( szNameTemp, szFileName );

	m_hmmio = mmioOpen( szNameTemp, NULL, MMIO_ALLOCBUF | MMIO_READ );

	hr = ReadMMIO();
	if( FAILED( hr ) )
	{
		mmioClose( m_hmmio, 0 );
		LOG.PrintTimeAndLog( 0, "ioWaveFile::Open - %s ReadMMIO Failed(%x)", szFileName, hr );
		return hr;
	}

	hr = ResetFile();
	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "ioWaveFile::Open - %s ResetFile Failed(%x)", szFileName, hr );
		return hr;
	}

	m_dwSize = m_ck.cksize;

    return hr;
}

HRESULT ioWaveFile::OpenFromMemory( BYTE* pbData, ULONG ulDataSize )
{
	SAFEDELETEARRAY( m_pwfx );

	MMIOINFO mmioInfo;
	ZeroMemory(&mmioInfo, sizeof(MMIOINFO));
	mmioInfo.fccIOProc = FOURCC_MEM;
	mmioInfo.cchBuffer = ulDataSize;
	mmioInfo.pchBuffer = (char*)pbData;
	m_hmmio = mmioOpen( NULL, &mmioInfo, MMIO_ALLOCBUF | MMIO_READ );

	HRESULT hr = ReadMMIO();
	if( FAILED( hr ) )
	{
		mmioClose( m_hmmio, 0 );
		LOG.PrintTimeAndLog( 0, "ioWaveFile::OpenFromMemory - ReadMMIO Failed(%x)", hr );
		return hr;
	}

	hr = ResetFile();
	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "ioWaveFile::OpenFromMemory - ResetFile Failed(%x)", hr );
		return hr;
	}

	m_dwSize = m_ck.cksize;

    return hr;
}

HRESULT ioWaveFile::ReadMMIO()
{
    MMCKINFO        ckIn;           // chunk info. for general use.
    PCMWAVEFORMAT   pcmWaveFormat;  // Temp PCM structure to load in.       

    m_pwfx = NULL;

    if( ( 0 != mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) ) )
	{
		LOG.PrintTimeAndLog( 0, "ioWaveFile::ReadMMIO - mmioDescend Failed" );
		return E_FAIL;
	}

    // Check to make sure this is a valid wave file
    if( (m_ckRiff.ckid != FOURCC_RIFF) ||
        (m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
	{
		LOG.PrintTimeAndLog( 0, "ioWaveFile::ReadMMIO - mmioFOURCC Failed" );
		return E_FAIL;
	}

    // Search the input file for for the 'fmt ' chunk.
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if( 0 != mmioDescend( m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
	{
		LOG.PrintTimeAndLog( 0, "ioWaveFile::ReadMMIO - mmioDescend2 Failed" );
		return E_FAIL;
	}

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them
    if( ckIn.cksize < (LONG)sizeof(PCMWAVEFORMAT) )
	{
		LOG.PrintTimeAndLog( 0, "ioWaveFile::ReadMMIO - sizeof(PCMWAVEFORMAT) Failed" );
		return E_FAIL;
	}

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if( mmioRead( m_hmmio, (HPSTR) &pcmWaveFormat, 
                  sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
	{
		LOG.PrintTimeAndLog( 0, "ioWaveFile::ReadMMIO - mmioRead Failed" );
		return E_FAIL;
	}

    // Allocate the waveformatex, but if its not pcm format, read the next
    // word, and thats how many extra bytes to allocate.
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
        if( NULL == m_pwfx )
		{
			LOG.PrintTimeAndLog( 0, "ioWaveFile::ReadMMIO - m_pwfx Failed" );
			return E_FAIL;
		}

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = 0;
    }
    else
    {
        // Read in length of extra bytes.
        WORD cbExtraBytes = 0L;
        if( mmioRead( m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD) )
        {
			LOG.PrintTimeAndLog( 0, "ioWaveFile::ReadMMIO - mmioRead2 Failed" );
			return E_FAIL;
		}

        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) + cbExtraBytes ];
        if( NULL == m_pwfx )
		{
			LOG.PrintTimeAndLog( 0, "ioWaveFile::ReadMMIO - new m_pwfx Failed" );
			return E_FAIL;
		}

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = cbExtraBytes;

        // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
        if( mmioRead( m_hmmio, (CHAR*)(((BYTE*)&(m_pwfx->cbSize))+sizeof(WORD)),
                      cbExtraBytes ) != cbExtraBytes )
        {
            SAFEDELETE( m_pwfx );

            LOG.PrintTimeAndLog( 0, "ioWaveFile::ReadMMIO - mmioRead3 Failed" );
			return E_FAIL;
        }
    }

    // Ascend the input file out of the 'fmt ' chunk.
    if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
    {
        SAFEDELETE( m_pwfx );

        LOG.PrintTimeAndLog( 0, "ioWaveFile::ReadMMIO - mmioAscend Failed" );
		return E_FAIL;
    }

    return S_OK;
}

HRESULT ioWaveFile::ResetFile()
{    
	if( m_hmmio == NULL )
		return CO_E_NOTINITIALIZED;
	
	// Seek to the data
	if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC), SEEK_SET ) )
	{
		LOG.PrintTimeAndLog( 0, "ioWaveFile::ResetFile - mmioSeek Failed" );
		return E_FAIL;
	}
	
	// Search the input file for the 'data' chunk.
	m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if( 0 != mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
	{
		LOG.PrintTimeAndLog( 0, "ioWaveFile::ResetFile - mmioDescend Failed" );
		return E_FAIL;
	}
	
    return S_OK;
}

HRESULT ioWaveFile::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
	MMIOINFO mmioinfoIn; // current status of m_hmmio
	
	if( m_hmmio == NULL )
		return CO_E_NOTINITIALIZED;
	
	if( pBuffer == NULL || pdwSizeRead == NULL )
		return E_INVALIDARG;
	
	if( pdwSizeRead != NULL )
		*pdwSizeRead = 0;
	
	if( 0 != mmioGetInfo( m_hmmio, &mmioinfoIn, 0 ) )
	{
		LOG.PrintTimeAndLog( 0, "ioWaveFile::Read - mmioGetInfo Failed" );
		return E_FAIL;
	}
	
	UINT cbDataIn = dwSizeToRead;
	if( cbDataIn > m_ck.cksize ) 
		cbDataIn = m_ck.cksize;       
	
	m_ck.cksize -= cbDataIn;
    
	for( DWORD cT = 0; cT < cbDataIn; cT++ )
	{
		// Copy the bytes from the io to the buffer.
		if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
		{
			if( 0 != mmioAdvance( m_hmmio, &mmioinfoIn, MMIO_READ ) )
			{
				LOG.PrintTimeAndLog( 0, "ioWaveFile::Read - mmioAdvance Failed" );
				return E_FAIL;
			}
			
			if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
			{
				LOG.PrintTimeAndLog( 0, "ioWaveFile::Read - mmioinfoIn.pchNext Failed" );
				return E_FAIL;
			}
		}
		
		// Actual copy.
		*((BYTE*)pBuffer+cT) = *((BYTE*)mmioinfoIn.pchNext);
		mmioinfoIn.pchNext++;
	}
	
	if( 0 != mmioSetInfo( m_hmmio, &mmioinfoIn, 0 ) )
	{
		LOG.PrintTimeAndLog( 0, "ioWaveFile::Read - mmioSetInfo Failed" );
		return E_FAIL;
	}
	
	if( pdwSizeRead != NULL )
		*pdwSizeRead = cbDataIn;
 
	return S_OK;
}

HRESULT ioWaveFile::Close()
{
    mmioClose( m_hmmio, 0 );
	m_hmmio = NULL;

	return S_OK;
}
