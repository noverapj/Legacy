#ifndef __ioExerciseCharIndexManager_h__
#define __ioExerciseCharIndexManager_h__

#include "../Util/Singleton.h"

#define EXERCISE_NONE                   0x00         // 구매 용병
#define EXERCISE_GENERAL                0x01         // 일반 체험 용병
#define EXERCISE_PCROOM                 0x02         // PC방 체험 용병
#define EXERCISE_EVENT                  0x03         // 레벨 제한 없이 본부에서만 사용 가능한 체험 용병( ExerciseSoldierEvent )
#define EXERCISE_RENTAL                 0x04         // 대여한 용병
#define EXERCISE_ARENA_MODE				0x05         // 2018-07-25 by bckim, 아레나 모드 추가
class ioExerciseCharIndexManager : public Singleton< ioExerciseCharIndexManager >
{
protected:
	enum
	{
		// 체험용병 index는 4284967295 ~ 4294967294 까지 천만개를 사용.
		// 실제용병 index가 4284967295 가까워지면 확장 고려할 것
		MAX_EXERCISE_CHAR_INDEX = 0xfffffffe,
		MIN_EXERCISE_CHAR_INDEX = 0xff67697f,
	};
protected:
	IntVec m_vIndexAdd;
	int    m_iStartAdd;
	int    m_iEndAdd;

public:
	void  Init( const DWORD dwServerIndex );
	DWORD Pop();
	void  Add(DWORD dwExerciseIndex );
	bool  IsHave();
	bool  IsRight( DWORD dwIndex );
	
public:
	ioExerciseCharIndexManager(void);
	virtual ~ioExerciseCharIndexManager(void);
};

#define g_ExerciseCharIndexMgr ioExerciseCharIndexManager::GetSingleton()

#endif // __ioExerciseCharIndexManager_h__


