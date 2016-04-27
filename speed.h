#ifndef SPEED_H
#define SPEED_H

/// The interval in milliseconds in which the clip's position is updated.
const int FLOATING_CLIP_UPDATE_INTERVAL_MS = 1;
/// The numbers of pixels a clip moves in one millisecond.
const int FLOATING_CLIP_PIXELS_PER_MS = 1;
const int FLOATING_CLIP_SPEED = FLOATING_CLIP_UPDATE_INTERVAL_MS * FLOATING_CLIP_PIXELS_PER_MS;
const int ROOM_WIND_CHANGE_INTERVAL_MS = 350; /// This constant defines how often the wind changes its direction.

#endif
