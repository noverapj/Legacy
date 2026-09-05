#pragma once

struct PcRoomCharInfo
{
	int m_iClassType;
	int m_bCharDelete;

	PcRoomCharInfo()
	{
		m_iClassType = 0;
		m_bCharDelete = false;
	}
};

typedef std::vector<PcRoomCharInfo> PcRoomCharInfoVec;

class User;
class ioCharacter;

class ioUserPcRoom
{
public:
	typedef std::vector< ioCharacter*> PCRoomCharVec;


protected:
	PCRoomCharVec m_PCRoomCharVec;
	IntVec m_NewPCRoomCharVec;

	int m_iExcercisePCRoomCharMax;
	bool m_bForcedAuthority;

public:
	void Initialize();
	void SlotClear();

public:
	bool IsNewPcRoomChar( int iCharIndex );
	bool HasCalss( int iClassType );
	
public:
	void InsertPcRoomChar( ioCharacter* pChar );

public:
	int GetUseSlotCount();
	int GetPcRoomCharMax();
	int GetDeleteCount( const PcRoomCharInfoVec& InfoVec );
	int GetHasCount( const PcRoomCharInfoVec& InfoVec );

	const ioCharacter* GetPcRoomChar( UINT iSlotIndex );
	const ioUserPcRoom::PCRoomCharVec& GetRoomCharVec();

public:
	void AllocPCRoomChar( User* pUser, const PcRoomCharInfoVec& InfoVec );
	void DeletePCRoomCharBySlot( User* pUser );
	void CheckNewPcRoomCharSlot( User* pUser );

public:
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, User* pUser, bool bDummyNode = false );

public:
	ioUserPcRoom();
	virtual ~ioUserPcRoom();
};