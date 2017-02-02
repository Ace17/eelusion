#include "game/entities/switch.h"
#include "game/entities/wheel.h"
#include "game/entities/spider.h"
#include "game/entities/teleporter.h"
#include "game/entities/detector.h"
#include "game/entities/bonus.h"
#include "game/entities/player.h"
#include "game/level_graph.h"

int interpretTile(Vector2i pos, Vector2i& start, IGame* game, int val, int& portalId)
{
  switch(val)
  {
  case ' ':
    return 0;
  case '!':
    start = pos;
    return 0;
  case '?':
    {
      auto teleporter = new Teleporter;
      teleporter->pos = Vector2f(pos.x, pos.y);
      game->spawn(teleporter);
      return 0;
    }
  case '@':
    {
      auto bonus = makeBonus(0, 0);
      bonus->pos = Vector2f(pos.x, pos.y);
      game->spawn(bonus.release());
      return 0;
    }
  case '$':
    {
      auto bonus = makeBonus(3, UPGRADE_SHOOT);
      bonus->pos = Vector2f(pos.x, pos.y);
      game->spawn(bonus.release());
      return 0;
    }
  case '%':
    {
      auto bonus = makeBonus(4, UPGRADE_CLIMB);
      bonus->pos = Vector2f(pos.x, pos.y);
      game->spawn(bonus.release());
      return 0;
    }
  case '^':
    {
      auto bonus = makeBonus(5, UPGRADE_DASH);
      bonus->pos = Vector2f(pos.x, pos.y);
      game->spawn(bonus.release());
      return 0;
    }
  case '.':
    return 1;
  case 'X': return 5;
  case 'O': return 3;
  case 'Z': return 4;
  case 'a':
  case 'b':
  case 'c':
  case 'd':
    {
      auto sw = new Switch(val - 'a');
      sw->pos = Vector2f(pos.x + 0.3, pos.y + 0.15);
      game->spawn(sw);
      return 0;
    }
  case 'A':
  case 'B':
  case 'C':
  case 'D':
    {
      auto sw = new Door(val - 'A', game);
      sw->pos = Vector2f(pos.x, pos.y);
      game->spawn(sw);
      return 0;
    }
  case '#':
    {
      auto ent = new BreakableDoor;
      ent->pos = Vector2f(pos.x, pos.y);
      game->spawn(ent);
      return 0;
    }
  case '*':
    {
      auto wh = new Wheel;
      wh->pos = Vector2f(pos.x, pos.y);
      game->spawn(wh);
      return 0;
    }
  case '&':
    {
      auto wh = new Spider;
      wh->pos = Vector2f(pos.x, pos.y);
      game->spawn(wh);
      return 0;
    }
  case 'P':
    {
      auto portal = new Detector;
      portal->pos = Vector2f(pos.x, pos.y);
      portal->size = Size2f(0.1, 3);
      portal->id = portalId++;
      game->spawn(portal);
      return 0;
    }
  default:
    return 4;
  }
}

Level loadLevel(Matrix<char> const& input, IGame* game)
{
  Level r;
  r.tiles.resize(input.size);

  int portalId = 0;

  for(auto pos : rasterScan(input.size.width, input.size.height))
  {
    auto const x = pos.first;
    auto const y = pos.second;

    auto val = input.get(x, y);
    auto tile = interpretTile(Vector2i(x, y), r.start, game, val, portalId);

    r.tiles.set(x, y, tile);
  }

  return r;
}

