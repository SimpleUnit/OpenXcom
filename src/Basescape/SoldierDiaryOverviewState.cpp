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
#include "SoldierDiaryOverviewState.h"
#include "SoldierDiaryPerformanceState.h"
#include "SoldierDiaryMissionState.h"
#include "SoldierInfoState.h"
#include <sstream>
#include "../Mod/Mod.h"
#include "../Mod/RuleInterface.h"
#include "../Engine/Game.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SoldierDeath.h"
#include "../Savegame/SoldierDiary.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/MissionStatistics.h"
#include "../Savegame/BattleUnitStatistics.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldier Diary screen.
 * @param base Pointer to the base to get info from.
 * @param soldierId ID of the selected soldier.
 * @param soldierInfoState Pointer to the Soldier Info screen.
 */
SoldierDiaryOverviewState::SoldierDiaryOverviewState(Base *base, size_t soldierId, SoldierInfoState *soldierInfoState) : _base(base), _soldierId(soldierId), _soldierInfoState(soldierInfoState)
{
	if (_base == 0)
	{
		_list = _game->getSavedGame()->getDeadSoldiers();
	}
	else
	{
		_list = _base->getSoldiers();
	}

	// Create objects
	if (_game->getMod()->getStalkMode())
		_window = new Surface(320, 200, 0, 0);
	else
		_window = new Window(this, 320, 200, 0, 0);
	_btnKills = new TextButton(70, 16, 8, 176);
	_btnMissions = new TextButton(70, 16, 86, 176);
	_btnCommendations = new TextButton(70, 16, 164, 176);
	_btnOk = new TextButton(70, 16, 242, 176);
	_btnPrev = new TextButton(28, 14, 8, 8);
	_btnNext = new TextButton(28, 14, 284, 8);
	_txtTitle = new Text(310, 16, 5, 8);
	if (!_base)
	{
		_txtDeathTitle = new Text(150, 9, 16, 36);
		_txtDeathDate = new Text(90, 9, 218, 36);
		_txtDeathInfo = new Text(292, 9, 16, 44);
	}
	int offset = (!_base) ? 24 : 0;
	_txtMission = new Text(114, 9, 16, 36 + offset);
	_txtRating = new Text(102, 9, 120, 36 + offset);
	_txtDate = new Text(90, 9, 218, 36 + offset);
	_lstDiary = new TextList(288, 120 - offset, 8, 44 + offset);

	// Set palette
	setInterface("soldierDiary");

	add(_window, "window", "soldierDiary");
	add(_btnOk, "button", "soldierDiary");
	add(_btnKills, "button", "soldierDiary");
	add(_btnMissions, "button", "soldierDiary");
	add(_btnCommendations, "button", "soldierDiary");
	add(_btnPrev, "button", "soldierDiary");
	add(_btnNext, "button", "soldierDiary");
	add(_txtTitle, "text1", "soldierDiary");
	if (!_base)
	{
		add(_txtDeathTitle, "text2", "soldierDiary");
		add(_txtDeathInfo, "text3", "soldierDiary");
		add(_txtDeathDate, "text3", "soldierDiary");
	}
	add(_txtMission, "text2", "soldierDiary");
	add(_txtRating, "text2", "soldierDiary");
	add(_txtDate, "text2", "soldierDiary");
	add(_lstDiary, "list", "soldierDiary");

	centerAllSurfaces();

	// Set up objects
	if (_game->getMod()->getStalkMode())
	{
		auto bgImageName = _game->getMod()->getInterface("soldierDiary")->getBackgroundImage();
		auto bgImage = _game->getMod()->getSurface(bgImageName);
		bgImage->blitNShade(_window, 0, 0);
	}
	else
		setWindowBackground((Window*)_window, "soldierDiary");

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldierDiaryOverviewState::btnOkClick, Options::keyCancel);

	_btnKills->setText(tr("STR_COMBAT"));
	_btnKills->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnKillsClick);

	_btnMissions->setText(tr("STR_PERFORMANCE"));
	_btnMissions->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnMissionsClick);

	_btnCommendations->setText(tr("STR_AWARDS"));
	_btnCommendations->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnCommendationsClick);
	_btnCommendations->setVisible(!_game->getMod()->getCommendationsList().empty());

	_btnPrev->setText("<<");
	if (_base == 0)
	{
		_btnPrev->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnNextClick);
		_btnPrev->onKeyboardPress((ActionHandler)&SoldierDiaryOverviewState::btnNextClick, Options::keyBattlePrevUnit);
	}
	else
	{
		_btnPrev->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnPrevClick);
		_btnPrev->onKeyboardPress((ActionHandler)&SoldierDiaryOverviewState::btnPrevClick, Options::keyBattlePrevUnit);
	}

	_btnNext->setText(">>");
	if (_base == 0)
	{
		_btnNext->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnPrevClick);
		_btnNext->onKeyboardPress((ActionHandler)&SoldierDiaryOverviewState::btnPrevClick, Options::keyBattleNextUnit);
	}
	else
	{
		_btnNext->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnNextClick);
		_btnNext->onKeyboardPress((ActionHandler)&SoldierDiaryOverviewState::btnNextClick, Options::keyBattleNextUnit);
	}

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	// Text is decided in init()

	_txtMission->setText(tr("STR_MISSION"));

	_txtRating->setText(tr("STR_RATING_UC"));

	_txtDate->setText(tr("STR_DATE_UC"));

	_lstDiary->setColumns(5, 104, 98, 30, 25, 35);
	_lstDiary->setSelectable(true);
	_lstDiary->setBackground(_window);
	_lstDiary->setMargin(8);
	_lstDiary->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::lstDiaryInfoClick);
}

