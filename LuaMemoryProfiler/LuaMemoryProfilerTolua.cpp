/**
 * @author 李锦俊
 * @email mybios@qq.com
 */
#include "LuaMemoryProfiler.h"
#include <map>
#include "lua.hpp"

extern "C"
{
	// 最大索引
	lua_Integer _maxSnapshotIndex = 0;
	// 在lua调用中创建的快照临时保存在这里
	std::map<lua_Integer, MemorySnapshot> _snapshots;

	// 创建一个内存快照，并返回id
	int lua_captureSnapshot(lua_State* L)
	{
		bool autogc = lua_isnil(L, 1) ? true : lua_toboolean(L, 1);
		MemorySnapshot snapshot;
		if (autogc)
		{
			lua_gc(L, LUA_GCCOLLECT, 0);
		}
		CaptureMemorySnapshot(L, snapshot);
		_snapshots[++_maxSnapshotIndex] = snapshot;
		lua_pushinteger(L, _maxSnapshotIndex);
		return 1;
	}

	// 把一个内存快照id保存到xml文件中
	int lua_saveSnapshotXml(lua_State* L)
	{
		lua_Integer id = luaL_checkinteger(L, 1);
		const char* fileName = luaL_checkstring(L, 2);
		auto iter = _snapshots.find(id);
		if (iter != _snapshots.end())
		{
			WriteSnapshotXml(iter->second, fileName);
			lua_pushboolean(L, 1);
		}
		return 0;
	}

	// 比较两个内存快照（传入两个id），并返回一个新的快照id
	int lua_compareSnapshot(lua_State* L)
	{
		lua_Integer id1 = luaL_checkinteger(L, 1);
		lua_Integer id2 = luaL_checkinteger(L, 2);

		auto iter1 = _snapshots.find(id1);
		auto iter2 = _snapshots.find(id2);
		if (iter1 != _snapshots.end() && iter2 != _snapshots.end())
		{
			MemorySnapshot diff;
			CompareSnapshotDiff(iter1->second, iter2->second, diff);
			_snapshots[++_maxSnapshotIndex] = diff;
			lua_pushinteger(L, _maxSnapshotIndex);
			return 1;
		}
		return 0;
	}

	// 删除所有内存快照
	int lua_clearSnapshots(lua_State* L)
	{
		_snapshots.clear();
		_maxSnapshotIndex = 0;
		return 0;
	}


	static const luaL_Reg api_funcs[] = {
	  {"capture", lua_captureSnapshot},
	  {"save", lua_saveSnapshotXml},
	  {"compare", lua_compareSnapshot},
	  {"clear", lua_clearSnapshots},
	  {NULL, NULL}
	};

	// 导出函数，一般是这么用
/*
luaL_requiref(L, "LuaMemoryProfiler", luaopen_LuaMemoryProfiler, 1);
lua_pop(L, 1);  // remove lib
*/
	int luaopen_LuaMemoryProfiler(lua_State* L)
	{
		luaL_newlib(L, api_funcs);
		return 1;
	}
}