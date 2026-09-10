#include "stdafx.h" 

#include "SlangUtil.h"

#include <mbstring.h>

template<> CSlangUtil* Singleton< CSlangUtil >::ms_Singleton = 0;

CSlangUtil::CSlangUtil()
{
	m_iMaxString = 0;

	m_pFilterString = NULL;
	m_pFilterMultiByteString = NULL;
}

CSlangUtil::~CSlangUtil()
{
	ClearSlangList();
}

void CSlangUtil::ClearSlangList()
{
	m_iMaxString = 0;

	m_vString.clear();
	m_vGuildNameLimit.clear();

	SAFEDELETEARRAY(m_pFilterString);
	SAFEDELETEARRAY(m_pFilterMultiByteString);

	memset( m_szPreChat, 0, sizeof(m_szPreChat) );
	memset( m_szAppendLineResult, 0, sizeof(m_szAppendLineResult) );
}

void CSlangUtil::SetSlangData( const char* str )
{	
	m_vString.push_back( std::string(str) );
	m_iMaxString = (int) m_vString.size();
}

inline void CSlangUtil::FilterChar( const char *src, char *dst, char *szFilterChar )
{
	if(szFilterChar == NULL) return;

	bool bFiltered = false;
	char *srcp = (char *) src;
	char *dstp = dst;
	int iCnt = 0;
	while (*srcp) 
	{
		// lead byte면 byte filer을 생략 2칸 이동

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
		if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, *srcp ) )
#else
		if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, *srcp ) ) 
#endif

#else
		if( IsDBCSLeadByte(*srcp)) 
#endif

		{
			*dstp = *srcp;   
			dstp += 1;
			srcp += 1;

			if( !(*srcp) )
				break;

			*dstp = *srcp;   
			dstp += 1;
		}
		else if (strchr(szFilterChar, *srcp) == NULL) 
		{
			*dstp = *srcp;   
			dstp += 1;
		}
		else
		{
			bFiltered = true;
		}

		srcp += 1;
		if( iCnt++ > MAX_LOOP ) // 비상STOP
		{
			LOG.PrintTimeAndLog(0, "%s : %d : Over Max loop : %s", __FUNCTION__, __LINE__, src );
			break;
		}
	}
	*dstp = 0;

	// 앞에 글자로 NULL이 변형되는 것을 방지하기 위해서
	if( bFiltered )
	{
		dstp += 1;
		*dstp = 0;
	}
}

inline void CSlangUtil::FilterMultiByteChar( const char *szSrc,
											 const int iSrcLength,
											 char *szDst,
											 const int iDstLength,
											 char *szFilterChar )
{
	if(szFilterChar == NULL) 
	{
		StringCbCopy( szDst, iDstLength, szSrc );
		return;
	}

	bool bFiltered = false;
	char szMultiByte[3]="";
	int iSrcPos = 0;
	int iDstPos = 0;
	int iCnt=0;

	while( szSrc[iSrcPos] != NULL )
	{
		if( iSrcPos >= iSrcLength )
			break;
		if( iDstPos >= iDstLength )
			break;

		if( (iSrcPos + 2) <= iSrcLength )
		{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
			if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, szSrc[iSrcPos] ) )
#else
			if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, szSrc[iSrcPos] ) )
#endif

#else
			if( IsDBCSLeadByte(szSrc[iSrcPos]) )
#endif

			{
				StringCbCopyN( szMultiByte, sizeof(szMultiByte), &szSrc[iSrcPos], 2 );

				if( _mbsstr((const unsigned char*)szFilterChar, (const unsigned char*)szMultiByte) == NULL )
				{
					StringCbCopyN( &szDst[iDstPos],iDstLength - iDstPos, &szSrc[iSrcPos], 2);
					iDstPos+=2;
				}
				else
				{
					bFiltered = true;
				}
				iSrcPos+=2;
			}
			else
			{
				StringCbCopyN( &szDst[iDstPos],iDstLength - iDstPos, &szSrc[iSrcPos], 1);
				iDstPos++;
				iSrcPos++;
			}
		}
		else if( (iSrcPos + 1) <= iSrcLength )
		{
			StringCbCopyN( &szDst[iDstPos],iDstLength - iDstPos, &szSrc[iSrcPos], 1);
			iDstPos++;
			iSrcPos++;
		}

		if( iCnt++ > MAX_LOOP ) // 비상STOP
		{
			LOG.PrintTimeAndLog(0, "%s : %d : Over Max loop : %s", __FUNCTION__, __LINE__, szSrc );
			break;
		}

		if( iSrcPos >= iSrcLength )
			break;
		if( iDstPos >= iDstLength )
			break;
	}

	// 앞에 글자로 NULL이 변형되는 것을 방지하기 위해서
	if( bFiltered )
	{
		if( (iDstPos + 1) < iDstLength )
		{
			szDst[iDstPos]=NULL;
			szDst[iDstPos+1]=NULL;
		}
	}
	else
	{
		if( (iDstPos) < iDstLength )
		{
			szDst[iDstPos]=NULL;
		}
	}
}

