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

#include "ArticleStateCraftWeapon.h"
#include "../Mod/ArticleDefinition.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleCraftWeapon.h"
#include "../Engine/Game.h"
#include "../Engine/Surface.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Unicode.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"
#include "../Mod/RuleInterface.h"

namespace OpenXcom
{

	ArticleStateCraftWeapon::ArticleStateCraftWeapon(ArticleDefinitionCraftWeapon *defs, std::shared_ptr<ArticleCommonState> state) : ArticleState(defs->id, std::move(state))
	{
		RuleCraftWeapon *weapon = _game->getMod()->getCraftWeapon(defs->id, true);
		RuleInterface *itf = _game->getMod()->getInterface("articleCraftWeapon");

		CraftWeaponCategory category = CWC_WEAPON;
		int offset = 0;
		if (weapon->getHidePediaInfo())
		{
			if (weapon->getTractorBeamPower() > 0)
			{
				category = CWC_TRACTOR_BEAM;
				offset = 32; // 2 * 16
			}
			else
			{
				category = CWC_EQUIPMENT;
				offset = 80; // 5 * 16
			}
		}

		// add screen elements
		_txtTitle = new Text(200, 32, 5, 24);

		int textColor = itf->getElement("text")->color;
		int textColor2 = itf->getElement("text")->color2;
		int listColor1 = itf->getElement("list")->color;
		int listColor2 = itf->getElement("list")->color2;

		ArticleState::initPaletteBg(defs, itf, "", "PAL_BATTLEPEDIA");
		ArticleState::initLayout();
		ArticleState::initButtons(itf->getElement("button")->color);

		// add other elements
		_txtTitle->setColor(itf->getElement("text")->color);
		add(_txtTitle, "title", "articleCraftWeapon", _bg);

		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(tr(defs->getTitleForPage(_state->current_page)));

		_txtInfo = new Text(310, 32 + offset, 5, 160 - offset);
		_lstInfo = new TextList(250, 111 - offset, 5, 80);

		_txtInfo->setColor(textColor);
		_txtInfo->setSecondaryColor(textColor2);

		switch (category)
		{
		case CWC_TRACTOR_BEAM:
			add(_txtInfo, "textHidePediaTractor", "articleCraftWeapon", _bg);
			add(_lstInfo, "listHidePediaTractor", "articleCraftWeapon", _bg);
			break;
		case CWC_EQUIPMENT:
			add(_txtInfo, "textHidePedia", "articleCraftWeapon", _bg);
			add(_lstInfo, "listHidePedia", "articleCraftWeapon", _bg);
			break;
		default:
			add(_txtInfo, "text", "articleCraftWeapon", _bg);
			add(_lstInfo, "list", "articleCraftWeapon", _bg);
		}
		_txtInfo->setWordWrap(true);
		_txtInfo->setScrollable(true);
		_txtInfo->setText(tr(defs->getTextForPage(_state->current_page)));

		_lstInfo->setVisible(category != CWC_EQUIPMENT);

		_lstInfo->setColor(listColor1);
		_lstInfo->setColumns(2, _lstInfo->getWidth() - 70, 70);
		_lstInfo->setDot(true);
		_lstInfo->setBig();

		if (category == CWC_WEAPON)
		{
			_lstInfo->addRow(2, tr("STR_DAMAGE").c_str(), Unicode::formatNumber(weapon->getDamage()).c_str());
			_lstInfo->setCellColor(0, 1, listColor2);

			_lstInfo->addRow(2, tr("STR_RANGE").c_str(), tr("STR_KILOMETERS").arg(weapon->getRange()).c_str());
			_lstInfo->setCellColor(1, 1, listColor2);

			_lstInfo->addRow(2, tr("STR_ACCURACY").c_str(), Unicode::formatPercentage(weapon->getAccuracy()).c_str());
			_lstInfo->setCellColor(2, 1, listColor2);

			_lstInfo->addRow(2, tr("STR_RE_LOAD_TIME").c_str(), tr("STR_SECONDS").arg(weapon->getStandardReload()).c_str());
			_lstInfo->setCellColor(3, 1, listColor2);

			_lstInfo->addRow(2, tr("STR_ROUNDS").c_str(), Unicode::formatNumber(weapon->getAmmoMax()).c_str());
			_lstInfo->setCellColor(4, 1, listColor2);
		}
		else if (category == CWC_TRACTOR_BEAM)
		{
			_lstInfo->addRow(2, tr("STR_TRACTOR_BEAM_POWER").c_str(), Unicode::formatNumber(weapon->getTractorBeamPower()).c_str());
			_lstInfo->setCellColor(0, 1, listColor2);

			_lstInfo->addRow(2, tr("STR_RANGE").c_str(), tr("STR_KILOMETERS").arg(weapon->getRange()).c_str());
			_lstInfo->setCellColor(1, 1, listColor2);
		}

		centerAllSurfaces();
	}

	ArticleStateCraftWeapon::~ArticleStateCraftWeapon()
	{}

}
