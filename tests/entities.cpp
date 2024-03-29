/*
 * Copyright (C) 2017 - Sebastien Alaiwan
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 */

#include <algorithm>
#include <cmath>

#include "entities/bonus.h"
#include "entities/explosion.h"

#include "engine/tests/tests.h"

template<typename T>
Actor getActor(T& ent)
{
  vector<Actor> actors;
  actors.clear();
  ent->addActors(actors);
  return actors[0];
}

unittest("Entity: explosion")
{
  auto explosion = makeExplosion();

  assert(!explosion->dead);
  assertEquals(0, getActor(explosion).ratio);

  for(int i = 0; i < 10000; ++i)
    explosion->tick();

  assert(explosion->dead);
  assertEquals(100, int(getActor(explosion).ratio * 100));
}

#include "entities/player.h"

struct NullPlayer : Player
{
  virtual void think(Control const &) {}
  virtual float health() { return 0; }
  virtual void addUpgrade(int) {}
  virtual void addActors(vector<Actor> &) const {}
};

struct NullVariable : IVariable
{
  int get() { return 0; };
  void set(int) {};
  unique_ptr<Handle> observe(Observer) { return nullptr; };
};

static NullVariable nullVariable;

struct NullGame : IGame
{
  virtual void playSound(SOUND) {}
  virtual void stopMusic() {};
  virtual void spawn(Entity*) {}
  virtual IVariable* getVariable(int) { return &nullVariable; }
  virtual void postEvent(unique_ptr<Event> ) {}
  virtual Vector2f getPlayerPosition() { return Vector2f(0, 0); }
  virtual void textBox(char const*) {}
  virtual void setAmbientLight(float) {}
  virtual void respawn() {};
};

struct NullPhysicsProbe : IPhysicsProbe
{
  // called by entities
  bool moveBody(Body* body, Vector2f delta)
  {
    auto rect = body->getFBox();
    rect.pos.x += delta.x;
    rect.pos.y += delta.y;

    if(isSolid(body, roundBox(rect)))
      return false;

    body->pos += delta;
    return true;
  }

  bool isSolid(const Body* /*body*/, IntBox rect) const
  {
    return rect.pos.y < 0;
  }

  Body* getBodiesInBox(IntBox, int, bool, const Body*) const
  {
    return nullptr;
  }
};

float g_AmbientLight = 0;

unittest("Entity: pickup bonus")
{
  struct MockPlayer : NullPlayer
  {
    virtual void addUpgrade(int upgrade)
    {
      upgrades |= upgrade;
    }

    int upgrades = 0;
  };

  NullGame game;
  NullPhysicsProbe physics;
  MockPlayer player;

  auto ent = makeBonus(0, 4, "cool text");
  ent->game = &game;
  ent->physics = &physics;

  assert(!ent->dead);
  assertEquals(0, player.upgrades);

  ent->onCollision(&player);

  assert(ent->dead);
  assertEquals(4, player.upgrades);
}

bool nearlyEquals(float expected, float actual)
{
  return abs(expected - actual) < 0.01;
}

unittest("Entity: animate")
{
  auto ent = makeBonus(0, 4, "hello");

  float minVal = 10.0f;
  float maxVal = -10.0f;

  for(int i = 0; i < 1000; ++i)
  {
    auto actor = getActor(ent);
    minVal = min(minVal, actor.ratio);
    maxVal = max(maxVal, actor.ratio);
    ent->tick();
  }

  assert(nearlyEquals(0, minVal));
  assert(nearlyEquals(1, maxVal));
}

