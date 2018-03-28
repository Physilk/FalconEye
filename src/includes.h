#pragma once

#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#include <memory>
#include <vector>
#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <iostream>
#include <limits>
#include <chrono>

#include <lua.hpp>
#include <lualib.h>

#ifndef WIN32
#include <png.h>
#endif
#include "image.h"
#include "image_io.h"
#include "vec.h"
#include "mat.h"
#include "orbiter.h"
#include "mesh.h"
#include "wavefront.h"

#include "luaObjectInterface.h"
#include "tools.h"
#include "ray.h"
#include "bbox.h"
#include "material.h"
#include "light.h"
#include "SceneObject.h"
#include "scene.h"
#include "bintree.h"

#ifndef NB_THREADS
	#define NB_THREADS 8
#endif


#endif
