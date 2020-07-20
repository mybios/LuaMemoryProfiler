/**
 * @author 李锦俊
 * @email mybios@qq.com
 */
#pragma once
extern "C"
{
	typedef struct lua_State lua_State;
}
#include <unordered_map>
#include <list>
#include <vector>
#include <algorithm>


// 相同调用栈下的内存分配/释放统计
struct MemoryStat
{
	MemoryStat()
		: allocBytes(0)
		, allocCount(0)
		, sumAllocBytes(0)
		, sumAllocCount(0)
		, sumDeallocBytes(0)
		, sumDeallocCount(0)
		, allocType(0)
		, stackString{ 0 }
		, gcObjectRawSize(0)
	{

	}
	//  记录gc对象额外占用的空间，例如table的数组slot
	int gcObjectRawSize;
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
	//  数据类型，用位来表达不同类型混合在一个地方创建类似这样（如果包含表格和字符串，则等于1 << LUA_TTABLE | 1 << LUA_TSTRING）
	int allocType;
	// 分配时的堆栈
	char stackString[1];

	MemoryStat& operator += (const MemoryStat& other)
	{
		this->allocBytes += other.allocBytes;
		this->allocCount += other.allocCount;
		this->sumAllocBytes += other.sumAllocBytes;
		this->sumAllocCount += other.sumAllocCount;
		this->sumDeallocBytes += other.sumDeallocBytes;
		this->sumDeallocCount += other.sumDeallocCount;
		this->gcObjectRawSize += other.gcObjectRawSize;
		this->allocType |= other.allocType;
		return *this;
	}
	MemoryStat& operator -= (const MemoryStat& other)
	{
		this->allocBytes -= other.allocBytes;
		this->allocCount -= other.allocCount;
		this->sumAllocBytes -= other.sumAllocBytes;
		this->sumAllocCount -= other.sumAllocCount;
		this->sumDeallocBytes -= other.sumDeallocBytes;
		this->sumDeallocCount -= other.sumDeallocCount;
		this->gcObjectRawSize -= other.gcObjectRawSize;
		this->allocType |= other.allocType;
		return *this;
	}
	// 记录一次内存分配操作
	void recordAlloc(size_t allocBytes)
	{
		this->allocBytes += (int)allocBytes;
		this->allocCount++;
		this->sumAllocBytes += (int)allocBytes;
		this->sumAllocCount++;
	}

	// 记录一次内存释放操作
	void recordDealloc(size_t deallocBytes)
	{
		this->allocBytes -= (int)deallocBytes;
		this->allocCount--;
		this->sumDeallocBytes += (int)deallocBytes;
		this->sumDeallocCount++;
	}
};

// 一个内存快照
struct MemorySnapshot
{
	// 节点名（调用栈的一行）
	std::string name;
	// 这一行的数据
	MemoryStat stat;
	// 统计的子节点
	std::unordered_map<std::string, MemorySnapshot> children;
};


class LuaMemoryProfiler
{
	struct MemoryStatPool
	{
		// 当前用到哪个字节
		size_t offset = 0;
		size_t size = 0;
		// 一次性分配这么大
		uint8_t buff[1] = { 0 };
	};
	template<typename T>
	struct stack_string_equal_to
	{
		bool operator()(const T& _Left, const T& _Right) const
		{
			return strcmp(_Left, _Right) == 0;
		}
	};

	static const size_t _FNV_prime = 16777619U;

	template<typename T>
	struct stack_string_hash
	{
		size_t operator ()(const T& v) const
		{
			size_t len = strlen(v);
			size_t _Val = 0;
			for (size_t _Idx = 0; _Idx < len; ++_Idx) {
				_Val ^= static_cast<size_t>(v[_Idx]);
				_Val *= _FNV_prime;
			}
			return _Val;
		}
	};

	// 缓存所有
	std::list<MemoryStatPool*> _memoryStatBuffer;
	// 以lua调用栈作为key，以MemoryStat作为value记录所有内存分配情况。
	std::unordered_map<const char*, MemoryStat*, stack_string_hash<const char*>, stack_string_equal_to<const char*>> _memoryStates;
	// 全局内存状态
	MemoryStat _globalMemoryStates;
public:
	virtual ~LuaMemoryProfiler();
	LuaMemoryProfiler();
	const MemoryStat& getGlobalMemoryStates()const;
	// 从内存池中分配出MemoryStat
	MemoryStat* allocMemoryStat(const char* stackString, size_t stackStringSize);
	struct LuaMemoryExtra
	{
		//int magicCode;
		MemoryStat* stat;
	};
	void* allocHook(lua_State* L, void* ptr, size_t osize, size_t nsize);
	// 捕获一个内存快照
	void captureMemorySnapshot(lua_State* L, MemorySnapshot& snapshot);
};


extern "C"
{
	// 创建带内存剖析器的lua状态
	lua_State* LuaNewState();
	// 释放带内存剖析器的lua状态
	void CloseLuaState(lua_State* L);
	// 从lua状态里获得LuaMemoryProfiler
	LuaMemoryProfiler* GetMemoryProfilerFromState(lua_State* L);

	// 创建一个lua内存快照到snapshot
	void CaptureMemorySnapshot(lua_State* L, MemorySnapshot& snapshot);
	// 对两个内存快照进行对比，并得到一个新的快照diff
	// diff = b - a
	void CompareSnapshotDiff(const MemorySnapshot& a, const MemorySnapshot& b, MemorySnapshot& diff);
	// 把快照保存到文件中
	void WriteSnapshotXml(const MemorySnapshot& a, const char* xmlFile);
}
