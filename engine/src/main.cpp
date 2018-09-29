// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// Entry point.
// This is the only file where emscripten-specific stuff can appear.

#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

#include "app.h"

#define SDL_MAIN_HANDLED
#include "SDL.h"

using namespace std;

#ifdef __EMSCRIPTEN__
extern "C"
{
void emscripten_set_main_loop(void (* f)(), int, int);
}

static IApp* g_theApp;

static void tickTheApp()
{
  g_theApp->tick();
}

void runMainLoop(IApp* app)
{
  g_theApp = app.get();
  emscripten_set_main_loop(&tickTheApp, 0, 10);
}

#else

void runMainLoop(IApp* app)
{
  while(app->tick())
    SDL_Delay(1);
}

#endif

int main(int argc, char* argv[])
{
  try
  {
    vector<string> args {
      argv + 1, argv + argc
    };

    auto app = createApp(args);
    runMainLoop(app.get());
    return 0;
  }
  catch(exception const& e)
  {
    fprintf(stderr, "Fatal: %s\n", e.what());
    return 1;
  }
}

