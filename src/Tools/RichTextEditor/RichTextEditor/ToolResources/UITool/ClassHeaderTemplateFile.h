//----------------------------------------------------------------------------------
/*
Filename	: %FILENAME%
Author		: UITool
Date		: %DATE%
*/
//----------------------------------------------------------------------------------

#ifndef %DEFINE%
#define %DEFINE%

#include "%BASECLASSSNAME%.h"
%INCLUDE%
%MEMBERINCLUDE%

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
%CLASSSNAME%
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class %CLASSSNAME% : public %BASECLASSSNAME%
{
protected:

	enum
	{
%ID%
	};

public:

	//----------------------------------------------------------------------------------
	/*
	积己 / 家戈
	*/
	%CLASSSNAME%() : %BASECLASSSNAME%() 
	{
%MEMBERINIT%
	}

	virtual ~%CLASSSNAME%() 
	{
	}

	//----------------------------------------------------------------------------------
	/*
	楷搬 / 楷搬 产扁
	*/
	virtual void	iwm_create()
	{
		ioWndEX::iwm_create();

%MEMBERCONNECT%

%EVENTCONNECT%
	}

	virtual void	iwm_destroy()
	{
%EVENTDISCONNECT%

		ioWndEX::iwm_destroy();
	}

	//----------------------------------------------------------------------------------
	/*
	Event Virtual Functions
	*/
%EVENTFUNCTION%

protected:

%MEMBERDECLARE%
};


#endif // %DEFINE%