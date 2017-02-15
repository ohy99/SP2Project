#ifndef LOADATOM_H
#define LOADATOM_H

#include "MatrixStack.h"
#include <string>
bool LoadAtom(const char *file_path, MS* modelStack, double timeElapsed, const std::string& dagNode);

#endif