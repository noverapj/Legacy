#ifndef _cLua_h_
#define _cLua_h_

struct lua_State;

#define LuaGlue extern "C" int
extern "C" {
typedef int (*LuaFunctionType)(struct lua_State *pLuaState);
};

class cLua
{
public:
	cLua();
	virtual ~cLua();

	bool		RunScript(const char *pFilename);
	bool		RunString(const char *pCommand);
	bool        RunMemory(const char *pBuffer, const int iBufferSize);
	const char *GetErrorString(void);
	bool		AddFunction(const char *pFunctionName, LuaFunctionType pFunction);
	const char *GetStringArgument(int num, const char *pDefault=NULL);
	double		GetNumberArgument(int num, double dDefault=0.0);
	void		PushString(const char *pString);
	void		PushNumber(double value);

	void		SetErrorHandler(void(*pErrHandler)(const char *pError)) {m_pErrorHandler = pErrHandler;}

	lua_State	*GetScriptContext(void)		{return m_pScriptContext;}

	static int LuaExceptionProcess(lua_State * pLua);

private:
	lua_State	*m_pScriptContext;
	void(*m_pErrorHandler)(const char *pError);

};

#endif