bool CSlangUtil::IsSlang( const char * szFilteredString )
{
	char word[MAX_PATH];    
	memset(word, 0, sizeof(word));

    const char *sub, *subs, *subw;

    if (!szFilteredString[0]) return false;

    // optimized for multibyte code set.
	for( int i=0 ; i < m_iMaxString ; i++)
	{
		if(m_vString[i].empty())	break;

		GetField(m_vString[i].c_str(), ',', word);	
		sub = szFilteredString;
		int iCnt=0;
		while (*sub && *(sub + 1)) 
		{
			subs = sub;	// current comparison pointer in string
			subw = word;	// current comparison pointer in word
			int iCnt2=0;
			while (*subs && (*subs == *subw)) 
			{
				subs++; subw++;
				if (*subw == 0) 
				{
					return true;		// reach to null, curse
				}

				if( iCnt2++ > MAX_LOOP ) // 비상STOP
				{
					LOG.PrintTimeAndLog(0, "%s : %d : Over Max loop : %s", __FUNCTION__, __LINE__,  szFilteredString );
					break;
				}
			}
			sub += *sub < 0 ? 2 : 1;

			if( iCnt++ > MAX_LOOP ) // 비상STOP
			{
				LOG.PrintTimeAndLog(0, "%s : %d : Over Max loop : %s", __FUNCTION__, __LINE__, szFilteredString );
				break;
			}
		}
	}
	
	return false;
}

char* CSlangUtil::GetField(const char *buf, int delim, char *field)
{
	unsigned char *src = (unsigned char *)buf;
	int iCnt = 0;
    while ((int)*src == delim && *src != 0) 
	{
		src++;	// delimeter를 건너뛴다.
		if( iCnt++ > MAX_LOOP ) // 비상STOP
		{
			LOG.PrintTimeAndLog(0, "%s : %d : Over Max loop : %s", __FUNCTION__, __LINE__, buf );
			break;
		}
	}
	iCnt=0;
    while ((int)*src != delim && *src != 0) 
	{
		*field++ = *src++;
		if( iCnt++ > MAX_LOOP ) // 비상STOP
		{
			LOG.PrintTimeAndLog(0, "%s : %d : Over Max loop : %s", __FUNCTION__, __LINE__, buf );
			break;
		}
	}

    *field = '\0';
    return (char *)src;
}

