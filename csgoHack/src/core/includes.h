#pragma once

#include <Windows.h>

#include <atomic>
#include <mutex>

static std::atomic<bool> existsWorkingThread(false);
static std::atomic<bool> enemyInSight(false);

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "dx.h"
#include "hook.h"

struct Vector3;

inline std::shared_ptr<Vector3> globalPosPtr;
inline std::mutex mtx;

#include "hack.h"
#include "drawing.h"

#include "aimbot.h"
//#include "ents.h"
//#include "geom.h"
