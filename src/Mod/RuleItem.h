#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <string>
#include <vector>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include "RuleStatBonus.h"
#include "RuleDamageType.h"
#include "ModScript.h"
#include "RuleResearch.h"
#include "RuleBaseFacilityFunctions.h"

namespace OpenXcom
{

enum BattleType { BT_NONE, BT_FIREARM, BT_AMMO, BT_MELEE, BT_GRENADE, BT_PROXIMITYGRENADE, BT_MEDIKIT, BT_SCANNER, BT_MINDPROBE, BT_PSIAMP, BT_FLARE, BT_CORPSE, BT_ANOMALY };
enum BattleFuseType { BFT_NONE = -3, BFT_INSTANT = -2, BFT_SET = -1, BFT_FIX_MIN = 0, BFT_FIX_MAX = 64 };
enum BattleMediKitType { BMT_NORMAL = 0, BMT_HEAL = 1, BMT_STIMULANT = 2, BMT_PAINKILLER = 3 };
enum BattleMediKitAction { BMA_HEAL = 1, BMA_STIMULANT = 2, BMA_PAINKILLER = 4 };
enum ExperienceTrainingMode {
	ETM_DEFAULT,
	ETM_MELEE_100, ETM_MELEE_50, ETM_MELEE_33,
	ETM_FIRING_100, ETM_FIRING_50, ETM_FIRING_33,
	ETM_THROWING_100, ETM_THROWING_50, ETM_THROWING_33,
	ETM_FIRING_AND_THROWING,
	ETM_FIRING_OR_THROWING,
	ETM_REACTIONS,
	ETM_REACTIONS_AND_MELEE, ETM_REACTIONS_AND_FIRING, ETM_REACTIONS_AND_THROWING,
	ETM_REACTIONS_OR_MELEE, ETM_REACTIONS_OR_FIRING, ETM_REACTIONS_OR_THROWING,
	ETM_BRAVERY, ETM_BRAVERY_2X,
	ETM_BRAVERY_AND_REACTIONS,
	ETM_BRAVERY_OR_REACTIONS, ETM_BRAVERY_OR_REACTIONS_2X,
	ETM_PSI_STRENGTH, ETM_PSI_STRENGTH_2X,
	ETM_PSI_SKILL, ETM_PSI_SKILL_2X,
	ETM_PSI_STRENGTH_AND_SKILL, ETM_PSI_STRENGTH_AND_SKILL_2X,
	ETM_PSI_STRENGTH_OR_SKILL, ETM_PSI_STRENGTH_OR_SKILL_2X,
	ETM_NOTHING
};
enum BattleActionType : Uint8
{
	BA_NONE = 0,

	BA_TURN = 1,
	BA_WALK = 2,
	BA_KNEEL = 3,

	BA_PRIME = 4,
	BA_UNPRIME = 5,
	BA_THROW = 6,
	BA_AUTOSHOT = 7,
	BA_SNAPSHOT = 8,
	BA_AIMEDSHOT = 9,
	BA_HIT = 10,

	BA_USE = 11,
	BA_LAUNCH = 12,
	BA_MINDCONTROL = 13,
	BA_PANIC = 14,

	BA_RETHINK = 15,

	BA_CQB = 16,

	BA_TRIGGER_TIMED_GRENADE = 17,
	BA_TRIGGER_PROXY_GRENADE = 18,

	BA_SELF_DESTRUCT = 19,
};

enum class BattleActionOrigin { CENTRE = 0, LEFT, RIGHT }; // Used for off-centre shooting.

struct BattleActionCost;
class BattleItem;
class RuleSkill;
class Unit;
class SurfaceSet;
class Surface;
class Mod;
class RuleInventory;

enum UnitFaction : int;

struct RuleItemUseCost
{
	int Time;
	int Energy;
	int Morale;
	int Health;
	int Stun;
	int Mana;

	/// Default constructor.
	RuleItemUseCost() : Time(0), Energy(0), Morale(0), Health(0), Stun(0), Mana(0)
	{

	}
	/// Create new cost with one value for time units and another for rest.
	RuleItemUseCost(int tu, int rest = 0) : Time(tu), Energy(rest), Morale(rest), Health(rest), Stun(rest), Mana(rest)
	{

	}

	/// Add cost.
	RuleItemUseCost& operator+=(const RuleItemUseCost& cost)
	{
		Time += cost.Time;
		Energy += cost.Energy;
		Morale += cost.Morale;
		Health += cost.Health;
		Stun += cost.Stun;
		Mana += cost.Mana;
		return *this;
	}

	/**
	 * Load use cost.
	 * @param a Item use cost.
	 * @param node YAML node.
	 * @param name Name of action type.
	 */
	void loadCost(const YAML::Node& node, const std::string& name)
	{
		loadInt(Time, node["tu" + name]);
		if (const YAML::Node& cost = node["cost" + name])
		{
			loadInt(Time, cost["time"]);
			loadInt(Energy, cost["energy"]);
			loadInt(Morale, cost["morale"]);
			loadInt(Health, cost["health"]);
			loadInt(Stun, cost["stun"]);
			loadInt(Mana, cost["mana"]);
		}
	}

