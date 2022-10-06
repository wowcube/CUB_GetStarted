#include "getstarted_vars.inc"

const MASCOT      = 0;
const EMPTY       = 1;
const ICON        = 2;
const TEXT_BOX    = 3;
const RIGHT_WAY   = 4;
const WRONG_WAY   = 5;
const FINGER_ANIM = 6;
const BUBBLE_TEXT = 7;

new getstarted_tapTutorial{} = {
    EMPTY, MASCOT, EMPTY, TEXT_BOX,
    EMPTY, MASCOT, EMPTY, TEXT_BOX,
    EMPTY, EMPTY, EMPTY, TEXT_BOX,
    EMPTY, EMPTY, EMPTY, TEXT_BOX,
    EMPTY, EMPTY, EMPTY, TEXT_BOX,
    EMPTY, EMPTY, EMPTY, TEXT_BOX,
};

new getstarted_tapStartTutorial{} = {
    RIGHT_WAY, BUBBLE_TEXT, FINGER_ANIM, WRONG_WAY,
    RIGHT_WAY, BUBBLE_TEXT, FINGER_ANIM, WRONG_WAY,
    ICON, EMPTY, TEXT_BOX, EMPTY,
    ICON, EMPTY, TEXT_BOX, EMPTY,
    ICON, EMPTY, TEXT_BOX, EMPTY,
    ICON, EMPTY, TEXT_BOX, EMPTY,
};

const FULFILLMENT_DROP_TIME = 3000;
const TAPS_TO_FINISH_TUTORIAL = 6;
new fulfillmentTimer = 0;
new tapTutorialStage = 0;
new beginTapTutorial = 0;
new fillTapTutorial  = 0;

new alreadyCheckTapReaction = 0;

new mascotTapReactAnimFlag = 0;
new mascotTapReactAngle = 75;
new tapAngleAnimChange = 4;

new tapAnimationFrame = 0;
new repeatAnimFlag = 0;
new fingerAngle = 0;
new mascotPosY  = 0;

const TAP_RIM_ANIM_REPEAT = 3000;
new wrongTapRimTimer = 180;
new wrongTapRimFlag = 1;
new wrongTapRimResetTimer = 0;

new rightTapRimFlag = 1;
new rightTapRimAlpha_1 = 0;
new rightTapRimAlpha_2 = 0;
new rightTapRimAlpha_3 = 0;
new rightTapRimTimer_1 = 180;
new rightTapRimTimer_2 = 180;
new rightTapRimTimer_3 = 180;
new rightTapRimResetTimer = 0;

MascotTapReaction() {
    if (!alreadyCheckTapReaction) {
        mascotTapReactAngle += tapAngleAnimChange;
        if ((mascotTapReactAngle == 87) || (mascotTapReactAngle == 75)) {
            tapAngleAnimChange = -tapAngleAnimChange;
        }
        if (mascotTapReactAngle == 75) {
            mascotTapReactAnimFlag = 0;
        }
        alreadyCheckTapReaction = 1;
    }
}

GetTapFillColor(tapsInPercents) {
    return (0xFF000000
    | 0x68 + ((0xB5 - 0x68) * tapsInPercents) / 100 << 16
    | 0x39 + ((0x2A - 0x39) * tapsInPercents) / 100 << 8
    | 0x8D + ((0xAD - 0x8D) * tapsInPercents) / 100);
}

TapFillBackground(angle) {
    new tapsInPercents = (tapTutorialStage * 100) / TAPS_TO_FINISH_TUTORIAL;
    new rect[GFX_RECTANGLE];
    rect.x = 0;
    rect.y = 0;
    rect.w = 240;
    rect.h = 240;
    switch (angle) {
        case 0: {
            if (tapsInPercents <= 50) {
                rect.y = rect.h - (480 * tapsInPercents) / 100;
            }
        }
        case 90: {
            if (tapsInPercents >= 50) {
                rect.w = (480 * tapsInPercents) / 100 - rect.w;
            } else {
                rect.w = rect.h = 0;
            }
        }
        case 180: {
            if (tapsInPercents >= 50) {
                rect.h = (480 * tapsInPercents) / 100 - rect.h;
            } else {
                rect.w = rect.h = 0;
            }
        }
        case 270: {
            if (tapsInPercents <= 50) {
                rect.x = rect.w - (480 * tapsInPercents) / 100;
            }
        }
    }

    GFX_drawRectangle(rect, GetTapFillColor(tapsInPercents));
}

