#pragma once

//implements https://github.com/Ryan-rsm-McKenzie/CCExtender/blob/master/src/EditorIDCache.h
namespace Cache
{
	class EditorID
	{
	public:
		static EditorID* GetSingleton();

		void FillMap();

		std::string GetEditorID(RE::FormID a_formID);
		void CacheEditorID(const RE::TESForm* a_form, const char* a_editorID);

    private:
		using Lock = std::mutex;
		using Locker = std::scoped_lock<Lock>;

		EditorID() = default;
		EditorID(const EditorID&) = delete;
		EditorID(EditorID&&) = delete;
		~EditorID() = default;

		EditorID& operator=(const EditorID&) = delete;
		EditorID& operator=(EditorID&&) = delete;

		mutable Lock _lock;
		robin_hood::unordered_flat_map<RE::FormID, std::string> _formIDToEditorIDMap;
	};

	namespace FormType
	{
		inline constexpr frozen::map<RE::FormType, std::string_view, 7> whitelistMap = {
			{ RE::FormType::kFACT, "Faction"sv },
			{ RE::FormType::kCLAS, "Class"sv },
			{ RE::FormType::kCSTY, "CombatStyle"sv },
			{ RE::FormType::kRACE, "Race"sv },
			{ RE::FormType::kOTFT, "Outfit"sv },
			{ RE::FormType::kNPC_, "NPC"sv },
			{ RE::FormType::kVTYP, "VoiceType"sv }
		};

