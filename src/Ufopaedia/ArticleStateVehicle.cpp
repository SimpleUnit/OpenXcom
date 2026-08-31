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

#include "../Mod/ArticleDefinition.h"
#include "ArticleStateVehicle.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Mod/Mod.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"
#include "../Mod/Unit.h"
#include "../Mod/Armor.h"
#include "../Mod/RuleItem.h"
#include "../Mod/RuleInterface.h"

namespace OpenXcom
{

	ArticleStateVehicle::ArticleStateVehicle(ArticleDefinitionVehicle *defs, std::shared_ptr<ArticleCommonState> state) : ArticleState(defs->id, std::move(state))
	{
		RuleItem *item = _game->getMod()->getItem(defs->id, true);
		Unit *unit = item->getVehicleUnit();
		if (!unit)
		{
			throw Exception("ArticleStateVehicle: Item " + defs->id + " is missing a vehicle unit definition!");
		}
		const Armor *armor = unit->getArmor();
		RuleInterface* itf = _game->getMod()->getInterface("articleVehicle");

		// add screen elements
		_txtTitle = new Text(310, 17, 5, 23);
		_txtInfo = new Text(300, 150, 10, 122);
		_lstStats = new TextList(300, 89, 10, 48);

		ArticleState::initPaletteBg(defs, itf, "BACK10.SCR", "PAL_UFOPAEDIA");
		ArticleState::initLayout();
		ArticleState::initButtons(itf->getElement("button")->color);
		_btnInfo->setVisible(false);

		// add other elements
		add(_txtTitle, "title", "articleVehicle", _bg);
		if (item->getVehicleClipAmmo())
		{
			_txtInfo->setY(138);
			add(_txtInfo, "text", "articleVehicle", _bg);
		}
		else
		{
			add(_txtInfo, "textNoAmmo", "articleVehicle", _bg);
		}
		add(_lstStats, "list", "articleVehicle", _bg);

		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(tr(defs->getTitleForPage(_state->current_page)));

		_txtInfo->setWordWrap(true);
		_txtInfo->setScrollable(true);
		_txtInfo->setText(tr(defs->getTextForPage(_state->current_page)));

		_lstStats->setColumns(2, _lstStats->getWidth() - 145, 145);
		_lstStats->setDot(true);

		std::ostringstream ss;
		ss << unit->getStats()->tu;
		_lstStats->addRow(2, tr("STR_TIME_UNITS").c_str(), ss.str().c_str());

		std::ostringstream ss2;
		ss2 << unit->getStats()->health;
		_lstStats->addRow(2, tr("STR_HEALTH").c_str(), ss2.str().c_str());

		std::ostringstream ss3;
		ss3 << armor->getFrontArmor();
		_lstStats->addRow(2, tr("STR_FRONT_ARMOR").c_str(), ss3.str().c_str());

		std::ostringstream ss4;
		ss4 << armor->getLeftSideArmor();
		_lstStats->addRow(2, tr("STR_LEFT_ARMOR").c_str(), ss4.str().c_str());

		std::ostringstream ss5;
		ss5 << armor->getRightSideArmor();
		_lstStats->addRow(2, tr("STR_RIGHT_ARMOR").c_str(), ss5.str().c_str());

		std::ostringstream ss6;
		ss6 << armor->getRearArmor();
		_lstStats->addRow(2, tr("STR_REAR_ARMOR").c_str(), ss6.str().c_str());

		std::ostringstream ss7;
		ss7 << armor->getUnderArmor();
		_lstStats->addRow(2, tr("STR_UNDER_ARMOR").c_str(), ss7.str().c_str());

		_lstStats->addRow(2, tr("STR_WEAPON").c_str(), tr(defs->weapon).c_str());

		if (item->getVehicleClipAmmo())
		{
			const RuleItem *ammo = item->getVehicleClipAmmo();

			std::ostringstream ss8;
			if (item->getIgnoreAmmoPower())
			{
				ss8 << item->getPower();
			}
			else
			{
				ss8 << ammo->getPower();
			}
			_lstStats->addRow(2, tr("STR_WEAPON_POWER").c_str(), ss8.str().c_str());

			_lstStats->addRow(2, tr("STR_AMMUNITION").c_str(), tr(ammo->getName()).c_str());

			std::ostringstream ss9;
			ss9 << item->getVehicleClipSize();

			_lstStats->addRow(2, tr("STR_ROUNDS").c_str(), ss9.str().c_str());
		}
		else
		{
			std::ostringstream ss8;
			ss8 << item->getPower();
			_lstStats->addRow(2, tr("STR_WEAPON_POWER").c_str(), ss8.str().c_str());
		}
		centerAllSurfaces();
	}

	ArticleStateVehicle::~ArticleStateVehicle()
	{}

}