const char* CSlangUtil::ConvertString( const char* szOriginalStr, bool bSetPreChat /*= false */ )
{
	static std::string szCleanUpString;
    char *result, *string, *szTempString, *szPreString, *szPreException;

	szCleanUpString.clear();
	szCleanUpString = szOriginalStr;

	// 인자의 전체 크기를 넘는 수정을 해서는 안됨
	int max_len = strlen(szOriginalStr)+1;

	if ( max_len <= 1) return szCleanUpString.c_str();

    if (!szOriginalStr[0]) return szCleanUpString.c_str();

	result = new char[max_len];
	string = new char[max_len];
	szTempString = new char[max_len];
	szPreString  = new char[max_len];
	szPreException = new char[max_len];

	memset(result, 0, max_len);
	memset(string, 0, max_len);
	memset(szTempString, 0, max_len);
	memset(szPreString, 0, max_len);
	memset(szPreException, 0, max_len);

	FilterChar(szOriginalStr, szTempString, m_pFilterString);
	FilterMultiByteChar(szTempString, max_len, string, max_len, m_pFilterMultiByteString);

	StringCbCopy( szPreString, max_len, szTempString );

	// 기본 필터링
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetChangedString( string );

	char *pFirstResult = NULL;
	bool isFirstFilter = false;
    if (IsSlang(string)) 
	{
		StringCbCopy( szPreException, max_len, string );

		if( RePlaceSlang(string, result, max_len) == false)
		{
			szCleanUpString.clear();
			delete[] result;
			delete[] string;
			delete[] szTempString;
			delete[] szPreString;

			return szCleanUpString.c_str();
		}

		RePlaceException( szPreException, result, max_len );

		result[max_len-1] = 0;		// prevent overflow
		szCleanUpString = result;
		pFirstResult = result;
		isFirstFilter = true;
	}
	else
		pFirstResult = (char*)string;

    // 숫자 필터링
	memset(szTempString, 0, max_len);
	char szFilterChar2[]={'0','1','2','3','4','5','6','7','8','9',NULL}; 
	FilterChar(pFirstResult, szTempString, szFilterChar2);

	memset(result, 0, max_len);
	FilterChar(szPreString, result, szFilterChar2);

	if( bSetPreChat )
	{
		memset( m_szPreChat, 0, sizeof(m_szPreChat) );
		StringCbCopy( m_szPreChat, sizeof(m_szPreChat), result );
	}

	if( pLocal )
		pLocal->GetChangedString( szTempString );

	bool bSecondFilter = false;
	memset(result, 0, max_len);
	if (IsSlang(szTempString)) 
	{
		memset(szPreException, 0, max_len);
		StringCbCopy( szPreException, max_len, szTempString );

		if( RePlaceSlang(szTempString, result, max_len) == false)
		{
			szCleanUpString.clear();
			delete[] result;
			delete[] string;
			delete[] szTempString;
			delete[] szPreString;
			return szCleanUpString.c_str();
		}

		RePlaceException( szPreException, result, max_len );

		result[max_len-1] = 0;		// prevent overflow
		szCleanUpString = result;
		bSecondFilter = true;
	}

	// ' ' 복원
	if(isFirstFilter)
	{
		memset(string, 0, max_len);
		memset(szTempString, 0, max_len);

		//FilterChar(szOriginalStr, szTempString, &m_pFilterString[1]); // 공백 문자를 제외하고 필터링
		StringCbCopy( szTempString, max_len, szOriginalStr );

		if(bSecondFilter)
		{
			memset(result, 0, max_len);
			FilterMultiByteChar(szTempString, max_len, string, max_len, m_pFilterMultiByteString);
			//FilterMultiByteChar(szTempString, max_len, result, max_len, m_pFilterMultiByteString);
			//FilterChar(result, string, szFilterChar2);
		}
		else
		{
			FilterMultiByteChar(szTempString, max_len, string, max_len, m_pFilterMultiByteString);
		}
		
		int iCheckArray = 0;
		int iFirstMultiByteArray = -2;
		int iCleanUpSize = szCleanUpString.size();
		for (int i = 0;  i <  max_len - 1; i++)
		{
			if( iCheckArray == i)
			{
				if( i < iCleanUpSize)
				{
					char sz = szCleanUpString.at(i);

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
					if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, sz ) )
#else
					if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, sz ) )
#endif

#else
					if( IsDBCSLeadByte(sz) )
#endif

					{
						iFirstMultiByteArray = i;
						iCheckArray +=2;
					}
					else
						iCheckArray +=1;
				}
			}

			if( strchr(m_pFilterString, string[i]) && iFirstMultiByteArray != (i-1) )
			{
				iCleanUpSize = szCleanUpString.size();
				if( i >= iCleanUpSize )
				{
					szCleanUpString = szCleanUpString + string[i];
				}
				else
				{
					std::basic_string <char>::iterator str_Iter = ( szCleanUpString.begin()+ i );
					if(str_Iter._Ptr != NULL )
						szCleanUpString.insert(str_Iter, string[i] );
					iCheckArray   = i+1;
				}
			}

			/*
			if(string[i] == ' ' && iFirstMultiByteArray != (i-1)  ) // 멀티바이트 중간에 ' ' 넣지 못하도록 함.
			{
				iCleanUpSize = szCleanUpString.size();
				if( i >= iCleanUpSize ) continue;

				std::basic_string <char>::iterator str_Iter = ( szCleanUpString.begin()+ i );
				if(str_Iter != NULL )
					szCleanUpString.insert(str_Iter, ' ');
				iCheckArray   = i+1;
			}
			*/
		}
	}

	delete[] result;
	delete[] string;
	delete[] szTempString;
	delete[] szPreString;

    return szCleanUpString.c_str();
}

