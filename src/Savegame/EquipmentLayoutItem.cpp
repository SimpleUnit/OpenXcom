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
#include "EquipmentLayoutItem.h"
#include "../Mod/RuleInventory.h"
#include "../Engine/Collections.h"
#include "BattleItem.h"

namespace OpenXcom
{

/**
 * Initializes a new soldier-equipment layout item from YAML.
 * @param node YAML node.
 */
EquipmentLayoutItem::EquipmentLayoutItem(const YAML::Node &node)
{
	for (int slot = 0; slot < RuleItem::AmmoSlotMax; ++slot)
	{
		for (int chamberSpot = 0; chamberSpot < RuleItem::ChamberMax; ++chamberSpot)
		{
			_ammoItem[slot + chamberSpot * RuleItem::AmmoSlotMax] = "NONE";
		}
	}
	_attachment = nullptr;
	load(node);
}

/**
 * Initializes a new soldier-equipment layout item.
 * @param itemType Item's type.
 * @param slot Occupied slot's id.
 * @param slotX Position-X in the occupied slot.
 * @param slotY Position-Y in the occupied slot.
 * @param ammoItem The ammo has to be loaded into the item. (it's type)
 * @param fuseTimer The turn until explosion of the item. (if it's an activated grenade-type)
 */
EquipmentLayoutItem::EquipmentLayoutItem(const BattleItem* item) :
	_itemType(item->getRules()->getType()),
	_slotX(item->getSlotX()), _slotY(item->getSlotY()),
	_ammoItem{}, _fuseTimer(item->getFuseTimer()),
	_fixed(item->getRules()->isFixed())
{
	if (item->getSlot())
		_slot = item->getSlot()->getId();
	else
		_slot = "";

	for (int slot = 0; slot < RuleItem::AmmoSlotMax; ++slot)
	{
		if (item->needsAmmoForSlot(slot))
		{
			for (int chamberSpot = 0; chamberSpot < RuleItem::ChamberMax; ++chamberSpot)
			{
				const BattleItem *clip = item->getAmmoForSlot(slot, chamberSpot);
				if (clip)
					_ammoItem[slot + chamberSpot * RuleItem::AmmoSlotMax] = clip->getRules()->getType();
				else
					_ammoItem[slot + chamberSpot * RuleItem::AmmoSlotMax] = "NONE";
			}
		}
		else
		{
			for (int chamberSpot = 0; chamberSpot < RuleItem::ChamberMax; ++chamberSpot)
				_ammoItem[slot + chamberSpot * RuleItem::AmmoSlotMax] = "NONE";
		}
	}

	if (item->getAttachment())
	{
		_attachment = new EquipmentLayoutItem(item->getAttachment());
	}
	else
	{
		_attachment = nullptr;
	}
}

/**
 *
 */
EquipmentLayoutItem::~EquipmentLayoutItem()
{
}

/**
 * Returns the item's type which has to be in a slot.
 * @return item type.
 */
const std::string& EquipmentLayoutItem::getItemType() const
{
	return _itemType;
}

/**
 * Returns the slot to be occupied.
 * @return slot name.
 */
const std::string& EquipmentLayoutItem::getSlot() const
{
	return _slot;
}

/**
 * Returns the position-X in the slot to be occupied.
 * @return slot-X.
 */
int EquipmentLayoutItem::getSlotX() const
{
	return _slotX;
}

/**
 * Returns the position-Y in the slot to be occupied.
 * @return slot-Y.
 */
int EquipmentLayoutItem::getSlotY() const
{
	return _slotY;
}

/**
 * Returns the ammo has to be loaded into the item.
 * @return ammo type.
 */
const std::string& EquipmentLayoutItem::getAmmoItemForSlot(int slot, int chamberSpot) const
{
	return _ammoItem[slot + chamberSpot * RuleItem::AmmoSlotMax];
}

/**
 * Returns the turn until explosion of the item. (if it's an activated grenade-type)
 * @return turn count.
 */
int EquipmentLayoutItem::getFuseTimer() const
{
	return _fuseTimer;
}

/**
 * Is this a fixed weapon entry?
 * @return True, if this is a fixed weapon entry.
 */
bool EquipmentLayoutItem::isFixed() const
{
	return _fixed;
}

/**
 * Gets attachment layout
 * @return Attachment layout information.
 */
const EquipmentLayoutItem *EquipmentLayoutItem::getAttachment() const
{
	return _attachment;
}

/**
 * Loads the soldier-equipment layout item from a YAML file.
 * @param node YAML node.
 */
void EquipmentLayoutItem::load(const YAML::Node &node)
{
	_itemType = node["itemType"].as<std::string>(_itemType);
	_slot = node["slot"].as<std::string>(_slot);
	_slotX = node["slotX"].as<int>(0);
	_slotY = node["slotY"].as<int>(0);
	_ammoItem[0] = node["ammoItem"].as<std::string>(_ammoItem[0]);
	if (const YAML::Node &ammoSlots = node["ammoItemSlots"])
	{
		for (int slot = 0; slot < RuleItem::AmmoSlotMax; ++slot)
		{
			for (int chamberSpot = 0; chamberSpot < RuleItem::ChamberMax; ++chamberSpot)
			{
				if (ammoSlots[slot + chamberSpot * RuleItem::AmmoSlotMax])
				{
					_ammoItem[slot + chamberSpot * RuleItem::AmmoSlotMax] = ammoSlots[slot + chamberSpot * RuleItem::AmmoSlotMax].as<std::string>();
				}
			}
		}
	}


	_fuseTimer = node["fuseTimer"].as<int>(-1);
	_fixed = node["fixed"].as<bool>(false);

	if (const YAML::Node &attachment = node["attachment"])
	{
		if (_attachment != nullptr)
			delete _attachment;
		_attachment = new EquipmentLayoutItem(attachment);
	}
}

/**
 * Saves the soldier-equipment layout item to a YAML file.
 * @return YAML node.
 */
YAML::Node EquipmentLayoutItem::save() const
{
	YAML::Node node;
	node.SetStyle(YAML::EmitterStyle::Flow);
	node["itemType"] = _itemType;
	node["slot"] = _slot;
	// only save this info if it's needed, reduce clutter in saves
	if (_slotX != 0)
	{
		node["slotX"] = _slotX;
	}
	if (_slotY != 0)
	{
		node["slotY"] = _slotY;
	}
	if (_ammoItem[0] != "NONE")
	{
		node["ammoItem"] = _ammoItem[0];
	}

	Collections::untilLastIf(
		_ammoItem,
		[](const std::string &s)
		{
			return s != "NONE";
		},
		[&](const std::string &s)
		{
			node["ammoItemSlots"].push_back(s);
		});
	if (_fuseTimer >= 0)
	{
		node["fuseTimer"] = _fuseTimer;
	}
	if (_fixed)
	{
		node["fixed"] = _fixed;
	}
	if (_attachment)
		node["attachment"] = _attachment->save();
	return node;
}

}
