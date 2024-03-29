// Copyright (C) 2018 - Sebastien Alaiwan
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#pragma once

#include "entity.h"

struct Player : Entity
{
  virtual void think(Control const& s) = 0;
  virtual float health() = 0;
  virtual void addUpgrade(int upgrade) = 0;
  virtual bool hasKey() { return false; }
};

enum
{
  UPGRADE_WHIP = 1,
  UPGRADE_CLIMB = 2,
  UPGRADE_GHOST = 4,
  UPGRADE_KEY = 8,
  UPGRADE_BALL = 16,
  UPGRADE_SLIDE = 32,
  UPGRADE_BODY = 64,
};