const char* CSlangUtil::ConvertStringWithPreLine( const char* szOriginalStr )
{	
	std::string szCleanUpAppendString;

	int iFilteredSize = strlen( m_szPreChat );
	if( iFilteredSize > 0 )
	{
		szCleanUpAppendString = m_szPreChat;
		szCleanUpAppendString += szOriginalStr;
	}
	else
	{
		szCleanUpAppendString = szOriginalStr;
	}

	memset( m_szAppendLineResult, 0, sizeof(m_szAppendLineResult) );
	StringCbCopy( m_szAppendLineResult,
				  sizeof(m_szAppendLineResult),
				  ConvertString( szCleanUpAppendString.c_str(), true ) );

	if( iFilteredSize > 0 )
	{
		int iRemovePos = 0;
		int iMaxPos = strlen( m_szAppendLineResult );

		int iCnt=0;
		while( iRemovePos < iMaxPos && iRemovePos < iFilteredSize )
		{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
			if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)m_szAppendLineResult[iRemovePos] ) )
#else
			if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)m_szAppendLineResult[iRemovePos] ) )
#endif

#else
			if( IsDBCSLeadByte( (BYTE)m_szAppendLineResult[iRemovePos] ) )
#endif

				iRemovePos += 2;
			else
				iRemovePos++;

			if( iCnt++ > MAX_LOOP ) // 비상STOP
			{
				LOG.PrintTimeAndLog(0, "%s : %d : Over Max loop : %s", __FUNCTION__, __LINE__, m_szAppendLineResult );
				break;
			}
		}

		if( iRemovePos > 0 )
		{
			if( iRemovePos < iMaxPos )
			{
				int iNewLength = iMaxPos - iRemovePos;
				if( iNewLength >= MAX_LOOP )
				{
					// 사이즈 오버?
					char szError[MAX_PATH] = "";
					sprintf( szError, "크래쉬 위험 함수 : CSlangUtil::ConvertStringWithPreLine Type A:%s - %d", g_MyInfo.GetPublicID().c_str(), iNewLength ); //Except Extracting Hangeul
					SP2Packet kPacket( LUPK_LOG );
					kPacket << "ClientError";  // 로그 파일 타입
					kPacket << szError;
					kPacket << 317;  // 오류번호:317
					kPacket << true; // write db
					P2PNetwork::SendLog(kPacket);

				}
				memmove( m_szAppendLineResult, m_szAppendLineResult + iRemovePos, iNewLength );
				m_szAppendLineResult[iNewLength] = 0;
			}
			else
			{
				memset( m_szAppendLineResult, 0, sizeof(m_szAppendLineResult) );
			}
		}

		iRemovePos = 0;
		iMaxPos = strlen( m_szPreChat );

		iCnt=0;
		while( iRemovePos < iMaxPos && iRemovePos < iFilteredSize )
		{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
			if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)m_szPreChat[iRemovePos] ) )
#else
			if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)m_szPreChat[iRemovePos] ) )
#endif

#else
			if( IsDBCSLeadByte( (BYTE)m_szPreChat[iRemovePos] ) )
