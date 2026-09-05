#ifndef _RoomCopyNode_h_
#define _RoomCopyNode_h_

#include "RoomParent.h"
#include "CopyNodeParent.h"

class RoomCopyNode : public RoomParent,
					 public CopyNodeParent					 
{
protected:
	// 기본 정보
	int			 m_iRoomIndex;
	RoomStyle	 m_iRoomStyle;
	int			 m_iRoomLevel;            //킬데스 레벨 적용됨.
	ModeType     m_iModeType;
	int			 m_iSubType;
	int          m_iMapNumber;
	int			 m_iMaxPlayer;
	int			 m_iCurPlayer;
	int			 m_iCurJoiner;
	bool         m_bSafetyLevelRoom;
	int          m_iTeamRatePoint;
	bool         m_bTimeClose;

	// 추가 광장 정보
	int			 m_iRoomNumber;
	int			 m_iSubState;		// 광장 npc 이벤트 flag
	int			 m_iMasterLevel;
	ioHashString m_szRoomName;
	ioHashString m_szMaster;
	ioHashString m_szPassword;
	PlazaType    m_ePlazaType;        //광장 대화 속성( 연습광장 - 대화광장 - 길드광장 )

protected:
	void InitData();

public:
	virtual void OnCreate( ServerNode *pCreator );
	virtual void OnDestroy();

public:
	void ApplySyncMode( SP2Packet &rkPacket );
	void ApplySyncCurUser( SP2Packet &rkPacket );
	void ApplySyncPlazaInfo( SP2Packet &rkPacket );
	void ApplySyncCreate( SP2Packet &rkPacket );

public:
	void SetRoomIndex( int iRoomIndex ){ m_iRoomIndex = iRoomIndex; }
	void SetTimeClose( bool bTimeClose ){ m_bTimeClose = bTimeClose; }
	void SetJoinUserCnt( int iJoinUserCnt ){ m_iCurJoiner = iJoinUserCnt; }
	void SetPlayUserCnt( int iPlayUserCnt ){ m_iCurPlayer = iPlayUserCnt; }
	bool IsServerUserFull();

public:
	virtual int GetRoomIndex();
	virtual RoomStyle GetRoomStyle();
	virtual int GetAverageLevel();
	virtual int GetGapLevel( int iMyLevel );
	virtual ModeType GetModeType();
	virtual int GetModeSubNum();
	virtual int GetModeMapNum(); 
	virtual int GetMaxPlayer();
	virtual int GetJoinUserCnt();
	virtual int GetPlayUserCnt();
	virtual int GetRoomNumber();
	virtual int GetMasterLevel();
	virtual ioHashString GetRoomName();
	virtual ioHashString GetMasterName();
	virtual bool IsRoomMasterID();
	virtual ioHashString GetRoomPW();
	virtual bool IsRoomPW();
	virtual bool IsRoomFull();
	virtual const bool IsRoomOriginal(){ return false; }
	virtual bool IsRoomEmpty(){ return false;}
	virtual bool IsSafetyLevelRoom() const { return m_bSafetyLevelRoom; }
	virtual int  GetTeamRatePoint(){ return m_iTeamRatePoint; }
	virtual int  GetPlazaRoomLevel();
	virtual bool IsTimeCloseRoom() const { return m_bTimeClose; }
	virtual PlazaType GetPlazaModeType() const;
	virtual int  GetSubState() const { return m_iSubState; }

public:
	virtual void OnPlazaRoomInfo( UserParent *pUser );

public:
	RoomCopyNode();
	virtual ~RoomCopyNode();
};
typedef std::vector< RoomCopyNode * > vRoomCopyNode;
typedef vRoomCopyNode::iterator vRoomCopyNode_iter;

#endif