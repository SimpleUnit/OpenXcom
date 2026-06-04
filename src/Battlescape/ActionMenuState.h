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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#include "../Engine/State.h"
#include "BattlescapeGame.h"
#include "BattlescapeState.h"

#define ACTIONMENUSIZE 7

namespace OpenXcom
{

class ActionMenuItem;

/**
 * Window that allows the player
 * to select a battlescape action.
 */
class ActionMenuState : public State
{
protected:
	BattleAction *_action;
	ActionMenuItem *_actionMenu[ACTIONMENUSIZE];
	BattlescapeState *_parent;
	bool _parentIsRightHand;
	/// Adds a new menu item for an action.
	void addItem(BattleActionType ba, const std::string &name, int *id, SDLKey key);
	/// Acts on the action instance that has been chosen and set.
	void handleAction();
public:
	/// Default constructor, used by SkillMenuState.
	ActionMenuState(BattleAction *action, BattlescapeState *parent, bool rightHand);
	/// Creates the Action Menu state.
	ActionMenuState(BattleAction *action, int x, int y, BattlescapeState *parent, bool rightHand);
	/// Cleans up the Action Menu state.
	~ActionMenuState();
	/// Init function.
	void init() override;
	/// Handler for right-clicking anything.
	void handle(Action *action) override;
	/// Handler for clicking a action menu item.
	virtual void btnActionMenuItemClick(Action *action);
	/// Separate handler for selecting attachment access menu item.
	virtual void btnAttachmentToggleClick(Action *action);
	/// Update the resolution settings, we just resized the window.
	void resize(int &dX, int &dY) override;
};

}
