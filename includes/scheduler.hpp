#pragma once

#include <stdlib.h>
#include <exception>
#include <time.h>
#include <unistd.h>
#include <sys/msg.h>
#include <queue>
#include <sys/time.h>
#include <iostream>
using namespace std;

#include "ChildProcess.hpp"
#include "ParentProcess.hpp"

void printInfo(void);
