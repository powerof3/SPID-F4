#include "Distribute.h"
#include "LookupConfigs.h"
#include "LookupForms.h"

void MessageHandler(F4SE::MessagingInterface::Message* a_message)
{
	if (a_message->type == F4SE::MessagingInterface::kGameDataReady) {
		logger::info("{:*^30}", "LOOKUP");

		Cache::EditorID::GetSingleton()->FillMap();

		if (Lookup::GetForms()) {
			Distribute::ApplyToNPCs();
			Distribute::LeveledActor::Install();
			Distribute::DeathItemManager::Register();
		}
	}
}

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

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Query(const F4SE::QueryInterface* a_F4SE, F4SE::PluginInfo* a_info)
{
	a_info->infoVersion = F4SE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	const auto ver = a_F4SE->RuntimeVersion();
	if (ver < F4SE::RUNTIME_LATEST) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Load(const F4SE::LoadInterface* a_f4se)
{
	InitializeLog();

	logger::info(FMT_STRING("{} log opened."sv), Version::PROJECT.data());

	F4SE::Init(a_f4se);

	if (INI::Read()) {
		const auto messaging = F4SE::GetMessagingInterface();
		messaging->RegisterListener(MessageHandler);
	}

	return true;
}