	/**
	 * Load use cost type (flat or percent).
	 * @param a Item use type.
	 * @param node YAML node.
	 * @param name Name of action type.
	 */
	void loadPercent(const YAML::Node& node, const std::string& name)
	{
		if (const YAML::Node& cost = node["flat" + name])
		{
			if (cost.IsScalar())
			{
				loadTriBool(Time, cost);
			}
			else
			{
				loadTriBool(Time, cost["time"]);
				loadTriBool(Energy, cost["energy"]);
				loadTriBool(Morale, cost["morale"]);
				loadTriBool(Health, cost["health"]);
				loadTriBool(Stun, cost["stun"]);
				loadTriBool(Mana, cost["mana"]);
			}
		}
	}

	/**
	 * Load nullable bool value and store it in int (with null as -1).
	 * @param a value to set.
	 * @param node YAML node.
	 */
	void loadTriBool(int& a, const YAML::Node& node) const
	{
		if (node)
		{
			if (node.IsNull())
			{
				a = -1;
			}
			else
			{
				a = node.as<bool>();
			}
		}
	}

	/**
	 * Load nullable int (with null as -1).
	 * @param a value to set.
	 * @param node YAML node.
	 */
	void loadInt(int& a, const YAML::Node& node) const
	{
		if (node)
		{
			if (node.IsNull())
			{
				a = -1;
			}
			else
			{
				a = node.as<int>();
			}
		}
	}
};

/**
 * Common configuration of item action.
 */
struct RuleItemAction
{
	int accuracy = 0;
	int range = 0;
	int shots = 1;
	int spendPerShot = 1;
	bool followProjectiles = true;
	int ammoSlot = 0;
	RuleItemUseCost cost;
	RuleItemUseCost flat;
	bool arcing = false; // Only overrides arcing: false on a weapon for a specific action
	std::string name;
	std::string shortName;
};

/**
 * Config for fuse triggers.
 */
struct RuleItemFuseTrigger
{
	bool defaultBehavior = true;
	bool throwTrigger = false;
	bool throwExplode = false;
	bool proximityTrigger = false;
	bool proximityExplode = false;
};

namespace helper
{

/**
 * Defined outside struct BattleActionAttack to allow forward declaration of it for RuleStatBonus
 */
struct BattleActionAttackReadOnlyImpl
{
	BattleActionType type;
	const BattleUnit *attacker = nullptr;
	const BattleItem *weapon_item = nullptr;
	const BattleItem *damage_item = nullptr;
	const RuleSkill *skill_rules = nullptr;
};

}


/**
 * Helper struct that has all the data needed for weapon attack.
 */
struct BattleActionAttack
{
	BattleActionType type;
	BattleUnit *attacker = nullptr;
	BattleItem *weapon_item = nullptr;
	BattleItem *damage_item = nullptr;
	const RuleSkill *skill_rules = nullptr;

	/**
	 * Helper class that have only readonly access to data.
	 */
	using ReadOnly = helper::BattleActionAttackReadOnlyImpl;

	/**
	 * Cast operator to readonly version.
	 */
	operator ReadOnly() const
	{
		return { type, attacker, weapon_item, damage_item, skill_rules, };
	}

	/// Get Action Attack from Action cost.
	static BattleActionAttack GetBeforeShoot(const BattleActionCost &action);
	/// Get Action Attack with updated unit and ammo.
	static BattleActionAttack GetBeforeShoot(BattleActionType type, BattleUnit *unit, BattleItem *wepon, const RuleSkill *skill = nullptr);
	/// Get Action Attack from Action cost with defined ammo.
	static BattleActionAttack GetAferShoot(const BattleActionCost &action, BattleItem *ammo);
	/// Get Action Attack with updated unit with defined ammo.
	static BattleActionAttack GetAferShoot(BattleActionType type, BattleUnit *unit, BattleItem *wepon, BattleItem *ammo, const RuleSkill *skill = nullptr);
};

/**
 * Represents a specific type of item.
 * Contains constant info about an item like
 * storage size, sell price, etc.
 * @sa Item
 */
class RuleItem
{
public:
	/// Maximum number of ammo slots on weapon.
	static const int AmmoSlotMax = 4;
	static const int ChamberMax = 16;
	/// Special ammo slot that represent usage of weapon itself as ammo.
	static const int AmmoSlotSelfUse = -1;
	static const int MedikitSlots = 3;

	/// Load ammo slot with checking correct range.
	static void loadAmmoSlotChecked(int& result, const YAML::Node& node, const std::string& parentName);

private:
	std::string _type, _ufopediaType, _name, _nameAsAmmo; // two types of objects can have the same name
	std::string _requiresBuyCountry;
	std::vector<std::string> _requiresName;
	std::vector<std::string> _requiresBuyName;
	std::vector<const RuleResearch *> _requires, _requiresBuy;
	RuleBaseFacilityFunctions _requiresBuyBaseFunc;
	std::map<std::string, int> _recoveryDividers;
	std::map<std::string, std::vector<int> > _recoveryTransformationsName;
	std::map<const RuleItem*, std::vector<int> > _recoveryTransformations;
	std::vector<std::string> _categories;

