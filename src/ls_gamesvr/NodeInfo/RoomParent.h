#ifndef _RoomParent_h_
#define _RoomParent_h_

#include "BlockNode.h"

class UserParent;

class RoomParent : public BlockNode
{
	public:
	enum
	{
		ROOM_INFO	= 100,
	};

	enum RoomLeaveType
	{
		RLT_NORMAL,
		RLT_KICK,
		RLT_BADNETWORK,
		RLT_HIGH_LEVEL,
		RLT_LOW_LEVEL,
		RLT_SYS_ERROR,
	};

	//가상함수의 향연
	public:
	virtual int GetRoomIndex() = 0;
	virtual RoomStyle GetRoomStyle() = 0;
	virtual int GetAverageLevel() = 0;
	virtual int GetGapLevel( int iMyLevel ) = 0;
	virtual ModeType GetModeType() = 0;
	virtual int GetModeSubNum() = 0;
	virtual int GetModeMapNum() = 0; 
	virtual int GetMaxPlayer() = 0;
	virtual int GetJoinUserCnt() = 0;
	virtual int GetPlayUserCnt() = 0;
	virtual int GetRoomNumber() = 0;
	virtual int GetMasterLevel() = 0;
	virtual ioHashString GetRoomName() = 0;
	virtual ioHashString GetMasterName() = 0;
	virtual bool IsRoomMasterID() = 0;
	virtual ioHashString GetRoomPW() = 0;
	virtual bool IsRoomPW() = 0;
	virtual bool IsRoomFull() = 0;
	virtual const bool IsRoomOriginal() = 0;
	virtual bool IsRoomEmpty() = 0;
	virtual bool IsSafetyLevelRoom() const = 0;
	virtual int  GetTeamRatePoint() = 0;
	virtual int  GetPlazaRoomLevel() = 0;
	virtual bool IsTimeCloseRoom() const = 0;
	virtual PlazaType GetPlazaModeType() const = 0;
	virtual int	 GetSubState() const = 0;

	public:
	virtual void OnPlazaRoomInfo( UserParent *pUser ) = 0;

	public:
	RoomParent();
	virtual ~RoomParent();
};
typedef std::vector< RoomParent * > vRoomParent;
typedef vRoomParent::iterator vRoomParent_iter;
#endif