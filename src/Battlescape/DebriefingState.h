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
#include "../Engine/State.h"
#include <string>
#include <vector>
#include <map>
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class BattleItem;
class Craft;
class Base;
class Region;
class Country;
class RuleItem;
class RuleEvent;
class BattleUnit;
struct UnitStats;

struct DebriefingStat {
	std::string item;
	int qty;
	int score;
	bool recovery;

	DebriefingStat(const std::string &_item, bool _recovery) : item(_item), qty(0), score(0), recovery(_recovery) {};
	};

struct ReequipStat { std::string item; int qty; std::string craft; int listOrder; };

struct RecoveryItem { std::string name; int value; };

/**
 * Debriefing screen shown after a Battlescape
 * mission that displays the results.
 */
class DebriefingState : public State
{
private:
	typedef std::pair<std::string, UnitStats> SoldierStatsEntry;

	RuleEvent *_eventToSpawn;
	Region *_region;
	Country *_country;
	Base *_base;
	std::vector<DebriefingStat*> _stats;
	std::vector<SoldierStatsEntry> _soldierStats;
	TextButton *_btnOk, *_btnStats, *_btnSell, *_btnTransfer;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtQuantity, *_txtScore, *_txtRecovery, *_txtRating;
	Text *_txtSoldier, *_txtTU, *_txtStamina, *_txtHealth, *_txtBravery, *_txtReactions;
	Text *_txtFiring, *_txtThrowing, *_txtMelee, *_txtStrength, *_txtPsiStrength, *_txtPsiSkill;
	TextList *_lstStats, *_lstRecovery, *_lstTotal, *_lstSoldierStats, *_lstRecoveredItems;
	std::string _currentTooltip;
	Text *_txtTooltip;
	std::vector<ReequipStat> _missingItems;
	std::map<const RuleItem*, int> _rounds, _roundsPainKiller, _roundsStimulant, _roundsHeal, _recoveredItems;
	Uint8 _ammoColor;
	std::map<int, RecoveryItem*> _recoveryStats;
	bool _positiveScore, _destroyBase, _promotions, _showSellButton, _initDone;
	std::map<int, int>  _containmentStateInfo;
	int _limitsEnforced;
	MissionStatistics *_missionStatistics;
	std::vector<Soldier*> _soldiersCommended, _deadSoldiersCommended;
	/// Adds to the debriefing stats.
	void addStat(const std::string &name, int quantity, int score);
	/// Prepares debriefing.
	void prepareDebriefing();
	/// Adds item(s) to base stores.
	void addItemsToBaseStores(const RuleItem *ruleItem, Base *base, int quantity, bool considerTransformations, bool countScavenge);
	void addItemsToBaseStores(const std::string &itemType, Base *base, int quantity, bool considerTransformations, bool countScavenge);
	/// Recovers items from the battlescape.
	void recoverItems(std::vector<BattleItem*> *from, Base *base);
	/// Recovers a civilian from the battlescape.
	void recoverCivilian(BattleUnit *from, Base *base);
	/// Recovers an alien from the battlescape.
	void recoverAlien(BattleUnit *from, Base *base);
	/// Reequips a craft after a mission.
	void reequipCraft(Base *base, Craft *craft, bool vehicleItemsCanBeDestroyed);
	/// 0 = score, 1 = stat improvement, 2 = recovered items
	int _pageNumber;
	/// Sets the visibility according to the _pageNumber
	void applyVisibility();
	/// Creates a string for the soldier stats table from a stat difference value
	std::string makeSoldierString(int stat);
public:
	/// Creates the Debriefing state.
	DebriefingState();
	/// Cleans up the Debriefing state.
	~DebriefingState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Prepare debriefing.
	void init() override;
	/// Handler for clicking the STATS button.
	void btnStatsClick(Action *action);
	/// Handler for clicking the SELL button.
	void btnSellClick(Action *action);
	/// Handler for clicking the TRANSFER button.
	void btnTransferClick(Action *action);
	/// Handler for showing tooltip.
	void txtTooltipIn(Action *action);
	/// Handler for hiding tooltip.
	void txtTooltipOut(Action *action);
	// Gets the number of recovered items of certain type.
	int getRecoveredItemCount(const RuleItem *rule);
	// Gets the total number of recovered items.
	int getTotalRecoveredItemCount();
	// Decreases the number of recovered items by the sold/transferred amount.
	void decreaseRecoveredItemCount(const RuleItem *rule, int amount);
	// Hides the SELL and TRANSFER buttons.
	void hideSellTransferButtons();
};

}
