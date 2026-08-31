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

#include <sstream>
#include "ArticleStateCraft.h"
#include "../Mod/ArticleDefinition.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleCraft.h"
#include "../Mod/RuleInterface.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Unicode.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"

namespace OpenXcom
{

	ArticleStateCraft::ArticleStateCraft(ArticleDefinitionCraft *defs, std::shared_ptr<ArticleCommonState> state) : ArticleState(defs->id, std::move(state))
	{
		RuleCraft *craft = _game->getMod()->getCraft(defs->id, true);
		RuleInterface* itf = _game->getMod()->getInterface("articleCraft");

		// add screen elements
		_txtTitle = new Text(210, 32, 5, 24);

		ArticleState::initPaletteBg(defs, itf, "", "PAL_UFOPAEDIA");
		ArticleState::initLayout();
		ArticleState::initButtons(itf->getElement("button")->color);

		// add other elements
		add(_txtTitle, "title", "articleCraft", _bg);

		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(tr(defs->getTitleForPage(_state->current_page)));

		_txtInfo = new Text(defs->rect_text.width, defs->rect_text.height, defs->rect_text.x, defs->rect_text.y);
		add(_txtInfo);

		_txtInfo->setColor(itf->getElement("text")->color);
		_txtInfo->setSecondaryColor(itf->getElement("text")->color2);
		_txtInfo->setWordWrap(true);
		_txtInfo->setScrollable(true);
		_txtInfo->setText(tr(defs->getTextForPage(_state->current_page)));

		_txtStats = new Text(defs->rect_stats.width, defs->rect_stats.height, defs->rect_stats.x, defs->rect_stats.y);
		add(_txtStats);

		_txtStats->setColor(itf->getElement("list")->color);
		_txtStats->setSecondaryColor(itf->getElement("list")->color2);

		std::ostringstream ss;
		ss << tr("STR_MAXIMUM_SPEED_UC").arg(Unicode::formatNumber(craft->getMaxSpeed())) << '\n';
		ss << tr("STR_ACCELERATION").arg(craft->getAcceleration()) << '\n';
		int range;
		switch (_game->getMod()->getPediaReplaceCraftFuelWithRangeType())
		{
			// Both max range alone and average range get rounded
			case 0:
			case 2:
				range = craft->calculateRange(_game->getMod()->getPediaReplaceCraftFuelWithRangeType());
				if (range == -1)
				{
					ss << tr("STR_MAXIMUM_RANGE").arg(tr("STR_INFINITE_RANGE")) << '\n';
					break;
				}

				// Round the answer to
				if (range < 100)
				{
					// don't round if it's small!
				}
				else if (range < 1000)
				{
					// nearest 10 nautical miles
					range += 10 / 2;
					range -= range % 10;
				}
				else
				{
					// nearest 100 nautical miles
					range += 100 / 2;
					range -= range % 100;
				}

				ss << tr("STR_MAXIMUM_RANGE").arg(Unicode::formatNumber(range)) << '\n';
				break;
			// Min-maxxers can fret over exact numbers
			case 1:
				if (craft->calculateRange(0) == -1)
				{
					ss << tr("STR_MAXIMUM_RANGE").arg(tr("STR_INFINITE_RANGE")) << '\n';
					break;
				}

				ss << tr("STR_MINIMUM_RANGE").arg(Unicode::formatNumber(craft->calculateRange(1))) << '\n';
				ss << tr("STR_MAXIMUM_RANGE").arg(Unicode::formatNumber(craft->calculateRange(0))) << '\n';
				break;
			default :
				ss << tr("STR_FUEL_CAPACITY").arg(Unicode::formatNumber(craft->getMaxFuel())) << '\n';
				break;
		}
		ss << tr("STR_WEAPON_PODS").arg(craft->getWeapons()) << '\n';
		ss << tr("STR_DAMAGE_CAPACITY_UC").arg(Unicode::formatNumber(craft->getMaxDamage())) << '\n';
		if (craft->getMaxUnits() == craft->getMaxUnitsLimit())
		{
			ss << tr("STR_CARGO_SPACE").arg(craft->getMaxUnits()) << '\n';
		}
		else
		{
			std::ostringstream ss2;
			ss2 << craft->getMaxUnits() << "/" << craft->getMaxUnitsLimit();
			ss << tr("STR_CARGO_SPACE").arg(ss2.str()) << '\n';
		}
		if (craft->getPilots() > 0)
		{
			ss << tr("STR_COCKPIT_CAPACITY").arg(craft->getPilots()) << '\n';
		}
		if (craft->getMaxVehiclesAndLargeSoldiers() == craft->getMaxVehiclesAndLargeSoldiersLimit())
		{
			ss << tr("STR_HWP_CAPACITY").arg(craft->getMaxVehiclesAndLargeSoldiers());
		}
		else
		{
			std::ostringstream ss2;
			ss2 << craft->getMaxVehiclesAndLargeSoldiers() << "/" << craft->getMaxVehiclesAndLargeSoldiersLimit();
			ss << tr("STR_HWP_CAPACITY").arg(ss2.str());
		}
		_txtStats->setText(ss.str());

		centerAllSurfaces();
	}

	ArticleStateCraft::~ArticleStateCraft()
	{}

}
