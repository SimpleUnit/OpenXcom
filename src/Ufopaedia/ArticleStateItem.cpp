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
#include <algorithm>
#include "Ufopaedia.h"
#include "ArticleStateItem.h"
#include "../Mod/Mod.h"
#include "../Mod/ArticleDefinition.h"
#include "../Mod/RuleItem.h"
#include "../Engine/Game.h"
#include "../Engine/Surface.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Unicode.h"
#include "../Interface/NumberText.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"
#include "../Mod/RuleInterface.h"
#include "../fmath.h"
#include "StatsForNerdsState.h"

namespace OpenXcom
{

	ArticleStateItem::ArticleStateItem(ArticleDefinitionItem *defs, std::shared_ptr<ArticleCommonState> state) : ArticleState(defs->id, std::move(state))
	{
		RuleItem *item = _game->getMod()->getItem(defs->weapon, false);
		if (!item)
		{
			item = _game->getMod()->getItem(defs->id, true);
		}
		RuleInterface* itf = _game->getMod()->getInterface("articleItem");

		int bottomOffset = 20;
		std::string accuracyModifier;
		bool isAccurracyModded = false;
		if (item->getBattleType() == BT_MELEE)
		{
			accuracyModifier = addRuleStatBonus(*item->getMeleeMultiplierRaw());
			isAccurracyModded = item->getMeleeMultiplierRaw()->isModded();
		}
		else
		{
			accuracyModifier = addRuleStatBonus(*item->getAccuracyMultiplierRaw());
			isAccurracyModded = item->getAccuracyMultiplierRaw()->isModded();
		}
		std::string powerBonus = addRuleStatBonus(*item->getDamageBonusRaw());

		if (!isAccurracyModded)
		{
			// don't show default accuracy multiplier
			bottomOffset = 9;
		}

		if (_game->getMod()->getExtraNerdyPediaInfoType() == 0)
		{
			// feature turned off
			bottomOffset = 0;
		}
		else if (item->getBattleType() == BT_AMMO)
		{
			// don't show accuracy multiplier... even if someone mods it by mistake, it still makes no sense
			bottomOffset = 9;
		}
		else if (item->getBattleType() == BT_FIREARM || item->getBattleType() == BT_MELEE)
		{
			if (item->getClipSize() != 0 || item->getIgnoreAmmoPower())
			{
				// correct info loaded already... weapon has built-in ammo or ignores ammo power bonuses
			}
			else
			{
				// need to check power bonus on all compatible ammo
				bool first = true;
				bool allSame = true;
				for (int slot = 0; slot < RuleItem::AmmoSlotMax; ++slot)
				{
					for (auto* ammoItemRule : *item->getCompatibleAmmoForSlot(slot))
					{
						if (first)
						{
							powerBonus = addRuleStatBonus(*ammoItemRule->getDamageBonusRaw());
							first = false;
						}
						else
						{
							std::string otherPowerBonus = addRuleStatBonus(*ammoItemRule->getDamageBonusRaw());
							if (powerBonus != otherPowerBonus)
							{
								allSame = false;
								powerBonus = tr("STR_MULTIPLE_DIFFERENT_BONUSES");
							}
						}
						if (!allSame) break;
					}
					if (!allSame) break;
				}
			}
		}
		else if (item->getBattleType() == BT_PSIAMP)
		{
			// correct info loaded already
		}
		else
		{
			// nothing to show for other item types
			bottomOffset = 0;
		}

		if (powerBonus.empty())
		{
			if (!isAccurracyModded)
			{
				// both power bonus and accuracy multiplier are vanilla, hide info completely
				bottomOffset = 0;
			}
			else
			{
				// display zero instead of empty string
				powerBonus = "0";
			}
		}

		// add screen elements
		_txtTitle = new Text(148, 32, 5, 24);
		_txtWeight = new Text(88, 8, 104, 55);

		int arrowColor = itf->getElement("button")->color;
		if (itf->getElementOptional("arrow"))
		{
			arrowColor = itf->getElement("arrow")->color;
		}
		int textColor = itf->getElement("text")->color;
		int textColor2 = itf->getElement("text")->color2;
		int listColor1 = itf->getElement("list")->color;
		int listColor2 = itf->getElement("list")->color2;
		int ammoColor = itf->getElement("ammoColor")->color;

		ArticleState::initPaletteBg(defs, itf, "BACK08.SCR", "PAL_BATTLEPEDIA");
		ArticleState::initLayout();
		ArticleState::initButtons(itf->getElement("button")->color);

		// add other elements
		_txtTitle->setColor(itf->getElement("text")->color);
		add(_txtTitle, "title", "articleItem", _bg);
		add(_txtWeight, "weightText", "articleItem", _bg);

		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(tr(defs->getTitleForPage(_state->current_page)));

		_txtWeight->setColor(textColor);
		_txtWeight->setAlign(ALIGN_RIGHT);

		// IMAGE
		_image = new Surface(32, 48, 157, 5);
		add(_image, "image", "articleItem", _bg);

		item->drawHandSprite(_game->getMod()->getSurfaceSet("BIGOBS.PCK"), _image);

		_txtWeaponClipSize = new NumberText(30, 5, 157, 5);
		add(_txtWeaponClipSize, "image", "articleItem", _bg);

		_txtWeaponClipSize->setX(_txtWeaponClipSize->getX() + 2);
		_txtWeaponClipSize->setY(_txtWeaponClipSize->getY() + 2);
		_txtWeaponClipSize->setColor(textColor);
		_txtWeaponClipSize->setValue(item->getClipSize());
		_txtWeaponClipSize->setVisible(Options::oxcePediaShowClipSize && item->getClipSize() > 0);

		int ammoSlot = defs->getAmmoSlotForPage(_state->current_page);
		int ammoSlotPrevUsage = defs->getAmmoSlotPrevUsageForPage(_state->current_page);
		const std::vector<const RuleItem*> dummy;
		const std::vector<const RuleItem*> *ammo_data = ammoSlot != RuleItem::AmmoSlotSelfUse ? item->getCompatibleAmmoForSlot(ammoSlot) : &dummy;

		int weight = item->getWeight();
		std::string weightLabel = tr("STR_WEIGHT_PEDIA1").arg(weight);
		if (!ammo_data->empty())
		{
			// Note: weight including primary ammo only!
			const RuleItem *ammo_rule = (*ammo_data)[0];
			weightLabel = tr("STR_WEIGHT_PEDIA2").arg(weight).arg(weight + ammo_rule->getWeight() * item->getChamberSize(0));
		}
		_txtWeight->setText(weight != 0 ? weightLabel : "");

		// SHOT STATS TABLE (for firearms and melee only)
		if (item->getBattleType() == BT_FIREARM || item->getBattleType() == BT_MELEE)
		{
			_txtShotType = new Text(100, 17, 8, 66);
			_txtShotType->setColor(textColor);
			add(_txtShotType, "shotTypeLabel", "articleItem", _bg);
			_txtShotType->setWordWrap(true);
			_txtShotType->setText(tr("STR_SHOT_TYPE"));

			_txtAccuracy = new Text(50, 17, 104, 66);
			_txtAccuracy->setColor(textColor);
			add(_txtAccuracy, "accuracyLabel", "articleItem", _bg);
			_txtAccuracy->setWordWrap(true);
			_txtAccuracy->setText(tr("STR_ACCURACY_UC"));

			_txtTuCost = new Text(60, 17, 158, 66);
			_txtTuCost->setColor(textColor);
			add(_txtTuCost, "tuCostLabel", "articleItem", _bg);
			_txtTuCost->setWordWrap(true);
			_txtTuCost->setText(tr("STR_TIME_UNIT_COST"));

			_lstInfo = new TextList(204, 55, 8, 82);
			add(_lstInfo, "list", "articleItem", _bg);

			_lstInfo->setColor(listColor2); // color for % data!
			int actionNameWidth = itf->getElement("list")->custom;
			_lstInfo->setColumns(3, actionNameWidth, (_lstInfo->getWidth() - actionNameWidth) / 2, (_lstInfo->getWidth() - actionNameWidth) / 2);
			if (itf->getElement("list")->TFTDMode)
				_lstInfo->setSmall();
			else
				_lstInfo->setBig();
		}

		auto addAttack = [&](int& row, const std::string& name, std::pair<RuleItemUseCost, RuleItemUseFlat> costs, const RuleItemAction *config, const RuleItem *weapon)
		{
			if (row < 3 && costs.first.Time > 0 && config->ammoSlot == ammoSlot)
			{
				std::string tu = Unicode::formatPercentage(costs.first.Time);
				if (costs.second.Time)
				{
					tu.erase(tu.end() - 1);
				}
				int range = std::min(config->range, weapon->getMaxRange());
				std::string label = config->shortName.empty() ? tr(name).arg(config->shots).arg(range) : tr(config->shortName).arg(config->shots).arg(range);
				_lstInfo->addRow(3,
					label.c_str(),
					Unicode::formatPercentage(config->accuracy).c_str(),
					tu.c_str());
				_lstInfo->setCellColor(row, 0, listColor1);
				row++;
			}
		};

		int current_row = 0;
		if (item->getBattleType() == BT_FIREARM)
		{
			addAttack(current_row, "STR_SHOT_TYPE_AUTO", item->getCostsAction(BA_AUTOSHOT, nullptr, nullptr), item->getConfigAuto(), item);

			addAttack(current_row, "STR_SHOT_TYPE_SNAP", item->getCostsAction(BA_SNAPSHOT, nullptr, nullptr), item->getConfigSnap(), item);

			addAttack(current_row, "STR_SHOT_TYPE_AIMED", item->getCostsAction(BA_AIMEDSHOT, nullptr, nullptr), item->getConfigAimed(), item);

			//optional melee
			addAttack(current_row, "STR_SHOT_TYPE_MELEE", item->getCostsAction(BA_HIT, nullptr, nullptr), item->getConfigMelee(), item);

			// text_info is BELOW the info table (table can have 0-3 rows)
			int shift = (3 - current_row) * 16;
			if (ammo_data->size() == 2 && current_row <= 1)
			{
				shift -= (2 - current_row) * 16;
			}
			_txtInfo = new Text((ammo_data->size()<3 ? 300 : 180), 56 + shift - bottomOffset, 8, 138 - shift);
		}
		else if (item->getBattleType() == BT_MELEE)
		{
			addAttack(current_row, "STR_SHOT_TYPE_MELEE", item->getCostsAction(BA_HIT, nullptr, nullptr), item->getConfigMelee(), item);

			// text_info is BELOW the info table (with 1 row only)
			_txtInfo = new Text(300, 88 - bottomOffset, 8, 106);
		}
		else
		{
			// text_info is larger and starts on top
			_txtInfo = new Text(300, 125 - bottomOffset, 8, 67);
		}

		std::string elementID;
		if (current_row || ammo_data->size())
		{
			elementID = "textAB";
			elementID[4] = '0' + current_row;
			elementID[5] = '0' + Clamp((int)ammo_data->size(), 0, 3);
		}
		else
		{
			elementID = "text";
		}

		add(_txtInfo, elementID, "articleItem", _bg);
		_txtInfo->setColor(textColor);
		_txtInfo->setSecondaryColor(textColor2);
		_txtInfo->setWordWrap(true);
		_txtInfo->setScrollable(true);
		_txtInfo->setText(tr(defs->getTextForPage(_state->current_page)));

		Element elementOffset;
		if (bottomOffset == 9)
		{
			const Element* el = itf->getElementOptional("bottomOffset1");
			if (el)
				elementOffset = *el;
		}
		else if (bottomOffset == 20)
		{
			const Element* el = itf->getElementOptional("bottomOffset2");
			if (el)
				elementOffset = *el;

		}
		if (elementOffset.x != INT_MAX)
			_txtInfo->setX(_txtInfo->getX() + elementOffset.x);
		if (elementOffset.y != INT_MAX)
			_txtInfo->setY(_txtInfo->getY() + elementOffset.y);
		if (elementOffset.w != INT_MAX)
			_txtInfo->setWidth(_txtInfo->getWidth() + elementOffset.w);
		if (elementOffset.h != INT_MAX)
			_txtInfo->setHeight(_txtInfo->getHeight() + elementOffset.h);

		// STATS FOR NERDS extract
		_txtAccuracyModifier = new Text(300, 9, 8, 174);
		_txtPowerBonus = new Text(300, 17, 8, 183);

		add(_txtAccuracyModifier, "accuracyModifier", "articleItem", _bg);
		add(_txtPowerBonus, "powerModifier", "articleItem", _bg);

		_txtAccuracyModifier->setColor(textColor);
		_txtAccuracyModifier->setSecondaryColor(textColor2);
		_txtAccuracyModifier->setWordWrap(false);
		_txtAccuracyModifier->setText(tr("STR_ACCURACY_MODIFIER").arg(accuracyModifier));
		_txtAccuracyModifier->setVisible(bottomOffset >= 20);

		_txtPowerBonus->setColor(textColor);
		_txtPowerBonus->setSecondaryColor(textColor2);
		_txtPowerBonus->setWordWrap(true);
		_txtPowerBonus->setText(tr("STR_POWER_BONUS").arg(powerBonus));
		_txtPowerBonus->setVisible(bottomOffset > 0);

		// AMMO column
		std::ostringstream ss;

		for (int i = 0; i<3; ++i)
		{
			ss.str("");
			ss.clear();
			ss << "ammoType" << i+1;
			_txtAmmoType[i] = new Text(82, 16, 194, 20 + i*49);
			add(_txtAmmoType[i], ss.str(), "articleItem", _bg);
			_txtAmmoType[i]->setColor(textColor);
			_txtAmmoType[i]->setSecondaryColor(textColor2);
			_txtAmmoType[i]->setAlign(ALIGN_CENTER);
			_txtAmmoType[i]->setVerticalAlign(ALIGN_MIDDLE);
			_txtAmmoType[i]->setWordWrap(true);

			ss.str("");
			ss.clear();
			ss << "ammoDamage" << i+1;
			_txtAmmoDamage[i] = new Text(82, 17, 194, 40 + i*49);
			add(_txtAmmoDamage[i], ss.str(), "articleItem", _bg);
			_txtAmmoDamage[i]->setColor(ammoColor);
			_txtAmmoDamage[i]->setAlign(ALIGN_CENTER);
			_txtAmmoDamage[i]->setBig();

			ss.str("");
			ss.clear();
			ss << "ammoImage" << i+1;
			_imageAmmo[i] = new Surface(32, 48, 280, 16 + i*49);
			add(_imageAmmo[i], ss.str(), "articleItem", _bg);

			_txtAmmoClipSize[i] = new NumberText(30, 5, 2 + 280, 2 + 16 + i*49);
			add(_txtAmmoClipSize[i], "powerBonus", "articleItem", _bg);
			_txtAmmoClipSize[i]->setX(_imageAmmo[i]->getX() + 2);
			_txtAmmoClipSize[i]->setY(_imageAmmo[i]->getY() + 2);
			_txtAmmoClipSize[i]->setColor(textColor);
			_txtAmmoClipSize[i]->setSecondaryColor(textColor2);
			_txtAmmoClipSize[i]->setVisible(false);
		}

		auto addAmmoDamagePower = [&](int pos, const RuleItem *rule, const RuleItem* weaponRule)
		{
			_txtAmmoType[pos]->setText(tr(getDamageTypeText(rule->getDamageType()->ResistType)));

			ss.str("");ss.clear();
			if (weaponRule->getIgnoreAmmoPower())
			{
				ss << weaponRule->getPower();
			}
			else
			{
				ss << rule->getPower();
			}
			if (rule->getShotgunPellets())
			{
				ss << "x" << rule->getShotgunPellets();
			}
			_txtAmmoDamage[pos]->setText(ss.str());
			_txtAmmoDamage[pos]->setColor(getDamageTypeTextColor(rule->getDamageType()->ResistType, ammoColor));
		};

		switch (item->getBattleType())
		{
			case BT_FIREARM:
				if (item->getHidePower()) break;
				_txtDamage = new Text(82, 10, 194, 7);
				add(_txtDamage, "damageLabel", "articleItem", _bg);
				_txtDamage->setColor(textColor);
				_txtDamage->setSecondaryColor(textColor2);
				_txtDamage->setAlign(ALIGN_CENTER);
				_txtDamage->setText(tr("STR_DAMAGE_UC"));

				_txtAmmo = new Text(50, 10, 268, 7);
				add(_txtAmmo, "ammoLabel", "articleItem", _bg);
				_txtAmmo->setColor(textColor);
				_txtAmmo->setSecondaryColor(textColor2);
				_txtAmmo->setAlign(ALIGN_CENTER);
				_txtAmmo->setText(tr("STR_AMMO"));

				if (ammo_data->empty())
				{
					addAmmoDamagePower(0, item, item);
				}
				else
				{
					int maxShow = 3;
					int skipShow = maxShow * ammoSlotPrevUsage;
					int currShow = 0;
					for (auto* type : *ammo_data)
					{
						ArticleDefinition *ammo_article = _game->getMod()->getUfopaediaArticle(type->getType(), true);
						if (Ufopaedia::isArticleAvailable(_game->getSavedGame(), ammo_article))
						{
							if (skipShow > 0)
							{
								--skipShow;
								continue;
							}

							addAmmoDamagePower(currShow, type, item);

							type->drawHandSprite(_game->getMod()->getSurfaceSet("BIGOBS.PCK"), _imageAmmo[currShow]);
							_txtAmmoClipSize[currShow]->setValue(type->getClipSize());
							_txtAmmoClipSize[currShow]->setVisible(Options::oxcePediaShowClipSize && type->getClipSize() > 0);

							++currShow;
							if (currShow == maxShow)
							{
								break;
							}
						}
					}
				}
				break;
			case BT_AMMO:
			case BT_GRENADE:
			case BT_PROXIMITYGRENADE:
			case BT_ANOMALY:
			case BT_MELEE:
				if (item->getHidePower()) break;
				_txtDamage = new Text(82, 10, 194, 7);
				add(_txtDamage, "damageLabel", "articleItem", _bg);
				_txtDamage->setColor(textColor);
				_txtDamage->setSecondaryColor(textColor2);
				_txtDamage->setAlign(ALIGN_CENTER);
				_txtDamage->setText(tr("STR_DAMAGE_UC"));

				addAmmoDamagePower(0, item, item);
				break;
			default: break;
		}

		// multi-page indicator
		_txtArrows = new Text(32, 9, 280, 183);
		add(_txtArrows, "arrow", "articleItem", _bg);
		_txtArrows->setColor(arrowColor);
		_txtArrows->setAlign(ALIGN_RIGHT);
		std::ostringstream ss2;
		if (_state->hasPrevArticlePage()) ss2 << "<<";
		if (_state->hasNextArticlePage()) ss2 << " >>";
		_txtArrows->setText(ss2.str());

		centerAllSurfaces();
	}

