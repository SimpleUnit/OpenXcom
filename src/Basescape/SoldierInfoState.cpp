/*
 * Copyright 2010-2017 OpenXcom Developers.
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
#include "SoldierInfoState.h"
#include "SoldierDiaryOverviewState.h"
#include <algorithm>
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Options.h"
#include "../Interface/Bar.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Engine/Surface.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Soldier.h"
#include "../Engine/SurfaceSet.h"
#include "../Mod/Armor.h"
#include "../Menu/ErrorMessageState.h"
#include "SellState.h"
#include "SoldierArmorState.h"
#include "SoldierBonusState.h"
#include "SackSoldierState.h"
#include "../Mod/RuleInterface.h"
#include "../Mod/RuleSoldier.h"
#include "../Mod/RuleSoldierBonus.h"
#include "../Savegame/SoldierDeath.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldier Info screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from. NULL to use the dead soldiers list.
 * @param soldierId ID of the selected soldier.
 */
SoldierInfoState::SoldierInfoState(Base *base, size_t soldierId) : _base(base), _soldierId(soldierId), _soldier(0)
{
	if (_base == 0)
	{
		_list = _game->getSavedGame()->getDeadSoldiers();
		if (_soldierId >= _list->size())
		{
			_soldierId = 0;
		}
		else
		{
			_soldierId = _list->size() - (1 + _soldierId);
		}
	}
	else
	{
		_list = _base->getSoldiers();
	}

	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_rank = new Surface(26, 23, 4, 4);
	_flag = new InteractiveSurface(40, 20, 275, 6);
	_btnPrev = new TextButton(28, 14, 0, 33);
	_btnOk = new TextButton(48, 14, 30, 33);
	_btnNext = new TextButton(28, 14, 80, 33);
	_btnArmor = new TextButton(110, 14, 130, 33);
	_btnBonuses = new TextButton(16, 14, 242, 33);
	_edtSoldier = new TextEdit(this, 210, 16, 40, 9);
	_btnSack = new TextButton(60, 14, 260, 33);
	_btnDiary = new TextButton(60, 14, 260, 48);
	_txtRank = new Text(130, 9, 0, 48);
	_txtMissions = new Text(100, 9, 130, 48);
	_txtKills = new Text(100, 9, 200, 48);
	_txtStuns = new Text(60, 9, 260, 48);
	_txtCraft = new Text(130, 9, 0, 56);
	_txtRecovery = new Text(180, 9, 130, 56);
	_txtPsionic = new Text(150, 9, 0, 66);
	_txtDead = new Text(150, 9, 130, 33);

	int yPos = 80;
	int step = 11;
	if (_game->getMod()->isManaFeatureEnabled())
	{
		yPos = 81;
		step = 10;
	}

	_txtTimeUnits = new Text(120, 9, 6, yPos);
	_numTimeUnits = new Text(18, 9, 131, yPos);
	_barTimeUnits = new Bar(170, 7, 150, yPos);
	yPos += step;

	_txtStamina = new Text(120, 9, 6, yPos);
	_numStamina = new Text(18, 9, 131, yPos);
	_barStamina = new Bar(170, 7, 150, yPos);
	yPos += step;

	_txtHealth = new Text(120, 9, 6, yPos);
	_numHealth = new Text(18, 9, 131, yPos);
	_barHealth = new Bar(170, 7, 150, yPos);
	yPos += step;

	_txtBravery = new Text(120, 9, 6, yPos);
	_numBravery = new Text(18, 9, 131, yPos);
	_barBravery = new Bar(170, 7, 150, yPos);
	yPos += step;

	_txtReactions = new Text(120, 9, 6, yPos);
	_numReactions = new Text(18, 9, 131, yPos);
	_barReactions = new Bar(170, 7, 150, yPos);
	yPos += step;

	_txtFiring = new Text(120, 9, 6, yPos);
	_numFiring = new Text(18, 9, 131, yPos);
	_barFiring = new Bar(170, 7, 150, yPos);
	yPos += step;

	_txtThrowing = new Text(120, 9, 6, yPos);
	_numThrowing = new Text(18, 9, 131, yPos);
	_barThrowing = new Bar(170, 7, 150, yPos);
	yPos += step;

	_txtMelee = new Text(120, 9, 6, yPos);
	_numMelee = new Text(18, 9, 131, yPos);
	_barMelee = new Bar(170, 7, 150, yPos);
	yPos += step;

	_txtStrength = new Text(120, 9, 6, yPos);
	_numStrength = new Text(18, 9, 131, yPos);
	_barStrength = new Bar(170, 7, 150, yPos);
	yPos += step;

	if (_game->getMod()->isManaFeatureEnabled())
	{
		_txtMana = new Text(120, 9, 6, yPos);
		_numMana = new Text(18, 9, 131, yPos);
		_barMana = new Bar(170, 7, 150, yPos);
		yPos += step;
	}

	_txtPsiStrength = new Text(120, 9, 6, yPos);
	_numPsiStrength = new Text(18, 9, 131, yPos);
	_barPsiStrength = new Bar(170, 7, 150, yPos);
	yPos += step;

	_txtPsiSkill = new Text(120, 9, 6, yPos);
	_numPsiSkill = new Text(18, 9, 131, yPos);
	_barPsiSkill = new Bar(170, 7, 150, yPos);

	// Set palette
	setInterface("soldierInfo");

	add(_bg);
	add(_rank);
	add(_flag);
	add(_btnOk, "button", "soldierInfo");
	add(_btnPrev, "button", "soldierInfo");
	add(_btnNext, "button", "soldierInfo");
	add(_btnArmor, "button", "soldierInfo");
	add(_btnBonuses, "button", "soldierInfo");
	add(_edtSoldier, "text1", "soldierInfo");
	add(_btnSack, "button", "soldierInfo");
	add(_btnDiary, "button", "soldierInfo");
	add(_txtRank, "text1", "soldierInfo");
	add(_txtMissions, "text1", "soldierInfo");
	add(_txtKills, "text1", "soldierInfo");
	add(_txtStuns, "text1", "soldierInfo");
	add(_txtCraft, "text1", "soldierInfo");
	add(_txtRecovery, "text1", "soldierInfo");
	add(_txtPsionic, "text2", "soldierInfo");
	add(_txtDead, "text2", "soldierInfo");

	add(_txtTimeUnits, "text2", "soldierInfo");
	add(_numTimeUnits, "numbers", "soldierInfo");
	add(_barTimeUnits, "barTUs", "soldierInfo");

	add(_txtStamina, "text2", "soldierInfo");
	add(_numStamina, "numbers", "soldierInfo");
	add(_barStamina, "barEnergy", "soldierInfo");

	add(_txtHealth, "text2", "soldierInfo");
	add(_numHealth, "numbers", "soldierInfo");
	add(_barHealth, "barHealth", "soldierInfo");

	add(_txtBravery, "text2", "soldierInfo");
	add(_numBravery, "numbers", "soldierInfo");
	add(_barBravery, "barBravery", "soldierInfo");

	add(_txtReactions, "text2", "soldierInfo");
	add(_numReactions, "numbers", "soldierInfo");
	add(_barReactions, "barReactions", "soldierInfo");

	add(_txtFiring, "text2", "soldierInfo");
	add(_numFiring, "numbers", "soldierInfo");
	add(_barFiring, "barFiring", "soldierInfo");

	add(_txtThrowing, "text2", "soldierInfo");
	add(_numThrowing, "numbers", "soldierInfo");
	add(_barThrowing, "barThrowing", "soldierInfo");

	add(_txtMelee, "text2", "soldierInfo");
	add(_numMelee, "numbers", "soldierInfo");
	add(_barMelee, "barMelee", "soldierInfo");

	add(_txtStrength, "text2", "soldierInfo");
	add(_numStrength, "numbers", "soldierInfo");
	add(_barStrength, "barStrength", "soldierInfo");

	if (_game->getMod()->isManaFeatureEnabled())
	{
		add(_txtMana, "text2", "soldierInfo");
		add(_numMana, "numbers", "soldierInfo");
		add(_barMana, "barMana", "soldierInfo");
	}

	add(_txtPsiStrength, "text2", "soldierInfo");
	add(_numPsiStrength, "numbers", "soldierInfo");
	add(_barPsiStrength, "barPsiStrength", "soldierInfo");

	add(_txtPsiSkill, "text2", "soldierInfo");
	add(_numPsiSkill, "numbers", "soldierInfo");
	add(_barPsiSkill, "barPsiSkill", "soldierInfo");

	centerAllSurfaces();

	// Set up objects
	_game->getMod()->getSurface("BACK06.SCR")->blitNShade(_bg, 0, 0);

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldierInfoState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldierInfoState::btnOkClick, Options::keyCancel);

	_btnPrev->setText("<<");
	if (_base == 0)
	{
		_btnPrev->onMouseClick((ActionHandler)&SoldierInfoState::btnNextClick);
		_btnPrev->onKeyboardPress((ActionHandler)&SoldierInfoState::btnNextClick, Options::keyBattlePrevUnit);
	}
	else
	{
		_btnPrev->onMouseClick((ActionHandler)&SoldierInfoState::btnPrevClick);
		_btnPrev->onKeyboardPress((ActionHandler)&SoldierInfoState::btnPrevClick, Options::keyBattlePrevUnit);
	}

	_btnNext->setText(">>");
	if (_base == 0)
	{
		_btnNext->onMouseClick((ActionHandler)&SoldierInfoState::btnPrevClick);
		_btnNext->onKeyboardPress((ActionHandler)&SoldierInfoState::btnPrevClick, Options::keyBattleNextUnit);
	}
	else
	{
		_btnNext->onMouseClick((ActionHandler)&SoldierInfoState::btnNextClick);
		_btnNext->onKeyboardPress((ActionHandler)&SoldierInfoState::btnNextClick, Options::keyBattleNextUnit);
	}

	_btnArmor->setText(tr("STR_ARMOR"));
	_btnArmor->onMouseClick((ActionHandler)&SoldierInfoState::btnArmorClick);

	_btnBonuses->setText(tr("STR_BONUSES_BUTTON")); // tiny button, default translation is " "
	_btnBonuses->onMouseClick((ActionHandler)&SoldierInfoState::btnBonusesClick);

	_edtSoldier->setBig();
	_edtSoldier->onChange((ActionHandler)&SoldierInfoState::edtSoldierChange);
	_edtSoldier->onMousePress((ActionHandler)&SoldierInfoState::edtSoldierPress);

	// Can't change nationality of dead soldiers
	if (_base != 0)
	{
		// Ignore also if flags are used to indicate number of kills
		if (_game->getMod()->getFlagByKills().empty())
		{
			_flag->onMouseClick((ActionHandler)&SoldierInfoState::btnFlagClick, SDL_BUTTON_LEFT);
			_flag->onMouseClick((ActionHandler)&SoldierInfoState::btnFlagClick, SDL_BUTTON_RIGHT);
		}
	}

	_btnSack->setText(tr("STR_SACK"));
	_btnSack->onMouseClick((ActionHandler)&SoldierInfoState::btnSackClick);

	_btnDiary->setText(tr("STR_DIARY"));
	_btnDiary->onMouseClick((ActionHandler)&SoldierInfoState::btnDiaryClick);
	_btnDiary->setVisible(Options::soldierDiaries);

	_txtPsionic->setText(tr("STR_IN_PSIONIC_TRAINING"));

	_txtTimeUnits->setText(tr("STR_TIME_UNITS"));

	_barTimeUnits->setScale(1.0);

	_txtStamina->setText(tr("STR_STAMINA"));

	_barStamina->setScale(1.0);

	_txtHealth->setText(tr("STR_HEALTH"));

	_barHealth->setScale(1.0);

	_txtBravery->setText(tr("STR_BRAVERY"));

	_barBravery->setScale(1.0);

	_txtReactions->setText(tr("STR_REACTIONS"));

	_barReactions->setScale(1.0);

	_txtFiring->setText(tr("STR_FIRING_ACCURACY"));

	_barFiring->setScale(1.0);

	_txtThrowing->setText(tr("STR_THROWING_ACCURACY"));

	_barThrowing->setScale(1.0);

	_txtMelee->setText(tr("STR_MELEE_ACCURACY"));

	_barMelee->setScale(1.0);

	_txtStrength->setText(tr("STR_STRENGTH"));

	_barStrength->setScale(1.0);

	if (_game->getMod()->isManaFeatureEnabled())
	{
		_txtMana->setText(tr("STR_MANA_POOL"));
		_barMana->setScale(1.0);
	}

	_txtPsiStrength->setText(tr("STR_PSIONIC_STRENGTH"));

	_barPsiStrength->setScale(1.0);

	_txtPsiSkill->setText(tr("STR_PSIONIC_SKILL"));

	_barPsiSkill->setScale(1.0);
}

