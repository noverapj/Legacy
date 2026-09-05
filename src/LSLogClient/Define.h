#ifndef ___DEFINE_H__
#define ___DEFINE_H__

#define ID_NUMBER       12
#define ID_NUM_PLUS_ONE 13
#define PW_NUMBER       12
#define PW_NUM_PLUS_ONE 13
#define PW_ENCRYPT_NUMBER	24
#define PW_ENCRYPT_PLUS_ONE 25

#define MAX_CHARACTER   3       //유저가 소유할수 있는 최대 캐릭터 수  
#define MAX_ITEMSLOT    3       //유저가 소유할수 있는 최대 아이템 슬롯 수. 
#define MAX_SLOTBAG     20      //한개의 슬롯에 들어갈수있는 최대값.
#define MAX_USE_SLOTBAG 16		//한개 슬롯에서 현재 사용가능 최대값

#define MAX_RIGHT_SLOT_SIZE 20	//아이템 도감 별 최대 인덱스 사이즈

#define MAX_PLAYER		16

#define KINDRED_HUMAN	1
#define KINDRED_ELF		2
#define KINDRED_DWARF	3

#define	EQUIP_UNKNOWN	1000
#define EQUIP_WEAPON    0
#define EQUIP_ARMOR     1
#define EQUIP_HELM      2
#define EQUIP_CLOAK     3
#define EQUIP_OBJECT	4
#define EQUIP_WEAR      5
#define MAX_EQUIP_SLOT	6		//Character EquipSlot의 최대사이즈

#define MAX_CHAR_DBITEM_SLOT   4	//InventorySlot의 최대사이즈

#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }
#define SAFEDELETEARRAY(x)	if(x != NULL) { delete [] x; x = NULL; }

struct Vector3
{
	float x,y,z;

	Vector3(){}
	Vector3( float _x, float _y, float _z )
	{
		x = _x;
		y = _y;
		z = _z;
	}
};

typedef std::vector< Vector3 > Vector3Vec;
typedef std::deque< Vector3 > Vector3Deq;

enum TeamType
{
	TEAM_NONE,
	TEAM_RED,
	TEAM_BLUE
};

enum WinTeamType
{
	WTT_NONE,
	WTT_RED_TEAM,
	WTT_BLUE_TEAM,
	WTT_DRAW,
	WTT_VICTORY_RED_TEAM,
	WTT_VICTORY_BLUE_TEAM
};

enum ModeType
{
	MT_SYMBOL = 1,
	MT_CATCH  = 2,
	MT_KING = 3,
	MAX_MODE_TYPE
};

#define MAX_MODE ( MAX_MODE_TYPE - 1 )

typedef std::vector<int> IntVec;
typedef std::vector<DWORD> DWORDVec;

extern LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );
extern char *_i64toStr(__int64 i64Num);

#endif