#ifndef __ioChannelingNodeDaum_h__
#define __ioChannelingNodeDaum_h__

#include "ioChannelingNodeParent.h"

class ioChannelingNodeDaum : public ioChannelingNodeParent
{
protected:
	ioHashString m_sGetURL;
	ioHashString m_sOutputURL;
	ioHashString m_sApikey;
	ioHashString m_sSubscriptionRetractURL;

protected:
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
	virtual void _OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPakcet);

public:
	virtual void ThreadGetCash( const ioData &rData );
	virtual void ThreadOutputCash( const ioData &rData );
	virtual void ThreadSubscriptionRetract( const ioData& rData);

public:
	virtual ChannelingType GetType();

protected:
	void UTF8ToAnsi( IN const char *szUTF8, OUT char *szAnsi, OUT int &riReturnAnsiSize, IN int iAnsiBufferSize );

protected:
//-------------------------------------------------------------------------DAUM
void decode(unsigned char* s, unsigned char* ret);
char *pt(unsigned char *md, char* buf);
char *getSig(char *apiKey, char* data, char* ts, char* nonce);
void getNewNonce(char *nonce); 
void getNewTimeStamp(char *ts); 
//-------------------------------------------------------------------------

public:
	ioChannelingNodeDaum(void);
	virtual ~ioChannelingNodeDaum(void);
};

#endif // __ioChannelingNodeDaum_h__