/**
 *
 */
SoldierInfoState::~SoldierInfoState()
{

}

/**
 * Updates soldier stats when
 * the soldier changes.
 */
void SoldierInfoState::init()
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
	_edtSoldier->setBig();
	_edtSoldier->setText(_soldier->getName());
	UnitStats *initial = _soldier->getInitStats();
	UnitStats *current = _soldier->getCurrentStats();

	bool hasBonus = _soldier->prepareStatsWithBonuses(_game->getMod()); // refresh all bonuses
	UnitStats withArmor = *_soldier->getStatsWithAllBonuses();
	_btnBonuses->setVisible(hasBonus);

	SurfaceSet *texture = _game->getMod()->getSurfaceSet("BASEBITS.PCK");
	auto frame = texture->getFrame(_soldier->getRankSprite());
	if (frame)
	{
		frame->blitNShade(_rank, 0, 0);
	}

	std::ostringstream flagId;
	flagId << "Flag";
	const std::vector<int> mapping = _game->getMod()->getFlagByKills();
	if (mapping.empty())
	{
		flagId << _soldier->getNationality() + _soldier->getRules()->getFlagOffset();
	}
	else
	{
		int index = 0;
		for (auto item : mapping)
		{
			if (_soldier->getKills() <= item)
			{
				break;
			}
			index++;
		}
		flagId << index + _soldier->getRules()->getFlagOffset();
	}
	Surface *flagTexture = _game->getMod()->getSurface(flagId.str().c_str(), false);
	_flag->clear();
	if (flagTexture != 0)
	{
		flagTexture->blitNShade(_flag, _flag->getWidth() - flagTexture->getWidth(), 0); // align right
	}

	std::ostringstream ss;
	ss << withArmor.tu;
	_numTimeUnits->setText(ss.str());
	_barTimeUnits->setMax(current->tu);
	_barTimeUnits->setValue(withArmor.tu);
	_barTimeUnits->setValue2(std::min(withArmor.tu, initial->tu));

	std::ostringstream ss2;
	ss2 << withArmor.stamina;
	_numStamina->setText(ss2.str());
	_barStamina->setMax(current->stamina);
	_barStamina->setValue(withArmor.stamina);
	_barStamina->setValue2(std::min(withArmor.stamina, initial->stamina));

	std::ostringstream ss3;
	ss3 << withArmor.health;
	_numHealth->setText(ss3.str());
	_barHealth->setMax(current->health);
	_barHealth->setValue(withArmor.health);
	_barHealth->setValue2(std::min(withArmor.health, initial->health));

	std::ostringstream ss4;
	ss4 << withArmor.bravery;
	_numBravery->setText(ss4.str());
	_barBravery->setMax(current->bravery);
	_barBravery->setValue(withArmor.bravery);
	_barBravery->setValue2(std::min(withArmor.bravery, initial->bravery));

	std::ostringstream ss5;
	ss5 << withArmor.reactions;
	_numReactions->setText(ss5.str());
	_barReactions->setMax(current->reactions);
	_barReactions->setValue(withArmor.reactions);
	_barReactions->setValue2(std::min(withArmor.reactions, initial->reactions));

	std::ostringstream ss6;
	ss6 << withArmor.firing;
	_numFiring->setText(ss6.str());
	_barFiring->setMax(current->firing);
	_barFiring->setValue(withArmor.firing);
	_barFiring->setValue2(std::min(withArmor.firing, initial->firing));

	std::ostringstream ss7;
	ss7 << withArmor.throwing;
	_numThrowing->setText(ss7.str());
	_barThrowing->setMax(current->throwing);
	_barThrowing->setValue(withArmor.throwing);
	_barThrowing->setValue2(std::min(withArmor.throwing, initial->throwing));

	std::ostringstream ss8;
	ss8 << withArmor.melee;
	_numMelee->setText(ss8.str());
	_barMelee->setMax(current->melee);
	_barMelee->setValue(withArmor.melee);
	_barMelee->setValue2(std::min(withArmor.melee, initial->melee));

	std::ostringstream ss9;
	ss9 << withArmor.strength;
	_numStrength->setText(ss9.str());
	_barStrength->setMax(current->strength);
	_barStrength->setValue(withArmor.strength);
	_barStrength->setValue2(std::min(withArmor.strength, initial->strength));

	std::string wsArmor;
	if (_soldier->getArmor() == _soldier->getRules()->getDefaultArmor())
	{
		wsArmor= tr("STR_ARMOR_").arg(tr(_soldier->getArmor()->getType()));
	}
	else
	{
		wsArmor = tr(_soldier->getArmor()->getType());
	}

	_btnArmor->setText(wsArmor);

	_btnSack->setVisible(_game->getSavedGame()->getMonthsPassed() > -1 && !(_soldier->getCraft() && _soldier->getCraft()->getStatus() == "STR_OUT"));

	_txtRank->setText(tr("STR_RANK_").arg(tr(_soldier->getRankString())));

	_txtMissions->setText(tr("STR_MISSIONS").arg(_soldier->getMissions()));

	_txtKills->setText(tr("STR_KILLS").arg(_soldier->getKills()));

	_txtStuns->setText(tr("STR_STUNS").arg(_soldier->getStuns()));
	_txtStuns->setVisible(!Options::soldierDiaries);

	std::string craft;
	if (_soldier->getCraft() == 0)
	{
		craft = tr("STR_NONE_UC");
	}
	else
	{
		craft = _soldier->getCraft()->getName(_game->getLanguage());
	}
	_txtCraft->setText(tr("STR_CRAFT_").arg(craft));

	auto recovery = _base ? _base->getSumRecoveryPerDay() : BaseSumDailyRecovery();
	auto getDaysOrInfinity = [&](int days)
	{
		if (days < 0)
		{
			return std::string{ "∞" };
		}
		else
		{
			return std::string{tr("STR_DAY", days)};
		}
	};
	if (_soldier->isWounded())
	{
		int recoveryTime = _soldier->getNeededRecoveryTime(recovery);
		_txtRecovery->setText(tr("STR_WOUND_RECOVERY").arg(getDaysOrInfinity(recoveryTime)));
	}
	else
	{
		_txtRecovery->setText("");
		if (_soldier->getManaMissing() > 0)
		{
			int manaRecoveryTime = _soldier->getManaRecovery(recovery.ManaRecovery);
			_txtRecovery->setText(tr("STR_MANA_RECOVERY").arg(getDaysOrInfinity(manaRecoveryTime)));
		}
		if (_soldier->getHealthMissing() > 0)
		{
			int healthRecoveryTime = _soldier->getHealthRecovery(recovery.HealthRecovery);
			_txtRecovery->setText(tr("STR_HEALTH_RECOVERY").arg(getDaysOrInfinity(healthRecoveryTime)));
		}
	}

	_txtPsionic->setVisible(_soldier->isInPsiTraining());

	if (_game->getMod()->isManaFeatureEnabled())
	{
		if (_game->getSavedGame()->isManaUnlocked(_game->getMod()))
		{
			std::ostringstream ss16;
			ss16 << withArmor.mana;
			_numMana->setText(ss16.str());
			_barMana->setMax(current->mana);
			_barMana->setValue(withArmor.mana);
			_barMana->setValue2(std::min(withArmor.mana, initial->mana));

			_txtMana->setVisible(true);
			_numMana->setVisible(true);
			_barMana->setVisible(true);
		}
		else
		{
			_txtMana->setVisible(false);
			_numMana->setVisible(false);
			_barMana->setVisible(false);
		}
	}

	if (current->psiSkill > 0 || (Options::psiStrengthEval && _game->getSavedGame()->isResearched(_game->getMod()->getPsiRequirements())))
	{
		std::ostringstream ss14;
		ss14 << withArmor.psiStrength;
		_numPsiStrength->setText(ss14.str());
		_barPsiStrength->setMax(current->psiStrength);
		_barPsiStrength->setValue(withArmor.psiStrength);
		_barPsiStrength->setValue2(std::min(withArmor.psiStrength, initial->psiStrength));

		_txtPsiStrength->setVisible(true);
		_numPsiStrength->setVisible(true);
		_barPsiStrength->setVisible(true);
	}
	else
	{
		_txtPsiStrength->setVisible(false);
		_numPsiStrength->setVisible(false);
		_barPsiStrength->setVisible(false);
	}

	if (current->psiSkill > 0)
	{
		std::ostringstream ss15;
		ss15 << withArmor.psiSkill;
		_numPsiSkill->setText(ss15.str());
		_barPsiSkill->setMax(current->psiSkill);
		_barPsiSkill->setValue(withArmor.psiSkill);
		_barPsiSkill->setValue2(std::min(withArmor.psiSkill, initial->psiSkill));

		_txtPsiSkill->setVisible(true);
		_numPsiSkill->setVisible(true);
		_barPsiSkill->setVisible(true);
	}
	else
	{
		_txtPsiSkill->setVisible(false);
		_numPsiSkill->setVisible(false);
		_barPsiSkill->setVisible(false);
	}

	// Dead can't talk
	if (_base == 0)
	{
		_btnArmor->setVisible(false);
		_btnSack->setVisible(false);
		_txtCraft->setVisible(false);
		_txtDead->setVisible(true);
		std::string status = "STR_MISSING_IN_ACTION";
		if (_soldier->getDeath() && _soldier->getDeath()->getCause())
		{
			status = "STR_KILLED_IN_ACTION";
		}
		_txtDead->setText(tr(status, _soldier->getGender()));
	}
	else
	{
		_txtDead->setVisible(false);
	}
}

