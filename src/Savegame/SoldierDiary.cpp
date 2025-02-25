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
#include "SoldierDiary.h"
#include <algorithm>
#include "../Mod/RuleCommendations.h"
#include "../Mod/Mod.h"
#include "BattleUnitStatistics.h"
#include "MissionStatistics.h"
#include <algorithm>

namespace OpenXcom
{

/**
 * Initializes a new blank diary.
 */
SoldierDiary::SoldierDiary() : _daysWoundedTotal(0), _totalShotByFriendlyCounter(0), _totalShotFriendlyCounter(0), _loneSurvivorTotal(0),
	_monthsService(0), _unconciousTotal(0),	_shotAtCounterTotal(0), _hitCounterTotal(0), _ironManTotal(0), _longDistanceHitCounterTotal(0),
	_lowAccuracyHitCounterTotal(0), _shotsFiredCounterTotal(0), _shotsLandedCounterTotal(0), _shotAtCounter10in1Mission(0), _hitCounter5in1Mission(0),
	_timesWoundedTotal(0), _KIA(0), _allAliensKilledTotal(0), _allAliensStunnedTotal(0), _woundsHealedTotal(0), _allUFOs(0), _allMissionTypes(0),
	_statGainTotal(0), _revivedUnitTotal(0), _wholeMedikitTotal(0), _braveryGainTotal(0), _bestOfRank(0),
	_MIA(0), _martyrKillsTotal(0), _postMortemKills(0), _slaveKillsTotal(0), _bestSoldier(false),
	_revivedSoldierTotal(0), _revivedHostileTotal(0), _revivedNeutralTotal(0), _globeTrotter(false)
{
}

/**
 *
 */
SoldierDiary::~SoldierDiary()
{
	for (std::vector<SoldierCommendations*>::iterator i = _commendations.begin(); i != _commendations.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<BattleUnitKills*>::iterator i = _killList.begin(); i != _killList.end(); ++i)
	{
		delete *i;
	}
}

/**
 * Loads the diary from a YAML file.
 * @param node YAML node.
 */
void SoldierDiary::load(const YAML::Node& node, const Mod *mod)
{
	if (const YAML::Node &commendations = node["commendations"])
	{
		for (YAML::const_iterator i = commendations.begin(); i != commendations.end(); ++i)
		{
			SoldierCommendations *sc = new SoldierCommendations(*i, mod);
			if (sc->getRule())
			{
				_commendations.push_back(sc);
			}
			else
			{
				// obsolete commendation, ignore it... otherwise it would cause a crash later
				delete sc;
			}
		}
	}
	if (const YAML::Node &killList = node["killList"])
	{
		for (YAML::const_iterator i = killList.begin(); i != killList.end(); ++i)
			_killList.push_back(new BattleUnitKills(*i));
	}
	_missionIdList = node["missionIdList"].as<std::vector<int> >(_missionIdList);
	_daysWoundedTotal = node["daysWoundedTotal"].as<int>(_daysWoundedTotal);
	_totalShotByFriendlyCounter = node["totalShotByFriendlyCounter"].as<int>(_totalShotByFriendlyCounter);
	_totalShotFriendlyCounter = node["totalShotFriendlyCounter"].as<int>(_totalShotFriendlyCounter);
	_loneSurvivorTotal = node["loneSurvivorTotal"].as<int>(_loneSurvivorTotal);
	_monthsService = node["monthsService"].as<int>(_monthsService);
	_unconciousTotal = node["unconciousTotal"].as<int>(_unconciousTotal);
	_shotAtCounterTotal = node["shotAtCounterTotal"].as<int>(_shotAtCounterTotal);
	_hitCounterTotal = node["hitCounterTotal"].as<int>(_hitCounterTotal);
	_ironManTotal = node["ironManTotal"].as<int>(_ironManTotal);
	_longDistanceHitCounterTotal = node["longDistanceHitCounterTotal"].as<int>(_longDistanceHitCounterTotal);
	_lowAccuracyHitCounterTotal = node["lowAccuracyHitCounterTotal"].as<int>(_lowAccuracyHitCounterTotal);
	_shotsFiredCounterTotal = node["shotsFiredCounterTotal"].as<int>(_shotsFiredCounterTotal);
	_shotsLandedCounterTotal = node["shotsLandedCounterTotal"].as<int>(_shotsLandedCounterTotal);
	_shotAtCounter10in1Mission = node["shotAtCounter10in1Mission"].as<int>(_shotAtCounter10in1Mission);
	_hitCounter5in1Mission = node["hitCounter5in1Mission"].as<int>(_hitCounter5in1Mission);
	_timesWoundedTotal = node["timesWoundedTotal"].as<int>(_timesWoundedTotal);
	_allAliensKilledTotal = node["allAliensKilledTotal"].as<int>(_allAliensKilledTotal);
	_allAliensStunnedTotal = node["allAliensStunnedTotal"].as<int>(_allAliensStunnedTotal);
	_woundsHealedTotal = node["woundsHealedTotal"].as<int>(_woundsHealedTotal);
	_allUFOs = node["allUFOs"].as<int>(_allUFOs);
	_allMissionTypes = node["allMissionTypes"].as<int>(_allMissionTypes);
	_statGainTotal = node["statGainTotal"].as<int>(_statGainTotal);
	_revivedUnitTotal = node["revivedUnitTotal"].as<int>(_revivedUnitTotal);
	_revivedSoldierTotal = node["revivedSoldierTotal"].as<int>(_revivedSoldierTotal);
	_revivedHostileTotal = node["revivedHostileTotal"].as<int>(_revivedHostileTotal);
	_revivedNeutralTotal = node["revivedNeutralTotal"].as<int>(_revivedNeutralTotal);
	_wholeMedikitTotal = node["wholeMedikitTotal"].as<int>(_wholeMedikitTotal);
	_braveryGainTotal = node["braveryGainTotal"].as<int>(_braveryGainTotal);
	_bestOfRank = node["bestOfRank"].as<int>(_bestOfRank);
	_bestSoldier = node["bestSoldier"].as<bool>(_bestSoldier);
	_martyrKillsTotal = node["martyrKillsTotal"].as<int>(_martyrKillsTotal);
	_postMortemKills = node["postMortemKills"].as<int>(_postMortemKills);
	_globeTrotter = node["globeTrotter"].as<bool>(_globeTrotter);
	_slaveKillsTotal = node["slaveKillsTotal"].as<int>(_slaveKillsTotal);
}

/**
 * Saves the diary to a YAML file.
 * @return YAML node.
 */
YAML::Node SoldierDiary::save() const
{
	YAML::Node node;
	for (std::vector<SoldierCommendations*>::const_iterator i = _commendations.begin(); i != _commendations.end(); ++i)
			node["commendations"].push_back((*i)->save());
	for (std::vector<BattleUnitKills*>::const_iterator i = _killList.begin(); i != _killList.end(); ++i)
			node["killList"].push_back((*i)->save());
	if (!_missionIdList.empty()) { YAML::Node t; t = _missionIdList; t.SetStyle(YAML::EmitterStyle::Flow); node["missionIdList"] = t; }
	if (_daysWoundedTotal) node["daysWoundedTotal"] = _daysWoundedTotal;
	if (_totalShotByFriendlyCounter) node["totalShotByFriendlyCounter"] = _totalShotByFriendlyCounter;
	if (_totalShotFriendlyCounter) node["totalShotFriendlyCounter"] = _totalShotFriendlyCounter;
	if (_loneSurvivorTotal) node["loneSurvivorTotal"] = _loneSurvivorTotal;
	if (_monthsService) node["monthsService"] = _monthsService;
	if (_unconciousTotal) node["unconciousTotal"] = _unconciousTotal;
	if (_shotAtCounterTotal) node["shotAtCounterTotal"] = _shotAtCounterTotal;
	if (_hitCounterTotal) node["hitCounterTotal"] = _hitCounterTotal;
	if (_ironManTotal) node["ironManTotal"] = _ironManTotal;
	if (_longDistanceHitCounterTotal) node["longDistanceHitCounterTotal"] = _longDistanceHitCounterTotal;
	if (_lowAccuracyHitCounterTotal) node["lowAccuracyHitCounterTotal"] = _lowAccuracyHitCounterTotal;
	if (_shotsFiredCounterTotal) node["shotsFiredCounterTotal"] = _shotsFiredCounterTotal;
	if (_shotsLandedCounterTotal) node["shotsLandedCounterTotal"] = _shotsLandedCounterTotal;
	if (_shotAtCounter10in1Mission) node["shotAtCounter10in1Mission"] = _shotAtCounter10in1Mission;
	if (_hitCounter5in1Mission) node["hitCounter5in1Mission"] = _hitCounter5in1Mission;
	if (_timesWoundedTotal) node["timesWoundedTotal"] = _timesWoundedTotal;
	if (_allAliensKilledTotal) node["allAliensKilledTotal"] = _allAliensKilledTotal;
	if (_allAliensStunnedTotal) node["allAliensStunnedTotal"] = _allAliensStunnedTotal;
	if (_woundsHealedTotal) node["woundsHealedTotal"] = _woundsHealedTotal;
	if (_allUFOs) node["allUFOs"] = _allUFOs;
	if (_allMissionTypes) node["allMissionTypes"] = _allMissionTypes;
	if (_statGainTotal) node["statGainTotal"] =_statGainTotal;
	if (_revivedUnitTotal) node["revivedUnitTotal"] = _revivedUnitTotal;
	if (_revivedSoldierTotal) node["revivedSoldierTotal"] = _revivedSoldierTotal;
	if (_revivedHostileTotal) node["revivedHostileTotal"] = _revivedHostileTotal;
	if (_revivedNeutralTotal) node["revivedNeutralTotal"] = _revivedNeutralTotal;
	if (_wholeMedikitTotal) node["wholeMedikitTotal"] = _wholeMedikitTotal;
	if (_braveryGainTotal) node["braveryGainTotal"] = _braveryGainTotal;
	if (_bestOfRank) node["bestOfRank"] = _bestOfRank;
	if (_bestSoldier) node["bestSoldier"] = _bestSoldier;
	if (_martyrKillsTotal) node["martyrKillsTotal"] = _martyrKillsTotal;
	if (_postMortemKills) node["postMortemKills"] = _postMortemKills;
	if (_globeTrotter) node["globeTrotter"] = _globeTrotter;
	if (_slaveKillsTotal) node["slaveKillsTotal"] = _slaveKillsTotal;
	return node;
}

/**
 * Update soldier diary statistics.
 * @param unitStatistics BattleUnitStatistics to get stats from.
 * @param missionStatistics MissionStatistics to get stats from.
 */
void SoldierDiary::updateDiary(BattleUnitStatistics *unitStatistics, std::vector<MissionStatistics*> *allMissionStatistics, Mod *rules)
{
	if (allMissionStatistics->empty()) return;
	MissionStatistics* missionStatistics = allMissionStatistics->back();
	std::vector<BattleUnitKills*> &unitKills = unitStatistics->kills;
	for (std::vector<BattleUnitKills*>::const_iterator kill = unitKills.begin() ; kill != unitKills.end() ; ++kill)
	{
		(*kill)->makeTurnUnique();
		_killList.push_back(*kill);
	}
	unitKills.clear();
	if (missionStatistics->success)
	{
		if (unitStatistics->loneSurvivor)
			_loneSurvivorTotal++;
		if (unitStatistics->ironMan)
			_ironManTotal++;
		if (unitStatistics->nikeCross)
			_allAliensKilledTotal++;
		if (unitStatistics->mercyCross)
			_allAliensStunnedTotal++;
	}
	_daysWoundedTotal += unitStatistics->daysWounded;
	if (unitStatistics->daysWounded)
		_timesWoundedTotal++;

	if (unitStatistics->wasUnconcious)
		_unconciousTotal++;
	_shotAtCounterTotal += unitStatistics->shotAtCounter;
	_shotAtCounter10in1Mission += (unitStatistics->shotAtCounter)/10;
	_hitCounterTotal += unitStatistics->hitCounter;
	_hitCounter5in1Mission += (unitStatistics->hitCounter)/5;
	_totalShotByFriendlyCounter += unitStatistics->shotByFriendlyCounter;
	_totalShotFriendlyCounter += unitStatistics->shotFriendlyCounter;
	_longDistanceHitCounterTotal += unitStatistics->longDistanceHitCounter;
	_lowAccuracyHitCounterTotal += unitStatistics->lowAccuracyHitCounter;
	_shotsFiredCounterTotal += unitStatistics->shotsFiredCounter;
	_shotsLandedCounterTotal += unitStatistics->shotsLandedCounter;
	if (unitStatistics->KIA)
		_KIA++;
	if (unitStatistics->MIA)
		_MIA++;
	_woundsHealedTotal += unitStatistics->woundsHealed;
	if (getUFOTotal(allMissionStatistics).size() >= rules->getUfosList().size())
		_allUFOs = 1;
	if ((getUFOTotal(allMissionStatistics).size() + getTypeTotal(allMissionStatistics).size()) == (rules->getUfosList().size() + rules->getDeploymentsList().size() - 2))
		_allMissionTypes = 1;
	if (getCountryTotal(allMissionStatistics).size() == rules->getCountriesList().size())
		_globeTrotter = true;
	_martyrKillsTotal += unitStatistics->martyr;
	_slaveKillsTotal += unitStatistics->slaveKills;

	// Stat change long hand calculation
	_statGainTotal = 0; // Reset.
	_statGainTotal += unitStatistics->delta.tu;
	_statGainTotal += unitStatistics->delta.stamina;
	_statGainTotal += unitStatistics->delta.health;
	_statGainTotal += unitStatistics->delta.bravery / 10; // Normalize
	_statGainTotal += unitStatistics->delta.reactions;
	_statGainTotal += unitStatistics->delta.firing;
	_statGainTotal += unitStatistics->delta.throwing;
	_statGainTotal += unitStatistics->delta.strength;
	_statGainTotal += unitStatistics->delta.mana;
	_statGainTotal += unitStatistics->delta.psiStrength;
	_statGainTotal += unitStatistics->delta.melee;
	_statGainTotal += unitStatistics->delta.psiSkill;

	_braveryGainTotal = unitStatistics->delta.bravery;
	_revivedUnitTotal += (unitStatistics->revivedSoldier + unitStatistics->revivedHostile + unitStatistics->revivedNeutral);
	_revivedSoldierTotal += unitStatistics->revivedSoldier;
	_revivedNeutralTotal += unitStatistics->revivedNeutral;
	_revivedHostileTotal += unitStatistics->revivedHostile;
	_wholeMedikitTotal += std::min( std::min(unitStatistics->woundsHealed, unitStatistics->appliedStimulant), unitStatistics->appliedPainKill);
	_missionIdList.push_back(missionStatistics->id);
}

/**
 * Get soldier commendations.
 * @return SoldierCommendations list of soldier's commendations.
 */
std::vector<SoldierCommendations*> *SoldierDiary::getSoldierCommendations()
{
	return &_commendations;
}

/**
 * Manage the soldier's commendations.
 * Award new ones, if deserved.
 * @return bool Has a commendation been awarded?
 */
bool SoldierDiary::manageCommendations(Mod *mod, std::vector<MissionStatistics*> *missionStatistics)
{
	const int BATTLE_TYPES = 14;
	const std::string battleTypeArray[BATTLE_TYPES] = { "BT_NONE", "BT_FIREARM", "BT_AMMO", "BT_MELEE", "BT_GRENADE",
		"BT_PROXIMITYGRENADE", "BT_MEDIKIT", "BT_SCANNER", "BT_MINDPROBE", "BT_PSIAMP", "BT_FLARE", "BT_CORPSE", "BT_ANOMALY", "BT_END" };
	const int DAMAGE_TYPES = 21;
	const std::string damageTypeArray[DAMAGE_TYPES] = { "DT_NONE", "DT_AP", "DT_IN", "DT_HE", "DT_LASER", "DT_PLASMA",
		"DT_STUN", "DT_MELEE", "DT_ACID", "DT_SMOKE",
		"DT_10", "DT_11", "DT_12", "DT_13", "DT_14", "DT_15", "DT_16", "DT_17", "DT_18", "DT_19", "DT_END" };

	const std::map<std::string, RuleCommendations *> commendationsList = mod->getCommendationsList();
	bool awardedCommendation = false;                   // This value is returned if at least one commendation was given.
	std::map<std::string, int> nextCommendationLevel;   // Noun, threshold.
	std::vector<std::string> modularCommendations;      // Commendation name.
	bool awardCommendationBool = false;                 // This value determines if a commendation will be given.
	// Loop over all possible commendations
	for (std::map<std::string, RuleCommendations *>::const_iterator i = commendationsList.begin(); i != commendationsList.end(); )
	{
		awardCommendationBool = true;
		nextCommendationLevel.clear();
		nextCommendationLevel["noNoun"] = 0;
		modularCommendations.clear();
		// Loop over all the soldier's commendations, see if we already have the commendation.
		// If so, get the level and noun.
		for (std::vector<SoldierCommendations*>::const_iterator j = _commendations.begin(); j != _commendations.end(); ++j)
		{
			if ( (*i).first == (*j)->getType() )
			{
				nextCommendationLevel[(*j)->getNoun()] = (*j)->getDecorationLevelInt() + 1;
			}
		}
		// Go through each possible criteria. Assume the medal is awarded, set to false if not.
		// As soon as we find a medal criteria that we FAIL TO achieve, then we are not awarded a medal.
		for (std::map<std::string, std::vector<int> >::const_iterator j = (*i).second->getCriteria()->begin(); j != (*i).second->getCriteria()->end(); ++j)
		{
			// Skip this medal if we have reached its max award level.
			if ((unsigned int)nextCommendationLevel["noNoun"] >= (*j).second.size())
			{
				awardCommendationBool = false;
				break;
			}
			// These criteria have no nouns, so only the nextCommendationLevel["noNoun"] will ever be used.
			else if( nextCommendationLevel.count("noNoun") == 1 &&
				  ( ((*j).first == "totalKills" && (unsigned int)getKillTotal() < (unsigned int)(*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalMissions" && getMissionTotalFiltered(missionStatistics, (*i).second) < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalWins" && getWinTotal(missionStatistics) < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalScore" && getScoreTotal(missionStatistics) < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalStuns" && getStunTotal() < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalDaysWounded" && _daysWoundedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalBaseDefenseMissions" && getBaseDefenseMissionTotal(missionStatistics) < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalTerrorMissions" && getTerrorMissionTotal(missionStatistics) < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalNightMissions" && getNightMissionTotal(missionStatistics, mod) < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalNightTerrorMissions" && getNightTerrorMissionTotal(missionStatistics, mod) < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalMonthlyService" && _monthsService < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalFellUnconcious" && _unconciousTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalShotAt10Times" && _shotAtCounter10in1Mission < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalHit5Times" && _hitCounter5in1Mission < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalFriendlyFired" && (_totalShotByFriendlyCounter < (*j).second.at(nextCommendationLevel["noNoun"]) || _KIA || _MIA)) ||
					((*j).first == "total_lone_survivor" && _loneSurvivorTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalIronMan" && _ironManTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalImportantMissions" && getImportantMissionTotal(missionStatistics) < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalLongDistanceHits" && _longDistanceHitCounterTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalLowAccuracyHits" && _lowAccuracyHitCounterTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalReactionFire" && getReactionFireKillTotal(mod) < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalTimesWounded" && _timesWoundedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalDaysWounded" && _daysWoundedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalValientCrux" && getValiantCruxTotal(missionStatistics) < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "isDead" && _KIA < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalTrapKills" && getTrapKillTotal(mod) < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalAlienBaseAssaults" && getAlienBaseAssaultTotal(missionStatistics) < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalAllAliensKilled" && _allAliensKilledTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalAllAliensStunned" && _allAliensStunnedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalWoundsHealed" && _woundsHealedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalAllUFOs" && _allUFOs < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalAllMissionTypes" && _allMissionTypes < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalStatGain" && _statGainTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalRevives" && _revivedUnitTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalSoldierRevives" && _revivedSoldierTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalHostileRevives" && _revivedHostileTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalNeutralRevives" && _revivedNeutralTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalWholeMedikit" && _wholeMedikitTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalBraveryGain" && _braveryGainTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "bestOfRank" && _bestOfRank < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "bestSoldier" && (int)_bestSoldier < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "isMIA" && _MIA < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalMartyrKills" && _martyrKillsTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalPostMortemKills" && _postMortemKills < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "globeTrotter" && (int)_globeTrotter < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalSlaveKills" && _slaveKillsTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ) )

			{
				awardCommendationBool = false;
				break;
			}
			// Medals with the following criteria are unique because they need a noun.
			// And because they loop over a map<> (this allows for maximum moddability).
			else if ((*j).first == "totalKillsWithAWeapon" || (*j).first == "totalMissionsInARegion" || (*j).first == "totalKillsByRace" || (*j).first == "totalKillsByRank")
			{
				std::map<std::string, int> tempTotal;
				if ((*j).first == "totalKillsWithAWeapon")
					tempTotal = getWeaponTotal();
				else if ((*j).first == "totalMissionsInARegion")
					tempTotal = getRegionTotal(missionStatistics);
				else if ((*j).first == "totalKillsByRace")
					tempTotal = getAlienRaceTotal();
				else if ((*j).first == "totalKillsByRank")
					tempTotal = getAlienRankTotal();
				// Loop over the temporary map.
				// Match nouns and decoration levels.
				for(std::map<std::string, int>::const_iterator k = tempTotal.begin(); k != tempTotal.end(); ++k)
				{
					int criteria = -1;
					std::string noun = (*k).first;
					// If there is no matching noun, get the first award criteria.
					if (nextCommendationLevel.count(noun) == 0)
						criteria = (*j).second.front();
					// Otherwise, get the criteria that reflects the soldier's commendation level.
					else if ((unsigned int)nextCommendationLevel[noun] != (*j).second.size())
						criteria = (*j).second.at(nextCommendationLevel[noun]);

					// If a criteria was set AND the stat's count exceeds the criteria.
					if (criteria != -1 && (*k).second >= criteria)
					{
						modularCommendations.push_back(noun);
					}
				}
				// If it is still empty, we did not get a commendation.
				if (modularCommendations.empty())
				{
					awardCommendationBool = false;
					break;
				}
			}
			// Medals that are based on _how_ a kill was achieved are found here.
			else if ((*j).first == "killsWithCriteriaCareer" || (*j).first == "killsWithCriteriaMission" || (*j).first == "killsWithCriteriaTurn")
			{
				// Fetch the kill criteria list.
				if (!(*i).second->getKillCriteria())
					break;
				const std::vector<std::vector<std::pair<int, std::vector<std::string> > > > *_killCriteriaList = (*i).second->getKillCriteria();

				int totalKillGroups = 0; // holds the total number of kill groups which satisfy one of the OR criteria blocks
				bool enoughForNextCommendation = false;

				// Loop over the OR vectors.
				// if OR criteria are not disjunctive (e.g. "kill 1 enemy" or "kill 1 enemy"), each one will be counted and added to totals - avoid that if you want good statistics
				for (std::vector<std::vector<std::pair<int, std::vector<std::string> > > >::const_iterator orCriteria = _killCriteriaList->begin(); orCriteria != _killCriteriaList->end(); ++orCriteria)
				{
					// prepare counters
					std::vector<int> referenceBlockCounters;
					referenceBlockCounters.assign((*orCriteria).size(), 0);
					int referenceTotalCounters = 0;
					for (std::vector<std::pair<int, std::vector<std::string> > >::const_iterator andCriteria = orCriteria->begin(); andCriteria != orCriteria->end(); ++andCriteria)
					{
						int index = andCriteria - orCriteria->begin();
						referenceBlockCounters[index] = (*andCriteria).first;
						referenceTotalCounters += (*andCriteria).first;
					}
					std::vector<int> currentBlockCounters;
					if ((*j).first == "killsWithCriteriaCareer") {
						currentBlockCounters = referenceBlockCounters;
					}
					int currentTotalCounters = referenceTotalCounters;
					int lastTimeSpan = -1;
					bool skipThisTimeSpan = false;
					// Loop over the KILLS, seeking to fulfill all criteria from entire AND block within the specified time span (career/mission/turn)
					for (std::vector<BattleUnitKills*>::const_iterator singleKill = _killList.begin(); singleKill != _killList.end(); ++singleKill)
					{
						int thisTimeSpan = -1;
						if ((*j).first == "killsWithCriteriaMission")
						{
							thisTimeSpan = (*singleKill)->mission;
						}
						else if ((*j).first == "killsWithCriteriaTurn")
						{
							thisTimeSpan = (*singleKill)->turn;
						}
						if (thisTimeSpan != lastTimeSpan)
						{
							// next time span, reset counters
							lastTimeSpan = thisTimeSpan;
							skipThisTimeSpan = false;
							currentBlockCounters = referenceBlockCounters;
							currentTotalCounters = referenceTotalCounters;
						}
						// same time span, we're skipping the rest of it if we already fulfilled criteria
						else if (skipThisTimeSpan)
						{
							continue;
						}

						bool andCriteriaMet = false;

						// Loop over the AND vectors.
						for (std::vector<std::pair<int, std::vector<std::string> > >::const_iterator andCriteria = orCriteria->begin(); andCriteria != orCriteria->end(); ++andCriteria)
						{
							bool foundMatch = true;

							// Loop over the DETAILs of one AND vector.
							for (std::vector<std::string>::const_iterator detail = andCriteria->second.begin(); detail != andCriteria->second.end(); ++detail)
							{
								int battleType = 0;
								for (; battleType != BATTLE_TYPES; ++battleType)
								{
									if ((*detail) == battleTypeArray[battleType])
									{
										break;
									}
								}

								int damageType = 0;
								for (; damageType != DAMAGE_TYPES; ++damageType)
								{
									if ((*detail) == damageTypeArray[damageType])
									{
										break;
									}
								}

								// See if we find _no_ matches with any criteria. If so, break and try the next kill.
								RuleItem *weapon = mod->getItem((*singleKill)->weapon);
								RuleItem *weaponAmmo = mod->getItem((*singleKill)->weaponAmmo);
								if (weapon == 0 || weaponAmmo == 0 ||
									((*singleKill)->rank != (*detail) && (*singleKill)->race != (*detail) &&
									 (*singleKill)->weapon != (*detail) && (*singleKill)->weaponAmmo != (*detail) &&
									 (*singleKill)->getUnitStatusString() != (*detail) && (*singleKill)->getUnitFactionString() != (*detail) &&
									 (*singleKill)->getUnitSideString() != (*detail) && (*singleKill)->getUnitBodyPartString() != (*detail) &&
									 weaponAmmo->getDamageType()->ResistType != damageType && weapon->getBattleType() != battleType))
								{
									foundMatch = false;
									break;
								}
							} /// End of DETAIL loop.

							if (foundMatch)
							{
								int index = andCriteria - orCriteria->begin();
								// some current block counters might go into negatives, this is used to tally career kills correctly
								// currentTotalCounters will always ensure we're counting in proper batches
								if (currentBlockCounters[index]-- > 0 && --currentTotalCounters <= 0)
								{
									// we just counted all counters in a block to zero, this certainly means that the entire block criteria is fulfilled
									andCriteriaMet = true;
									break;
								}
							}
						} /// End of AND loop.

						if (andCriteriaMet)
						{
							// early exit if we got enough, no reason to continue iterations
							if (++totalKillGroups >= (*j).second.at(nextCommendationLevel["noNoun"]))
							{
								enoughForNextCommendation = true;
								break;
							}

							// "killsWithCriteriaTurn" and "killsWithCriteriaMission" are "peak achivements", they are counted once per their respective time span if criteria are fulfilled
							// so if we got them, we're skipping the rest of this time span to avoid counting more than once
							// e.g. 20 kills in a mission will not be counted as "10 kills in a mission" criteria twice
							// "killsWithCriteriaCareer" are totals, so they are never skipped this way
							if ((*j).first == "killsWithCriteriaTurn" || (*j).first == "killsWithCriteriaMission")
							{
								skipThisTimeSpan = true;
							}
							// for career kills we'll ADD reference counters to the current values and recalculate current total
							// this is used to count instances of full criteria blocks, e.g. if rules state that a career commendation must be awarded for 2 kills of alien leaders
							// and 1 kill of  alien commander, then we must ensure there's 2 leader kills + 1 commander kill for each instance of criteria fulfilled
							else if ((*j).first == "killsWithCriteriaCareer")
							{
								currentTotalCounters = 0;
								for (std::size_t i2 = 0; i2 < currentBlockCounters.size(); i2++)
								{
									currentBlockCounters[i2] += referenceBlockCounters[i2];
									currentTotalCounters += std::max(currentBlockCounters[i2], 0);
								}
							}
						}
					} /// End of KILLs loop.

					if (enoughForNextCommendation)
						break; // stop iterating here too, we've got enough

				} /// End of OR loop.

				if (!enoughForNextCommendation)
					awardCommendationBool = false;

			}
		}
		if (awardCommendationBool)
		{
			// If we do not have modular medals, but are awarded a different medal,
			// its noun will be "noNoun".
			if (modularCommendations.empty())
			{
				modularCommendations.push_back("noNoun");
			}
			for (std::vector<std::string>::const_iterator j = modularCommendations.begin(); j != modularCommendations.end(); ++j)
			{
				bool newCommendation = true;
				for (std::vector<SoldierCommendations*>::iterator k = _commendations.begin() ; k != _commendations.end() ; ++k)
				{
					if ( (*k)->getType() == (*i).first && (*k)->getNoun() == (*j))
					{
						(*k)->addDecoration();
						newCommendation = false;
						break;
					}
				}
				if (newCommendation)
				{
					_commendations.push_back(new SoldierCommendations((*i).first, (*j), mod));
				}
			}
			awardedCommendation = true;
		}
		else
		{
			++i;
		}
	}
	return awardedCommendation;
}

/**
 * Get vector of mission ids.
 * @return Vector of mission ids.
 */
std::vector<int> &SoldierDiary::getMissionIdList()
{
	return _missionIdList;
}

/**
 * Get vector of kills.
 * @return vector of BattleUnitKills
 */
std::vector<BattleUnitKills*> &SoldierDiary::getKills()
{
	return _killList;
}

/**
 * Get list of kills sorted by rank
 * @return
 */
std::map<std::string, int> SoldierDiary::getAlienRankTotal()
{
	std::map<std::string, int> list;
	for(std::vector<BattleUnitKills*>::const_iterator kill = _killList.begin(); kill != _killList.end(); ++kill)
	{
		list[(*kill)->rank]++;
	}
	return list;
}

/**
 *
 */
std::map<std::string, int> SoldierDiary::getAlienRaceTotal()
{
	std::map<std::string, int> list;
	for(std::vector<BattleUnitKills*>::const_iterator kill = _killList.begin(); kill != _killList.end(); ++kill)
	{
		list[(*kill)->race]++;
	}
	return list;
}

/**
 *
 */
std::map<std::string, int> SoldierDiary::getWeaponTotal()
{
	std::map<std::string, int> list;
	for(std::vector<BattleUnitKills*>::const_iterator kill = _killList.begin(); kill != _killList.end(); ++kill)
	{
		if ((*kill)->faction == FACTION_HOSTILE)
			list[(*kill)->weapon]++;
	}
	return list;
}

/**
 *
 */
std::map<std::string, int> SoldierDiary::getWeaponAmmoTotal()
{
	std::map<std::string, int> list;
	for(std::vector<BattleUnitKills*>::const_iterator kill = _killList.begin(); kill != _killList.end(); ++kill)
	{
		if ((*kill)->faction == FACTION_HOSTILE)
			list[(*kill)->weaponAmmo]++;
	}
	return list;
}

/**
 *  Get a map of the amount of missions done in each region.
 *  @param MissionStatistics
 */
std::map<std::string, int> SoldierDiary::getRegionTotal(std::vector<MissionStatistics*> *missionStatistics) const
{
	std::map<std::string, int> regionTotal;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				regionTotal[(*i)->region]++;
			}
		}
	}

	return regionTotal;
}

/**
 *  Get a map of the amount of missions done in each country.
 *  @param MissionStatistics
 */
std::map<std::string, int> SoldierDiary::getCountryTotal(std::vector<MissionStatistics*> *missionStatistics) const
{
	std::map<std::string, int> countryTotal;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				countryTotal[(*i)->country]++;
			}
		}
	}

	return countryTotal;
}

