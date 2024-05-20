#pragma once

#define MIN_WINDOW_WIDTH  900
#define MIN_WINDOW_HEIGHT 600

// The screen width that's defined as 1:1, from which other 
// resolutions can calculate pixel sizes
#define UNIT_WINDOW_WIDTH  1500
#define UNIT_WINDOW_HEIGHT 1000

#define PLAYER_EYE_LEVEL 2.35f

/** maximum number of supported point lights.
 *  must match the corresponding macro 
 *  in the fragment shader
 */
#define MAX_POINT_LIGHTS 16