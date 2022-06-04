#pragma once

namespace Forms
{
	template <class T>
	struct FormMap
	{
		FormDataMap<T> forms;

		explicit operator bool()
		{
			return !forms.empty();
		}
	};

	inline FormMap<RE::SpellItem> spells;
	inline FormMap<RE::BGSPerk> perks;
	inline FormMap<RE::TESBoundObject> items;
	inline FormMap<RE::TESLevSpell> levSpells;
	inline FormMap<RE::TESPackage> packages;
	inline FormMap<RE::BGSOutfit> outfits;
	inline FormMap<RE::BGSKeyword> keywords;
	inline FormMap<RE::TESBoundObject> deathItems;
	inline FormMap<RE::TESFaction> factions;
}

namespace Lookup
{
	using namespace Forms;

	namespace detail
	{
		inline bool formID_to_form(RE::TESDataHandler* a_dataHandler, const FormIDPairVec& a_formIDVec, FormVec& a_formVec)
		{
			if (a_formIDVec.empty()) {
				return true;
			}
			for (auto& [formID, modName] : a_formIDVec) {
				if (modName && !formID) {
					if (INI::is_mod_name(*modName)) {
						if (const RE::TESFile* filterMod = a_dataHandler->GetModByName(*modName); filterMod) {
							logger::info("			Filter ({}) INFO - mod found", filterMod->GetFilename());
							a_formVec.push_back(filterMod);
						} else {
							logger::error("			Filter ({}) SKIP - mod cannot be found", *modName);
						}
					} else {
						auto filterForm = RE::TESForm::GetFormByEditorID(*modName);
						if (filterForm) {
							const auto formType = filterForm->GetFormType();
							if (const auto type = Cache::FormType::GetWhitelistFormString(formType); !type.empty()) {
								a_formVec.push_back(filterForm);
							} else {
								logger::error("			Filter ({}) SKIP - invalid formtype ({})", *modName, Cache::FormType::GetBlacklistFormString(formType));
							}
						} else {
							logger::error("			Filter ({}) SKIP - form doesn't exist", *modName);
						}
					}
				} else if (formID) {
					auto filterForm = modName ?
                                          a_dataHandler->GetForm(*formID, *modName) :
                                          RE::TESForm::GetFormByID(*formID);
					if (filterForm) {
						const auto formType = filterForm->GetFormType();
						if (const auto type = Cache::FormType::GetWhitelistFormString(formType); !type.empty()) {
							a_formVec.push_back(filterForm);
						} else {
							logger::error("			Filter [0x{:X}] ({}) SKIP - invalid formtype ({})", *formID, modName.value_or(""), Cache::FormType::GetBlacklistFormString(formType));
						}
					} else {
						logger::error("			Filter [0x{:X}] ({}) SKIP - form doesn't exist", *formID, modName.value_or(""));
					}
				}
			}
			return !a_formVec.empty();
		}
	}

	template <class Form>
	void get_forms(RE::TESDataHandler* a_dataHandler, const std::string& a_type, const INIDataMap& a_INIDataMap, FormMap<Form>& a_FormDataMap)
	{
		if (a_INIDataMap.empty()) {
			return;
		}

		logger::info("	Starting {} lookup", a_type);

		for (auto& [formOrEditorID, INIDataVec] : a_INIDataMap) {
			Form* form = nullptr;

			if (std::holds_alternative<FormIDPair>(formOrEditorID)) {
				if (auto [formID, modName] = std::get<FormIDPair>(formOrEditorID); formID) {
					if (modName) {
						form = a_dataHandler->GetForm<Form>(*formID, *modName);
						if constexpr (std::is_same_v<Form, RE::TESBoundObject>) {
							if (!form) {
								const auto base = a_dataHandler->GetForm(*formID, *modName);
								form = base ? static_cast<Form*>(base) : nullptr;
							}
						}
					} else {
						form = RE::TESForm::GetFormByID<Form>(*formID);
						if constexpr (std::is_same_v<Form, RE::TESBoundObject>) {
							if (!form) {
								const auto base = RE::TESForm::GetFormByID(*formID);
								form = base ? static_cast<Form*>(base) : nullptr;
							}
						}
					}
					if (!form) {
						logger::error("		{} [0x{:X}] ({}) FAIL - formID doesn't exist", a_type, *formID, modName.value_or(""));
					}
				}
			} else if (std::holds_alternative<std::string>(formOrEditorID)) {
				if (auto editorID = std::get<std::string>(formOrEditorID); !editorID.empty()) {
					form = RE::TESForm::GetFormByEditorID<Form>(editorID);
					if constexpr (std::is_same_v<Form, RE::TESBoundObject>) {
						if (!form) {
							const auto base = RE::TESForm::GetFormByEditorID(editorID);
							form = base ? static_cast<Form*>(base) : nullptr;
						}
					}
					if (!form) {
						logger::error("		{} FAIL - editorID doesn't exist", editorID);
					}
				}
			}

			if (!form) {
				continue;
			}

			std::vector<FormData> formDataVec;
			for (auto& [strings, filterIDs, level, gender, itemCount, chance] : INIDataVec) {
				bool invalidEntry = false;

				std::array<FormVec, 3> filterForms;
				for (std::uint32_t i = 0; i < 3; i++) {
					if (!detail::formID_to_form(a_dataHandler, filterIDs[i], filterForms[i])) {
						invalidEntry = true;
						break;
					}
				}

				if (!invalidEntry) {
					FormData formData{ strings, filterForms, level, gender, chance, itemCount };
					formDataVec.emplace_back(formData);
				}
			}

			a_FormDataMap.forms[form] = { 0, formDataVec };
		}
	}

	bool GetForms();
}