/**
 *  Get a map of the amount of missions done in each type.
 *  @param MissionStatistics
 */
std::map<std::string, int> SoldierDiary::getTypeTotal(std::vector<MissionStatistics*> *missionStatistics) const
{
	std::map<std::string, int> typeTotal;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				typeTotal[(*i)->type]++;
			}
		}
	}

	return typeTotal;
}

/**
 *  Get a map of the amount of missions done in each UFO.
 *  @param MissionStatistics
 */
std::map<std::string, int> SoldierDiary::getUFOTotal(std::vector<MissionStatistics*> *missionStatistics) const
{
	std::map<std::string, int> ufoTotal;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				ufoTotal[(*i)->ufo]++;
			}
		}
	}

	return ufoTotal;
}

/**
 *
 */
int SoldierDiary::getKillTotal() const
{
	int killTotal = 0;

	for (std::vector<BattleUnitKills*>::const_iterator i = _killList.begin(); i != _killList.end(); ++i)
	{
		if ((*i)->status == STATUS_DEAD && (*i)->faction == FACTION_HOSTILE)
		{
			killTotal++;
		}
	}

	return killTotal;
}

/**
 *
 */
int SoldierDiary::getMissionTotal() const
{
	return _missionIdList.size();
}

/**
 *
 */
int SoldierDiary::getMissionTotalFiltered(std::vector<MissionStatistics*>* missionStatistics, const RuleCommendations* rule) const
{
	if (!rule->getMissionTypeNames().empty())
	{
		int total = 0;
		for (auto* ms : *missionStatistics)
		{
			if (ms->success)
			{
				if (std::find(_missionIdList.begin(), _missionIdList.end(), ms->id) != _missionIdList.end())
				{
					if (std::find(rule->getMissionTypeNames().begin(), rule->getMissionTypeNames().end(), ms->type) != rule->getMissionTypeNames().end())
					{
						++total;
					}
				}
			}
		}
		return total;
	}
	else if (!rule->getMissionMarkerNames().empty())
	{
		int total = 0;
		for (auto* ms : *missionStatistics)
		{
			if (ms->success)
			{
				if (std::find(_missionIdList.begin(), _missionIdList.end(), ms->id) != _missionIdList.end())
				{
					if (std::find(rule->getMissionMarkerNames().begin(), rule->getMissionMarkerNames().end(), ms->markerName) != rule->getMissionMarkerNames().end())
					{
						++total;
					}
				}
			}
		}
		return total;
	}

	return (int)_missionIdList.size();
}

