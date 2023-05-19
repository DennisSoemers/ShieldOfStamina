#include "hooks.h"
#include "dataHandler.h"
namespace Utils {
	inline void damageav(RE::Actor* a, RE::ActorValue av, float val)
	{
		a->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, -val);
	}

	float GetBlockCost(const RE::HitData& hitData) {
        // Same implementation here as Get_block_cost_1403BED80 (address for Skyrim 1.5.97)
        // Re-implemented instead of calling original game's function so I don't have to figure out
        // addresses in other versions of the game
        // Downside: implementation probably slightly slower due to different way of getting game setting values.
        const auto gmstCollection = RE::GameSettingCollection::GetSingleton();
        float fStaminaBlockDmgMult = 0.25f;
        float fStaminaBlockStaggerMult = 5.f;
        float fStaminaBlockBase = 0.f;

		if (gmstCollection) {
            const auto staminaBlockDmgMultSetting = gmstCollection->GetSetting("fStaminaBlockDmgMult");
            const auto staminaBlockStaggerMultSetting = gmstCollection->GetSetting("fStaminaBlockStaggerMult");
            const auto staminaBlockBaseSetting = gmstCollection->GetSetting("fStaminaBlockBase");

			if (staminaBlockDmgMultSetting) {
                fStaminaBlockDmgMult = staminaBlockDmgMultSetting->GetFloat();
			}
            if (staminaBlockStaggerMultSetting) {
                fStaminaBlockStaggerMult = staminaBlockStaggerMultSetting->GetFloat();
            }
            if (staminaBlockBaseSetting) {
                fStaminaBlockBase = staminaBlockBaseSetting->GetFloat();
            }
		}

		return ((hitData.percentBlocked * hitData.physicalDamage) * fStaminaBlockDmgMult) +
               ((fStaminaBlockStaggerMult * (float)hitData.stagger) + fStaminaBlockBase);
    }
}

/*stamina blocking*/
void hitEventHook::processHit(RE::Actor* target, RE::HitData& hitData) {
	//check iff hit is blocked
	using HITFLAG = RE::HitData::Flag;
	if (!(hitData.flags.any(HITFLAG::kBlocked))) {
		_ProcessHit(target, hitData);
		return;
	}

	//nullPtr check in case Skyrim fucks up
	auto aggressor = hitData.aggressor.get();
	if (!target || !aggressor) {
		_ProcessHit(target, hitData);
		return;
	}

	bool isPlayerTarget = target->IsPlayerRef();
	bool isPlayerAggressor = aggressor->IsPlayerRef();
	float staminaDamageBase = hitData.totalDamage;
	float staminaDamageMult;
	logger::debug("base stamina damage is {}", staminaDamageBase);
	using namespace settings;
	if (hitData.flags.any(HITFLAG::kBlockWithWeapon)) {
		logger::debug("hit blocked with weapon");
		if (isPlayerTarget) {
			staminaDamageMult = bckWpnStaminaMult_PC_Block_NPC;
		}
		else {
			if (isPlayerAggressor) {
				staminaDamageMult = bckWpnStaminaMult_NPC_Block_PC;
			}
			else {
				staminaDamageMult = bckWpnStaminaMult_NPC_Block_NPC;
			}
				
		}
	}
	else {
		logger::debug("hit blocked with shield");
		if (isPlayerTarget) {
			staminaDamageMult = bckShdStaminaMult_PC_Block_NPC;
		}
		else {
			if (isPlayerAggressor) {
				staminaDamageMult = bckShdStaminaMult_NPC_Block_PC;
			}
			else {
				staminaDamageMult = bckShdStaminaMult_NPC_Block_NPC;
			}
		}
	}
	float staminaDamage = staminaDamageBase * staminaDamageMult;
	float targetStamina = target->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina);

	// Subtract the amount of stamina that the target is going to lose
	// from the vanilla game's blocking costs
	targetStamina -= Utils::GetBlockCost(hitData);
	if (targetStamina < 0.f) {
        targetStamina = 0.f;
	}
	
	//check whether there's enough stamina to block incoming attack
	if (targetStamina < staminaDamage) {
		logger::debug("not enough stamina to block, blocking part of damage!");
		if (settings::guardBreak) {
			logger::debug("guard break!");
			target->NotifyAnimationGraph("staggerStart");
		}
		hitData.totalDamage = hitData.totalDamage - (targetStamina / staminaDamageMult);
		Utils::damageav(target, RE::ActorValue::kStamina,
			targetStamina);
		logger::debug("failed to block {} damage", hitData.totalDamage);
	}
	else {
		hitData.totalDamage = 0;
		Utils::damageav(target, RE::ActorValue::kStamina,
			staminaDamage);
	}

	_ProcessHit(target, hitData);
}

bool staminaRegenHook::shouldRegenStamina(RE::ActorState* a_this, uint16_t a_flags) {
	//if bResult is true, prevents regen.
	bool bResult = _shouldRegenStamina(a_this, a_flags); // is sprinting?

	if (!bResult) {
		RE::Actor* actor = SKSE::stl::adjust_pointer<RE::Actor>(a_this, REL::Module::get().version() < SKSE::RUNTIME_SSE_1_6_629 ? -0xB8 : -0xC0);
		bResult = actor->IsBlocking();
	}

	return bResult;
}