/**
 *
 */
SoldierDiaryOverviewState::~SoldierDiaryOverviewState()
{

}

/**
 *  Clears all the variables and reinitializes the list of medals for the soldier.
 *
 */
void SoldierDiaryOverviewState::init()
{
	State::init();
	if (_list->empty())
	{
		_game->popState();
		return;
	}
	if (_soldierId >= _list->size())
	{
		_soldierId = 0;
	}
	_soldier = _list->at(_soldierId);
	_txtTitle->setText(_soldier->getName());

	if (!_base && _soldier->getDeath())
	{
		std::string deathTitleText, deathInfoText;
		const BattleUnitKills *cause = _soldier->getDeath()->getCause();
		if (cause)
		{
			deathTitleText = _game->getLanguage()->getString("STR_KILLED_IN_ACTION", _soldier->getGender());
			deathInfoText = tr("STR_KILLER_AND_WEAPON")
				.arg(cause->getUnitName(_game->getLanguage()))
				.arg(tr(cause->weapon));
		}
		else
		{
			deathTitleText = _game->getLanguage()->getString("STR_MISSING_IN_ACTION", _soldier->getGender());
		}

		std::ostringstream deathDateText;
		const GameTime *t = _soldier->getDeath()->getTime();
		deathDateText << t->getDayString(_game->getLanguage()) << " " << _game->getLanguage()->getString(t->getMonthString()) << " " << t->getYear();

		_txtDeathTitle->setText(deathTitleText);
		_txtDeathDate->setText(deathDateText.str());
		_txtDeathInfo->setText(deathInfoText);
	}

	_lstDiary->clearList();

	std::vector<MissionStatistics*> *missionStatistics = _game->getSavedGame()->getMissionStatistics();

	unsigned int row = 0;
	for (std::vector<MissionStatistics*>::iterator j = missionStatistics->begin() ; j != missionStatistics->end() ; ++j)
	{
		int missionId = (*j)->id;
		bool wasOnMission = false;

		// See if this mission is part of the soldier's vector of missions
		for (std::vector<int>::const_iterator k = _soldier->getDiary()->getMissionIdList().begin(); k != _soldier->getDiary()->getMissionIdList().end(); ++k)
		{
			if (missionId == (*k))
			{
				wasOnMission = true;
				break;
			}
		}
		if (!wasOnMission)
		{
			continue;
		}

		std::ostringstream ss;
		ss << (*j)->time.getYear();

		_lstDiary->addRow(5, (*j)->getMissionName(_game->getLanguage()).c_str(),
							 (*j)->getRatingString(_game->getLanguage()).c_str(),
							 (*j)->time.getDayString(_game->getLanguage()).c_str(),
							 tr((*j)->time.getMonthString()).c_str(),
							 ss.str().c_str());
		row++;
	}
	if (row > 0 && _lstDiary->getScroll() >= row)
	{
		_lstDiary->scrollTo(0);
	}
}

/**
 * Set the soldier's Id.
 */
void SoldierDiaryOverviewState::setSoldierId(size_t soldier)
{
	_soldierId = soldier;
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::btnOkClick(Action *)
{
	_soldierInfoState->setSoldierId(_soldierId);
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::btnKillsClick(Action *)
{
	_game->pushState(new SoldierDiaryPerformanceState(_base, _soldierId, this, DIARY_KILLS));
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::btnMissionsClick(Action *)
{
	_game->pushState(new SoldierDiaryPerformanceState(_base, _soldierId, this, DIARY_MISSIONS));
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::btnCommendationsClick(Action *)
{
	_game->pushState(new SoldierDiaryPerformanceState(_base, _soldierId, this, DIARY_COMMENDATIONS));
}


/**
 * Goes to the previous soldier.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::btnPrevClick(Action *)
{
	if (_soldierId == 0)
		_soldierId = _list->size() - 1;
	else
		_soldierId--;
	init();
}

/**
 * Goes to the next soldier.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::btnNextClick(Action *)
{
	_soldierId++;
	if (_soldierId >= _list->size())
		_soldierId = 0;
	init();
}

/**
 * Shows the selected soldier's info.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::lstDiaryInfoClick(Action *)
{
	int absoluteRowEntry = _lstDiary->getSelectedRow();
	_game->pushState(new SoldierDiaryMissionState(_soldier, absoluteRowEntry));
}

}
