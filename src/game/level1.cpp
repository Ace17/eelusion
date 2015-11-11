#include <cassert>
#include "engine/geom.h"
#include "engine/raii.h"

void loadLevel1(Matrix<int>& tiles, Vector2i& start)
{
  const auto W = 32 * 2;
  const auto H = 32 * 2;

  const char data[H][W + 1] =
  {
    "................................................................",
    "...       .........................       ......................",
    "...                                                           ..",
    "... !              .......... .....       .......  .......... ..",
    "............       .......... ...................  .......... ..",
    "............       .........  ..............       .X.......  ..",
    ".       ....   ... .........   ..       ....   ... .........   .",
    ".              ...     ......  ..              ... ..........  .",
    ".           ..............     ..           ..............     .",
    ".       ..................    ...       ..................    ..",
    ".  ..........        ........ ...  ..........        ........ ..",
    ". ........... ...... ........ ...  .......... ...... ........ ..",
    "............. ...... ............ ........... ...... ........ ..",
    ".             ......          ... ..          ......          ..",
    "..           ................      .         ...................",
    ".. ......................................... ...................",
    ".. .........................................              ......",
    "..        ...................             .........XXXXXXXXXXXXX",
    "XXX                           XXXXX                           XX",
    "XXX                XXXXXXXXXX XXXXX                XXXXXXXXXX XX",
    "XXXXXXXXXXXXXXXXXX XXXXXXXXXX XXXXXXXXXXXXXX       XXXXXXXXXX XX",
    "XXXXXXXXXXXX       XZXXXXXXX  XXXXXXXXXXXXXX       XXXXXXXXX  XX",
    "X       XXXX   XXX XXXXXXXXX   XXXXXXXXXXXXX   XXX XXXXXXXXX   X",
    "X              XXX XXXXXXXXXX  XXXXXXXXX       XXX XXXXXXXXXX  X",
    "X           XXXXXXXXXXXXXX     XXXXXXXXX    XXXXXXXXXXX        X",
    "X       XXXXXXXXXXXXXXXXXX    XXXXXXXXXXXXXXXXXXXXXXXXX XX    XX",
    "X  XXXXXXXXXX        XXXXXXXXXXXXXXXXXXXXXXXX        XX XXXXX XX",
    "X  XXXXXXXXXX XXXXXX XXXXXXXX            XXXX XXXXXX XX        X",
    "X XXXXXXXXXXX XXXXXX XXXXXXXX            XXXX XXXXXX XXXXXXXXXXX",
    "X             XXXXXX          XXXXXXXXXX      XZXXXX           X",
    "XXX          XXXXXXXXXXXXXXXXXXXXXXXXXXX     XXXXXXXXXXXXXXX  XX",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXX   XX",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX          XXXXXXXXXXXXXXXXX   XXX",
    "XX     XXXXXXXXX                XXXXXXXXXXXXXXXXXXXXXXXXX   XXXX",
    "XX  XXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXXXXXXX      XXXXX   XXXXX",
    "ZZ  ZZZ          ZZZZZZZZZZZZZZZZZZZZZZXXXXXXXXXX  XXXX    XXXXX",
    "ZZ ZZZZ ZZZZZZZZZZZZZZZ       ZZZZZZZZZ             !   ZZZZZZZZ",
    "ZZ      ZZZZZZZZZZZZZZZ                                 ZZZZZZZZ",
    "ZZZZZZZZZZZZZZZZZ                        ZZZZZZZZZZZZZZZZZZZZZZZ",
    "ZZZZZZZZZZ                    ZZZZZZZZZ  ZZZ       Z      Z    Z",
    "ZZZZ             ZZZZZZZZZZZZZZZZZZZZZZ                        Z",
    "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ                        Z",
    "ZZZZZZZZZZ       ZZZZZZZZZZZZZZZZZZZZZZXXZZZ       Z      Z    Z",
    "ZZZZZZZZZZ               Z      ZZZZZZZ  ZZZZZZZZZZZZZ  ZZZZZZZZ",
    "ZZZZZZZZZZ       ZZ             ZZZZZZZ  ZZZZZZZZZZZ      ZZZZZZ",
    "ZZZZZZ       ZZ  ZZ      Z           ZZ                   ZZZZZZ",
    "ZZZZZZ       ZZ  ZZ      Z      XXX  ZZ  ZZZZZZZZZZZ      ZZZZZZ",
    "ZZZZZZ           ZZZZ  ZZZZZZZZZXXXZ ZZ  ZZZZZZZZZZZZZZZZZZZZZZZ",
    "ZZZZZZ           ZZZZ          ZZZZZ ZZ  ZZ                ZZZZZ",
    "ZZZZZZ  ZZZZZZZZZZZZZZZZZZZZZZ ZZZZZ     ZZ    ZZ ZZZZZZZZ ZZZZZ",
    "ZZZZZZ  ZZZZZZZZZZZZZ       ZZ ZZZZZZZZZZZZZZZZZZ ZZZZZZ   ZZZZZ",
    "ZZ               ZZZZ       ZZ ZZZZZZZZZZZ        ZZZZZZ  ZZZZZZ",
    "ZZ ZZZZZZZZZZZZZZZZZZ          ZZZZZZZZ        ZZZZZ       ZZZZZ",
    "ZZ ZZZZZZZZZZZ              ZZZZZZZZZZZ    ZZZZZZZZ    ZZZ    ZZ",
    "ZZ       ZZZZZ  ZZZZZ       ZZZZZZZZZZZ  Z ZZZZZ       ZZZ    ZZ",
    "ZZZ      ZZZZZ  ZZZZZZZZZZZZZZZZZZZZ       ZZZZZ       ZZZ    ZZ",
    "ZZZZZZZZZZZZ      ZZZ     ZZZZ       ZZZZZZZZ      ZZZZZZZ ZZZZZ",
    "ZZZZZZZZZZZZ          ZZZ       ZZZZZZZZZZZZZ ZZZZZZZZZZZZ ZZZZZ",
    "ZZZZ              ZZZZZZZZZZZZZZZZZZZZZZZZ            ZZZZZZZZZZ",
    "ZZ    ZZZZZZ      ZZZZZZZZZZZZZZZZ         ZZZZZZZZ   ZZZXXXXXXX",
    "ZZ  ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZX     X",
    "ZZ                                                             X",
    "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZX     X",
    "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZXXXXXXX",
  };

  for(int y = 0; y < H; ++y)
    for(int x = 0; x < W; ++x)
    {
      int tile = 0;
      switch(data[H - 1 - y][x])
      {
      case ' ':
        tile = 0;
        break;
      case '!':
        tile = 0;
        start = Vector2i(x, y);
        break;
      case '.': tile = 1;
        break;
      case 'X': tile = 2;
        break;
      case 'O': tile = 3;
        break;
      case 'Z': tile = 4;
        break;
      }

      tiles.set(x, y, tile);
    }
}

