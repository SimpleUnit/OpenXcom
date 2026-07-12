/*
 * Copyright 2010-2020 OpenXcom Developers.
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
#include "RuleSkill.h"
#include "Mod.h"
#include "../Engine/Collections.h"
#include "../Engine/ScriptBind.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"

namespace OpenXcom
{

RuleSkill::RuleSkill(const std::string& type) : _type(type),
	_targetMode(BA_NONE), _compatibleBattleType(BT_NONE), _isPsiRequired(false), _checkHandsOnly(true), _checkHandsOnly2(false)
{
}

/**
 * Loads the skill from a YAML file.
 * @param node YAML node.
 * @param mod Mod for the skill.
 */
void RuleSkill::load(const YAML::YamlNodeReader& reader, Mod *mod, const ModScript& parsers)
{
	if (const auto& parent = reader["refNode"])
	{
		load(parent, mod, parsers);
	}

	int targetMode = (int)reader["targetMode"].readVal(_targetMode);
	targetMode = targetMode < 0 ? 0 : targetMode;
	targetMode = targetMode > BA_CQB ? 0 : targetMode;
	_targetMode = static_cast<BattleActionType>(targetMode);

	int compBattleType = (int)reader["battleType"].readVal(_compatibleBattleType);
	compBattleType = compBattleType < 0 ? 0 : compBattleType;
	compBattleType = compBattleType > BT_CORPSE ? 0 : compBattleType;
	_compatibleBattleType = static_cast<BattleType>(compBattleType);

	reader.tryRead("isPsiRequired", _isPsiRequired);
	reader.tryRead("checkHandsOnly", _checkHandsOnly);
	reader.tryRead("checkHandsOnly2", _checkHandsOnly2);

	_cost.loadCost(reader, "Use");
	_flat.loadFlat(reader, "Use");

	mod->loadUnorderedNames(_type, _compatibleWeaponNames, reader["compatibleWeapons"]);
	mod->loadUnorderedNames(_type,_requiredBonusNames, reader["requiredBonuses"]);

	_scriptValues.load(reader, parsers.getShared());

	_skillScripts.load(_type, reader, parsers.skillScripts);
}

/**
 * Cross link with other Rules.
 */
void RuleSkill::afterLoad(const Mod* mod)
{
	mod->linkRule(_compatibleWeapons, _compatibleWeaponNames);
	mod->linkRule(_requiredBonuses, _requiredBonusNames);

	//remove not needed data
	Collections::removeAll(_compatibleWeaponNames);
	Collections::removeAll(_requiredBonusNames);
}


////////////////////////////////////////////////////////////
//					Script binding
////////////////////////////////////////////////////////////

namespace
{

void getTypeScript(const RuleSkill* r, ScriptText& txt)
{
	if (r)
	{
		txt = { r->getType().c_str() };
		return;
	}
	else
	{
		txt = ScriptText::empty;
	}
}

std::string debugDisplayScript(const RuleSkill* rs)
{
	if (rs)
	{
		std::string s;
		s += RuleSkill::ScriptName;
		s += "(name: \"";
		s += rs->getType();
		s += "\")";
		return s;
	}
	else
	{
		return "null";
	}
}

}

/**
 * Gets the cost (and whether or not the cost is flat) of given action
 * @param unit Which unit is performing this action (or null pointer if not applicable)
 * @param item Which item will be used to perform the action (or null pointer if not applicable)
 * @return The pair of cost structs. First element for `cost` variable, second element for `flat` variable.
 */
std::pair<RuleItemUseCost, RuleItemUseFlat> RuleSkill::getCosts(const BattleUnit *unit, const BattleItem *item) const
{
	RuleItemUseCost resCost = getDefault(_cost);
	RuleItemUseFlat resFlat = getDefault(_flat);

	ModScript::SkillCost::Output args{resCost.Time, resCost.Energy, resCost.Morale, resCost.Health, resCost.Stun, resCost.Mana,
									  resFlat.Time, resFlat.Energy, resFlat.Morale, resFlat.Health, resFlat.Stun, resFlat.Mana};
	ModScript::SkillCost::Worker work{this, unit, item, item ? item->getRules() : nullptr, _targetMode};
	work.execute(_skillScripts, args);
	resCost.Time = std::get<0>(args.data);
	resCost.Energy = std::get<1>(args.data);
	resCost.Morale = std::get<2>(args.data);
	resCost.Health = std::get<3>(args.data);
	resCost.Stun = std::get<4>(args.data);
	resCost.Mana = std::get<5>(args.data);
	resFlat.Time = std::get<6>(args.data);
	resFlat.Energy = std::get<7>(args.data);
	resFlat.Morale = std::get<8>(args.data);
	resFlat.Health = std::get<9>(args.data);
	resFlat.Stun = std::get<10>(args.data);
	resFlat.Mana = std::get<11>(args.data);
	return std::pair(resCost, resFlat);
}

/**
 * Register RuleSkill in script parser.
 * @param parser Script parser.
 */
void RuleSkill::ScriptRegister(ScriptParserBase* parser)
{
	Bind<RuleSkill> rs = { parser };

	rs.add<&getTypeScript>("getType");

	rs.addScriptValue<&RuleSkill::_scriptValues>();
	rs.addDebugDisplay<&debugDisplayScript>();
}

}
