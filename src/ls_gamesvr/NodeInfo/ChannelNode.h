#ifndef _ChannelNode_h_
#define _ChannelNode_h_

#include "ChannelParent.h"

typedef struct tagChannelUser
{
	DWORD m_dwUserIndex;
	ioHashString m_szPublicID;
	tagChannelUser()
	{
		m_dwUserIndex = 0;
	}

}ChannelUser;
typedef vector< ChannelUser > vChannelUser;
typedef vChannelUser::iterator vChannelUser_iter;
class ChannelNode : public ChannelParent
{
	protected:
	DWORD  m_dwIndex;

	protected:
	vChannelUser m_vUserNode;
	ioHashString m_szManToManID;

	public:
	void OnCreate();
	void OnDestroy();

	public:
	virtual const DWORD GetIndex(){ return m_dwIndex; }
	virtual void EnterChannel( const DWORD dwUserIndex, const ioHashString &szUserName );
	virtual void LeaveChannel( const DWORD dwUserIndex, const ioHashString &szUserName );
	virtual void SendPacketTcp( SP2Packet &rkPacket, DWORD dwUserIndex = 0 );
	virtual const bool IsChannelOriginal(){ return true; }

	public:
	bool IsLiveChannel();
	bool IsChannelUser( const ioHashString &szID );

	public:
	int GetNodeSize(){ return m_vUserNode.size(); }
	const ioHashString &GetManToManID(){ return m_szManToManID; }

	public:
	void SetManToManID( ioHashString szManToManID ){ m_szManToManID = szManToManID; }

	public:
	void FillUserJoinInfo( SP2Packet &rkPacket );

	public:
	ChannelNode( DWORD dwIndex = 0 );
	virtual ~ChannelNode();
};

#endif