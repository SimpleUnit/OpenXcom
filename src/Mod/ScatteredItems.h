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

#include <yaml-cpp/yaml.h>
#include <SDL_types.h>
#include <string>

namespace OpenXcom
{
struct ScatteredItems
{
	std::string itemId;
	int amount, randomAmount;
	ScatteredItems() : itemId(), amount(0), randomAmount(0){};
};
}

namespace YAML
{
template <>
struct convert<OpenXcom::ScatteredItems>
{
	static Node encode(const OpenXcom::ScatteredItems &rhs)
	{
		Node node;
		node["itemId"] = rhs.itemId;
		node["amount"] = rhs.amount;
		node["randomAmount"] = rhs.randomAmount;
		return node;
	}

	static bool decode(const Node &node, OpenXcom::ScatteredItems &rhs)
	{
		if (!node.IsMap())
			return false;

		rhs.itemId = node["itemId"].as<std::string>(rhs.itemId);
		rhs.amount = node["amount"].as<int>(rhs.amount);
		rhs.randomAmount = node["randomAmount"].as<int>(rhs.randomAmount);
		return true;
	}
};
}
