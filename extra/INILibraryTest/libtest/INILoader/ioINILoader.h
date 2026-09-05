

#ifndef _ioINILoader_h_
#define _ioINILoader_h_

class ioINIParser;
class ioTextStream;

class ioINILoader
{
private:
	char	m_szFileName[MAX_PATH];
	char	m_szTitle[MAX_PATH];

	ioINIParser *m_pParser;

public:
	void SetFileName( const char *szFileName , bool bMemoryParsing = true);
	void SetTitle( const char *szTitle );

	bool DoMemoryParsing();
	bool DoMemoryParsingFromMemory( ioTextStream &rkStream );
	
	const char* GetFileName() const { return m_szFileName; }
	const char* GetTitle() const { return m_szTitle; }

public:
	bool LoadBool( const char *szTitle, const char *szKeyName, bool bDefault );
	int  LoadInt( const char *szTitle, const char *szKeyName, int iDefault );
	float LoadFloat( const char *szTitle, const char *szKeyName, float fDefault );
	void LoadVector( const char *szTitle, Vector3 &vVec );
	void LoadString( const char *szTitle,
					 const char *szKeyName,
					 const char *szDefault,
					 char *szBuf,
					 int iBufLen );

public:
	bool LoadBool( const char *szKeyName, bool bDefault );
	int  LoadInt( const char *szKeyName, int iDefault );
	float LoadFloat( const char *szKeyName, float fDefault );
	void LoadVector( Vector3 &vVec );
	void LoadString( const char *szKeyName,
					 const char *szDefault,
					 char *szBuf,
					 int iBufLen );

public:
	void SaveString( const char *szTitle, const char *szKeyName, const char *szBuf );
	void SaveInt( const char *szTitle, const char *szKeyName, int iValue );
	void SaveFloat( const char *szTitle, const char *szKeyName, float fValue, bool bNoLimit = false );
	void SaveBool( const char *szTitle, const char *szKeyName, bool bValue );
	void SaveVector( const char *szTitle, const Vector3 &vVec );

public:
	void SaveString( const char *szKeyName, const char *szBuf );
	void SaveInt( const char *szKeyName, int iValue );
	void SaveFloat( const char *szKeyName, float fValue, bool bNoLimit = false );
	void SaveBool( const char *szKeyName, bool bValue );
	void SaveVector( const Vector3 &vVec );

public:
	ioINILoader();
	ioINILoader( const char *szFileName , bool bMemoryParsing = true );
	ioINILoader( const char *szPath, const char *szFileName , bool bMemoryParsing = true );
	~ioINILoader();
};

#endif