	Unit* _vehicleUnit;
	double _size;
	int _monthlyBuyLimit;
	int _costBuy, _costSell, _transferTime, _weight;
	int _throwRange, _underwaterThrowRange;
	int _bigSprite;
	int _floorSprite;
	int _handSprite, _bulletSprite;
	int _specialIconSprite;
	std::vector<int> _reloadSound, _primeSound, _unprimeSound;
	std::vector<int> _fireSound, _hitSound;
	int _hitAnimation, _hitAnimFrames;
	std::vector<int> _hitMissSound;
	int _hitMissAnimation, _hitMissAnimFrames;
	std::vector<int> _meleeSound;
	int _meleeAnimation, _meleeAnimFrames;
	std::vector<int> _meleeMissSound;
	int _meleeMissAnimation, _meleeMissAnimFrames;
	std::vector<int> _meleeHitSound, _explosionHitSound, _psiSound;
	int _psiAnimation, _psiAnimFrames;
	std::vector<int> _psiMissSound;
	int _psiMissAnimation, _psiMissAnimFrames;
	int _power, _powerForAnimation;
	bool _hidePower;
	float _powerRangeReduction;
	float _powerRangeThreshold;
	std::vector<std::vector<std::string>> _compatibleAmmoNames = std::vector<std::vector<std::string>>(AmmoSlotMax);
	std::vector<const RuleItem*> _compatibleAmmo[AmmoSlotMax];
	std::unordered_map<const RuleItem*, int> _compatibleAmmoSlots;
	RuleDamageType _damageType, _meleeType;
	bool _damageTypeSet, _meleeTypeSet;
	RuleItemAction _confAimed, _confAuto, _confSnap, _confMelee;
	int _accuracyUse, _accuracyMind, _accuracyPanic, _accuracyThrow, _accuracyCloseQuarters;
	int _noLOSAccuracyPenalty;
	RuleItemUseCost _costUse, _costMind, _costPanic, _costThrow, _costPrime, _costUnprime;
	int _clipSize, _specialChance, _tuLoad[AmmoSlotMax], _tuUnload[AmmoSlotMax], _chamberSize[AmmoSlotMax];
	BattleType _battleType;
	BattleFuseType _fuseType;
	RuleItemFuseTrigger _fuseTriggerEvents;
	bool _hiddenOnMinimap;
	bool _multipleDischarges;
	int _proximityRadius, _lightRadius;
	std::string _medikitActionName, _psiAttackName, _primeActionName, _unprimeActionName, _primeActionMessage, _unprimeActionMessage;

	bool _twoHanded, _blockBothHands, _fixedWeapon, _fixedWeaponShow, _isConsumable, _isFireExtinguisher;
	bool _isExplodingInHands, _specialUseEmptyHand, _specialUseEmptyHandShow;
	int _inventoryMoveCostPercent = 100;
	std::string _defaultInventorySlotName;
	const RuleInventory* _defaultInventorySlot;
	int _defaultInvSlotX, _defaultInvSlotY;
	std::vector<std::string> _supportedInventorySectionsNames;
	std::vector<const RuleInventory*> _supportedInventorySections;
	int _waypoints, _invWidth, _invHeight;

	int _painKiller, _heal, _stimulant;
	BattleMediKitType _medikitType;
	bool _medikitTargetSelf, _medikitTargetImmune;
	int _medikitTargetMatrix;
	std::string _medikitBackground;
	int _woundRecovery, _healthRecovery, _stunRecovery, _energyRecovery, _manaRecovery, _moraleRecovery, _painKillerRecovery;

	int _recoveryPoints;
	int _armor;
	int _turretType;
	int _aiUseDelay, _aiMeleeHitCount;
	bool _recover, _recoverCorpse, _ignoreInBaseDefense, _ignoreInCraftEquip, _liveAlien;
	int _liveAlienPrisonType;
	int _attraction;
	RuleItemUseCost _flatUse, _flatThrow, _flatPrime, _flatUnprime;
	bool _arcingShot;
	ExperienceTrainingMode _experienceTrainingMode;
	int _manaExperience;
	int _listOrder, _maxRange, _minRange, _dropoff, _bulletSpeed, _explosionSpeed, _shotgunPellets;
	int _shotgunBehaviorType, _shotgunSpread, _shotgunChoke;
	std::map<std::string, std::string> _zombieUnitByArmorMale, _zombieUnitByArmorFemale, _zombieUnitByType;
	std::string _zombieUnit, _spawnUnit;
	int _spawnUnitFaction;
	int _targetMatrix;
	bool _LOSRequired, _underwaterOnly, _landOnly, _psiReqiured, _manaRequired;
	int _meleePower, _specialType, _vaporColor, _vaporDensity, _vaporProbability;
	int _vaporColorSurface, _vaporDensitySurface, _vaporProbabilitySurface;
	std::vector<int> _customItemPreviewIndex;
	int _kneelBonus, _oneHandedPenalty;
	int _monthlySalary, _monthlyMaintenance;
	int _sprayWaypoints;
	bool _silenced;
	RuleStatBonus _damageBonus, _meleeBonus, _accuracyMulti, _meleeMulti, _throwMulti, _closeQuartersMulti;
	ModScript::BattleItemScripts::Container _battleItemScripts;
	ModScript::CostScripts::Container _costScripts;
	ScriptValues<RuleItem> _scriptValues;

