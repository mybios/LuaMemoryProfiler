/**
 * @author 李锦俊
 * @email mybios@qq.com
 */
#include "LuaMemoryProfiler.h"
#include "lua.hpp"

extern "C"
{
	int luaopen_LuaMemoryProfiler(lua_State* L);
}

int LuaMemoryProfilerLuaTest()
{
	lua_State *L = LuaNewState();
	luaL_openlibs(L);
	luaL_requiref(L, "LuaMemoryProfiler", luaopen_LuaMemoryProfiler, 1);
	lua_pop(L, 1);  // remove lib

	std::string testScript1 = R"(
-- 全局泄露对象
g_leak = {}
-- 捕获一个内存快照，并保存到snapshot1，这个snapshot1是一个integer的id，不会影响lua的虚拟机gc状态
local snapshot1 = LuaMemoryProfiler.capture()
local function runLeak()
    for i = 1, 10 do
        -- 这里发生了内存泄露
		local s = "leak string" .. tostring(i)
        g_leak[i] = {s}
    end
end
runLeak()

-- 在发生泄露后，捕获一个新的内存快照
local snapshot2 = LuaMemoryProfiler.capture()

-- 可以把快照snapshot1和snapshot2保存下来观察，输出lua_snapshot1.xml和lua_snapshot2.xml
LuaMemoryProfiler.save(snapshot1, "lua_snapshot1.xml")
LuaMemoryProfiler.save(snapshot2, "lua_snapshot2.xml")
-- 对比快照1和快照2，生成差异快照，并输出到文件lua_diff_1_2.xml
LuaMemoryProfiler.save(LuaMemoryProfiler.compare(snapshot1, snapshot2), "lua_diff_1_2.xml")
   )";

	int ret = luaL_loadbufferx(L, testScript1.c_str(), testScript1.size(), "LuaScript", 0);
	lua_call(L, 0, 0);

	lua_close(L);

	return 0;
}