/**
 * Disables the soldier input.
 * @param action Pointer to an action.
 */
void SoldierInfoState::edtSoldierPress(Action *)
{
	if (_base == 0)
	{
		_edtSoldier->setFocus(false);
	}
}

/**
 * Set the soldier Id.
 */
void SoldierInfoState::setSoldierId(size_t soldier)
{
	_soldierId = soldier;
}

/**
 * Changes the soldier's name.
 * @param action Pointer to an action.
 */
void SoldierInfoState::edtSoldierChange(Action *)
{
	_soldier->setName(_edtSoldier->getText());
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnOkClick(Action *)
{

	_game->popState();
	if (_game->getSavedGame()->getMonthsPassed() > -1 && Options::storageLimitsEnforced && _base != 0 && _base->storesOverfull())
	{
		_game->pushState(new SellState(_base, 0));
		_game->pushState(new ErrorMessageState(tr("STR_STORAGE_EXCEEDED").arg(_base->getName()), _palette, _game->getMod()->getInterface("soldierInfo")->getElement("errorMessage")->color, "BACK01.SCR", _game->getMod()->getInterface("soldierInfo")->getElement("errorPalette")->color));
	}
}

/**
 * Goes to the previous soldier.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnPrevClick(Action *)
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
void SoldierInfoState::btnNextClick(Action *)
{
	_soldierId++;
	if (_soldierId >= _list->size())
		_soldierId = 0;
	init();
}

/**
 * Shows the Select Armor window.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnArmorClick(Action *)
{
	if (!_soldier->getCraft() || (_soldier->getCraft() && _soldier->getCraft()->getStatus() != "STR_OUT"))
	{
		_game->pushState(new SoldierArmorState(_base, _soldierId, SA_GEOSCAPE));
	}
}

/**
 * Shows the SoldierBonus window.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnBonusesClick(Action *)
{
	_game->pushState(new SoldierBonusState(_base, _soldierId));
}

/**
 * Shows the Sack Soldier window.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnSackClick(Action *)
{
	_game->pushState(new SackSoldierState(_base, _soldierId));
}

/**
 * Shows the Diary Soldier window.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnDiaryClick(Action *)
{
	_game->pushState(new SoldierDiaryOverviewState(_base, _soldierId, this));
}

/**
* Changes soldier's nationality.
* @param action Pointer to an action.
*/
void SoldierInfoState::btnFlagClick(Action *action)
{
	int temp = _soldier->getNationality();
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		temp += 1;
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		temp += -1;
	}

	const std::vector<SoldierNamePool*> &names = _soldier->getRules()->getNames();
	if (!names.empty())
	{
		const int max = names.size();
		if (temp > max - 1)
		{
			temp = 0;
		}
		else if (temp < 0)
		{
			temp = max - 1;
		}
	}
	else
	{
		temp = 0;
	}

	_soldier->setNationality(temp);
	init();
}

}
