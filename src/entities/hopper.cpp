// Copyright (C) 2018 - Sebastien Alaiwan
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#include "base/util.h"
#include "base/scene.h"

#include "entity.h"
#include "models.h"
#include "sounds.h"
#include "entities/explosion.h"
#include "entities/move.h"
#include "collision_groups.h"
#include "toggle.h" // decrement

#include <cstdlib> // rand

struct Hopper : Entity, Damageable
{
  Hopper()
  {
    vel = NullVector;
    dir = -1.0f;
    size = UnitSize;
    collisionGroup = CG_WALLS;
    collidesWith = CG_SOLIDPLAYER;
    Body::onCollision = [this] (Body* other) { onCollide(other); };
  }

  virtual void addActors(vector<Actor>& actors) const override
  {
    auto r = Actor { pos, MDL_HOPPER };

    r.scale = size;
    r.pos += Vector(-(r.scale.width - size.width) * 0.5, 0);

    if(blinking)
      r.effect = Effect::Blinking;

    r.action = 0;
    r.ratio = (time % 80) / 80.0f;

    if(dir > 0)
      r.scale.width = -r.scale.width;

    actors.push_back(r);
  }

  virtual void tick() override
  {
    ++time;

    vel.y -= 0.005; // gravity

    if(ground && time % 50 == 0 && rand() % 4 == 0)
    {
      vel.y = 0.13;
      ground = false;
    }

    if(ground)
      vel.x = 0;
    else
      vel.x = dir * 0.03;

    auto trace = slideMove(this, vel);

    if(!trace.horz)
      dir = -dir;

    if(!trace.vert)
    {
      ground = true;
      vel.y = 0;
    }

    decrement(blinking);
  }

  void onCollide(Body* other)
  {
    if(auto damageable = dynamic_cast<Damageable*>(other))
      damageable->onDamage(5);
  }

  virtual void onDamage(int amount) override
  {
    blinking = 100;
    life -= amount;

    game->playSound(SND_HURT_ENEMY);

    if(life < 0)
    {
      game->playSound(SND_DIE_ENEMY);
      dead = true;
    }
  }

  int life = 30;
  int time = 0;
  bool ground = false;
  float dir;
  Vector vel;
};

#include "entity_factory.h"
static auto const reg1 = registerEntity("hopper", [] (IEntityConfig*)  -> unique_ptr<Entity> { return make_unique<Hopper>(); });

