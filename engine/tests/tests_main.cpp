/*
 * Copyright (C) 2017 - Sebastien Alaiwan
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 */

// Unit test framework: entry point

#include <iostream>
#include "tests.h"

using namespace std;

int main(int argc, char* argv[])
{
  char const* filter = "";

  if(argc == 2)
    filter = argv[1];

  try
  {
    RunTests(filter);
    return 0;
  }
  catch(std::exception const& e)
  {
    cerr << "Fatal: " << e.what() << endl;
    return 1;
  }
}
