

#ifndef _EtcHelpFunc_h_
#define _EtcHelpFunc_h_

namespace Help
{
	void GetParseData( IN const ioHashString &rSourceData, OUT ioHashStringVec &rvParseStr, IN char chToken );
	void DelEndCh( OUT ioHashString &rSourceData );

	void EncryptDecryptData( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize, IN const char *szUserKey, IN int iUserKeySize );
	void Encode( IN const char* szPlain, IN int iPlainSize, OUT char *szCipher, IN int iCipherSize, IN const char *szUserKey, IN int iUserKey );
	void Decode( IN const char *szCipher, IN int iCipherSize, OUT char* szPlain, IN int iPlainSize, IN const char *szUserKey, IN int iUserKey  );
	bool GetHostByName( OUT ioHashStringVec &rvIPList, char* szURL );

	bool GetParseJSon( IN const char * szJsonSource, IN const ioHashStringVec &rvKeyVec, OUT ioHashStringVec &rvValueVec );
	DWORD GetStringIPToDWORDIP( const char *szIP );
}

#endif