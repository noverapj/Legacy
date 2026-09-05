#ifndef __ioUserPet_h__
#define __ioUserPet_h__

class cSerialize;

class ioUserPet : public ioDBDataController
{
public:
	enum
	{
#if defined ( DIVIDE_PET)
		MAX_SLOT     = 50,
#else
		MAX_SLOT     = 20,
#endif
		NONE_EQUIP	 = -1
	};

	typedef struct tagPetSlot
	{
		int m_iIndex;
		int m_iPetCode;
		int m_iPetRank;
		int m_iCurLevel;
		int m_iCurExp;
		int m_iMaxExp; //DB에서 처음 가져올때 체워넣고 레벨업시 다시 체워넣고
#if defined( DIVIDE_PET ) 
		int m_iClassType;
#endif
		bool m_bEquip;
		bool m_bChange;

		tagPetSlot()
		{
			Init();
		}

		void Init()
		{
			m_iIndex = 0;		//펫의 db인덱스 ( 빈 위치 == 0, index번호를 받아야하는 슬롯 == NEW_INDEX )
			m_iPetCode = 0;
			m_iPetRank = 0;
			m_iCurLevel = 0;
			m_iMaxExp = 0;
			m_iCurExp = 0;
#if defined( DIVIDE_PET ) 
			m_iClassType = 0;
#endif
			m_bEquip = false;
			m_bChange = false;
		}
	}PETSLOT;

protected:
	//펫 슬롯 벡터
	typedef std::vector< PETSLOT > vPetSlotList;
	vPetSlotList m_vPetSlotList;

	//m_vPetSlotList의 인덱스
	int m_iEquipPetListIdx;

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );	
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	bool GetPetInfo( const int& iPetIndex, PETSLOT& rkPet, int& iListIndx );
	bool GetPetInfo( const int& iPetIndex, int& iListIndx );
	bool GetPetInfo( const int& iPetIndex, PETSLOT& rkPet );

	int  GetPetVecIndex( const int& iPetIndex );
	int  GetPetCode( const int iPetIndex );

	void SetPetInfo( const PETSLOT& rkPet );

	bool EquipPet( const int& iPetIndex, PETSLOT& rkPet );
	bool ClearPetEquip( const int& iPetIndex ); 

	int PetNurture( User* pUser, const int& iPetIndex, const int& iEtcItemCode, PETSLOT& rkPetInfo );

	bool AddData( PETSLOT &rkNewSlot, CQueryData &query_data );
	void AddPet( const int& iNewIndex, const int& iPetCode, const int& iPetRank, const int& iPetLevel );
	bool DeleteData( const int& iPetIndex );

	bool GetEquipPetInfo( PETSLOT &rkNewSlot );
	
	bool CheckPetHavePossible( );
	int CheckEmptyVecIndex();

	void SetPetData( const PETSLOT &rkPetSlot );

public:
	int CheckEquipPetVecIndex();
	void CheckEquipIndexError();

public:
	void TestInput();

public:
	ioUserPet();
	virtual ~ioUserPet();
};

#endif