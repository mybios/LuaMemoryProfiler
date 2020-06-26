#include "LuaMemoryProfiler.h"
#include <iostream>
extern "C"
{
#include "ltable.h"
#include "lstate.h"
#include "lobject.h"
#include "lapi.h"
#include "lgc.h"
}
#include "lua.hpp"


LuaMemoryProfiler::~LuaMemoryProfiler()
{
	for (auto iter : _memoryStatBuffer)
	{
		delete iter;
	}
}
LuaMemoryProfiler::LuaMemoryProfiler()
	: _memoryStatBuffer()
{
	// 先初始化一个，确保每次.back都是成功
	_memoryStatBuffer.push_back(new MemoryStatPool());
}
lua_State* L = nullptr;

const MemoryStat& LuaMemoryProfiler::getGlobalMemoryStates()const
{
	return _globalMemoryStates;
}

// 从内存池中分配出MemoryStat
MemoryStat* LuaMemoryProfiler::allocMemoryStat(const char* stackString, size_t stackStringSize)
{
	auto iter = _memoryStates.find(stackString);
	if (iter == _memoryStates.end())
	{
		MemoryStatPool* pool = _memoryStatBuffer.back();
		size_t size = sizeof(MemoryStat) + stackStringSize;
		if (MEMORY_STAT_POOL_SIZE - pool->offset < size)
		{
			_memoryStatBuffer.push_back(new MemoryStatPool());
			pool = _memoryStatBuffer.back();
		}
		MemoryStat* stat = (MemoryStat*)(pool->buff + pool->offset);
		pool->offset += size;
		memcpy(stat->stackString, stackString, stackStringSize);
		iter = _memoryStates.insert(std::make_pair((const char*)stat->stackString, stat)).first;
	}

	return iter->second;
}

#define FRAME_SPLIT_CHAR ","

// 获得堆栈字符串，以,分割
int getTacebackString(lua_State* L, char* outStack, size_t inSize, size_t* outSize)
{
	outStack[0] = 0;
	lua_Debug infos[128];
	int level = 0;
	size_t offset = 0;

	while (level < sizeof(infos) / sizeof(infos[0]) && lua_getstack(L, level, &infos[level]))
	{
		level++;
	}

	for (int i = level - 1; i >= 0; i--)
	{
		lua_Debug& info = infos[i];
		lua_getinfo(L, "nSl", &info);
		offset += snprintf(outStack + offset, inSize - offset, "%s(%s:%d)", info.name ? info.name : "", info.source ? info.source : "", info.currentline);
		if (i > 0)
		{
			outStack[offset] = FRAME_SPLIT_CHAR[0];
			offset++;
		}
	}
	*outSize = offset;

	return level;
}

struct LuaMemoryExtra
{
	//int magicCode;
	MemoryStat* stat;
};

#define getRawMemoryPtr(luaMemoryPtr) (luaMemoryPtr ? (int8_t*)luaMemoryPtr - sizeof(LuaMemoryExtra) : 0)
#define getLuaMemoryExtra(luaMemoryPtr) (LuaMemoryExtra*)getRawMemoryPtr(luaMemoryPtr)
#define getLuaMemoryPtr(extraPtr) (extraPtr ? (int8_t*)extraPtr + sizeof(LuaMemoryExtra) : 0)

void* LuaMemoryProfiler::allocHook(lua_State* L, void* ptr, size_t osize, size_t nsize)
{
	if (ptr)
	{
		_globalMemoryStates.recordDealloc(osize);
		LuaMemoryExtra* oldStat = getLuaMemoryExtra(ptr);
		if (oldStat->stat)
		{
			oldStat->stat->recordDealloc(osize);
		}
	}

	if (nsize == 0)
	{
		free(getRawMemoryPtr(ptr));
		return 0;
	}
	else
	{
		_globalMemoryStates.recordAlloc(nsize);
		// 在前面额外分配多sizeof(LuaMemoryExtra)
		LuaMemoryExtra* extra = (LuaMemoryExtra*)realloc(getLuaMemoryExtra(ptr), sizeof(LuaMemoryExtra) + nsize);
		if (extra == nullptr)
			return nullptr;
		// GCObject 的第一次分配，其中osize是LUA_TYPE
		if (ptr == nullptr && osize)
		{
			if (L)
			{
				char stackString[2048];
				size_t stackStringSize;
				int level = getTacebackString(L, stackString, sizeof(stackString), &stackStringSize);
				MemoryStat* stat = allocMemoryStat(stackString, stackStringSize);

				int luaType = (int)osize;
				stat->allocType |= 1 << luaType;
				stat->recordAlloc(nsize);
				extra->stat = stat;
			}
			else
			{
				extra->stat = nullptr;
			}
		}
		// 非GCObject的第一次分配
		else if (ptr == nullptr)
		{
			extra->stat = nullptr;
		}
		// 多次分配
		//else if(extra->stat)
		//{
		//	extra->stat->recordAlloc(nsize);
		//}
		return getLuaMemoryPtr(extra);
	}
}


