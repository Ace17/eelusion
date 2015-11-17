#pragma once

#include <algorithm>

#include "engine/util.h"
#include "engine/scene.h"
#include "game/entity.h"
#include "game/models.h"
#include "game/sounds.h"

class Wheel : public Entity
{
public:
  Wheel()
  {
    dir = -1.0f;
  }

  virtual Actor getActor() const override
  {
    auto r = Actor(pos + Vector2f(0, -0.1), MDL_WHEEL);

    if(blinking)
      r.effect = EFFECT_BLINKING;

    r.action = 0;
    r.ratio = (time % 200) / 200.0f;

    if(dir > 0)
      r.scale.x = -r.scale.x;

    return r;
  }

  bool move(Vector2f delta)
  {
    auto nextPos = pos + delta;

    if(game->isSolid(nextPos + Vector2f(0.10, 0)))
      return false;

    if(game->isSolid(nextPos + Vector2f(0.60, 0)))
      return false;

    if(game->isSolid(nextPos + Vector2f(0.10, 0.80)))
      return false;

    if(game->isSolid(nextPos + Vector2f(0.60, 0.80)))
      return false;

    pos = nextPos;
    return true;
  }

  virtual void tick() override
  {
    ++time;

    vel.x = dir * 0.003;
    vel.y -= 0.00005; // gravity

    // horizontal move
    if(!move(Vector2f(vel.x, 0)))
      dir = -dir;

    // vertical move
    if(!move(Vector2f(0, vel.y)))
      vel.y = 0;

    blinking = max(0, blinking - 1);
  }

  virtual void onCollide(Entity*) override
  {
    blinking = 100;
  }

  Int time;
  float dir;
};

