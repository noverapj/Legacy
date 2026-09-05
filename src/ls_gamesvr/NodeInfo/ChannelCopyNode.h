#ifndef _ChannelCopyNode_h_
#define _ChannelCopyNode_h_

#include "ChannelParent.h"
#include "CopyNodeParent.h"

class ChannelCopyNode : public ChannelParent,
				        public CopyNodeParent					 
{
protected:
	// 기본 정보
	DWORD        m_dwIndex;

protected:
	void InitData();

public:
	virtual void OnCreate( ServerNode *pCreator );
	virtual void OnDestroy();

public:
	virtual const DWORD GetIndex(){ return m_dwIndex; }
	virtual void EnterChannel( const DWORD dwUserIndex, const ioHashString &szUserName );
	virtual void LeaveChannel( const DWORD dwUserIndex, const ioHashString &szUserName );
	virtual void SendPacketTcp( SP2Packet &rkPacket, DWORD dwUserIndex = 0 );
	virtual const bool IsChannelOriginal(){ return false; }

public:
	void SetIndex( DWORD dwIndex ){ m_dwIndex = dwIndex; }

public:
	ChannelCopyNode();
	virtual ~ChannelCopyNode();
};

typedef std::vector< ChannelCopyNode * > vChannelCopyNode;
typedef vChannelCopyNode::iterator vChannelCopyNode_iter;
#endif