	/// Get final value of cost.
	RuleItemUseCost getDefault(const RuleItemUseCost& a, const RuleItemUseCost& b) const;
	/// Load bool as int from yaml.
	void loadBool(bool& a, const YAML::Node& node) const;
	/// Load bool as int from yaml.
	void loadTriBool(int& a, const YAML::Node& node) const;
	/// Load int from yaml.
	void loadInt(int& a, const YAML::Node& node) const;
	/// Load RuleItemUseCost from yaml.
	void loadCost(RuleItemUseCost& a, const YAML::Node& node, const std::string& name) const;
	/// Load RuleItemUseCost as bool from yaml.
	void loadPercent(RuleItemUseCost& a, const YAML::Node& node, const std::string& name) const;
	/// Load RuleItemAction from yaml.
	void loadConfAction(RuleItemAction& a, const YAML::Node& node, const std::string& name) const;
	/// Gets a random sound from a given vector.
	int getRandomSound(const std::vector<int> &vector, int defaultValue = -1) const;
	/// Load RuleItemFuseTrigger from yaml.
	void loadConfFuse(RuleItemFuseTrigger& a, const YAML::Node& node, const std::string& name) const;

public:
	/// Name of class used in script.
	static constexpr const char *ScriptName = "RuleItem";
	/// Register all useful function used by script.
	static void ScriptRegister(ScriptParserBase* parser);

	/// Creates a blank item ruleset.
	RuleItem(const std::string &type);
	/// Cleans up the item ruleset.
	~RuleItem();
	/// Updates item categories based on replacement rules.
	void updateCategories(std::map<std::string, std::string> *replacementRules);
	/// Loads item data from YAML.
	void load(const YAML::Node& node, Mod *mod, int listIndex, const ModScript& parsers);
	/// Cross link with other rules.
	void afterLoad(const Mod* mod);

	/// Gets the item's type.
	const std::string &getType() const;
	/// Gets the item's ufopedia type.
	const std::string &getUfopediaType() const;
	/// Gets the item's name.
	const std::string &getName() const;
	/// Gets the item's name when loaded in weapon.
	const std::string &getNameAsAmmo() const;
	/// Gets the item's requirements.
	const std::vector<const RuleResearch*> &getRequirements() const;
	/// Gets the item's buy requirements.
	const std::vector<const RuleResearch*> &getBuyRequirements() const;
	/// Gets the allied country name required to buy this item.
	const std::string& getRequiresBuyCountry() const { return _requiresBuyCountry; }
	/// Gets the base functions required to buy craft.
	RuleBaseFacilityFunctions getRequiresBuyBaseFunc() const { return _requiresBuyBaseFunc; }
	/// Gets the dividers used for recovery of special items.
	const std::map<std::string, int> &getRecoveryDividers() const;
	/// Gets the item(s) to be recovered instead of this item.
	const std::map<const RuleItem*, std::vector<int> > &getRecoveryTransformations() const;
	/// Gets the item's categories.
	const std::vector<std::string> &getCategories() const;
	/// Checks if the item belongs to a category.
	bool belongsToCategory(const std::string &category) const;
	/// Gets unit rule if the item is vehicle weapon.
	Unit* getVehicleUnit() const;
	/// Gets the item's size.
	double getSize() const;
	/// Gets the item's monthly buy limit.
	int getMonthlyBuyLimit() const { return _monthlyBuyLimit; }
	/// Gets the item's purchase cost.
	int getBuyCost() const;
	/// Gets the item's sale cost.
	int getSellCost() const;
	/// Gets the item's transfer time.
	int getTransferTime() const;
	/// Gets the item's weight.
	int getWeight() const;
	/// Gets the item's maximum throw range.
	int getThrowRange() const { return _throwRange; }
	int getThrowRangeSq() const { return _throwRange * _throwRange; }
	/// Gets the item's maximum underwater throw range.
	int getUnderwaterThrowRange() const { return _underwaterThrowRange; }
	int getUnderwaterThrowRangeSq() const { return _underwaterThrowRange * _underwaterThrowRange; }
	/// Gets the item's reference in BIGOBS.PCK for use in inventory.
	int getBigSprite() const;
	/// Gets the item's reference in FLOOROB.PCK for use in battlescape.
	int getFloorSprite() const;
	/// Gets the item's reference in HANDOB.PCK for use in inventory.
	int getHandSprite() const;
	/// Gets the item's reference in SPICONS.DAT for special weapon button.
	int getSpecialIconSprite() const;

