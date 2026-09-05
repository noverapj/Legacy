#ifndef _CopyNodeParent_h_
#define _CopyNodeParent_h_

class ServerNode;
class SP2Packet;
class CopyNodeParent
{
public:
	enum CopyType
	{
		NONE_TYPE = 0,
		ROOM_TYPE,
		USER_TYPE,
		BATTLEROOM_TYPE,
		CHANNEL_TYPE,
		LADDERTEAM_TYPE,
		SHUFFLEROOM_TYPE,
	};

protected:
	ServerNode   *m_pCreator;
	CopyType      m_eCopyType;

public:
	virtual void OnCreate( ServerNode *pCreator );
	virtual void OnDestroy();

public:
	const ioHashString &GetServerIP();
	const int   &GetServerPort();
	const int   &GetClientPort();

public:
	virtual bool SendMessage( SP2Packet &rkPacket );

public:
	CopyNodeParent();
	virtual ~CopyNodeParent();
};

#endif