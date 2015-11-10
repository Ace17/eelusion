/**
 * @file raii.h
 * @brief layer for completing the C++ standard library
 * @author Sebastien Alaiwan
 * @date 2015-11-10
 */

/*
 * Copyright (C) 2015 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 */

#pragma once

#include <vector>
#include <memory>
#include <string>

using namespace std;

template<typename T>
class uvector : public vector<unique_ptr<T>>
{
};

template<typename T>
class uptr : public unique_ptr<T>
{
public:
  uptr(T* p) : unique_ptr<T>(p)
  {
  }
};

template<typename T>
uptr<T> unique(T* p)
{
  return uptr<T>(p);
}

inline bool endsWith(string const& value, string const& ending)
{
  if(ending.size() > value.size())
    return false;

  return equal(ending.rbegin(), ending.rend(), value.rbegin());
}

inline string setExtension(string name, string ext)
{
  auto e = name.rfind('.');
  return name.substr(0, e) + "." + ext;
}

template<typename T>
struct Matrix
{
  Matrix(int width_, int height_) : width(width_), height(height_)
  {
    data.resize(width * height);
  }

  int getWidth() const
  {
    return width;
  }

  int getHeight() const
  {
    return height;
  }

  T& get(int x, int y)
  {
    assert(isInside(x, y));
    return data[x + y * width];
  }

  const T& get(int x, int y) const
  {
    assert(isInside(x, y));
    return data[x + y * width];
  }

  void set(int x, int y, T const& val)
  {
    assert(isInside(x, y));
    get(x, y) = val;
  }

  template<typename Lambda>
  void scan(Lambda f)
  {
    for(int y = 0; y < height; y++)
      for(int x = 0; x < width; x++)
        f(x, y, get(x, y));
  }

  template<typename Lambda>
  void scan(Lambda f) const
  {
    for(int y = 0; y < height; y++)
      for(int x = 0; x < width; x++)
        f(x, y, get(x, y));
  }

  bool isInside(int x, int y) const
  {
    if(x < 0 || y < 0)
      return false;

    if(x >= width || y >= height)
      return false;

    return true;
  }

private:
  vector<T> data;
  const int width;
  const int height;
};