	ArticleStateItem::~ArticleStateItem()
	{}

	std::string ArticleStateItem::addRuleStatBonus(const RuleStatBonus &value)
	{
		std::ostringstream ss;
		bool isFirst = true;
		for (const auto& item : *value.getBonusRaw())
		{
			int power = 0;
			for (float number : item.second)
			{
				++power;
				if (!AreSame(number, 0.0f))
				{
					float numberAbs = number;
					if (!isFirst)
					{
						if (number > 0.0f)
						{
							ss << " + ";
						}
						else
						{
							ss << " - ";
							numberAbs = std::abs(number);
						}
					}
					if (item.first == "flatOne")
					{
						ss << numberAbs * 1;
					}
					if (item.first == "flatHundred")
					{
						ss << numberAbs * pow(100, power);
					}
					else
					{
						if (!AreSame(numberAbs, 1.0f))
						{
							ss << numberAbs << "*";
						}

						if (_game->getMod()->getExtraNerdyPediaInfoType() > 1)
						{
							ss << tr(StatsForNerdsState::shortTranslationMap.at(item.first));
						}
						else
						{
							ss << tr(StatsForNerdsState::translationMap.at(item.first));
						}
						if (power > 1)
						{
							ss << "^" << power;
						}
					}
					isFirst = false;
				}
			}
		}
		return ss.str();
	}

