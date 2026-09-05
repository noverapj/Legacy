

#ifndef _ioOggSound_h_
#define _ioOggSound_h_

class ioOggStreamSound
{
protected:
    LPDIRECTSOUNDBUFFER  m_pDSBuffer;
    DWORD                m_dwDSBufferSize;
	OggVorbis_File		 *m_pOVF;
	ioMemFile			 m_MemFile;

protected:
    DWORD	m_dwLastPlayPos;
    DWORD	m_dwPlayProgress;
    DWORD	m_dwNotifySize;
    DWORD	m_dwNextWriteOffset;

protected:
	HANDLE	m_hNotifyEvent;
	bool	m_bFillNextWithSilence;
	bool	m_bLoop;

protected:
	bool InitStream( LPDIRECTSOUND8 pDS);
	bool RestoreBuffer( BOOL *pbRestored );
	bool FirstFillBuffer();
	void ClearData();
	bool HandleWaveStreamNotification();

public:
	bool OpenFile( LPDIRECTSOUND8 pDS, char *szFileName );
	bool OpenFromMemory( LPDIRECTSOUND8 pDS, const BYTE *pBuf, int iBufSize );
	void UpdateStream();

public:
	void Play( bool bLoop = true );
	void Stop();
	bool IsSoundPlay();
	bool ReSet();
	void SetVolume( int iVolume );
	
public:
	HANDLE GetEventHandle();
	const char* GetFileName();
	BYTE* AllocateMemory(const int iSize);

public:
    ioOggStreamSound();
    virtual ~ioOggStreamSound();
};

#endif