size_t getGCObjectRawSize(GCObject* p)
{
	switch (p->tt)
	{
	case LUA_TSHRSTR:
	{
		TString* s = gco2ts(p);
		return s->shrlen;
	}
	break;
	case LUA_TLNGSTR:
	{
		TString* s = gco2ts(p);
		return s->u.lnglen;
	}
	break;
	case LUA_TUSERDATA:
	{
		Udata* u = gco2u(p);
		return (size_t)u->len;
	}
	break;
	case LUA_TTABLE:
	{
		Table* h = gco2t(p);
		return (size_t)(sizenode(h) + h->sizearray);
	}
	break;
	}
	return 0;
}

MemorySnapshot& addMemoryStatFrameToSnapshot(const std::string& frameName, const MemoryStat& stat, MemorySnapshot& snapshot)
{
	MemorySnapshot* outputSnapshot = nullptr;
	auto iter = snapshot.children.find(frameName);
	if (iter != snapshot.children.end())
	{
		outputSnapshot = &iter->second;
	}
	else
	{
		outputSnapshot = &snapshot.children.insert(std::make_pair(frameName, MemorySnapshot())).first->second;
		outputSnapshot->name = frameName;
		outputSnapshot->stat.allocType = stat.allocType;
	}
	outputSnapshot->stat += stat;

	return *outputSnapshot;
}

void addMemoryStatToSnapshot(const MemoryStat& stat, MemorySnapshot& snapshot)
{
	snapshot.stat += stat;

	const char* p = stat.stackString;
	size_t offset = 0;
	// 分隔符
	const char splitChar = FRAME_SPLIT_CHAR[0];
	MemorySnapshot* currentSnapshot = &snapshot;
	while (true)
	{
		if (p[offset] == 0 || p[offset] == splitChar)
		{
			std::string frameName(p, offset);
			currentSnapshot = &addMemoryStatFrameToSnapshot(frameName, stat, *currentSnapshot);

			if (p[offset] == 0)
				break;

			p += offset + 1;
			offset = -1;
		}
		offset++;
	}

}

void LuaMemoryProfiler::captureMemorySnapshot(lua_State* L, MemorySnapshot& snapshot)
{
	snapshot.name = "root";
	// 先重置所有gcObjectRawSize，因为table可能动态增长
	for (auto& iter : _memoryStates)
	{
		iter.second->gcObjectRawSize = 0;
	}
	// 统计所有对象的大小，并统计记录到MemoryStat中
	if (L)
	{
		GCObject* p = G(L)->allgc;
		while (p != NULL)
		{
			LuaMemoryExtra* extra = getLuaMemoryExtra(p);
			if (extra->stat)
			{
				extra->stat->gcObjectRawSize += (int)getGCObjectRawSize(p);
			}
			p = p->next;
		}
	}
	// 汇总所有调用栈按行分割后的数据
	for (auto& iter : _memoryStates)
	{
		addMemoryStatToSnapshot(*iter.second, snapshot);
	}
}

