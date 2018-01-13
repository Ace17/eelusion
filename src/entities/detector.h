/*
 * Copyright (C) 2017 - Sebastien Alaiwan
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 */

#pragma once

#include "base/util.h"
#include "base/scene.h"

#include "entity.h"
#include "models.h"
#include "sounds.h"
#include "toggle.h"
#include "collision_groups.h"

struct TouchDetectorEvent : Event
{
  TouchDetectorEvent(int whichOne_)
  {
    whichOne = whichOne_;
  }

  int whichOne;
};

struct Detector : Entity
{
  Detector(int id_)
  {
    id = id_;
    size = Size(0.1, 3);
    solid = false;
    collisionGroup = 0; // dont' trigger other detectors
    collidesWith = CG_PLAYER | CG_SOLIDPLAYER;
    Body::onCollision = [this] (Body* other) { onCollide(other); };
  }

  virtual Actor getActor() const override
  {
    auto r = Actor(pos, MDL_RECT);
    r.scale = size;
    return r;
  }

  virtual void tick() override
  {
    decrement(touchDelay);
  }

  void onCollide(Body*)
  {
    if(touchDelay)
      return;

    game->playSound(SND_SWITCH);
    game->postEvent(make_unique<TouchDetectorEvent>(id));
    touchDelay = 1000;
  }

  int id = 0;
  int touchDelay = 0;
};

struct RoomBoundaryDetector : Entity
{
  RoomBoundaryDetector()
  {
    size = Size(1, 1);
    solid = false;
    collisionGroup = 0;
    collidesWith = CG_PLAYER | CG_SOLIDPLAYER;
    Body::onCollision = [this] (Body* other) { onCollide(other); };
  }

  virtual Actor getActor() const override
  {
    auto r = Actor(pos, MDL_RECT);
    r.scale = size;
    return r;
  }

  void onCollide(Body*)
  {
    if(touched)
      return;

    game->postEvent(make_unique<TouchLevelBoundary>(targetLevel, transform));
    touched = true;
  }

  int targetLevel = 0;
  Vector transform;
  bool touched = false;
};

struct RoomBoundaryBlocker : Entity
{
  RoomBoundaryBlocker(int groupsToBlock)
  {
    size = Size(1, 1);
    solid = true;
    collisionGroup = CG_WALLS;
    collidesWith = groupsToBlock;
  }

  virtual Actor getActor() const override
  {
    auto r = Actor(pos, MDL_RECT);
    r.scale = size;
    r.effect = Effect::Blinking;
    return r;
  }
};

