#ifndef LOADATOM_H
#define LOADATOM_H

#include "MatrixStack.h"
#include <string>
bool LoadAtom(const char *file_path, MS* modelStack, unsigned atThisFrame, const std::string& dagNode);

#endif