	/// Gets cost of moving item around inventory.
	int getInventoryMoveCostPercent() const { return _inventoryMoveCostPercent; }
	/// Gets if the item is two-handed.
	bool isTwoHanded() const;
	/// Gets if the item can only be used by both hands.
	bool isBlockingBothHands() const;
	/// Gets if the item is fixed.
	bool isFixed() const;
	/// Do show fixed weapon on unit.
	bool getFixedShow() const;
	/// Get name of the default inventory slot.
	const RuleInventory* getDefaultInventorySlot() const { return _defaultInventorySlot; }
	/// Get inventory slot default X position.
	int getDefaultInventorySlotX() const { return _defaultInvSlotX; }
	/// Get inventory slot default Y position.
	int getDefaultInventorySlotY() const { return _defaultInvSlotY; }
	/// Gets the item's supported inventory sections.
	const std::vector<const RuleInventory*> &getSupportedInventorySections() const { return _supportedInventorySections; }
	/// Checks if the item can be placed into a given inventory section.
	bool canBePlacedIntoInventorySection(const RuleInventory* inventorySection) const;

	/// Gets if the item is a launcher.
	int getWaypoints() const;
	/// Gets the item's bullet sprite reference.
	int getBulletSprite() const;
	/// Gets the item's reload sound.
	int getReloadSound() const;
	const std::vector<int> &getReloadSoundRaw() const { return _reloadSound; }
	/// Gets the item's prime sound.
	int getPrimeSound() const;
	const std::vector<int>& getPrimeSoundRaw() const { return _primeSound; }
	/// Gets the item's unprime sound.
	int getUnprimeSound() const;
	const std::vector<int>& getUnprimeSoundRaw() const { return _unprimeSound; }
	/// Gets the item's fire sound.
	int getFireSound() const;
	const std::vector<int> &getFireSoundRaw() const { return _fireSound; }

	/// Gets the item's hit sound.
	int getHitSound() const;
	const std::vector<int> &getHitSoundRaw() const { return _hitSound; }
	/// Gets the item's hit animation.
	int getHitAnimation() const;
	/// Gets the item's hit animation frame count.
	int getHitAnimationFrames() const { return _hitAnimFrames; }
	/// Gets the item's hit sound.
	int getHitMissSound() const;
	const std::vector<int> &getHitMissSoundRaw() const { return _hitMissSound; }
	/// Gets the item's psiUse miss animation.
	int getHitMissAnimation() const;
	/// Gets the item's psiUse miss animation frame count.
	int getHitMissAnimationFrames() const { return _hitMissAnimFrames; }

	/// What sound does this weapon make when you swing this at someone?
	int getMeleeSound() const;
	const std::vector<int> &getMeleeSoundRaw() const { return _meleeSound; }
	/// Get the melee animation starting frame (comes from hit.pck).
	int getMeleeAnimation() const;
	/// Gets the melee animation frame count.
	int getMeleeAnimationFrames() const { return _meleeAnimFrames; }
	/// What sound does this weapon make when you miss a swing?
	int getMeleeMissSound() const;
	const std::vector<int> &getMeleeMissSoundRaw() const { return _meleeMissSound; }
	/// Get the melee miss animation starting frame (comes from hit.pck).
	int getMeleeMissAnimation() const;
	/// Gets the melee miss animation frame count.
	int getMeleeMissAnimationFrames() const { return _meleeMissAnimFrames; }
	/// What sound does this weapon make when you punch someone in the face with it?
	int getMeleeHitSound() const;
	const std::vector<int> &getMeleeHitSoundRaw() const { return _meleeHitSound; }
	/// What sound does explosion sound?
	int getExplosionHitSound() const;
	const std::vector<int> &getExplosionHitSoundRaw() const { return _explosionHitSound; }

	/// Gets the item's psi hit sound.
	int getPsiSound() const;
	const std::vector<int> &getPsiSoundRaw() const { return _psiSound; }
	/// Get the psi animation starting frame (comes from hit.pck).
	int getPsiAnimation() const;
	/// Gets the psi animation frame count.
	int getPsiAnimationFrames() const { return _psiAnimFrames; }
	/// Gets the item's psi miss sound.
	int getPsiMissSound() const;
	const std::vector<int> &getPsiMissSoundRaw() const { return _psiMissSound; }
	/// Get the psi miss animation starting frame (comes from hit.pck).
	int getPsiMissAnimation() const;
	/// Gets the psi miss animation frame count.
	int getPsiMissAnimationFrames() const { return _psiMissAnimFrames; }


	/// Gets the item's power.
	int getPower() const;
	/// Gets the item's power used for AoE explosion animation.
	int getPowerForAnimation() const { return _powerForAnimation; }
	/// Should the item's power be displayed in Ufopedia or not?
	bool getHidePower() const { return _hidePower; }
	/// Ok, so this isn't a melee type weapon but we're using it for melee... how much damage should it do?
	int getMeleePower() const;

