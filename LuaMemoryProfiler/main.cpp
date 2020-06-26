/**
 * @author 李锦俊
 * @email mybios@qq.com
 */
#include "LuaMemoryProfiler.h"

int LuaMemoryProfilerLuaTest();
int LuaMemoryProfilerCppTest();
int main()
{
	LuaMemoryProfilerCppTest();
	LuaMemoryProfilerLuaTest();
	return 0;
}
