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
#include "../Mod/Mod.h"
#include "../Mod/RuleBaseFacility.h"
#include "../Mod/RuleInterface.h"
#include "ArticleStateTFTD.h"
#include "ArticleStateTFTDFacility.h"
#include "../Engine/Game.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Engine/Unicode.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

	ArticleStateTFTDFacility::ArticleStateTFTDFacility(ArticleDefinitionTFTD *defs, std::shared_ptr<ArticleCommonState> state) : ArticleStateTFTD(defs, std::move(state))
	{
		_btnInfo->setVisible(_game->getMod()->getShowPediaInfoButton());

		RuleBaseFacility *facility = _game->getMod()->getBaseFacility(defs->id, true);
		RuleInterface* itf = _game->getMod()->getInterface("articleBaseFacilityTFTD");

		_lstInfo = new TextList(150, 50, 168, 150);
		add(_lstInfo, "list", "articleBaseFacilityTFTD", _bg);

		_lstInfo->setColor(_listColor1);
		_lstInfo->setColumns(2, 104, 46);
		_lstInfo->setDot(true);

		std::ostringstream ss;
		int row = 0;
		if (facility->getDefenseValue() > 0)
		{
			_txtInfo->setX(itf->getElement("textDefense")->x);
			_txtInfo->setY(itf->getElement("textDefense")->y);
			_txtInfo->setWidth(itf->getElement("textDefense")->w);
			_txtInfo->setHeight(itf->getElement("textDefense")->h);
			_lstInfo->setX(itf->getElement("listDefense")->x);
			_lstInfo->setY(itf->getElement("listDefense")->y);
			_lstInfo->setWidth(itf->getElement("listDefense")->w);
			_lstInfo->setHeight(itf->getElement("listDefense")->h);
			ss.str("");ss.clear();
			ss << facility->getDefenseValue();
			_lstInfo->addRow(2, tr("STR_DEFENSE_VALUE").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, _listColor2);

			ss.str("");ss.clear();
			ss << Unicode::formatPercentage(facility->getHitRatio());
			_lstInfo->addRow(2, tr("STR_HIT_RATIO").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, _listColor2);
		}

		ss.str("");ss.clear();
		_lstInfo->addRow(2, tr("STR_CONSTRUCTION_TIME").c_str(), tr("STR_DAY", facility->getBuildTime()).c_str());
		_lstInfo->setCellColor(row++, 1, _listColor2);

		ss << Unicode::formatFunding(facility->getBuildCost());
		_lstInfo->addRow(2, tr("STR_CONSTRUCTION_COST").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(row++, 1, _listColor2);

		ss.str("");ss.clear();
		ss << Unicode::formatFunding(facility->getMonthlyCost());
		_lstInfo->addRow(2, tr("STR_MAINTENANCE_COST").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(row++, 1, _listColor2);

		centerAllSurfaces();
	}

	ArticleStateTFTDFacility::~ArticleStateTFTDFacility()
	{}

}