	/// Gets amount of power dropped for range in voxels.
	float getPowerRangeReduction(float range) const;
	float getPowerRangeReductionRaw() const { return _powerRangeReduction; }
	float getPowerRangeThresholdRaw() const { return _powerRangeThreshold; }
	/// Gets amount of psi accuracy dropped for range in voxels.
	float getPsiAccuracyRangeReduction(float range) const;

	/// Get additional power from unit statistics
	int getPowerBonus(BattleActionAttack::ReadOnly attack) const;
	const RuleStatBonus *getDamageBonusRaw() const { return &_damageBonus; }

	/// Get additional power for melee attack in range weapon from unit statistics.
	int getMeleeBonus(BattleActionAttack::ReadOnly attack) const;
	const RuleStatBonus *getMeleeBonusRaw() const { return &_meleeBonus; }

	/// Get multiplier of accuracy from unit statistics
	int getAccuracyMultiplier(BattleActionAttack::ReadOnly attack) const;
	const RuleStatBonus *getAccuracyMultiplierRaw() const { return &_accuracyMulti; }

	/// Get multiplier of melee hit chance from unit statistics
	int getMeleeMultiplier(BattleActionAttack::ReadOnly  attack) const;
	const RuleStatBonus *getMeleeMultiplierRaw() const { return &_meleeMulti; }

	/// Get multiplier of throwing from unit statistics
	int getThrowMultiplier(BattleActionAttack::ReadOnly attack) const;
	const RuleStatBonus *getThrowMultiplierRaw() const { return &_throwMulti; }

	/// Get multiplier of close quarters combat from unit statistics
	int getCloseQuartersMultiplier(BattleActionAttack::ReadOnly attack) const;
	const RuleStatBonus *getCloseQuartersMultiplierRaw() const { return &_closeQuartersMulti; }


	/// Get configuration of aimed shot action.
	const RuleItemAction *getConfigAimed() const;
	/// Get configuration of autoshot action.
	const RuleItemAction *getConfigAuto() const;
	/// Get configuration of snapshot action.
	const RuleItemAction *getConfigSnap() const;
	/// Get configuration of melee action.
	const RuleItemAction *getConfigMelee() const;


	/// Gets the item's aimed shot accuracy.
	int getAccuracyAimed() const;
	/// Gets the item's autoshot accuracy.
	int getAccuracyAuto() const;
	/// Gets the item's snapshot accuracy.
	int getAccuracySnap() const;
	/// Gets the item's melee accuracy.
	int getAccuracyMelee() const;
	/// Gets the item's use accuracy.
	int getAccuracyUse() const;
	/// Gets the item's mind control accuracy.
	int getAccuracyMind() const;
	/// Gets the item's panic accuracy.
	int getAccuracyPanic() const;
	/// Gets the item's throw accuracy.
	int getAccuracyThrow() const;
	/// Gets the item's close quarters combat accuracy.
	int getAccuracyCloseQuarters(Mod *mod) const;
	/// Get penalty for firing this weapon on out-of-LOS targets
	int getNoLOSAccuracyPenalty(Mod *mod) const;
	/// Get `cost` and `flat` values for performing given action using this item
	std::pair<RuleItemUseCost, RuleItemUseCost> getCostsAction(BattleActionType action, const BattleUnit *unit, const BattleItem *weapon) const;
	/// Gets the item's load TU cost.
	int getTULoad(int slot) const;
	/// Gets the item's unload TU cost.
	int getTUUnload(int slot) const;
	/// Gets maximum number of clips that fit in given slot.
	int getChamberSize(int slot) const;
	/// Gets the ammo type for a vehicle.
	const RuleItem* getVehicleClipAmmo() const;
	/// Gets the maximum number of rounds for a vehicle. E.g. a vehicle that can load 6 clips with 10 rounds each, returns 60.
	int getVehicleClipSize() const;
	/// Gets the number of clips needed to fully load a vehicle. E.g. a vehicle that holds max 60 rounds and clip size is 10, returns 6.
	int getVehicleClipsLoaded() const;
	/// Gets list of compatible ammo.
	const std::vector<const RuleItem*> *getPrimaryCompatibleAmmo() const;
	/// Get slot position for ammo type.
	int getSlotForAmmo(const RuleItem* type) const;
	/// Get slot position for ammo type.
	const std::vector<const RuleItem*> *getCompatibleAmmoForSlot(int slot) const;

