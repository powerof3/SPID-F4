#pragma once

#define WIN32_LEAN_AND_MEAN

#define NOMB
#define NOMINMAX
#define NOSERVICE

#pragma warning(push)
#include "F4SE/F4SE.h"
#include "RE/Fallout.h"

#ifdef NDEBUG
#	include <spdlog/sinks/basic_file_sink.h>
#else
#	include <spdlog/sinks/msvc_sink.h>
#endif

#include <robin_hood.h>
#include <SimpleIni.h>
#include <frozen/map.h>
#include <robin_hood.h>
#include <srell.hpp>
#pragma warning(pop)

#define DLLEXPORT __declspec(dllexport)

namespace logger = F4SE::log;
namespace numeric = F4SE::stl::numeric;
namespace string = F4SE::stl::string;

using namespace std::literals;
using RNG = F4SE::stl::RNG;

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
		T::func = vtbl.write_vfunc(T::size, T::thunk);
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
