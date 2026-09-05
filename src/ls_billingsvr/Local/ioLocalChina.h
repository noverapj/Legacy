#ifndef __ioLocalChina_h__
#define __ioLocalChina_h__

#include "ioLocalParent.h"

class ioLocalChina  : public ioLocalParent
{
protected:
	ioHashString m_sLoginURL;
	ioHashString m_sBillingGetURL;
	ioHashString m_sBillingOutPutURL;
/*
protected:
	void EncryptDecryptData2( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize, IN const char *szUserKey, IN int iUserKeySize );
	void Encode( IN const char* szPlain, IN int iPlainSize, OUT char *szCipher, IN int iCipherSize, IN const char *szUserKey, IN int iUserKey );
	void Decode( IN const char *szCipher, IN int iCipherSize, OUT char* szPlain, IN int iPlainSize, IN const char *szUserKey, IN int iUserKey );
	void GetHexMD5( OUT char *szHexMD5, IN int iHexSize, IN const char *szSource );
	bool GetParseJSon( IN const char * szJsonSource, IN const ioHashStringVec &rvKeyVec, OUT ioHashStringVec &rvValueVec );
*/
public:
	virtual ioLocalManager::LocalType GetType();

	virtual void OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
	virtual void OnRefundCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void OnUserInfo( ServerNode *pServerNode, SP2Packet &rkPacket );

	virtual void Init();
public:
	ioLocalChina(void);
	virtual ~ioLocalChina(void);
};

#endif // __ioLocalChina_h__