#endif

				iRemovePos += 2;
			else
				iRemovePos++;

			if( iCnt++ > MAX_LOOP ) // 비상STOP
			{
				LOG.PrintTimeAndLog(0, "%s : %d : Over Max loop : %s", __FUNCTION__, __LINE__, m_szAppendLineResult );
				break;
			}
		}

		if( iRemovePos > 0 )
		{
			if( iRemovePos < iMaxPos )
			{
				int iNewLength = iMaxPos - iRemovePos;
				if( iNewLength >= MAX_LOOP )
				{
					// 사이즈 오버?
					char szError[MAX_PATH] = "";
					sprintf( szError, "크래쉬 위험 함수 : CSlangUtil::ConvertStringWithPreLine Type B:%s - %d", g_MyInfo.GetPublicID().c_str(), iNewLength ); //Except Extracting Hangeul
					SP2Packet kPacket( LUPK_LOG );
					kPacket << "ClientError";  // 로그 파일 타입
					kPacket << szError;
					kPacket << 317;  // 오류번호:317
					kPacket << true; // write db
					P2PNetwork::SendLog(kPacket);					
				}
				memmove( m_szPreChat, m_szPreChat + iRemovePos, iNewLength );
				m_szPreChat[iNewLength] = 0;
			}
			else
			{
				memset( m_szPreChat, 0, sizeof(m_szPreChat) );
			}
		}
	}

	return m_szAppendLineResult;
}

CSlangUtil& CSlangUtil::GetSingleton()
{
	return Singleton< CSlangUtil >::GetSingleton();
}

void CSlangUtil::LoadSlangData()
{
	ClearSlangList();

	ioINILoader_e kLoader( "config/sp2_chat_slang.ini" );

	kLoader.SetTitle_e("slang_normal");
	int iMax = kLoader.LoadInt("max", 0);

	char szBuff[MAX_PATH];
	char szKeyName[MAX_PATH];
	m_vString.clear();
	int i = 0;
	for(i = 0; i < iMax; i++)
	{
		memset(szBuff, 0, sizeof(szBuff));
		memset(szKeyName, 0, sizeof(szKeyName));

		wsprintf_e(szKeyName, "%d", i+1);
		kLoader.LoadString(szKeyName, "", szBuff, sizeof(szBuff));
		if(!strcmp(szBuff, ""))
			break;

		// 욕과 변경할 단어의 크기를 통일시킨다.
		char szSlangWord[MAX_PATH];
		char szReplaceWord[MAX_PATH];
		memset(szSlangWord, 0, sizeof(szSlangWord));
		memset(szReplaceWord, 0, sizeof(szReplaceWord));

		char *lptr = szBuff;
		lptr = GetField(lptr, ',', szSlangWord);		// get the first field	
		lptr = GetField(lptr, ',', szReplaceWord);	// get the second field

		int iSlangSize = 0;
		int iReplaceSize = 0;
		StringCbLength(szSlangWord, sizeof(szSlangWord), (size_t *)&iSlangSize);
		StringCbLength(szReplaceWord, sizeof(szReplaceWord), (size_t *)&iReplaceSize);

		if(iSlangSize != iReplaceSize)
		{
			if(iSlangSize > iReplaceSize)
			{
				for (int i = 0; i < iSlangSize - iReplaceSize ; i++)
					szReplaceWord[iReplaceSize+i] = ' ';
			}
			else
			{
				for (int i = 0; i < iReplaceSize - iSlangSize ; i++)
					szReplaceWord[iSlangSize+i] = NULL;
				
				iReplaceSize-=(iReplaceSize - iSlangSize);
				// 마지막 글자가 한글의 시작 바이트라면 한글을 삭제함
				for (int i = 0; i < iReplaceSize ; i++)
				{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
					if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, szReplaceWord[i] ) )
#else
					if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, szReplaceWord[i] ) ) 
#endif

#else
					if( IsDBCSLeadByte(szReplaceWord[i]) ) 
#endif

					{
						i++;
						if(iReplaceSize <= i)
							szReplaceWord[i-1] = ' ';
					}
				}
			}
		}
		memset(szBuff, 0, sizeof(szBuff));
		StringCbPrintf(szBuff, sizeof(szBuff), "%s,%s", szSlangWord, szReplaceWord);
		//

		SetSlangData(szBuff);
	}

	// filter
	kLoader.SetTitle_e("filter");
	iMax = kLoader.LoadInt( "max", 0);
	if( iMax != 0 && iMax <= MAX_FILTER )
	{
		SAFEDELETEARRAY(m_pFilterString);
		m_pFilterString = new char[iMax+1];
		memset(m_pFilterString, 0, iMax+1);

		if( m_pFilterString == NULL ) return;
		for (int i = 0; i < iMax ; i++)
		{
			char szKeyName[MAX_PATH]="";
			StringCbPrintf( szKeyName , sizeof(szKeyName) , "%d", i+1);
			int iAsciiCode = kLoader.LoadInt(szKeyName, -1);
			if( iAsciiCode != -1)
				m_pFilterString[i] = iAsciiCode;
		}
	}

	// filter multibyte
	kLoader.SetTitle_e("filter_multibyte");
	iMax = kLoader.LoadInt( "max", 0);
	if( iMax != 0 && iMax <= MAX_FILTER )
	{
		SAFEDELETEARRAY(m_pFilterMultiByteString);
		m_pFilterMultiByteString = new char[iMax+1];
		memset(m_pFilterMultiByteString, 0, iMax+1);

		if( m_pFilterMultiByteString == NULL ) return;
		for (int i = 0; i < iMax ; i++)
		{
			char szKeyName[MAX_PATH]="";
			StringCbPrintf( szKeyName , sizeof(szKeyName) , "%d", i+1);
			int iAsciiCode = kLoader.LoadInt(szKeyName, -1);
			if( iAsciiCode != -1)
				m_pFilterMultiByteString[i] = iAsciiCode;
		}
	}

	// guild name limit
	kLoader.SetTitle_e("guildname_limit");
	iMax = kLoader.LoadInt("max", 0);
	for(int i = 0; i < iMax; i++)
	{
		memset(szBuff, 0, sizeof(szBuff));
		memset(szKeyName, 0, sizeof(szKeyName));

		wsprintf_e(szKeyName, "%d", i+1);
		kLoader.LoadString(szKeyName, "", szBuff, sizeof(szBuff));
		if(!strcmp(szBuff, ""))
			break;
		m_vGuildNameLimit.push_back( std::string(szBuff) );
	}

	// exception word
	kLoader.SetTitle_e("exception_word");
	iMax = kLoader.LoadInt("max", 0);
	for(int i = 0; i < iMax; i++)
	{
		memset(szBuff, 0, sizeof(szBuff));
		memset(szKeyName, 0, sizeof(szKeyName));

		wsprintf_e(szKeyName, "%d", i+1);
		kLoader.LoadString(szKeyName, "", szBuff, sizeof(szBuff));
		
		if(!strcmp(szBuff, ""))
			break;
		
		m_vExceptionList.push_back( std::string(szBuff) );
	}
}

