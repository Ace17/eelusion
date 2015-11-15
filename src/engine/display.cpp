/**
 * @brief OpenGL stuff
 * @author Sebastien Alaiwan
 */

/*
 * Copyright (C) 2015 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 */

#include <cassert>
#include <sstream>
#include <vector>
#include <iostream>
#include <stdexcept>
using namespace std;

#define GL_GLEXT_PROTOTYPES 1
#include "GL/glu.h"
#include "GL/gl.h"
#include "GL/glext.h"
#include "SDL_video.h"
#include "SDL_image.h"

#include "util.h"
#include "scene.h"
#include "geom.h"
#include "model.h"

vector<Model> g_Models;
GLuint g_ProgramId;

#define SAFE_GL(a) \
  do { a; ensureGl(# a, __LINE__); } while(0)

void ensureGl(char const* expr, int line)
{
  auto const errorCode = glGetError();

  if(errorCode == GL_NO_ERROR)
    return;

  stringstream ss;
  ss << "OpenGL error" << endl;
  ss << "Expr: " << expr << endl;
  ss << "Line: " << line << endl;
  ss << "Msg: " << gluErrorString(errorCode) << endl;
  ss << "Code: " << errorCode;
  throw runtime_error(ss.str());
}

int compileShader(string code, int type)
{
  auto shaderId = glCreateShader(type);

  cout << "Compiling " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader ... ";
  auto srcPtr = code.c_str();
  SAFE_GL(glShaderSource(shaderId, 1, &srcPtr, nullptr));
  SAFE_GL(glCompileShader(shaderId));

  // Check compile result
  int logLength;
  GLint Result;
  SAFE_GL(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result));

  if(!Result)
  {
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    vector<char> msg(logLength);
    glGetShaderInfoLog(shaderId, logLength, nullptr, msg.data());
    cerr << msg.data();

    cerr << code << endl;

    throw runtime_error("Can't compile shader");
  }

  cout << "OK" << endl;

  return shaderId;
}

int linkShaders(vector<int> ids)
{
  // Link the program
  cout << "Linking shaders ... ";
  auto ProgramID = glCreateProgram();

  for(auto id : ids)
    glAttachShader(ProgramID, id);

  glLinkProgram(ProgramID);

  // Check the program
  GLint Result = GL_FALSE;
  int InfoLogLength;
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  vector<char> msgBuf(InfoLogLength);
  glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, msgBuf.data());
  cout << msgBuf.data();

  cout << "OK" << endl;

  return ProgramID;
}

SDL_Surface* loadPicture(string path)
{
  auto surface = IMG_Load((char*)path.c_str());

  if(!surface)
    throw runtime_error(string("Can't load texture: ") + SDL_GetError());

  if(surface->format->BitsPerPixel != 32)
    throw runtime_error("only 32 bit pictures are supported");

  return surface;
}

int loadTexture(string path, Rect2i rect)
{
  auto surface = loadPicture(path);

  if(rect.width == 0 && rect.height == 0)
    rect = Rect2i(0, 0, surface->w, surface->h);

  GLuint texture;

  auto const bpp = surface->format->BytesPerPixel;

  vector<uint8_t> img(rect.width * rect.height * bpp);

  auto src = (Uint8*)surface->pixels + rect.x * bpp + rect.y * surface->pitch;
  auto dst = (Uint8*)img.data();

  for(int y = 0; y < rect.height; ++y)
  {
    memcpy(dst, src, bpp * rect.width);
    src += surface->pitch;
    dst += bpp * rect.width;
  }

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rect.width, rect.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  SDL_FreeSurface(surface);

  return texture;
}

extern char VertexShaderCode[];
extern char FragmentShaderCode[];

GLuint loadShaders()
{
  auto const vertexId = compileShader(VertexShaderCode, GL_VERTEX_SHADER);
  auto const fragmentId = compileShader(FragmentShaderCode, GL_FRAGMENT_SHADER);

  auto const progId = linkShaders(vector<int>({ vertexId, fragmentId }));

  SAFE_GL(glDeleteShader(vertexId));
  SAFE_GL(glDeleteShader(fragmentId));

  return progId;
}

Model rectangularModel(float w, float h)
{
  const GLfloat myTriangle[] =
  {
    0, h, 0, 0, 0,
    0, 0, 0, 0, 1,
    w, 0, 0, 1, 1,
    w, h, 0, 1, 0,
  };

  const GLushort indices[] =
  {
    0, 1, 2,
    2, 3, 0,
  };

  Model model;

  model.size = 4;

  // Generate 1 buffer, put the resulting identifier in buffer
  SAFE_GL(glGenBuffers(1, &model.buffer));
  SAFE_GL(glBindBuffer(GL_ARRAY_BUFFER, model.buffer));
  SAFE_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(myTriangle), myTriangle, GL_STATIC_DRAW));

  SAFE_GL(glGenBuffers(1, &model.indices));
  SAFE_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.indices));
  SAFE_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

  model.numIndices = 6;

  return model;
}

