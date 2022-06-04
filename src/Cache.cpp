#include "Cache.h"

namespace Cache
{
	EditorID* EditorID::GetSingleton()
	{
		static EditorID singleton;
		return std::addressof(singleton);
	}

	void EditorID::FillMap()
	{
		const auto& [map, lock] = RE::TESForm::GetAllFormsByEditorID();
		const RE::BSAutoReadLock locker{ lock.get() };
		if (map) {
			for (auto& [id, form] : *map) {
				_formIDToEditorIDMap.emplace(form->GetFormID(), id.c_str());
			}
		}
	}

	void EditorID::CacheEditorID(const RE::TESForm* a_form, const char* a_editorID)
	{
		Locker locker(_lock);
		_formIDToEditorIDMap.emplace(a_form->GetFormID(), a_editorID);
	}

	std::string EditorID::GetEditorID(RE::FormID a_formID)
	{
		Locker locker(_lock);
		auto it = _formIDToEditorIDMap.find(a_formID);
		return it != _formIDToEditorIDMap.end() ? it->second : std::string();
	}

	std::string FormType::GetWhitelistFormString(const RE::FormType a_type)
	{
		auto it = whitelistMap.find(a_type);
		return it != whitelistMap.end() ? std::string(it->second) : std::string();
	}

	std::string FormType::GetBlacklistFormString(const RE::FormType a_type)
	{
		auto it = blacklistMap.find(a_type);
		return it != blacklistMap.end() ? std::string(it->second) : std::string();
	}
}
