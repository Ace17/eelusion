/**
 * @author Sebastien Alaiwan
 * @date 2015-11-16
 */

/*
 * Copyright (C) 2015 - Sebastien Alaiwan
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 */

#include <algorithm>

#include "engine/scene.h"
#include "engine/util.h"

#include "game/entities/player.h"
#include "game/entity.h"
#include "game/models.h"
#include "game/sounds.h"
#include "game/toggle.h"

auto const WALK_SPEED = 0.0075f;
auto const MAX_HORZ_SPEED = 0.02f;
auto const MAX_FALL_SPEED = 0.02f;
auto const CLIMB_DELAY = 100;
auto const HURT_DELAY = 500;

enum ACTION
{
  ACTION_VICTORY,
  ACTION_STAND,
  ACTION_STAND_SHOOT,
  ACTION_ENTRANCE,
  ACTION_WALK,
  ACTION_WALK_SHOOT,
  ACTION_DASH,
  ACTION_DASH_AIM,
  ACTION_JUMP,
  ACTION_FALL,
  ACTION_LADDER,
  ACTION_LADDER_END,
  ACTION_LADDER_SHOOT,
  ACTION_HADOKEN,
  ACTION_SLIDE,
  ACTION_SLIDE_SHOOT,
  ACTION_CLIMB,
  ACTION_HURT,
  ACTION_FULL,
};

enum ORIENTATION
{
  LEFT,
  RIGHT,
};

class Rockman : public Player
{
public:
  Rockman() : dir(RIGHT)
  {
    size = Size2f(0.9, 1.9);
    life = 31;
  }

  virtual Actor getActor() const override
  {
    auto r = Actor(pos, MDL_ROCKMAN);
    r.scale = Vector2f(3, 3);

    // re-center
    r.pos += Vector2f(-(r.scale.x - size.width) * 0.5, -0.1);

    if(hurtDelay || life < 0)
    {
      r.action = ACTION_HURT;
      r.ratio = 1.0f - hurtDelay / float(HURT_DELAY);
    }
    else
    {
      if(!ground)
      {
        if(climbDelay)
        {
          r.action = ACTION_CLIMB;
          r.ratio = 1.0f - climbDelay / float(CLIMB_DELAY);
          r.scale.x *= -1;
        }
        else
        {
          r.action = ACTION_FALL;
          r.ratio = vel.y > 0 ? 0 : 1;
        }
      }
      else
      {
        if(vel.x != 0)
        {
          r.ratio = (time % 500) / 500.0f;
          r.action = ACTION_WALK;
        }
        else
        {
          r.ratio = (time % 1000) / 1000.0f;
          r.action = ACTION_STAND;
        }
      }
    }

    if(dir == LEFT)
      r.scale.x *= -1;

    if(blinking)
      r.effect = EFFECT_BLINKING;

    return r;
  }

  void think(Control const& c) override
  {
    time++;
    computeVelocity(c);

    // horizontal move
    move(Vector2f(vel.x, 0));

    // vertical move

    if(move(Vector2f(0, vel.y)))
    {
      ground = false;
    }
    else
    {
      if(vel.y < 0 && !ground)
      {
        if(tryActivate(debounceLanding, 150))
          game->playSound(SND_LAND);

        ground = true;
      }

      vel.y = 0;
    }

    decrement(debounceFire);
    decrement(debounceLanding);
    decrement(climbDelay);

    if(firebutton.toggle(c.fire) && tryActivate(debounceFire, 150))
    {
      /*
         auto b = unique(new Bullet);
         b->pos = pos + Vector2f(0, 9);
         b->vel = Vector2f(0.0, BULLET_SPEED);
         game->spawn(b.release());
       */
      game->playSound(SND_BEEP);
    }
  }

  float health() override
  {
    return clamp(life / 31.0f, 0.0f, 1.0f);
  }

  void computeVelocity(Control c)
  {
    if(hurtDelay || life <= 0)
      c = Control();

    airMove(c);

    if(vel.x > 0)
      dir = RIGHT;

    if(vel.x < 0)
      dir = LEFT;

    // gravity
    vel.y -= 0.00005;

    if(jumpbutton.toggle(c.jump))
    {
      if(ground)
      {
        game->playSound(SND_JUMP);
        vel.y = 0.015;
      }
      else if(facingWall())
      {
        game->playSound(SND_JUMP);
        // wall climbing
        vel.x = dir == RIGHT ? -0.04 : 0.04;
        vel.y = 0.015;
        climbDelay = CLIMB_DELAY;
      }
    }

    // stop jump if the player release the button early
    if(vel.y > 0 && !c.jump)
      vel.y = 0;

    vel.x = clamp(vel.x, -MAX_HORZ_SPEED, MAX_HORZ_SPEED);
    vel.y = max(vel.y, -MAX_FALL_SPEED);
  }

  void airMove(Control c)
  {
    float wantedSpeed = 0;

    if(!climbDelay)
    {
      if(c.left)
        wantedSpeed -= WALK_SPEED;

      if(c.right)
        wantedSpeed += WALK_SPEED;
    }

    vel.x = (vel.x * 0.95 + wantedSpeed * 0.05);

    if(abs(vel.x) < 0.00001)
      vel.x = 0;
  }

  bool move(Vector2f delta)
  {
    auto nextPos = pos + delta;

    const Vector2f vertices[] =
    {
      Vector2f(0, 0),
      Vector2f(size.width, 0),
      Vector2f(0, size.height / 2.0),
      Vector2f(size.width, size.height / 2.0),
      Vector2f(0, size.height),
      Vector2f(size.width, size.height),
    };

    for(auto& v : vertices)
      if(game->isSolid(nextPos + v))
        return false;

    pos = nextPos;
    return true;
  }

  virtual void tick() override
  {
    decrement(blinking);
    decrement(hurtDelay);
  }

  virtual void onDamage(int amount) override
  {
    if(life <= 0)
      return;

    if(!blinking)
    {
      life -= amount;

      if(life < 0)
      {
        die();
        return;
      }

      hurtDelay = HURT_DELAY;
      blinking = 2000;
      game->playSound(SND_HURT);
    }
  }

  bool facingWall() const
  {
    auto const front = dir == RIGHT ? 0.7 : -0.7;

    if(game->isSolid(pos + Vector2f(size.width / 2 + front, 0.3)))
      return true;

    if(game->isSolid(pos + Vector2f(size.width / 2 + front, 1.2)))
      return true;

    return false;
  }

  void die()
  {
    game->playSound(SND_DIE);
  }

  Int debounceFire;
  Int debounceLanding;
  ORIENTATION dir;
  Bool ground;
  Toggle jumpbutton, firebutton;
  Int time;
  Int climbDelay;
  Int hurtDelay;
  Int life;
};

Player* createRockman()
{
  return new Rockman;
}