/**
 *  Get the total if wins.
 *  @param Mission Statistics
 */
int SoldierDiary::getWinTotal(std::vector<MissionStatistics*> *missionStatistics) const
{
	int winTotal = 0;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				if ((*i)->success)
				{
					winTotal++;
				}
			}
		}
	}

	return winTotal;
}

/**
 *
 */
int SoldierDiary::getStunTotal() const
{
	int stunTotal = 0;

	for (std::vector<BattleUnitKills*>::const_iterator i = _killList.begin(); i != _killList.end(); ++i)
	{
		if ((*i)->status == STATUS_UNCONSCIOUS && (*i)->faction == FACTION_HOSTILE)
		{
			stunTotal++;
		}
	}

	return stunTotal;
}

/**
 *
 */
int SoldierDiary::getPanickTotal() const
{
	int panickTotal = 0;

	for (std::vector<BattleUnitKills*>::const_iterator i = _killList.begin(); i != _killList.end(); ++i)
	{
		if ((*i)->status == STATUS_PANICKING && (*i)->faction == FACTION_HOSTILE)
		{
			panickTotal++;
		}
	}

	return panickTotal;
}

/**
 *
 */
int SoldierDiary::getControlTotal() const
{
	int controlTotal = 0;

	for (std::vector<BattleUnitKills*>::const_iterator i = _killList.begin(); i != _killList.end(); ++i)
	{
		if ((*i)->status == STATUS_TURNING && (*i)->faction == FACTION_HOSTILE)
		{
			controlTotal++;
		}
	}

	return controlTotal;
}

