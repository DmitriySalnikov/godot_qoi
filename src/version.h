#pragma once

#define GQOI_MAJOR 2
#define GQOI_MINOR 2
#define GQOI_PATCH 1
#define GQOI_VERSION ((GQOI_MAJOR << (8 * 3)) + (GQOI_MINOR << (8 * 2)) + (GQOI_PATCH << (8 * 1)))

#define _GQOI_VERSION_STR_TEXT(text) #text
#define _GQOI_VERSION_STR(major, minor, patch) (_GQOI_VERSION_STR_TEXT(major) "." _GQOI_VERSION_STR_TEXT(minor) "." _GQOI_VERSION_STR_TEXT(patch))
#define GQOI_VERSION_STR _GQOI_VERSION_STR(GQOI_MAJOR, GQOI_MINOR, GQOI_PATCH)