FingerTapAnimation() {
    ++tapAnimationFrame;
    if (tapAnimationFrame >= 12) {
        if (!repeatAnimFlag) {
            repeatAnimFlag = 1;
            fingerAngle = mascotPosY = tapAnimationFrame = 0;
        }
        if (tapAnimationFrame > 33) {
            repeatAnimFlag = 0;
            fingerAngle = mascotPosY = tapAnimationFrame = 0;
        }
        return;
    }
    if (tapAnimationFrame >= 6) {
        fingerAngle -= 8;
    } else if (tapAnimationFrame >= 0) {
        fingerAngle += 8;
    }
    
    if ((tapAnimationFrame >= 8) && (tapAnimationFrame <= 9)) {
        mascotPosY -= 10;
    } else if ((tapAnimationFrame >= 6) && (tapAnimationFrame <= 7)) {
        mascotPosY += 10;
    }
}

DrawTapTutorial(screen) {
    new angle = getStarted_screenData[screen].angle;
    
    new layoutElement = getStarted_screenData[screen].sideType * TOPOLOGY_POSITIONS_MAX + angle / 90;
    new screenLayout = (getstarted_tapStartTutorial{layoutElement});
    if (fillTapTutorial) {
        screenLayout = (getstarted_tapTutorial{layoutElement});
    }

    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);

    if (fillTapTutorial) {
        TapFillBackground(TOPOLOGY_getAngle(SetFacelet(SELF_ID, screen), TOPOLOGY_orientation_mode:ORIENTATION_MODE_GRAVITY));
    }
    switch (screenLayout) {
        case MASCOT: {
            GFX_drawImage([60, 60], 0xFF, 0, 0, MIRROR_BLANK, CIRCLE_QUARTER);

            if (mascotTapReactAnimFlag) {
                MascotTapReaction();
                GFX_drawImage([60, 60], 0xFF, 0, 0, MIRROR_BLANK, CIRCLE_QUARTER_PUSH);
            }
            GFX_drawImage([210, 210], 0xFF, 0, mascotTapReactAngle, MIRROR_BLANK, currentMascotSprite);
            GFX_drawImage([ 91, 120], 0xFF, 0, angle, MIRROR_BLANK, DIALOGUE_SMALL);
            if (beginTapTutorial) {
                GFX_drawText([55, 130], 17, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFA552, "tap!");
            } else {
                GFX_drawText([90, 120], SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "moving on");
                GFX_drawText([45, 120], SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "to the tap");
            }
        }
        case TEXT_BOX: {
            if (fillTapTutorial) {
                GFX_drawImage([60, 60], 0xFF, 0, 0, MIRROR_BLANK, CIRCLE_QUARTER);
                
                if (mascotTapReactAnimFlag) {
                    MascotTapReaction();
                    GFX_drawImage([60, 60], 0xFF, 0, 0, MIRROR_BLANK, CIRCLE_QUARTER_PUSH);
                }

                GFX_drawText([170, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFFFFFF, "tap to fill");
                GFX_drawText([210, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "the cube");
            } else {
                GFX_drawText([120, 140], 9, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "double tap");
                GFX_drawText([120, 100], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "the cube!");
            }
        }
        case EMPTY: {
            if (fillTapTutorial) {
                GFX_drawImage([60, 60], 0xFF, 0, 0, MIRROR_BLANK, CIRCLE_QUARTER);

                if (mascotTapReactAnimFlag) {
                    MascotTapReaction();
                    if ((angle % 180) == 0) {
                        GFX_drawLine([165,  86], [177,  86], 6, 0xFFFFFFFF);
                        GFX_drawLine([ 86, 165], [ 86, 177], 6, 0xFFFFFFFF);
                        GFX_drawLine([146, 146], [156, 156], 6, 0xFFFFFFFF);
                    }
                    GFX_drawImage([60, 60], 0xFF, 0, 0, MIRROR_BLANK, CIRCLE_QUARTER_PUSH);
                }
            }
        }
        case ICON: {
            GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, TAP_ICON);
        }
        case WRONG_WAY: {
            GFX_drawImage([90, 120], 0xFF, 0, angle, MIRROR_BLANK, WRONG_TAP_ICON);
            if (wrongTapRimFlag) {
                new step = (FixedCos(((wrongTapRimTimer += 10)) % 359) + 256) >> 1;
                GFX_drawImage([90, 120], step, 0, angle, MIRROR_BLANK, WRONG_TAP_RIM);
            } else {
                wrongTapRimResetTimer += deltaTime;
            }
            if ((wrongTapRimTimer % 360) == 180) {
                wrongTapRimFlag = 0;
            }
            if (wrongTapRimResetTimer >= TAP_RIM_ANIM_REPEAT) {
                wrongTapRimResetTimer = 0;
                wrongTapRimFlag = 1;
            }
            GFX_drawText([170, 120], SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "not a soft");
            GFX_drawText([210, 120], SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "touch");
        }
        case RIGHT_WAY: {
            if (rightTapRimFlag == 1) {
                rightTapRimAlpha_1 = (FixedCos(((rightTapRimTimer_1 += 15)) % 359) + 256) >> 1;
                if ((rightTapRimTimer_1 % 360) == 180) {
                    --rightTapRimFlag;
                } else if ((rightTapRimTimer_1 % 360) == 0) {
                    ++rightTapRimFlag;
                }
            }
            if (rightTapRimFlag == 2) {
                rightTapRimAlpha_2 = (FixedCos(((rightTapRimTimer_2 += 15)) % 359) + 256) >> 1;
                if ((rightTapRimTimer_2 % 360) == 180) {
                    --rightTapRimFlag;
                } else if ((rightTapRimTimer_2 % 360) == 0) {
                    ++rightTapRimFlag;
                }
            }
            if (rightTapRimFlag == 3) {
                rightTapRimAlpha_3 = (FixedCos(((rightTapRimTimer_3 += 15)) % 359) + 256) >> 1;
                if ((rightTapRimTimer_3 % 360) == 180) {
                    --rightTapRimFlag;
                }
            }
            if (!rightTapRimFlag) {
                rightTapRimResetTimer += deltaTime;
            }
            if (rightTapRimResetTimer >= TAP_RIM_ANIM_REPEAT) {
                rightTapRimResetTimer = 0;
                rightTapRimFlag = 1;
            }
            GFX_drawImage([120, 90], rightTapRimAlpha_1, 0, angle, MIRROR_BLANK, RIGHT_TAP_RIM_1);
            GFX_drawImage([120, 90], rightTapRimAlpha_2, 0, angle, MIRROR_BLANK, RIGHT_TAP_RIM_2);
            GFX_drawImage([120, 90], rightTapRimAlpha_3, 0, angle, MIRROR_BLANK, RIGHT_TAP_RIM_3);
            GFX_drawImage([120, 90], 0xFF, 0, angle, MIRROR_BLANK, RIGHT_TAP_ICON);
            GFX_drawText([120, 170], SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "but a light");
            GFX_drawText([120, 210], SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "knock");
        }
        case FINGER_ANIM: {
            FingerTapAnimation();
            GFX_drawImage([180, 200], 0xFF, 0, angle - 10 + fingerAngle, MIRROR_BLANK, FINGER_SPRITE);
            GFX_drawImage(GetGFXPoint(120, 60 - mascotPosY), 0xFF, 0, angle - 10, MIRROR_BLANK, currentMascotSprite);
            if ((tapAnimationFrame >= 5) && tapAnimationFrame <= 7) {
                GFX_drawLine([188, 163], [170, 155], 6, 0xFFFFFFFF);
                GFX_drawLine([ 95, 180], [ 92, 195], 6, 0xFFFFFFFF);
                GFX_drawLine([ 90, 160], [ 72, 168], 6, 0xFFFFFFFF);
            }
        }
        case BUBBLE_TEXT: {
            GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, SPEECH_BUBBLE_TAP_SPRITE);
            GFX_drawText([140, 140], 9, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "double tap");
            GFX_drawText([100, 140], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "the cube!");
        }
    }

    if (screen == 2) {
        alreadyCheckTapReaction = 0;
    }
}

UpdateTapTutorial(deltaTime) {
    if (SELF_ID == 0) {
        if (fillTapTutorial) {
            if (fulfillmentTimer >= FULFILLMENT_DROP_TIME) {
                --tapTutorialStage;
                if (tapTutorialStage < 0) {
                    tapTutorialStage = 0;
                }
                fulfillmentTimer = 0;
            } else {
                fulfillmentTimer += deltaTime;
            }

            if (tapTutorialStage >= TAPS_TO_FINISH_TUTORIAL) {
                SetApplicationState(FSM:successScreen);
                SND_play(TAP_STAGE_SUCCESS_SOUND, SOUND_VOLUME);
            }
        }
    }
}