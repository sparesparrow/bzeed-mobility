#pragma once

#include "secrets.h"

// If HTTPS_ROOT_CA is empty, set insecure (NOT recommended for production)
#ifndef HTTPS_ROOT_CA
#define HTTPS_ROOT_CA ""
#endif