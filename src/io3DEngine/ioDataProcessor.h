

#ifndef _ioDataProcessor_h_
#define _ioDataProcessor_h_

class ioStream;

class __EX ioDataProcessor
{
public:
	virtual void NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded ) = 0;

public:
	ioDataProcessor(){}
	virtual ~ioDataProcessor(){}
};

#endif