extern "C"
{

	// diff = b - a
	void CompareSnapshotDiff(const MemorySnapshot& a, const MemorySnapshot& b, MemorySnapshot& diff)
	{
		diff.name = a.name.size() ? a.name : b.name;
		diff.stat = b.stat;
		diff.stat -= a.stat;

		for (const auto& itera : a.children)
		{
			MemorySnapshot& c = diff.children.insert(std::make_pair(itera.first, MemorySnapshot())).first->second;
			const auto& iterb = b.children.find(itera.first);
			// 相同
			if (iterb != b.children.end())
			{
				CompareSnapshotDiff(itera.second, iterb->second, c);
			}
			// 删除
			else
			{
				CompareSnapshotDiff(itera.second, MemorySnapshot(), c);
			}
		}
		// 新增
		for (const auto& iterb : b.children)
		{
			const auto& itera = a.children.find(iterb.first);
			// 新增
			if (itera == a.children.end())
			{
				MemorySnapshot& c = diff.children.insert(std::make_pair(iterb.first, MemorySnapshot())).first->second;
				CompareSnapshotDiff(MemorySnapshot(), iterb.second, c);
			}
		}
	}

	/*

		// 当前内存存量字节（分配时累加，释放时减少）
		int allocBytes;
		// 当前内存存量次数（分配时+1，释放时-1）
		int allocCount;
		// 累计内存分配字节（分配时累加）
		int sumAllocBytes;
		// 累计内存分配次数（分配时+1）
		int sumAllocCount;
		// 累计内存释放字节（释放时减去）
		int sumDeallocBytes;
		// 累计内存释放次数（分配时-1）
		int sumDeallocCount;
		//  数据类型
		int allocType;
		// 分配时的堆栈
		char stackString[1];
	*/
	void _writeSnapshotXml(const MemorySnapshot& a, FILE* f, int level)
	{
		std::string tab(level, '\t');
		fprintf(f, "%s<node name=\"%s\" rawSize=\"%d\" allocBytes=\"%d\" allocCount=\"%d\" sumAllocBytes=\"%d\" sumAllocCount=\"%d\" sumDeallocBytes=\"%d\" sumDeallocCount=\"%d\" allocType=\"%d\">\n"
			, tab.c_str()
			, a.name.c_str()
			, a.stat.gcObjectRawSize
			, a.stat.allocBytes
			, a.stat.allocCount
			, a.stat.sumAllocBytes
			, a.stat.sumAllocCount
			, a.stat.sumDeallocBytes
			, a.stat.sumDeallocCount
			, a.stat.allocType
		);
		std::vector<const MemorySnapshot*> children;
		for (const auto& iter : a.children)
		{
			const auto& stat = iter.second;
			if (stat.stat.allocCount != 0
				|| stat.stat.sumAllocCount != 0
				|| stat.stat.sumDeallocCount != 0
				)
			{
				children.push_back(&stat);
			}
		}
		// 排序
		struct
		{
			bool operator()(const MemorySnapshot*& lhs, const MemorySnapshot*& rhs)
			{
				return lhs->stat.gcObjectRawSize + lhs->stat.allocBytes > rhs->stat.gcObjectRawSize + rhs->stat.allocBytes;
			}
		} StoreyBase;
		std::sort(children.begin(), children.end(), StoreyBase);

		level++;
		for (const auto& iter : children)
		{
			_writeSnapshotXml(*iter, f, level);
		}
		level--;
		fprintf(f, "%s</node>\n", tab.c_str());
	}

	void WriteSnapshotXml(const MemorySnapshot& a, const char* xmlFile)
	{
		FILE* f;
		auto e = fopen_s(&f, xmlFile, "wb");
		if (!f)
			return;
		fprintf(f, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
		_writeSnapshotXml(a, f, 0);
		fclose(f);
	}


	class LuaMemoryProfilerEx : public LuaMemoryProfiler
	{
	public:
		// 额外保存一个L，用来给LuaMemoryAllocHook使用
		lua_State* L = nullptr;
	};

	void* LuaMemoryAllocHook(void* ud, void* ptr, size_t osize, size_t nsize)
	{
		LuaMemoryProfilerEx* profiler = (LuaMemoryProfilerEx*)ud;
		return profiler->allocHook(profiler->L, ptr, osize, nsize);
	}

	lua_State* LuaNewState()
	{
		LuaMemoryProfilerEx* profiler = new LuaMemoryProfilerEx();
		lua_State* L = lua_newstate(LuaMemoryAllocHook, profiler);
		profiler->L = L;
		return L;
	}

	LuaMemoryProfiler* GetMemoryProfilerFromState(lua_State* L)
	{
		LuaMemoryProfiler* profiler;
		lua_Alloc alloc = lua_getallocf(L, (void**)&profiler);
		if (alloc == &LuaMemoryAllocHook)
		{
			return profiler;
		}
		else
		{
			return nullptr;
		}
	}

	void CloseLuaState(lua_State* L)
	{
		LuaMemoryProfiler* profiler = GetMemoryProfilerFromState(L);
		lua_close(L);
		if (profiler)
		{
			delete profiler;
		}
	}
	// 创建一个lua内存快照到snapshot
	void CaptureMemorySnapshot(lua_State* L, MemorySnapshot& snapshot)
	{
		LuaMemoryProfiler* profiler = GetMemoryProfilerFromState(L);
		if (!profiler)
			return;
		profiler->captureMemorySnapshot(L, snapshot);
	}
}