/**
 *
 */
int SoldierDiary::getDaysWoundedTotal() const
{
	return _daysWoundedTotal;
}

/**
 * Increment soldier's service time one month
 */
void SoldierDiary::addMonthlyService()
{
	_monthsService++;
}

/**
 * Returns the total months this soldier has been in service.
 */
int SoldierDiary::getMonthsService() const
{
	return _monthsService;
}

/**
 * Award special commendation to the original 8 soldiers.
 */
void SoldierDiary::awardOriginalEightCommendation(const Mod* mod)
{
	// TODO: Unhardcode this
	_commendations.push_back(new SoldierCommendations("STR_MEDAL_ORIGINAL8_NAME", "NoNoun", mod));
}

/**
 * Award posthumous best-of commendation.
 */
void SoldierDiary::awardBestOfRank(int score)
{
	_bestOfRank = score;
}

/**
 * Award posthumous best-of commendation.
 */
void SoldierDiary::awardBestOverall(int score)
{
	_bestSoldier = score;
}

/**
 * Award posthumous kills commendation.
 */
void SoldierDiary::awardPostMortemKill(int kills)
{
	_postMortemKills = kills;
}

/**
 *
 */
int SoldierDiary::getShotsFiredTotal() const
{
	return _shotsFiredCounterTotal;
}

