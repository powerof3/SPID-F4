#pragma once

#define WIN32_LEAN_AND_MEAN

#define NOMB
#define NOMINMAX
#define NOSERVICE

#include <ranges>
#include <set>
#include <shared_mutex>
#include <unordered_set>

#pragma warning(push)
#include "F4SE/F4SE.h"
#include "RE/Fallout.h"

#ifdef NDEBUG
#	include <spdlog/sinks/basic_file_sink.h>
#else
#	include <spdlog/sinks/msvc_sink.h>
#endif

#include <ClibUtil/distribution.hpp>
#include <ClibUtil/rng.hpp>
#include <ClibUtil/simpleINI.hpp>
#include <ClibUtil/singleton.hpp>
#include <ClibUtil/string.hpp>
#include <ClibUtil/timer.hpp>
#include <ankerl/unordered_dense.h>
#include <srell.hpp>

#include "LogBuffer.h"
#pragma warning(pop)

#define DLLEXPORT __declspec(dllexport)

namespace logger = F4SE::log;
namespace buffered_logger = LogBuffer;

using namespace clib_util;
using namespace clib_util::singleton;

using namespace std::literals;
using namespace string::literals;

using RNG = clib_util::RNG;

// for visting variants
template <class... Ts>
struct overload : Ts...
{
	using Ts::operator()...;
};

template <class K, class D>
using Map = ankerl::unordered_dense::map<K, D>;
template <class K>
using Set = ankerl::unordered_dense::set<K>;

struct string_hash
{
	using is_transparent = void;  // enable heterogeneous overloads
	using is_avalanching = void;  // mark class as high quality avalanching hash

	[[nodiscard]] std::uint64_t operator()(std::string_view str) const noexcept
	{
		return ankerl::unordered_dense::hash<std::string_view>{}(str);
	}
};

template <class D>
using StringMap = ankerl::unordered_dense::segmented_map<std::string, D, string_hash, std::equal_to<>>;
using StringSet = ankerl::unordered_dense::segmented_set<std::string, string_hash, std::equal_to<>>;

namespace stl
{
	using namespace F4SE::stl;

	template <class T>
	void write_thunk_call(std::uintptr_t a_src)
	{
		F4SE::AllocTrampoline(14);

		auto& trampoline = F4SE::GetTrampoline();
		T::func = trampoline.write_call<5>(a_src, T::thunk);
	}

	template <class F, size_t index, class T>
	void write_vfunc()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[index] };
		T::func = vtbl.write_vfunc(T::idx, T::thunk);
	}

	template <class F, class T>
	void write_vfunc()
	{
		write_vfunc<F, 0, T>();
	}
}

namespace RE
{
	using FormID = std::uint32_t;
	using RefHandle = std::uint32_t;
	using FormType = ENUM_FORM_ID;

	struct SEXES
	{
		enum SEX : std::uint32_t
		{
			kNone = static_cast<std::underlying_type_t<SEX>>(-1),
			kMale = 0,
			kFemale = 1,

			kTotal = 2
		};
	};
	using SEX = SEXES::SEX;
}

#include "Cache.h"
#include "Defs.h"
#include "Version.h"
