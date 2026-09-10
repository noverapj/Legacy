#pragma once


#ifndef _SLANGUTIL_H_
#define _SLANGUTIL_H_

class CSlangUtil : public Singleton< CSlangUtil >
{
protected:
		enum
		{
			MAX_FILTER   = 100,
			MAX_LOOP     = 1024,
		};

protected:
	int     m_iMaxString;
	std::vector<std::string> m_vString;
	char *m_pFilterString;
	char *m_pFilterMultiByteString;

	char m_szPreChat[MAX_LOOP];
	char m_szAppendLineResult[MAX_LOOP];

	std::vector<std::string> m_vExceptionList;

public:
	void LoadSlangData();

protected:
	void ClearSlangList();

protected:
	bool IsSlang(const char * szFilteredString);
	inline char* GetField(const char *buf, int delim, char *field);
	inline void FilterChar(const char *src, char *dst, char *szFilterChar);
	inline void FilterMultiByteChar( const char *szSrc, const int iSrcLength, char *szDst, const int iDstLength, char *szFilterChar );

	void  SetSlangData(const char* str);
	bool RePlaceSlang( char * string, char * result, int max_len );
	bool RePlaceException( char * string, char * result, int max_len );
	
public:
	const char* ConvertString( const char* szOriginalStr , bool bSetPreChat = false );
	const char* ConvertStringWithPreLine( const char* szOriginalStr );

	// 길드명 제한 : 욕설과는 무관하지만 필터링을 같이 사용함.
protected:
	std::vector<std::string> m_vGuildNameLimit;

public:
	bool IsLimitedGuildName( const char *szGuildName );

public:
	static CSlangUtil& GetSingleton();

public:
	CSlangUtil();
	virtual ~CSlangUtil();
};

#define g_SlangUtil CSlangUtil::GetSingleton()

#endif