/**
 *
 */
int SoldierDiary::getShotsLandedTotal() const
{
	return _shotsLandedCounterTotal;
}

/**
 *  Get the soldier's accuracy.
 */
int SoldierDiary::getAccuracy() const
{
	if (_shotsFiredCounterTotal != 0)
		return 100 * _shotsLandedCounterTotal / _shotsFiredCounterTotal;
	return 0;
}

/**
 *  Get trap kills total.
 */
int SoldierDiary::getTrapKillTotal(Mod *mod) const
{
	int trapKillTotal = 0;

	for (std::vector<BattleUnitKills*>::const_iterator i = _killList.begin(); i != _killList.end(); ++i)
	{
		RuleItem *item = mod->getItem((*i)->weapon);
		const BattleType type = (item == 0 ? BT_NONE : item->getBattleType());
		if ((*i)->hostileTurn() && (item == 0 || type == BT_GRENADE || type == BT_PROXIMITYGRENADE || type == BT_ANOMALY))
		{
			trapKillTotal++;
		}
	}

	return trapKillTotal;
}

/**
 *  Get reaction kill total.
 */
 int SoldierDiary::getReactionFireKillTotal(Mod *mod) const
 {
	int reactionFireKillTotal = 0;

	for (std::vector<BattleUnitKills*>::const_iterator i = _killList.begin(); i != _killList.end(); ++i)
	{
		RuleItem *item = mod->getItem((*i)->weapon);
		const BattleType type = (item == 0 ? BT_NONE : item->getBattleType());
		if ((*i)->hostileTurn() && item != 0 && type != BT_GRENADE && type != BT_PROXIMITYGRENADE && type != BT_ANOMALY)
		{
			reactionFireKillTotal++;
		}
	}

	return reactionFireKillTotal;
 }

