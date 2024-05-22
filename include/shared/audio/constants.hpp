#pragma once

#define QUIET_VOLUME  50.0f
#define MIDDLE_VOLUME 75.0f
#define FULL_VOLUME 100.0f

#define DEFAULT_VOLUME FULL_VOLUME

// Sound values fitting for something that should be audible only very close
#define SHORT_ATTEN 50.0f
#define SHORT_DIST 5.0f

// Sound values fitting for something that should be audible within a 10x10 room
#define MEDIUM_ATTEN 5.0f
#define MEDIUM_DIST 20.0f

// Sound values fitting for something that should be heard from pretty far (large room)
#define FAR_ATTEN 3.0f
#define FAR_DIST 35.0f