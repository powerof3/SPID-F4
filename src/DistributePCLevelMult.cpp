#include "DistributePCLevelMult.h"
#include "Distribute.h"
#include "DistributeManager.h"
#include "PCLevelMultManager.h"

namespace Distribute::PlayerLeveledActor
{
	struct HandleUpdatePlayerLevel
	{
		static void thunk(RE::Actor* a_actor)
		{
			if (const auto npc = a_actor->GetNPC(); npc && npc->HasKeyword(processed)) {
				auto npcData = NPCData(a_actor, npc);
				Distribute(npcData, true);
			}

			func(a_actor);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	// Reset previous save/character dist. entries
	struct Revert
	{
		static void thunk(RE::Actor* a_this, RE::BGSLoadFormBuffer* a_buf)
		{
			func(a_this, a_buf);

			if (const auto npc = a_this->GetNPC(); npc && npc->HasPCLevelMult() && npc->HasKeyword(processed)) {
				const auto pcLevelMultManager = PCLevelMult::Manager::GetSingleton();

				auto input = PCLevelMult::Input{ a_this, npc, true };
				input.playerID = pcLevelMultManager->GetOldPlayerID();

				pcLevelMultManager->ForEachDistributedEntry(input, false, [&](RE::FormType a_formType, const Set<RE::FormID>& a_formIDSet) {
					switch (a_formType) {
					case RE::FormType::kKYWD:
						npc->RemoveKeywords(detail::set_to_vec<RE::BGSKeyword>(a_formIDSet));
						break;
					case RE::FormType::kFACT:
						{
							const auto saved_factions = detail::set_to_vec<RE::TESFaction>(a_formIDSet);
							for (auto& factionRank : npc->factions) {
								if (std::ranges::find(saved_factions, factionRank.faction) != saved_factions.end()) {
									factionRank.rank = -1;
								}
							}
						}
						break;
					case RE::FormType::kPERK:
						npc->RemovePerks(detail::set_to_vec<RE::BGSPerk>(a_formIDSet));
						break;
					case RE::FormType::kSPEL:
						npc->GetSpellList()->RemoveSpells(detail::set_to_vec<RE::SpellItem>(a_formIDSet));
						break;
					case RE::FormType::kLVSP:
						npc->GetSpellList()->RemoveLevSpells(detail::set_to_vec<RE::TESLevSpell>(a_formIDSet));
						break;
					default:
						break;
					}
				});
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static inline size_t                           idx{ 0x15 };
	};

	// Re-add dist entries if level is valid
	struct LoadGame
	{
		static void thunk(RE::Actor* a_this, RE::BGSLoadFormBuffer* a_buf)
		{
			if (const auto npc = a_this->GetNPC(); npc && npc->HasPCLevelMult() && npc->HasKeyword(processed)) {
				const auto pcLevelMultManager = PCLevelMult::Manager::GetSingleton();
				const auto input = PCLevelMult::Input{ a_this, npc, true };

				if (!pcLevelMultManager->FindDistributedEntry(input)) {
					//start distribution of leveled entries for first time
					auto npcData = NPCData(a_this, npc);
					Distribute(npcData, input);
				} else {
					//handle redistribution
					pcLevelMultManager->ForEachDistributedEntry(input, true, [&](RE::FormType a_formType, const Set<RE::FormID>& a_formIDSet) {
						switch (a_formType) {
						case RE::FormType::kKYWD:
							npc->AddKeywords(detail::set_to_vec<RE::BGSKeyword>(a_formIDSet));
							break;
						case RE::FormType::kFACT:
							{
								const auto saved_factions = detail::set_to_vec<RE::TESFaction>(a_formIDSet);
								for (auto& factionRank : npc->factions) {
									if (std::ranges::find(saved_factions, factionRank.faction) != saved_factions.end()) {
										factionRank.rank = 1;
									}
								}
							}
							break;
						case RE::FormType::kPERK:
							npc->AddPerks(detail::set_to_vec<RE::BGSPerk>(a_formIDSet), 1);
							break;
						case RE::FormType::kSPEL:
							npc->GetSpellList()->AddSpells(detail::set_to_vec<RE::SpellItem>(a_formIDSet));
							break;
						case RE::FormType::kLVSP:
							npc->GetSpellList()->AddLevSpells(detail::set_to_vec<RE::TESLevSpell>(a_formIDSet));
							break;
						default:
							break;
						}
					});
				}
			}

			func(a_this, a_buf);
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static inline size_t                           idx{ 0x12 };
	};

	void Install()
	{
		// ProcessLists::HandlePlayerLevelUpdate
		// inlined into SetLevel in NG
		REL::Relocation<std::uintptr_t> target{ REL::ID(2232967), 0xA6 };
		stl::write_thunk_call<HandleUpdatePlayerLevel>(target.address());

		stl::write_vfunc<RE::Actor, Revert>();
		stl::write_vfunc<RE::Actor, LoadGame>();

		logger::info("{:*^50}", "HOOKS");
		logger::info("Installed leveled distribution hooks");
	}
}