/**
 *  Get the total of terror missions.
 *  @param Mission Statistics
 */
int SoldierDiary::getTerrorMissionTotal(std::vector<MissionStatistics*> *missionStatistics) const
{
	/// Not a UFO, not the base, not the alien base or colony
	int terrorMissionTotal = 0;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				if ((*i)->success && !(*i)->isBaseDefense() && !(*i)->isUfoMission() && !(*i)->isAlienBase())
				{
					terrorMissionTotal++;
				}
			}
		}
	}

	return terrorMissionTotal;
}

/**
 *  Get the total of night missions.
 *  @param Mission Statistics
 */
int SoldierDiary::getNightMissionTotal(std::vector<MissionStatistics*> *missionStatistics, const Mod* mod) const
{
	int nightMissionTotal = 0;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				if ((*i)->success && (*i)->isDarkness(mod) && !(*i)->isBaseDefense() && !(*i)->isAlienBase())
				{
					nightMissionTotal++;
				}
			}
		}
	}

	return nightMissionTotal;
}

/**
 *  Get the total of night terror missions.
 *  @param Mission Statistics
 */
int SoldierDiary::getNightTerrorMissionTotal(std::vector<MissionStatistics*> *missionStatistics, const Mod* mod) const
{
	int nightTerrorMissionTotal = 0;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				if ((*i)->success && (*i)->isDarkness(mod) && !(*i)->isBaseDefense() && !(*i)->isUfoMission() && !(*i)->isAlienBase())
				{
					nightTerrorMissionTotal++;
				}
			}
		}
	}

	return nightTerrorMissionTotal;
}

