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
#include "ArticleStateTFTD.h"
#include "../Engine/Game.h"
#include "../Engine/Surface.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleInterface.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"

namespace OpenXcom
{

	ArticleStateTFTD::ArticleStateTFTD(ArticleDefinitionTFTD *defs, std::shared_ptr<ArticleCommonState> state) : ArticleState(defs->id, std::move(state))
	{
		RuleInterface *ruleInterface;
		std::string interfaceName;
		switch (defs->getType())
		{
			case UFOPAEDIA_TYPE_TFTD:
				interfaceName = "articleTFTD";
				break;
			case UFOPAEDIA_TYPE_TFTD_CRAFT:
				interfaceName = "articleCraftTFTD";
				break;
			case UFOPAEDIA_TYPE_TFTD_CRAFT_WEAPON:
				interfaceName = "articleCraftWeaponTFTD";
				break;
			case UFOPAEDIA_TYPE_TFTD_VEHICLE:
				interfaceName = "articleVehicleTFTD";
				break;
			case UFOPAEDIA_TYPE_TFTD_ITEM:
				interfaceName = "articleItemTFTD";
				break;
			case UFOPAEDIA_TYPE_TFTD_ARMOR:
				interfaceName = "articleArmorTFTD";
				break;
			case UFOPAEDIA_TYPE_TFTD_BASE_FACILITY:
				interfaceName = "articleBaseFacilityTFTD";
				break;
			case UFOPAEDIA_TYPE_TFTD_USO:
				interfaceName = "articleUsoTFTD";
				break;
			default:
				interfaceName = "articleTFTD";
				break;
		}
		ruleInterface = _game->getMod()->getInterface(interfaceName);

		// Set palette
		if (defs->customPalette)
		{
			if (ruleInterface->getPalette() == "PAL_GEOSCAPE")
				_cursorColor = Mod::GEOSCAPE_CURSOR;
			else if (ruleInterface->getPalette() == "PAL_BASESCAPE")
				_cursorColor = Mod::BASESCAPE_CURSOR;
			else if (ruleInterface->getPalette() == "PAL_UFOPAEDIA")
				_cursorColor = Mod::UFOPAEDIA_CURSOR;
			else if (ruleInterface->getPalette() == "PAL_GRAPHS")
				_cursorColor = Mod::GRAPHS_CURSOR;
			else
				_cursorColor = Mod::BATTLESCAPE_CURSOR;

			setCustomPalette(_game->getMod()->getSurface(defs->image_id)->getPalette(), _cursorColor);
		}
		else
		{
			setStandardPalette(ruleInterface->getPalette());
		}

		int buttonColor = ruleInterface->getElement("button")->color;
		_textColor = ruleInterface->getElement("text")->color;
		_textColor2 = ruleInterface->getElement("text")->color2;
		const Element* el = ruleInterface->getElementOptional("list");
		if (el)
		{
			_listColor1 = ruleInterface->getElement("list")->color;
			_listColor2 = ruleInterface->getElement("list")->color2;
		}
		_arrowColor = _listColor2;
		if (ruleInterface->getElementOptional("arrow"))
		{
			_arrowColor = ruleInterface->getElement("arrow")->color;
		}
		if (ruleInterface->getElementOptional("ammoColor"))
		{
			_ammoColor = ruleInterface->getElement("ammoColor")->color;
		}

		ArticleState::initLayout();

		RuleInterface* commonPart = _game->getMod()->getInterface("articleTFTD");
		_btnInfo->setX(commonPart->getElement("buttonInfo")->x);
		_btnInfo->setY(commonPart->getElement("buttonInfo")->y);
		_btnInfo->setHeight(commonPart->getElement("buttonInfo")->h);
		_btnInfo->setWidth(commonPart->getElement("buttonInfo")->w);
		_btnInfo->setColor(buttonColor);
		_btnOk->setX(commonPart->getElement("buttonOK")->x);
		_btnOk->setY(commonPart->getElement("buttonOK")->y);
		_btnOk->setHeight(commonPart->getElement("buttonOK")->h);
		_btnOk->setWidth(commonPart->getElement("buttonOK")->w);
		_btnOk->setColor(buttonColor);
		_btnPrev->setX(commonPart->getElement("buttonPrev")->x);
		_btnPrev->setY(commonPart->getElement("buttonPrev")->y);
		_btnPrev->setHeight(commonPart->getElement("buttonPrev")->h);
		_btnPrev->setWidth(commonPart->getElement("buttonPrev")->w);
		_btnPrev->setColor(buttonColor);
		_btnNext->setX(commonPart->getElement("buttonNext")->x);
		_btnNext->setY(commonPart->getElement("buttonNext")->y);
		_btnNext->setHeight(commonPart->getElement("buttonNext")->h);
		_btnNext->setWidth(commonPart->getElement("buttonNext")->w);
		_btnNext->setColor(buttonColor);

		// Step 1: background image
		auto& bgImageName = ruleInterface->getBackgroundImage(_game->getMod(), _game->getSavedGame());
		if (!defs->customPalette)
		{
			_game->getMod()->getSurface(bgImageName)->blitNShade(_bg, 0, 0);
		}

		// Step 2: article image (optional)
		Surface *image = _game->getMod()->getSurface(defs->image_id, false);
		if (image)
		{
			image->blitNShade(_bg, 0, 0);
		}

		// Step 3: info button image
		Surface *button = _game->getMod()->getSurface(bgImageName + "-InfoButton", false);
		if (!defs->customPalette && button && _game->getMod()->getShowPediaInfoButton())
		{
			switch (defs->getType())
			{
				case UFOPAEDIA_TYPE_TFTD_ITEM:
				case UFOPAEDIA_TYPE_TFTD_ARMOR:
				case UFOPAEDIA_TYPE_TFTD_BASE_FACILITY:
				case UFOPAEDIA_TYPE_TFTD_CRAFT:
				case UFOPAEDIA_TYPE_TFTD_CRAFT_WEAPON:
				case UFOPAEDIA_TYPE_TFTD_USO:
					button->blitNShade(_bg, 0, 0);
					break;
				default:
					break;
			}
		}

		_txtInfo = new Text(defs->text_width, 136, 320 - defs->text_width, 34);
		_txtTitle = new Text(284, 16, 36, 14);
		_txtTitle->setColor(_textColor);

		add(_txtTitle, "title", interfaceName, _bg);
		add(_txtInfo, "text", interfaceName, _bg);

		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setAlign(ALIGN_CENTER);
		_txtTitle->setText(tr(defs->getTitleForPage(_state->current_page)));

		int widthDiff = _txtInfo->getWidth() - defs->text_width;
		if (widthDiff)
		{
			_txtInfo->setX(_txtInfo->getX() + widthDiff);
			_txtInfo->setWidth(_txtInfo->getWidth() - widthDiff);
		}

		_txtInfo->setColor(_textColor);
		_txtInfo->setSecondaryColor(_textColor2);
		_txtInfo->setWordWrap(true);
		_txtInfo->setScrollable(true);
		_txtInfo->setText(tr(defs->getTextForPage(_state->current_page)));

		// all of the above are common to the TFTD articles.

		if (defs->getType() == UFOPAEDIA_TYPE_TFTD)
		{
			// this command is contained in all the subtypes of this article,
			// and probably shouldn't run until all surfaces are added.
			// in the case of a simple image/title/text article,
			// we're done adding surfaces for now.
			centerAllSurfaces();
		}

	}

	ArticleStateTFTD::~ArticleStateTFTD()
	{}

}
