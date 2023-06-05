#pragma once
#define M_PI 3.14159265
#define RAD_TO_DEG(x) x * (180.f / M_PI)
#define DEG_TO_RAD(x) x * (M_PI / 180.f)

#include <framework.h>
#include <driver.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <mutex>
#include <shared_mutex>

#include <d3dx9.h>

#include "variables.hpp"
#include "vector.hpp"
#include "unity.hpp"
#include "eft.hpp"
#include "c_player.hpp"

using namespace framework;