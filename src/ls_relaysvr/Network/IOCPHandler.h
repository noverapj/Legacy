#pragma once
 

class IOCPHandler :
	public CCompletionHandler
{
public:
	IOCPHandler(void);
	virtual ~IOCPHandler(void);

public :
	bool Init(int nworkercount);
};

