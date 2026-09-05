#ifndef __ioRelativeGradeManager_h__
#define __ioRelativeGradeManager_h__

#include "../Util/Singleton.h"

class User;
class ioRelativeGradeManager : public Singleton< ioRelativeGradeManager >
{
protected:
	enum
	{
		ONE_STAR   = 1,
		TWO_STAR   = 2,
		THREE_STAR = 3,
		FOUR_STAR  = 4,
		FIVE_STAR  = 5,
	};

protected:
	struct RelativeGradePresent
	{
		ioHashString m_szSendID;

		short m_iPresentType;
		short m_iPresentState;
		short m_iPresentMent;

		int   m_iPresentPeriod;
		int   m_iPresentValue1;
		int   m_iPresentValue2;
		int   m_iPresentValue3;
		int   m_iPresentValue4;

		RelativeGradePresent()
		{
			m_iPresentType = m_iPresentState = m_iPresentMent = 0;
			m_iPresentPeriod = m_iPresentValue1 = m_iPresentValue2 = m_iPresentValue3 = m_iPresentValue4 = 0;
		}
	};
	typedef std::vector< RelativeGradePresent > vRelativeGradePresent;
	vRelativeGradePresent m_RelativeGradePresent[5];

protected:
	int m_iReduceRate;

public:
	void LoadINI();

public:
	static ioRelativeGradeManager& GetSingleton();

public:
	int GetReduceRate() { return m_iReduceRate; }
	void SendGeneralRewardPresent( User *pUser, int iRelativeGrade );

public:
	ioRelativeGradeManager(void);
	virtual ~ioRelativeGradeManager(void);
};

#define g_RelativeGradeMgr ioRelativeGradeManager::GetSingleton()

#endif // __ioRelativeGradeManager_h__