

#ifndef _ioPatchDefault_h_
#define _ioPatchDefault_h_

class ioPatch;

class __EX ioPatchDefault : public ioPatch
{
public:
	virtual bool UpdateLevel();
	virtual bool UpdateLevelSecond();
	virtual bool UpdateLevelThird();

protected:
	bool CheckBorders();

public:
	ioPatchDefault( ioLandScape *pLand, int nX, int nY );
	virtual ~ioPatchDefault();
};

#endif