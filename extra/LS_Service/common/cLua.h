#pragma once

#define LuaGlue extern "C" int


struct lua_State;

extern "C" {
	typedef int (*LuaFunctionType)(struct lua_State *pLuaState);
};

class cLua
{
public:
	cLua();
	virtual ~cLua();

public:
	bool		RunScript(const TCHAR *fileName);
	bool		RunString(const TCHAR *command);

	bool		AddFunction(const TCHAR *funcName, LuaFunctionType funcType);

	const TCHAR* GetStringArgument(int num, const TCHAR* value=NULL);
	double		GetNumberArgument(int num, double value=0.0);

	void		CallFunction(const TCHAR* funcName); 
	void		PushString(const TCHAR* arg);
	void		PushNumber(double arg);
	double		GetReturnValue(int args);

	lua_State	*GetScriptContext(void)	{	return m_pLuaContext;	}

public:
	const TCHAR* GetErrorString(void);

	void		SetErrorHandler(void(*pErrorHandler)(const TCHAR *errorCode));
	void		ErrorReport( const TCHAR *format, ... );

public:
	const TCHAR* ToTCHAR(const char* string);
	//WCHAR*		Utf8ToUnicode(const char* string);
	const char*	ToANSI(const TCHAR* string);
	const TCHAR* ToUnicode(const char* string);

private:
	BYTE*		GetFreeBuffer();

private:
	int			m_nBufferIndex;

private:
	lua_State*	m_pLuaContext;
	void		(*m_pErrorHandler)(const TCHAR *errorCode);
};
