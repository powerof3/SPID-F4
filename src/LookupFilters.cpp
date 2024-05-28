#include "LookupFilters.h"
#include "LookupNPC.h"

namespace Filter
{
	Data::Data(StringFilters a_strings, FormFilters a_formFilters, LevelFilters a_level, Traits a_traits, PercentChance a_chance) :
		strings(std::move(a_strings)),
		forms(std::move(a_formFilters)),
		levels(std::move(a_level)),
		traits(a_traits),
		chance(a_chance / 100)
	{
		hasLeveledFilters = HasLevelFiltersImpl();
	}

	Result Data::passed_string_filters(const NPCData& a_npcData) const
	{
		if (!strings.ALL.empty() && !a_npcData.HasStringFilter(strings.ALL, true)) {
			return Result::kFail;
		}

		if (!strings.NOT.empty() && a_npcData.HasStringFilter(strings.NOT)) {
			return Result::kFail;
		}

		if (!strings.MATCH.empty() && !a_npcData.HasStringFilter(strings.MATCH)) {
			return Result::kFail;
		}

		if (!strings.ANY.empty() && !a_npcData.ContainsStringFilter(strings.ANY)) {
			return Result::kFail;
		}

		return Result::kPass;
	}

	Result Data::passed_form_filters(const NPCData& a_npcData) const
	{
		if (!forms.ALL.empty() && !a_npcData.HasFormFilter(forms.ALL, true)) {
			return Result::kFail;
		}

		if (!forms.NOT.empty() && a_npcData.HasFormFilter(forms.NOT)) {
			return Result::kFail;
		}

		if (!forms.MATCH.empty() && !a_npcData.HasFormFilter(forms.MATCH)) {
			return Result::kFail;
		}

		return Result::kPass;
	}

	Result Data::passed_level_filters(const NPC::Data& a_npcData) const
	{
		// Actor Level
		if (!levels.actorLevel.IsInRange(a_npcData.GetLevel())) {
			return Result::kFail;
		}

		const auto npc = a_npcData.GetNPC();

		// AVs
		for (auto& [av, avRange] : levels.avLevels) {
			const auto avInfo = std::get<RE::ActorValueInfo*>(av);
			if (avInfo && !avRange.IsInRange(npc->GetActorValue(*avInfo))) {
				return Result::kFail;
			}
		}

		return Result::kPass;
	}

	Result Data::passed_trait_filters(const NPCData& a_npcData) const
	{
		auto npc = a_npcData.GetNPC();

		// Traits
		if (traits.sex && npc->GetSex() != *traits.sex) {
			return Result::kFail;
		}
		if (traits.unique && npc->IsUnique() != *traits.unique) {
			return Result::kFail;
		}
		if (traits.summonable && npc->IsSummonable() != *traits.summonable) {
			return Result::kFail;
		}
		if (traits.child && a_npcData.IsChild() != *traits.child) {
			return Result::kFail;
		}
		if (traits.leveled && a_npcData.IsLeveled() != *traits.leveled) {
			return Result::kFail;
		}

		return Result::kPass;
	}

	bool Data::HasLevelFilters() const
	{
		return hasLeveledFilters;
	}

	bool Data::HasLevelFiltersImpl() const
	{
		const auto& [actorLevel, avLevels] = levels;

		if (actorLevel.IsValid()) {
			return true;
		}

		return std::ranges::any_of(avLevels, [](const auto& avPair) {
			auto& [avType, avRange] = avPair;
			return avRange.IsValid();
		});
	}

	Result Data::PassedFilters(const NPCData& a_npcData) const
	{
		// Fail chance first to avoid running unnecessary checks
		if (chance < 1) {
			const auto randNum = RNG().generate();
			if (randNum > chance) {
				return Result::kFailRNG;
			}
		}

		if (passed_string_filters(a_npcData) == Result::kFail) {
			return Result::kFail;
		}

		if (passed_form_filters(a_npcData) == Result::kFail) {
			return Result::kFail;
		}

		if (passed_level_filters(a_npcData) == Result::kFail) {
			return Result::kFail;
		}

		return passed_trait_filters(a_npcData);
	}
}
