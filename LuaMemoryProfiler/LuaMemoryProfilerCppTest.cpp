/**
 * @author 李锦俊
 * @email mybios@qq.com
 */
#include "LuaMemoryProfiler.h"
#include "lua.hpp"


int LuaMemoryProfilerCppTest()
{
	lua_State *L = LuaNewState();
	luaL_openlibs(L);

	LuaMemoryProfiler &profiler = *GetMemoryProfilerFromState(L);

	MemorySnapshot snapshot0;
	lua_gc(L, LUA_GCCOLLECT, 0);
	profiler.captureMemorySnapshot(L, snapshot0);


	std::string testScript1 = R"(g_leak = {}
local function runLeak()
	for i = 1, 10 do
		g_leak[i] = {i}
	end
end
runLeak()
   )";

	std::string testScript2 = R"(for i = 11, 15 do
		g_leak[i] = {i}
	end
   )";

	std::string testScript3 = R"(local function removeSomething()
	for i = 1, 10 do
		g_leak[i] = nil
	end
end
removeSomething()
   )";

	int ret;

	ret = luaL_loadbufferx(L, testScript1.c_str(), testScript1.size(), "testScript1", 0);
	lua_call(L, 0, 0);

	MemorySnapshot snapshot1;
	lua_gc(L, LUA_GCCOLLECT, 0);
	profiler.captureMemorySnapshot(L, snapshot1);

	ret = luaL_loadbufferx(L, testScript2.c_str(), testScript2.size(), "testScript2", 0);
	lua_call(L, 0, 0);


	MemorySnapshot snapshot2;
	lua_gc(L, LUA_GCCOLLECT, 0);
	profiler.captureMemorySnapshot(L, snapshot2);

	ret = luaL_loadbufferx(L, testScript3.c_str(), testScript3.size(), "testScript3", 0);
	lua_call(L, 0, 0);


	MemorySnapshot snapshot3;
	lua_gc(L, LUA_GCCOLLECT, 0);
	profiler.captureMemorySnapshot(L, snapshot3);

	WriteSnapshotXml(snapshot0, "snapshot0.xml");
	WriteSnapshotXml(snapshot1, "snapshot1.xml");
	WriteSnapshotXml(snapshot2, "snapshot2.xml");
	WriteSnapshotXml(snapshot3, "snapshot3.xml");


	MemorySnapshot diff1_2, diff2_3, diff1_3;

	CompareSnapshotDiff(snapshot1, snapshot2, diff1_2);
	CompareSnapshotDiff(snapshot2, snapshot3, diff2_3);
	CompareSnapshotDiff(snapshot1, snapshot3, diff1_3);
	WriteSnapshotXml(diff1_2, "diff1_2.xml");
	WriteSnapshotXml(diff2_3, "diff2_3.xml");
	WriteSnapshotXml(diff1_3, "diff1_3.xml");

	lua_close(L);

	MemorySnapshot snapshot4;
	MemorySnapshot diff1_4, diff0_4;

	profiler.captureMemorySnapshot(0, snapshot4);
	WriteSnapshotXml(snapshot4, "snapshot4.xml");

	CompareSnapshotDiff(snapshot1, snapshot4, diff1_4);
	WriteSnapshotXml(diff1_4, "diff1_4.xml");

	CompareSnapshotDiff(snapshot0, snapshot4, diff0_4);
	WriteSnapshotXml(diff0_4, "diff0_4.xml");

	return 0;
}
