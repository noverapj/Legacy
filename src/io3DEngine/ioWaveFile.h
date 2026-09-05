

#ifndef _ioWaveFile_h_
#define _ioWaveFile_h_

class __EX ioWaveFile
{
private:
	WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
    HMMIO         m_hmmio;       // MM I/O handle for the WAVE
    MMCKINFO      m_ck;          // Multimedia RIFF chunk
    MMCKINFO      m_ckRiff;      // Use in opening a WAVE file
    DWORD         m_dwSize;      // The size of the wave file

protected:
	HRESULT ReadMMIO();
    HRESULT WriteMMIO( WAVEFORMATEX *pwfxDest );

public:
	HRESULT Open( LPCSTR szFileName );
    HRESULT OpenFromMemory( BYTE* pbData, ULONG ulDataSize );
    HRESULT Close();

    HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
    HRESULT ResetFile();

	DWORD   GetSize() const { return m_dwSize; }	
    WAVEFORMATEX* GetFormat() const { return m_pwfx; };

public:
	ioWaveFile();
	~ioWaveFile();
};

#endif
