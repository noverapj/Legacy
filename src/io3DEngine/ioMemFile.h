

#ifndef _ioOggMemFile_h_
#define _ioOggMemFile_h_

//VERY HARD CODE!!!!!!!!!!!!
/*
ioMemFile은 new로 할당해서 사용하면 안된다.!!!!
ogg의 ov_read() 함수에서 ioMemFile을 망기트린다.

vorbisfile.c 1569 lines 에

if(bitstream)*bitstream=vf->current_link; 부분이 있는데..

( 여기서 bitstream은 파일포인터인데, 보통은 - FILE*, 우리는 - ioMemFile )

bitstream에 저렇게 세팅될경우 ioMemFile의 virtual func table ptr가 vf->current_link로
세팅되어져 버린다..즉 데이터가 망가진다.

virtual func table ptr가 망가지므로 멤버함수 호출이 제대로 될리가 없다..-_-;

단 한가지 가능한 방법은 new로 할당해서 쓰지 않는 법이다.
동적할당도 아니고 상속도 아니기때문에 table을 사용할 일이 없으므로 영향을 받지 않는다.

또 한가지는 클래스가 아닌 구조체로 만들어서 사용하고 멤버함수 없이 모두 전역함수로 사용하면 될지도 모르겠다.

일단 www.vorbis.com에 질문을 넣어두기는 했는데..제대로 답이 올지 모르겠음..질문에 쓰인 영어가 엉망이라..-_-;
*/

class ioMemFile
{
private:
	char	m_szFileName[MAX_PATH];
	int		m_iFileSize;
	int		m_iCurPos;

	unsigned char	*m_pFileBuf;

public:
	bool OpenFile( const char *szName );
	BYTE* AllocateMemory( int iSize, const BYTE *pPtr = NULL);
	
public:
	int Read( void *pBuf, size_t iReadSize );

public:
	bool SetCurPos( int iPos );
	void Clear();
	void DecompressLSCFile();
	void Decrypt( BYTE *szSrc, int len );

public:
	inline int GetCurPos() const { return m_iCurPos; }
	inline int GetMemFileSize() const { return m_iFileSize; }
	inline const char* GetFileName() { return m_szFileName; }

public:
	const BYTE* GetPtr() const;
	BYTE* GetPtr();

public:
	ioMemFile();
	~ioMemFile();
};

#endif