	/// Gets the item's damage type.
	const RuleDamageType *getDamageType() const;
	bool isDamageTypeSet() const { return _damageTypeSet; }
	/// Gets the item's melee damage type for range weapons.
	const RuleDamageType *getMeleeType() const;
	bool isMeleeTypeSet() const { return _meleeTypeSet; }
	/// Gets the item's type.
	BattleType getBattleType() const;
	/// Gets the item's fuse type.
	BattleFuseType getFuseTimerType() const;
	/// Gets the item's default fuse value.
	int getFuseTimerDefault() const;
	/// Is this item (e.g. a mine) hidden on the minimap?
	bool isHiddenOnMinimap() const;
	/// Can this BT_ANOMALY item discharge more than once per turn?
	bool getMultipleDischarges() const;
	/// Get detection radius for proximity mines and anomalies
	int getProximityRadius() const;
	/// Get power of light emitted by this item (Only if battleType != BT_FLARE)
	int getLightRadius() const;
	/// Get fuse trigger event.
	const RuleItemFuseTrigger *getFuseTriggerEvent() const;

	/// Gets the item's inventory width.
	int getInventoryWidth() const;
	/// Gets the item's inventory height.
	int getInventoryHeight() const;
	/// Gets the ammo amount.
	int getClipSize() const;
	/// Gets the chance of special effect like zombify or corpse explosion or mine triggering.
	int getSpecialChance() const;
	/// Draws the item's hand sprite onto a surface.
	void drawHandSprite(const SurfaceSet *texture, Surface *surface, const BattleItem *item = 0, const SavedBattleGame* save = 0, int animFrame = 0) const;
	/// item's hand spite x offset
	int getHandSpriteOffX() const;
	/// item's hand spite y offset
	int getHandSpriteOffY() const;
	/// Gets the medikit heal quantity.
	int getHealQuantity() const;
	/// Gets the medikit pain killer quantity.
	int getPainKillerQuantity() const;
	/// Gets the medikit stimulant quantity.
	int getStimulantQuantity() const;
	/// Gets the medikit wound healed per shot.
	int getWoundRecovery() const;
	/// Gets the medikit health recovered per shot.
	int getHealthRecovery() const;
	/// Gets the medikit energy recovered per shot.
	int getEnergyRecovery() const;
	/// Gets the medikit stun recovered per shot.
	int getStunRecovery() const;
	/// Gets the medikit mana recovered per shot.
	int getManaRecovery() const { return _manaRecovery; }
	/// Gets the medikit morale recovered per shot.
	int getMoraleRecovery() const;
	/// Gets the medikit morale recovered based on missing health.
	float getPainKillerRecovery() const;
	/// Gets the medikit's allowed targets.
	bool getAllowTargetSelf() const { return _medikitTargetSelf; }
	bool getAllowTargetImmune() const { return _medikitTargetImmune; }
	bool getAllowTargetGround() const { return _medikitTargetMatrix & 21; } // 1 + 4 + 16
	bool getAllowTargetStanding() const { return _medikitTargetMatrix & 42; } // 2 + 8 + 32
	bool getAllowTargetFriendGround() const { return _medikitTargetMatrix & 1; }
	bool getAllowTargetFriendStanding() const { return _medikitTargetMatrix & 2; }
	bool getAllowTargetNeutralGround() const { return _medikitTargetMatrix & 4; }
	bool getAllowTargetNeutralStanding() const { return _medikitTargetMatrix & 8; }
	bool getAllowTargetHostileGround() const { return _medikitTargetMatrix & 16; }
	bool getAllowTargetHostileStanding() const { return _medikitTargetMatrix & 32; }
	int getMedikitTargetMatrixRaw() const { return _medikitTargetMatrix; }
	/// Is this (medikit-type & items with prime) item consumable?
	bool isConsumable() const;
	/// Does this item extinguish fire?
	bool isFireExtinguisher() const;
	/// Is this item explode in hands?
	bool isExplodingInHands() const;
	/// If this is used as a speacialWeapon, is it accessed by empty hand?
	bool isSpecialUsingEmptyHand() const;
	/// Display icon in an empty hand?
	bool showSpecialInEmptyHand() const { return _specialUseEmptyHandShow; }
	/// Gets the medikit use type.
	BattleMediKitType getMediKitType() const;
	/// Gets the medikit custom background.
	const std::string &getMediKitCustomBackground() const;
	/// Gets the max explosion radius.
	int getExplosionRadius(BattleActionAttack::ReadOnly attack) const;
	/// Gets the recovery points score
	int getRecoveryPoints() const;
	/// Gets the item's armor.
	int getArmor() const;
	/// Check if item is normal inventory item.
	bool isInventoryItem() const;
	/// Checks if item have some use in battlescape.
	bool isUsefulBattlescapeItem() const;
	/// Gets the item's recoverability.
	bool isRecoverable() const;
	/// Gets the corpse item's recoverability.
	bool isCorpseRecoverable() const;
	/// Checks if the item can be equipped in base defense mission.
	bool canBeEquippedBeforeBaseDefense() const;
	/// Checks if the item can be equipped to craft inventory.
	bool canBeEquippedToCraftInventory() const;
	/// Gets the item's turret type.
	int getTurretType() const;
	/// Gets first turn when AI can use item.
	int getAIUseDelay(const Mod *mod = 0) const;
	/// Gets how many melee hits AI should do.
	int getAIMeleeHitCount() const;
	/// Checks if this a live alien.
	bool isAlien() const;
	/// Returns to which type of prison does the live alien belong.
	int getPrisonType() const;