bool CSlangUtil::RePlaceSlang( char * string, char * result, int max_len )
{
	char slang[MAX_PATH], replace[MAX_PATH];
	memset(slang, 0, sizeof(slang));
	memset(replace, 0, sizeof(replace));

	char *lptr, *cptr;
	for(int i=0 ; i< m_iMaxString ; i++)
	{
		if( m_vString[i].empty()) break;

		lptr = (char*)m_vString[i].c_str();
		lptr = GetField(lptr, ',', slang);		// get the first field	
		lptr = GetField(lptr, ',', replace);	// get the second field
		cptr = string;
		strcpy(result, string);

		if( !Setting::IsSlangFilterChange() )
		{
			memset(replace, 0, sizeof(replace));
			int iSlangSize = strlen( slang );
			for (int j = 0; j < iSlangSize ; j++)
			{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
				if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, slang[j] ) )
#else
				if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, slang[j] ) ) 
#endif

#else
				if( IsDBCSLeadByte(slang[j]) ) 
#endif

				{
					replace[j] = '*';
					j++;
					replace[j] = ' ';
				}
				else
				{
					replace[j] = '*';
				}
			}
		}

		// 바뀔값이 없다면 다음으로 넘어가자.
		if (!replace[0]) continue;

		int iCnt=0;
		while (cptr = (char*)_mbsstr((const unsigned char*)string, (const unsigned char*)slang), cptr) 
		{
			// if there is no matching replacement of curse word
			if (!replace[0]) 
				return false;

			if ((max_len - 1) < (int)((cptr - string) + strlen(replace) + strlen(cptr + strlen(slang)))) 
			{
				// string too long, remove the curses that linger around
				strncpy(result, string, cptr - string);
				strcpy(result + (cptr - string), cptr + strlen(slang));
			}
			else
			{               	    
				strncpy(result, string, cptr - string);
				strcpy(result + (cptr - string), replace);
				strcpy(result + (cptr - string + strlen(replace)), cptr + strlen(slang));
			}
			// refresh intermediate result
			strcpy(string, result);

			if( iCnt++ > MAX_LOOP ) // 비상STOP
			{
				LOG.PrintTimeAndLog(0, "%s : %d : Over Max loop : %s : %s", __FUNCTION__, __LINE__, string , result );
				break;
			}
		}
	}
	return true;
}

