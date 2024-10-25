#pragma once
#include "Driver.h"




void handleExcp(_EXCEPTION_POINTERS* per);

const char* excpDiscriptor(const unsigned int code);