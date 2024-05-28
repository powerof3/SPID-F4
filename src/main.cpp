#include "DistributeManager.h"
#include "LookupConfigs.h"
#include "LookupForms.h"
#include "PCLevelMultManager.h"

bool shouldLookupForms{ false };
bool shouldLogErrors{ false };
bool shouldDistribute{ false };

void MessageHandler(F4SE::MessagingInterface::Message* a_message)
{
	switch (a_message->type) {
	case F4SE::MessagingInterface::kPostLoad:
		{
			if (std::tie(shouldLookupForms, shouldLogErrors) = Distribution::INI::GetConfigs(); shouldLookupForms) {
				logger::info("{:*^50}", "HOOKS");
				Distribute::Actor::Install();
			}
		}
		break;
	case F4SE::MessagingInterface::kGameDataReady:
		{
			if (shouldDistribute = Lookup::LookupForms(); shouldDistribute) {
				Distribute::Setup();
			}

			if (shouldLogErrors) {
				const auto error = std::format("[SPID] Errors found when reading configs. Check {}.log in {} for more info\n", Version::PROJECT, F4SE::log::log_directory()->string());
				RE::ConsoleLog::GetSingleton()->PrintLine(error.c_str());
			}
		}
		break;
	case F4SE::MessagingInterface::kPreLoadGame:
		{
			if (shouldDistribute) {
				const std::string savePath{ static_cast<char*>(a_message->data), a_message->dataLen };
				PCLevelMult::Manager::GetSingleton()->GetPlayerIDFromSave(savePath);
			}
		}
		break;
	case F4SE::MessagingInterface::kNewGame:
		{
			if (shouldDistribute) {
				PCLevelMult::Manager::GetSingleton()->SetNewGameStarted();
			}
		}
		break;
	default:
		break;
	}
}

extern "C" DLLEXPORT constinit auto F4SEPlugin_Version = []() noexcept {
	F4SE::PluginVersionData data{};

	data.PluginVersion({ Version::MAJOR, Version::MINOR, Version::PATCH });
	data.PluginName(Version::PROJECT.data());
	data.AuthorName("powerofthree");
	data.UsesAddressLibrary(true);
	data.UsesSigScanning(false);
	data.IsLayoutDependent(true);
	data.HasNoStructUse(false);
	data.CompatibleVersions({ F4SE::RUNTIME_LATEST });

	return data;
}();

void InitializeLog()
{
	auto path = logger::log_directory();
	if (!path) {
		stl::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= Version::PROJECT;
	*path += ".log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S:%e] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Load(const F4SE::LoadInterface* a_f4se)
{
	F4SE::Init(a_f4se);

	InitializeLog();

	const auto messaging = F4SE::GetMessagingInterface();
	messaging->RegisterListener(MessageHandler);

	return true;
}