	int ArticleStateItem::getDamageTypeTextColor(ItemDamageType dt, int ammoColor)
	{
		const Element *interfaceElement = 0;

		switch (dt)
		{
			case DT_NONE:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDTNone");
				break;

			case DT_AP:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDTAP");
				break;

			case DT_IN:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDTIN");
				break;

			case DT_HE:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDTHE");
				break;

			case DT_LASER:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDTLaser");
				break;

			case DT_PLASMA:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDTPlasma");
				break;

			case DT_STUN:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDTStun");
				break;

			case DT_MELEE:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDTMelee");
				break;

			case DT_ACID:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDTAcid");
				break;

			case DT_SMOKE:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDTSmoke");
				break;

			case DT_10:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDT10");
				break;

			case DT_11:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDT11");
				break;

			case DT_12:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDT12");
				break;

			case DT_13:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDT13");
				break;

			case DT_14:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDT14");
				break;

			case DT_15:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDT15");
				break;

			case DT_16:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDT16");
				break;

			case DT_17:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDT17");
				break;

			case DT_18:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDT18");
				break;

			case DT_19:
				interfaceElement = _game->getMod()->getInterface("articleItem")->getElementOptional("ammoColorDT19");
				break;

			default :
				break;
		}

		if (interfaceElement)
		{
			ammoColor = interfaceElement->color;
		}

		return ammoColor;
	}
}
