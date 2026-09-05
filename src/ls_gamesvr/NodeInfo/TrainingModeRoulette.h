
#include "../Util/IORandom.h"

struct GROUPRANGE
{
	int m_min;
	int m_max;
	GROUPRANGE() : m_min( 0 ), m_max( 0 ){}
	void Init()
	{
		m_min = m_max = 0;
	}
};

struct ANGLEDATA
{
	int m_maxRealPercent;
	int m_angle_min;
	int m_angle_max;
	int m_angle_PresentPeriod;
	int	m_angle_PresentType;
	int	m_angle_PresentMent;
	int	m_angle_PresentState;
	int	m_angle_PresentValue1;
	int	m_angle_PresentValue2;

	ANGLEDATA() : m_maxRealPercent(0), m_angle_min(0), m_angle_max(0), m_angle_PresentPeriod(0), m_angle_PresentType(0)
				, m_angle_PresentMent(0), m_angle_PresentState(0), m_angle_PresentValue1(0), m_angle_PresentValue2(0){}

	void Init()
	{
		m_maxRealPercent = m_angle_min = m_angle_max = m_angle_PresentPeriod = m_angle_PresentType = m_angle_PresentMent = 0;
		m_angle_PresentState = m_angle_PresentValue1 = m_angle_PresentValue2 = 0;
	}

	ANGLEDATA& operator=( ANGLEDATA& rhs )
	{
		m_maxRealPercent = rhs.m_maxRealPercent;
		m_angle_min = rhs.m_angle_min;
		m_angle_max = rhs.m_angle_max;
		m_angle_PresentPeriod = rhs.m_angle_PresentPeriod;
		m_angle_PresentType = rhs.m_angle_PresentType;
		m_angle_PresentMent = rhs.m_angle_PresentMent;
		m_angle_PresentState = rhs.m_angle_PresentState;
		m_angle_PresentValue1 = rhs.m_angle_PresentValue1;
		m_angle_PresentValue2 = rhs.m_angle_PresentValue2;
		return *this;
	}
};

class CEventRoulette
{
public:
	CEventRoulette();
	~CEventRoulette();

	void Init();
	void InitMember();
	void Destroy();

	enum
	{
		ROULETTE_START_SUCCESS			= 0,
		ROULETTE_START_NOT_ENOUGH_COIN	= 1,
		ROULETTE_JOIN_SUCCESS			= 2,
		ROULETTE_JOIN_TIME_OVER			= 3,
		ROULETTE_JOIN_ALREADY			= 4,
	};

	// Table Date
private:
	int		m_iBoardingTime;	// 탑승 가능 시간.
	int		m_iSpinTime;		// 돌림판 돌아가는 전체 시간.
	int		m_iCoinCount;		// 돌림판 돌리기위한 코인 필요 갯수.
	std::vector< GROUPRANGE > m_vecGroupRange;
	std::vector< std::vector< ANGLEDATA > > m_vecGroupAngle;

private:
	BOOL	m_bStart;		// 돌림판 돌면 True
	DWORD	m_dwStartTime;	// 돌림판 시작한 시간.
	DWORD	m_dwJoinEndTime;// 돌림판 탑승 종료 시간.
	DWORD	m_dwEndTime;	// 돌림판 끝나는 시간.
	int		m_iAngle;

	IORandom  m_Random;
	ANGLEDATA	m_kResultPresent;	// 선물 데이터
	std::map< int, User* > m_mapBoardingUser;	// 탑승 유저
	
public:
	void SetBoardingTime( const int iBoardingTime );
	const int GetBoardingTime(){ return m_iBoardingTime; }
	void SetSpinTime( const int iSpinTime );
	const int GetSpinTime(){ return m_iSpinTime; }
	void SetCoinCount( const int iCoinCount );
	const int GetUseCoinCount(){ return m_iCoinCount; }

	void InsertGroupRange( GROUPRANGE& rGroupRange );
	void InsertAngleData( std::vector< ANGLEDATA >& rAngleData );
	
	void SetState( const BOOL bState ){ m_bStart = bState; }
	const BOOL GetState(){ return m_bStart; }
	void SetStartTime( const DWORD dwCurrentTime ){ m_dwStartTime = dwCurrentTime; }
	const DWORD GetStartTime(){ return m_dwStartTime; }
	void SetJoinEndTime( const DWORD dwTime ){ m_dwJoinEndTime = dwTime; }
	const DWORD GetJoinEndTime(){ return m_dwJoinEndTime; }
	void SetEndTime( const DWORD dwTime ){ m_dwEndTime = dwTime; }
	const DWORD GetEndTime(){ return m_dwEndTime; }

public:
	void SetRouletteStart( User* pUser );
	BOOL SetRouletteJoin( User* pUser );
	void SetRouletteEnd( User* pSend );
	const int GetBoardingMemberCount();
	void GetBoardingMember( std::vector< User* >& rMember );

	int GetGroupRangePosition( const int iRoomUserCount );
	const int GetNewAngle( const int iPos );
	void SetAngle( const int iAngle ){ m_iAngle = iAngle; }
	const int GetAngle(){ return m_iAngle; }
	ANGLEDATA GetPresentInfo(){ return m_kResultPresent; }
	BOOL GetRouletteMaster( ioHashString& name );
	BOOL RemoveUser( User* pUser );
	void ModeInfo( SP2Packet& rkPacket );
};
