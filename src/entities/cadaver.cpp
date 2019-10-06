// Copyright (C) 2019 - Sebastien Alaiwan
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#include "base/scene.h"
#include "base/util.h"

#include "collision_groups.h"
#include "entities/player.h"
#include "entities/move.h"
#include "entity.h"
#include "models.h"
#include "sounds.h"
#include "toggle.h"

#include "rockman.h" // ACTION_CADAVER

struct Cadaver : Entity
{
  Cadaver()
  {
    size = Size2f(3, 0.5);

    Body::onCollision = [this] (Body* other)
      {
        if(counter == 0)
        {
          if(auto resurrectable = dynamic_cast<Resurrectable*>(other))
            resurrectable->resurrect();

          counter = 1;
        }
      };
  }

  virtual void addActors(vector<Actor>& actors) const override
  {
    auto r = Actor { pos, MDL_ROCKMAN };
    r.scale = Size(3, 3);
    r.action = ACTION_CADAVER;

    if(counter > 0)
      r.effect = Effect::Blinking;

    actors.push_back(r);
  }

  void tick() override
  {
    if(counter > 0)
      counter++;

    if(counter >= 100)
      dead = true;
  }

  int counter = 0;
};

#include "entity_factory.h"

static unique_ptr<Entity> makeCadaver(IEntityConfig* cfg)
{
  (void)cfg;
  return make_unique<Cadaver>();
}

static auto const reg1 = registerEntity("cadaver", &makeCadaver);

