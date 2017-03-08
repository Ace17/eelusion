#pragma once

#include "base/util.h"
#include "base/scene.h"
#include "game/entity.h"
#include "game/models.h"

struct Spikes : Entity
{
  Spikes()
  {
    size = Size2f(1, 1);
    solid = 0;
    collisionGroup = (1 << 1);
    collidesWith = 1; // only the player
  }

  virtual Actor getActor() const override
  {
    auto r = Actor(pos, MDL_SPIKES);
    r.scale = Vector2f(size.width, size.height);
    r.action = 0;
    r.ratio = 0;

    return r;
  }

  void onCollide(Entity* other) override
  {
    other->onDamage(1000);
  }
};