	/// Should this weapon arc?
	bool getArcingShot() const;
	/// Which experience training mode to use for this weapon?
	ExperienceTrainingMode getExperienceTrainingMode() const;
	/// How much mana experience does this weapon provide?
	int getManaExperience() const { return _manaExperience; }
	/// How much do aliens want this thing?
	int getAttraction() const;
	/// Get the list weight for this item.
	int getListOrder() const;
	/// How fast does a projectile fired from this weapon travel?
	int getBulletSpeed() const;
	/// How fast does the explosion animation play?
	int getExplosionSpeed() const;
	/// Get name of medikit action for action menu.
	const std::string &getMedikitActionName() const { return _medikitActionName; }
	/// Get name of psi attack for action menu.
	const std::string &getPsiAttackName() const { return _psiAttackName; }
	/// Get name of prime action for action menu.
	const std::string &getPrimeActionName() const { return _primeActionName; }
	/// Get message for prime action.
	const std::string &getPrimeActionMessage() const { return _primeActionMessage; }
	/// Get name of unprime action for action menu.
	const std::string &getUnprimeActionName() const { return _unprimeActionName; }
	/// Get message for unprime action.
	const std::string &getUnprimeActionMessage() const { return _unprimeActionMessage; }
	/// is this item a 2 handed weapon?
	bool isRifle() const;
	/// is this item a single handed weapon?
	bool isPistol() const;
	/// Get the max range of this weapon.
	int getMaxRange() const;
	/// Checks whether a given distance is out of range for this item.
	bool isOutOfRange(int distanceSq) const;
	/// Get the max range of aimed shots with this weapon.
	int getAimRange() const;
	/// Get the max range of snap shots with this weapon.
	int getSnapRange() const;
	/// Get the max range of auto shots with this weapon.
	int getAutoRange() const;
	/// Get the minimum effective range of this weapon.
	int getMinRange() const;
	/// Get the accuracy dropoff of this weapon.
	int getDropoff() const;
	/// Get the number of projectiles to trace.
	int getShotgunPellets() const;
	/// Get the shotgun behavior type.
	int getShotgunBehaviorType() const;
	/// Get the spread of shotgun projectiles.
	int getShotgunSpread() const;
	/// Get the shotgun choke value.
	int getShotgunChoke() const;
	/// Gets the weapon's zombie unit.
	const std::string &getZombieUnit(const BattleUnit* victim) const;
	const std::map<std::string, std::string> &getZombieUnitByArmorMaleRaw() const { return _zombieUnitByArmorMale; }
	const std::map<std::string, std::string> &getZombieUnitByArmorFemaleRaw() const { return _zombieUnitByArmorFemale; }
	const std::map<std::string, std::string> &getZombieUnitByTypeRaw() const { return _zombieUnitByType; }
	/// Gets the weapon's spawn unit.
	const std::string &getSpawnUnit() const;
	/// Gets which faction the spawned unit should have.
	int getSpawnUnitFaction() const;
	/// Checks if this item can be used to target a given faction.
	bool isTargetAllowed(UnitFaction targetFaction) const;
	int getTargetMatrixRaw() const { return _targetMatrix; }
	/// Check if LOS is required to use this item (only applies to psionic type items)
	bool isLOSRequired() const;
	/// Is this item restricted to underwater use?
	bool isWaterOnly() const;
	/// Is this item restricted to land use?
	bool isLandOnly() const;
	/// Is this item require unit with psi skill to use it?
	bool isPsiRequired() const;
	/// Does this item need mana to operate?
	bool isManaRequired() const;
	/// Get the associated special type of this item.
	int getSpecialType() const;
	/// Get the color offset to use for the vapor trail.
	int getVaporColor(int depth) const;
	/// Gets the vapor cloud density.
	int getVaporDensity(int depth) const;
	/// Gets the vapor cloud probability.
	int getVaporProbability(int depth) const;
	/// Gets the index of the sprite in the CustomItemPreview sprite set
	const std::vector<int> &getCustomItemPreviewIndex() const;
	/// Gets the kneel bonus.
	int getKneelBonus(Mod *mod) const;
	/// Gets the one-handed penalty.
	int getOneHandedPenalty(Mod *mod) const;
	/// Gets the monthly salary.
	int getMonthlySalary() const;
	/// Gets the monthly maintenance.
	int getMonthlyMaintenance() const;
	/// Gets how many waypoints are used for a "spray" attack
	int getSprayWaypoints() const;
	/// Gets whether or not this weapon prevents being spotted on hit or kill
	bool getSilenced() const;
	/// Gets script.
	template<typename Script>
	const typename Script::Container &getScript() const { return _battleItemScripts.get<Script>(); }
	/// Get all script values.
	const ScriptValues<RuleItem> &getScriptValuesRaw() const { return _scriptValues; }
};

}
