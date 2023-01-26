#ifndef _INC_H_
#define _INC_H_

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

//#ifndef FREEGLUT_STATIC
//#define FREEGLUT_STATIC
//#endif

#pragma comment (lib, "lib/glew32s.lib")
#pragma comment (lib, "lib/assimp.lib")
#pragma comment (lib, "lib/FreeImage.lib")
#pragma comment (lib, "lib/freeglut.lib")
#pragma comment (lib, "lib/AntTweakBar64.lib")

#include "gl/glew.h"
#include "gl/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#endif