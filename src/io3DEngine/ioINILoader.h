

#ifndef _ioINILoader_h_
#define _ioINILoader_h_

class ioINIParser;
class ioTextStream;

class __EX ioINILoader
{
private:
	char m_szFileName[MAX_PATH];
	char m_szTitle[MAX_PATH];
	char m_szStringMgrKeyName[MAX_PATH];

	ioINIParser *m_pParser;

public:
	void SetFileName( const char *szFileName , bool bMemoryParsing = true);
	void SetTitle( const char *szTitle );

	bool DoMemoryParsing();
	bool DoMemoryParsingFromMemory( ioTextStream &rkStream );

public:
	inline const char* GetFileName() const { return m_szFileName; }
	inline const char* GetTitle() const { return m_szTitle; }

public:
	bool  LoadBool( const char *szTitle, const char *szKeyName, bool bDefault );
	int   LoadInt( const char *szTitle, const char *szKeyName, int iDefault );
	float LoadFloat( const char *szTitle, const char *szKeyName, float fDefault );
	void  LoadVector( const char *szTitle, D3DXVECTOR3 *pVec );
	void  LoadString( const char *szTitle,
		const char *szKeyName,
		const char *szDefault,
		char *szBuf,
		int iBufLen );

public:
	bool  LoadBool( const char *szKeyName, bool bDefault );
	int   LoadInt( const char *szKeyName, int iDefault );
	float LoadFloat( const char *szKeyName, float fDefault );
	void  LoadVector( D3DXVECTOR3 *pVec );
	void  LoadString( const char *szKeyName,
		const char *szDefault,
		char *szBuf,
		int iBufLen );

public:
	void SaveString( const char *szTitle, const char *szKeyName, const char *szBuf );
	void SaveInt( const char *szTitle, const char *szKeyName, int iValue );
	void SaveFloat( const char *szTitle, const char *szKeyName, float fValue, bool bNoLimit = false );
	void SaveBool( const char *szTitle, const char *szKeyName, bool bValue );
	void SaveVector( const char *szTitle, const D3DXVECTOR3 &vVec );

public:
	void SaveString( const char *szKeyName, const char *szBuf );
	void SaveInt( const char *szKeyName, int iValue );
	void SaveFloat( const char *szKeyName, float fValue, bool bNoLimit = false );
	void SaveBool( const char *szKeyName, bool bValue );
	void SaveVector( const D3DXVECTOR3 &vVec );

public:	// Memory Parsing을 수행했을때만 사용가능
	int GetNumTotalTitle() const;
	int GetNumTotalKey( int iTitleIdx ) const;

	// 없으면 "" 리턴
	const char* GetTitle( int iIdx ) const;
	const char* GetKey( int iTitle, int iKey ) const;

	// 없으면 NULL 리턴
	const char* GetValue( int iTitle, int iKey ) const;

public:
	ioINILoader();
	ioINILoader( const char *szFileName , bool bMemoryParsing = true );
	ioINILoader( const char *szPath, const char *szFileName , bool bMemoryParsing = true );
	ioINILoader( const ioINILoader &rhs );
	~ioINILoader();

	//UJ 130812, put this at the end of class definition
	// VTable 순서에 영향이 있을 수 있으므로 무조건 맨 아래에 아래 내용을 유지할것
	//
	// 주의1: 최대 63글자(64bytes)까지만 지원
	// 주의2: debug 빌드에서는 string 보호가 되지 않는다
	// 주의3: 절대 문자열 상수 말고 char* szKey 등을 넘기지마라!! crash 가능!!
	//
public:
#ifndef SHIPPING //UJ
#define LoadBool_e		LoadBool
#define LoadInt_e		LoadInt
#define LoadFloat_e		LoadFloat
#define LoadString_e	LoadString
#define SetTitle_e		SetTitle
#define SetFileName_e	SetFileName
#else
	template <int N>
	__forceinline bool LoadBool_e( 
		const char (&k)[N], bool b )
	{
		_ENCSTR(k, _k);
		return LoadBool(_k, b);
	}

	template <int N, int M>
	__forceinline bool LoadBool_e( 
		const char (&t)[N], const char (&k)[M], bool b )
	{
		_ENCSTR(t, _t);
		_ENCSTR(k, _k);
		return LoadBool(_t, _k, b);
	}

	template <int N>
	__forceinline int LoadInt_e( 
		const char (&k)[N], int i )
	{
		_ENCSTR(k, _k);
		return LoadInt(_k, i);
	}

	template <int N, int M>
	__forceinline int LoadInt_e( 
		const char (&t)[N], const char (&k)[M], int i )
	{
		_ENCSTR(t, _t);
		_ENCSTR(k, _k);
		return LoadInt(_t, _k, i);
	}

	template <int N>
	__forceinline float LoadFloat_e( 
		const char (&k)[N], float f )
	{
		_ENCSTR(k, _k);
		return LoadFloat(_k, f);
	}

	template <int N, int M>
	__forceinline float LoadFloat_e( 
		const char (&t)[N], const char (&k)[M], float f )
	{
		_ENCSTR(t, _t);
		_ENCSTR(k, _k);
		return LoadFloat(_t, _k, f);
	}

	template <int N, int M>
	__forceinline void LoadString_e( 
		const char (&k)[N], const char (&d)[M], char *b, int i )
	{
		_ENCSTR(k, _k);
		_ENCSTR(d, _d);
		LoadString(_k, _d, b, i);
	}

	template <int N>
	__forceinline void LoadString_e( 
		const char (&k)[N], const char (&d)[1], char *b, int i )
	{
		_ENCSTR(k, _k);
		LoadString(_k, d, b, i);
	}

	template <int N, int M, int L>
	__forceinline void LoadString_e( 
		const char (&t)[N], const char (&k)[M], const char (&d)[L], char *b, int i )
	{
		_ENCSTR(t, _t);
		_ENCSTR(k, _k);
		_ENCSTR(d, _d);
		LoadString(_t, _k, _d, b, i);
	}

	template <int N, int M>
	__forceinline void LoadString_e( 
		const char (&t)[N], const char (&k)[M], const char (&d)[1], char *b, int i )
	{
		_ENCSTR(t, _t);
		_ENCSTR(k, _k);
		LoadString(_t, _k, d, b, i);
	}

	template <int N>
	__forceinline void SetTitle_e( 
		const char (&t)[N] )
	{
		_ENCSTR(t, _t);
		SetTitle(_t);
	}

	template <int N>
	__forceinline void SetFileName_e( 
		const char (&f)[N], bool b = true)
	{
		_ENCSTR(f, _f);
		SetFileName(_f, b);
	}
#endif // _DEBUG
};

#ifndef SHIPPING //UJ
#define ioINILoader_e	ioINILoader
#else
//UJ 130812, 스트링 상수 암호화 및 숨기기 위한 코드
class ioINILoader_e: public ioINILoader
{
private:
	char m_path[64];
	char m_fname[64];

	template <int N>
	__forceinline char* encodeString(const char (&s)[N], char* e)
	{
		__ENCSTR(s, e);
		return e;
	}

public:
	template <int N>
	__forceinline ioINILoader_e(const char (&fn)[N], bool b=true):
	ioINILoader( encodeString(fn, m_fname), b )
	{
	}

	template <int N, int M>
	__forceinline ioINILoader_e(const char (&path)[N], const char (&fn)[M], bool b=true):
	ioINILoader( 
		encodeString(path, m_path), 
		encodeString(fn, m_fname), b )
	{
	}

	ioINILoader_e( const ioINILoader &rhs )
		:ioINILoader( rhs )
	{
	}
};
#endif // _DEBUG
#endif