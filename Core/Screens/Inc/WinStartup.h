/*
 * WindowStartup.h
 *
 *  Created on: Aug 1, 2022
 *      Author: Alvin
 */

#ifndef SCREENHANDLERS_WINDOWSTARTUP_H_
#define SCREENHANDLERS_WINDOWSTARTUP_H_

#define MAX_INTRO_ANIMATION_FRAMES 101
#define DELAY_VIDEO_FPS 66 //(milli seconds)

typedef enum
{
	en_IntroAnimationFrame0 = 0,
	en_IntroAnimationFrameMax = (en_IntroAnimationFrame0 + MAX_INTRO_ANIMATION_FRAMES)
}enIntroFrameID;

void StartupIndroAnimationPlay(void);

#endif /* SCREENHANDLERS_WINDOWSTARTUP_H_ */