Model loadAnimation(string path)
{
  auto m = rectangularModel(1, 1);

  if(endsWith(path, ".json"))
  {
    auto m2 = loadModel(path);
    m.textures = move(m2.textures);
    return m;
  }
  else if(endsWith(path, ".mdl"))
  {
    path = setExtension(path, "png");

    for(int i = 0; i < 64; ++i)
    {
      auto col = i % 8;
      auto row = i / 8;
      m.addTexture(path, Rect2i(col * 16, row * 16, 16, 16));
    }
  }
  else
  {
    m.addTexture(path, Rect2i());
  }

  return m;
}

void Display_loadModel(int id, const char* path)
{
  if((int)g_Models.size() <= id)
    g_Models.resize(id + 1);

  g_Models[id] = loadAnimation(path);
}

void printOpenGlVersion()
{
  auto sVersion = (char const*)glGetString(GL_VERSION);
  auto sLangVersion = (char const*)glGetString(GL_SHADING_LANGUAGE_VERSION);

  cout << "OpenGL version: " << sVersion << endl;
  cout << "OpenGL shading version: " << sLangVersion << endl;
}

void Display_init(int width, int height)
{
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  if(!SDL_SetVideoMode(width, height, 0, SDL_OPENGL))
    throw runtime_error("Can't set video mode");

  printOpenGlVersion();

  GLuint VertexArrayID;
  SAFE_GL(glGenVertexArrays(1, &VertexArrayID));
  SAFE_GL(glBindVertexArray(VertexArrayID));

  g_ProgramId = loadShaders();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void drawActor(Rect2f where, int modelId, bool blinking, int frame)
{
  auto& model = g_Models.at(modelId);

  auto const colorId = glGetUniformLocation(g_ProgramId, "v_color");

  SAFE_GL(glUniform4f(colorId, 0, 0, 0, 0));

  if(blinking)
  {
    static int blinkCounter;
    blinkCounter++;

    if((blinkCounter / 4) % 2)
      SAFE_GL(glUniform4f(colorId, 0.8, 0.4, 0.4, 0));
  }

  // Get a handle for our "MVP" uniform.
  auto const matrixId = glGetUniformLocation(g_ProgramId, "MVP");

  if(matrixId < 0)
    throw runtime_error("glGetUniformLocation failed");

  if(model.textures.empty())
    throw runtime_error("model has no textures");

  frame %= model.textures.size();
  glBindTexture(GL_TEXTURE_2D, model.textures[frame]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  auto const dx = where.x;
  auto const dy = where.y;

  auto const sx = where.width * model.scale;
  auto const sy = where.height * model.scale;

  float mat[16] =
  {
    sx, 0, 0, 0,
    0, sy, 0, 0,
    0, 0, 1, 0,
    dx, dy, 0, 1,
  };

  SAFE_GL(glUniformMatrix4fv(matrixId, 1, GL_FALSE, mat));

  SAFE_GL(glBindBuffer(GL_ARRAY_BUFFER, model.buffer));
  SAFE_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.indices));

  auto const positionLoc = glGetAttribLocation(g_ProgramId, "a_position");
  auto const texCoordLoc = glGetAttribLocation(g_ProgramId, "a_texCoord");

  assert(positionLoc >= 0);
  assert(texCoordLoc >= 0);

  // connect the xyz to the "a_position" attribute of the vertex shader
  SAFE_GL(glEnableVertexAttribArray(positionLoc));
  SAFE_GL(glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr));

  // connect the uv coords to the "v_texCoord" attribute of the vertex shader
  SAFE_GL(glEnableVertexAttribArray(texCoordLoc));
  SAFE_GL(glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat))));

  SAFE_GL(glDrawElements(GL_TRIANGLES, model.numIndices, GL_UNSIGNED_SHORT, 0));

  SAFE_GL(glDisableVertexAttribArray(positionLoc));
}

void beginDraw()
{
  SAFE_GL(glUseProgram(g_ProgramId));

  SAFE_GL(glClearColor(0, 0, 0, 1));
  SAFE_GL(glClear(GL_COLOR_BUFFER_BIT));

  {
    auto const scaleMatrixId = glGetUniformLocation(g_ProgramId, "Scale");
    auto const s = 0.3f;
    float mat[16] =
    {
      s, 0, 0, 0,
      0, s, 0, 0,
      0, 0, s, 0,
      0, 0, 0, 1,
    };
    SAFE_GL(glUniformMatrix4fv(scaleMatrixId, 1, GL_FALSE, mat));
  }
}

void endDraw()
{
  SDL_GL_SwapBuffers();
}

