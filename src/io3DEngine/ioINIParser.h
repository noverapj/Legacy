

#ifndef _ioINIParser_h_
#define _ioINIParser_h_

#include "io3DCommon.h"
#include "ioResourceManager.h"

class ioTextStream;

#define STAITC_ACC_REFRESH_TIME 60000

class __EX ioINIMemoryStatic : public Singleton< ioINIMemoryStatic >
{
public:
	struct INISectionMem
	{		
		DWORD dwLoadTime;
		DWORD dwSectionMem;

		INISectionMem()
		{
			dwLoadTime		= 0;
			dwSectionMem	= 0;
		}
	};
	typedef std::map< ioHashString, INISectionMem > INISectionMemMap;

	struct INIMem
	{
		INISectionMemMap m_SectionMap;
	};
	typedef std::map< ioHashString, INIMem > INIMemMap;

protected:	
	INIMemMap	 m_MemoryMap;
	StaticSwitch m_StaticSwitch;
	
public:
	void SetSwitch( int eSwitch ){ m_StaticSwitch = static_cast<StaticSwitch>( eSwitch ); }

	ioINIMemoryStatic::INISectionMem* FindSection( INIMem& Mem, const ioHashString& SectionName );
	void AddSection( INIMem& Mem, const ioHashString& SectionName, DWORD dwMem );
	
	void AddMem( const ioHashString& KeyName, const ioHashString& SectionName, DWORD dwMem );
	void RemoveMem( const ioHashString& KeyName );

	void GetINIMemStatics( INIMem& Mem, DWORD& dwCount, DWORD& dwSize );
	void GetMemStatics( DWORD& dwCount, DWORD& dwSize );

	void GetExceptionINIMemStatics( INIMem& Mem, DWORD& dwSize );
	void GetExceptionMemStatics( DWORD& dwSize );

public:
	static ioINIMemoryStatic& GetSingleton();

public:
	ioINIMemoryStatic();
	virtual ~ioINIMemoryStatic();
};

class ioINIParser
{
private:
	typedef std::map< ioHashString, std::string > KeyList;
	typedef std::map< ioHashString, KeyList* > TitleList;

	TitleList m_TitleList;

	mutable ioHashString m_PreTitle;	// 이전에 사용한 타이틀 캐시용도
	mutable KeyList *m_pPreList;

	mutable ioHashString m_szTitleBuf;
	mutable ioHashString m_szKeyNameBuf;

//대략적인 INI 메모리 사용 측정(누적)
public:
	struct INISectionMem
	{		
		DWORD dwLoadTime;
		DWORD dwSectionMem;

		INISectionMem()
		{
			dwLoadTime		= 0;
			dwSectionMem	= 0;
		}
	};
	typedef std::map< ioHashString, INISectionMem > INISectionMemMap;

	struct INIMem
	{
		INISectionMemMap m_SectionMap;
	};
	typedef std::map< ioHashString, INIMem > INIMemMap;

	static INIMemMap m_MemoryMap;

public:
	static void AddMem( const ioHashString& KeyName, const ioHashString& SectionName, DWORD dwMem );
	static void RemoveMem( const ioHashString& KeyName );
	static void GetMemStatics( DWORD& dwCount, DWORD& dwSize );

public:
	bool ParsingFile( const char *szFileName );
	void ParseINI( ioTextStream &rkStream );
	void Clear();

private:
	std::string ParseTitle( const std::string &szTitle, ioTextStream &rkStream );
	void ParseKey( const std::string &line, KeyList *pKeyList );

public:
	int GetNumTotalTitle() const;
	int GetNumTotalKey( int iTitle ) const;

public:	// 없으면 ""
	const char* GetTitle( int iIdx ) const;
	const char* GetKey( int iTitle, int iKey ) const;

public:	// 없으면 NULL
	const char* GetValue( int iTitle, int iKey ) const;
	const char* GetValue( const char *szTitle, const char *szKeyName ) const;

public:
	ioINIParser();
	~ioINIParser();
};

#endif