/**
 *  Get the total of base defense missions.
 *  @param Mission Statistics
 */
int SoldierDiary::getBaseDefenseMissionTotal(std::vector<MissionStatistics*> *missionStatistics) const
{
	int baseDefenseMissionTotal = 0;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				if ((*i)->success && (*i)->isBaseDefense())
				{
					baseDefenseMissionTotal++;
				}
			}
		}
	}

	return baseDefenseMissionTotal;
}

/**
 *  Get the total of alien base assaults.
 *  @param Mission Statistics
 */
int SoldierDiary::getAlienBaseAssaultTotal(std::vector<MissionStatistics*> *missionStatistics) const
{
	int alienBaseAssaultTotal = 0;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				if ((*i)->success && (*i)->isAlienBase())
				{
					alienBaseAssaultTotal++;
				}
			}
		}
	}

	return alienBaseAssaultTotal;
}

/**
 *  Get the total of important missions.
 *  @param Mission Statistics
 */
int SoldierDiary::getImportantMissionTotal(std::vector<MissionStatistics*> *missionStatistics) const
{
	int importantMissionTotal = 0;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				if ((*i)->success && (*i)->type != "STR_UFO_CRASH_RECOVERY")
				{
					importantMissionTotal++;
				}
			}
		}
	}

	return importantMissionTotal;
}

