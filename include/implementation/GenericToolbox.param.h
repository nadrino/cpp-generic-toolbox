//
// Created by Adrien BLANCHET on 25/06/2021.
//

#ifndef GENERICTOOLBOX_PARAM_H
#define GENERICTOOLBOX_PARAM_H

#ifndef PROGRESS_BAR_ENABLE_RAINBOW
#define PROGRESS_BAR_ENABLE_RAINBOW 0
#endif

#ifndef PROGRESS_BAR_LENGTH
#define PROGRESS_BAR_LENGTH 36
#endif

#ifndef PROGRESS_BAR_SHOW_SPEED
#define PROGRESS_BAR_SHOW_SPEED 1
#endif

#ifndef PROGRESS_BAR_REFRESH_DURATION_IN_MS
// 33 ms per frame = 0.033 seconds per frame = 1/30 sec per frame = 30 fps
#define PROGRESS_BAR_REFRESH_DURATION_IN_MS 33
#endif

#ifndef PROGRESS_BAR_FILL_TAG
// multi-char is possible
#define PROGRESS_BAR_FILL_TAG "#"
#endif

#endif //GENERICTOOLBOX_PARAM_H