bool CSlangUtil::RePlaceException( char * string, char * result, int max_len )
{
	const char *word;

	bool bReplace = false;
	for(int i = 0;i < (int)m_vExceptionList.size();i++)
	{
		if(m_vExceptionList[i].empty())
			break;

		word = m_vExceptionList[i].c_str();

		int iCnt=0;
		char *cptr;

		while (cptr = (char*)_mbsstr((const unsigned char*)string, (const unsigned char*)word), cptr) 
		{
			if ((max_len - 1) < (int)((cptr - string) + strlen(word) + strlen(cptr + strlen(word)))) 
			{
				break;
			}
			else
			{
				strncpy( result + (cptr - string), word, strlen(word) );

				memset( cptr, '*', strlen(word) );

				bReplace = true;
			}

			if( iCnt++ > MAX_LOOP ) // 비상STOP
			{
				LOG.PrintTimeAndLog(0, "%s : %d : Over Max loop : %s : %s", __FUNCTION__, __LINE__, string , result );
				break;
			}
		}
	}

	return bReplace;
}

bool CSlangUtil::IsLimitedGuildName( const char *szGuildName )
{
	if( szGuildName == NULL ) return false;

	int iMaxLen = strlen( szGuildName ) + 1;

	static char szNumFilter[]={'0','1','2','3','4','5','6','7','8','9',NULL}; 
	char szSrcChar1[MAX_PATH], szSrcChar2[MAX_PATH], szSrcChar3[MAX_PATH], szSrcLastFilter[MAX_PATH];
	memset( szSrcChar1, 0, sizeof( szSrcChar1 ) );
	memset( szSrcChar2, 0, sizeof( szSrcChar2 ) );
	memset( szSrcChar3, 0, sizeof( szSrcChar3 ) );
	memset( szSrcLastFilter, 0, sizeof( szSrcLastFilter ) );

	FilterChar( szGuildName, szSrcChar1, m_pFilterString );
	FilterMultiByteChar( szSrcChar1, iMaxLen, szSrcChar2, iMaxLen, m_pFilterMultiByteString );
	FilterChar( szSrcChar2, szSrcChar3, szNumFilter );
	RePlaceException( szSrcChar3, szSrcLastFilter, MAX_PATH ); // szSrcChar3의 예외 단어를 **변경하고 | szSrcLastFilter는 필터된 글자를 복원한다.
	memset( szSrcLastFilter, 0, sizeof( szSrcLastFilter ) );   // 필터된 글자가 복원되므로 다시 초기화
	StringCbCopy( szSrcLastFilter, sizeof( szSrcLastFilter ), szSrcChar3 ); // 아래에서 금칙어 검사를 하므로 **변한 값을 넘겨준다.

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetChangedString( szSrcLastFilter );

	const char *word, *sub, *subs, *subw;
	for(int i = 0;i < (int)m_vGuildNameLimit.size();i++)
	{
		if(m_vGuildNameLimit[i].empty())
			break;

		word = m_vGuildNameLimit[i].c_str();	
		sub  = szSrcLastFilter;
		int iCnt=0;
		while (*sub && *(sub + 1)) 
		{
			subs = sub;	
			subw = word;
			int iCnt2=0;
			while (*subs && (*subs == *subw)) 
			{
				subs++; subw++;
				if (*subw == 0) 
				{
					return true;
				}
				if( iCnt2++ > MAX_LOOP ) // 비상STOP
				{
					LOG.PrintTimeAndLog(0, "%s : %d : Over Max loop : %s", __FUNCTION__, __LINE__, szGuildName );
					break;
				}
			}
			sub += *sub < 0 ? 2 : 1;
			if( iCnt++ > MAX_LOOP ) // 비상STOP
			{
				LOG.PrintTimeAndLog(0, "%s : %d : Over Max loop : %s", __FUNCTION__, __LINE__, szGuildName );
				break;
			}
		}
	}
	return false;
}