/**
 *  Get the total score.
 *  @param Mission Statistics
 */
int SoldierDiary::getScoreTotal(std::vector<MissionStatistics*> *missionStatistics) const
{
	int scoreTotal = 0;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				scoreTotal += (*i)->score;
			}
		}
	}

	return scoreTotal;
}

/**
 *  Get the Valiant Crux total.
 *  @param Mission Statistics
 */
int SoldierDiary::getValiantCruxTotal(std::vector<MissionStatistics*> *missionStatistics) const
{
	int valiantCruxTotal = 0;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id && (*i)->valiantCrux)
			{
				valiantCruxTotal++;
			}
		}
	}

	return valiantCruxTotal;
}

/**
 *  Get the loot value total.
 *  @param Mission Statistics
 */
int SoldierDiary::getLootValueTotal(std::vector<MissionStatistics*> *missionStatistics) const
{
	int lootValueTotal = 0;

	for (std::vector<MissionStatistics*>::const_iterator i = missionStatistics->begin(); i != missionStatistics->end(); ++i)
	{
		for (std::vector<int>::const_iterator j = _missionIdList.begin(); j != _missionIdList.end(); ++j)
		{
			if ((*j) == (*i)->id)
			{
				lootValueTotal += (*i)->lootValue;
			}
		}
	}

	return lootValueTotal;
}

/**
 * Initializes a new commendation entry from YAML.
 * @param node YAML node.
 */
SoldierCommendations::SoldierCommendations(const YAML::Node &node, const Mod* mod)
{
	load(node);
	_rule = mod->getCommendation(_type, false); //TODO: during load we can load obsolete value, some else need cleanup
}

/**
 * Initializes a soldier commendation.
 */
SoldierCommendations::SoldierCommendations(std::string commendationName, std::string noun, const Mod* mod) : _type(commendationName), _noun(noun), _decorationLevel(0), _isNew(true)
{
	_rule = mod->getCommendation(_type, true); //if there is no commendation rule then someone messed up
}

/**
 *
 */
SoldierCommendations::~SoldierCommendations()
{
}

/**
 * Loads the commendation from a YAML file.
 * @param node YAML node.
 */
void SoldierCommendations::load(const YAML::Node &node)
{
	_type = node["commendationName"].as<std::string>(_type);
	_noun = node["noun"].as<std::string>("noNoun");
	_decorationLevel = node["decorationLevel"].as<int>(_decorationLevel);
	_isNew = node["isNew"].as<bool>(false);
}

/**
 * Saves the commendation to a YAML file.
 * @return YAML node.
 */
YAML::Node SoldierCommendations::save() const
{
	YAML::Node node;
	node.SetStyle(YAML::EmitterStyle::Flow);
	node["commendationName"] = _type;
	if (_noun != "noNoun") node["noun"] = _noun;
	node["decorationLevel"] = _decorationLevel;
	return node;
}

/**
 * Get the soldier's commendation's name.
 * @return string Commendation name.
 */
const std::string& SoldierCommendations::getType() const
{
	return _type;
}

/**
 * Get the soldier's commendation's noun.
 * @return string Commendation noun
 */
const std::string& SoldierCommendations::getNoun() const
{
	return _noun;
}

/**
 * Get the soldier commendation level's name.
 * @return string Commendation level.
 */
std::string SoldierCommendations::getDecorationLevelName(int skipCounter) const
{
	std::ostringstream ss;
	ss << "STR_AWARD_" << _decorationLevel - skipCounter;
	return ss.str();
}

/**
 * Get the soldier commendation level's description.
 * @return string Commendation level description.
 */
std::string SoldierCommendations::getDecorationDescription() const
{
	std::ostringstream ss;
	ss << "STR_AWARD_DECOR_" << _decorationLevel;
	return ss.str();
}

/**
 * Get the soldier commendation level's int.
 * @return int Commendation level.
 */
int SoldierCommendations::getDecorationLevelInt() const
{
	return _decorationLevel;
}

/**
 * Get newness of commendation.
 * @return bool Is the commendation new?
 */
bool SoldierCommendations::isNew() const
{
	return _isNew;
}

/**
 * Set the newness of the commendation to old.
 */
void SoldierCommendations::makeOld()
{
	_isNew = false;
}

/**
 * Add a level of decoration to the commendation.
 * Sets isNew to true.
 */
void SoldierCommendations::addDecoration()
{
	_decorationLevel++;
	_isNew = true;
}

}