		inline constexpr frozen::map<RE::FormType, std::string_view, 159> blacklistMap = {

#define ELEMENT(a_elem) std::make_pair(RE::ENUM_FORM_ID::k##a_elem, #a_elem##sv)

			ELEMENT(NONE),
			ELEMENT(TES4),
			ELEMENT(GRUP),
			ELEMENT(GMST),
			ELEMENT(KYWD),
			ELEMENT(LCRT),
			ELEMENT(AACT),
			ELEMENT(TRNS),
			ELEMENT(CMPO),
			ELEMENT(TXST),
			ELEMENT(MICN),
			ELEMENT(GLOB),
			ELEMENT(DMGT),
			ELEMENT(CLAS),
			ELEMENT(FACT),
			ELEMENT(HDPT),
			ELEMENT(EYES),
			ELEMENT(RACE),
			ELEMENT(SOUN),
			ELEMENT(ASPC),
			ELEMENT(SKIL),
			ELEMENT(MGEF),
			ELEMENT(SCPT),
			ELEMENT(LTEX),
			ELEMENT(ENCH),
			ELEMENT(SPEL),
			ELEMENT(SCRL),
			ELEMENT(ACTI),
			ELEMENT(TACT),
			ELEMENT(ARMO),
			ELEMENT(BOOK),
			ELEMENT(CONT),
			ELEMENT(DOOR),
			ELEMENT(INGR),
			ELEMENT(LIGH),
			ELEMENT(MISC),
			ELEMENT(STAT),
			ELEMENT(SCOL),
			ELEMENT(MSTT),
			ELEMENT(GRAS),
			ELEMENT(TREE),
			ELEMENT(FLOR),
			ELEMENT(FURN),
			ELEMENT(WEAP),
			ELEMENT(AMMO),
			ELEMENT(NPC_),
			ELEMENT(LVLN),
			ELEMENT(KEYM),
			ELEMENT(ALCH),
			ELEMENT(IDLM),
			ELEMENT(NOTE),
			ELEMENT(PROJ),
			ELEMENT(HAZD),
			ELEMENT(BNDS),
			ELEMENT(SLGM),
			ELEMENT(TERM),
			ELEMENT(LVLI),
			ELEMENT(WTHR),
			ELEMENT(CLMT),
			ELEMENT(SPGD),
			ELEMENT(RFCT),
			ELEMENT(REGN),
			ELEMENT(NAVI),
			ELEMENT(CELL),
			ELEMENT(REFR),
			ELEMENT(ACHR),
			ELEMENT(PMIS),
			ELEMENT(PARW),
			ELEMENT(PGRE),
			ELEMENT(PBEA),
			ELEMENT(PFLA),
			ELEMENT(PCON),
			ELEMENT(PBAR),
			ELEMENT(PHZD),
			ELEMENT(WRLD),
			ELEMENT(LAND),
			ELEMENT(NAVM),
			ELEMENT(TLOD),
			ELEMENT(DIAL),
			ELEMENT(INFO),
			ELEMENT(QUST),
			ELEMENT(IDLE),
			ELEMENT(PACK),
			ELEMENT(CSTY),
			ELEMENT(LSCR),
			ELEMENT(LVSP),
			ELEMENT(ANIO),
			ELEMENT(WATR),
			ELEMENT(EFSH),
			ELEMENT(TOFT),
			ELEMENT(EXPL),
			ELEMENT(DEBR),
			ELEMENT(IMGS),
			ELEMENT(IMAD),
			ELEMENT(FLST),
			ELEMENT(PERK),
			ELEMENT(BPTD),
			ELEMENT(ADDN),
			ELEMENT(AVIF),
			ELEMENT(CAMS),
			ELEMENT(CPTH),
			ELEMENT(VTYP),
			ELEMENT(MATT),
			ELEMENT(IPCT),
			ELEMENT(IPDS),
			ELEMENT(ARMA),
			ELEMENT(ECZN),
			ELEMENT(LCTN),
			ELEMENT(MESG),
			ELEMENT(RGDL),
			ELEMENT(DOBJ),
			ELEMENT(DFOB),
			ELEMENT(LGTM),
			ELEMENT(MUSC),
			ELEMENT(FSTP),
			ELEMENT(FSTS),
			ELEMENT(SMBN),
			ELEMENT(SMQN),
			ELEMENT(SMEN),
			ELEMENT(DLBR),
			ELEMENT(MUST),
			ELEMENT(DLVW),
			ELEMENT(WOOP),
			ELEMENT(SHOU),
			ELEMENT(EQUP),
			ELEMENT(RELA),
			ELEMENT(SCEN),
			ELEMENT(ASTP),
			ELEMENT(OTFT),
			ELEMENT(ARTO),
			ELEMENT(MATO),
			ELEMENT(MOVT),
			ELEMENT(SNDR),
			ELEMENT(DUAL),
			ELEMENT(SNCT),
			ELEMENT(SOPM),
			ELEMENT(COLL),
			ELEMENT(CLFM),
			ELEMENT(REVB),
			ELEMENT(PKIN),
			ELEMENT(RFGP),
			ELEMENT(AMDL),
			ELEMENT(LAYR),
			ELEMENT(COBJ),
			ELEMENT(OMOD),
			ELEMENT(MSWP),
			ELEMENT(ZOOM),
			ELEMENT(INNR),
			ELEMENT(KSSM),
			ELEMENT(AECH),
			ELEMENT(SCCO),
			ELEMENT(AORU),
			ELEMENT(SCSN),
			ELEMENT(STAG),
			ELEMENT(NOCM),
			ELEMENT(LENS),
			ELEMENT(LSPR),
			ELEMENT(GDRY),
			ELEMENT(OVIS),

#undef ELEMENT
		};

		std::string GetWhitelistFormString(RE::FormType a_type);

		std::string GetBlacklistFormString(RE::FormType a_type);
	}

	//Cache spell formEditorIDs
	namespace LoadFormEditorIDs
	{
		struct SetFormEditorID_Cache
		{
			static bool thunk(RE::TESForm* a_this, const char* a_str)
			{
				if (!a_this->IsCreated() && !string::is_empty(a_str)) {
					EditorID::GetSingleton()->CacheEditorID(a_this, a_str);
				}
				return func(a_this, a_str);
			}
			static inline REL::Relocation<decltype(thunk)> func;

			static inline constexpr size_t size{ 0x3B };
		};

		inline void Install()
		{
			stl::write_vfunc<RE::SpellItem, SetFormEditorID_Cache>();

			logger::info("Installed spell editorID cache patch"sv);
		}
	}
}
