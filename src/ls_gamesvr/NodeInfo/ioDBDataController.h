#ifndef __ioDBDataController_h__
#define __ioDBDataController_h__

class User;
class CQueryResultData;

class ioDBDataController
{
public:
	enum
	{
		NEW_INDEX  = 0xffffffff,
	};

protected:
	User    *m_pUser;

public:
	virtual void Initialize( User *pUser ) = 0;
	virtual bool DBtoNewIndex( DWORD dwIndex ) = 0;
	virtual void DBtoData( CQueryResultData *query_data ) = 0;
	virtual void SaveData() = 0;
	virtual void FillMoveData( SP2Packet &rkPacket ) = 0;
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  ) = 0;

protected:
	ioDBDataController(void){ m_pUser = NULL; }
	virtual ~ioDBDataController(void){}
};

#endif // __ioDBDataController_h__