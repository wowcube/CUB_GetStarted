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
    new startX = 0;
    new startY = 0;
    new endX = 240;
    new endY = 240;
    switch (angle) {
        case 0: {
            if (tapsInPercents <= 50) {
                startY = endY - (480 * tapsInPercents) / 100;
            }
        }
        case 90: {
            if (tapsInPercents >= 50) {
                endX = (480 * tapsInPercents) / 100 - endX;
            } else {
                endX = endY = 0;
            }
        }
        case 180: {
            if (tapsInPercents >= 50) {
                endY = (480 * tapsInPercents) / 100 - endY;
            } else {
                endX = endY = 0;
            }
        }
        case 270: {
            if (tapsInPercents <= 50) {
                startX = endX - (480 * tapsInPercents) / 100;
            }
        }
    }

    abi_CMD_G2D_ADD_RECTANGLE(startX, startY, endX, endY, GetTapFillColor(tapsInPercents));
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

    abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR);

    if (fillTapTutorial) {
        TapFillBackground(TOPOLOGY_getAngle(SetFacelet(abi_cubeN, screen), TOPOLOGY_orientation:ORIENTATION_GRAVITY));
    }
    switch (screenLayout) {
        case MASCOT: {
            abi_CMD_G2D_ADD_SPRITE(CIRCLE_QUARTER, false, 60, 60, 0xFF, 0, 0, MIRROR_BLANK);

            if (mascotTapReactAnimFlag) {
                MascotTapReaction();
                abi_CMD_G2D_ADD_SPRITE(CIRCLE_QUARTER_PUSH, false, 60, 60, 0xFF, 0, 0, MIRROR_BLANK);
            }
            abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 210, 210, 0xFF, 0, mascotTapReactAngle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(DIALOGUE_SMALL, false, 91, 120, 0xFF, 0, angle, MIRROR_BLANK);
            if (beginTapTutorial) {
                abi_CMD_TEXT("tap!", 0, 55, 130, 17, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xA5, 0x52, true);
            } else {
                abi_CMD_TEXT("moving on", 0, 90, 120, SIMPLE_FONT_SIZE + 1, angle, TEXT_ALIGN_TOP_CENTER, 0x54, 0x26, 0x82, true);
                abi_CMD_TEXT("to the tap", 0, 45, 120, SIMPLE_FONT_SIZE + 1, angle, TEXT_ALIGN_BOTTOM_CENTER, 0x54, 0x26, 0x82, true);
            }
        }
        case TEXT_BOX: {
            if (fillTapTutorial) {
                abi_CMD_G2D_ADD_SPRITE(CIRCLE_QUARTER, false, 60, 60, 0xFF, 0, 0, MIRROR_BLANK);
                
                if (mascotTapReactAnimFlag) {
                    MascotTapReaction();
                    abi_CMD_G2D_ADD_SPRITE(CIRCLE_QUARTER_PUSH, false, 60, 60, 0xFF, 0, 0, MIRROR_BLANK);
                }

                abi_CMD_TEXT("tap to fill", 0, 170, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("the cube",    0, 210, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
            } else {
                abi_CMD_TEXT("double tap", 0, 120, 140, 9, angle, TEXT_ALIGN_CENTER, 0xFF, 0xA5, 0x52, true);
                abi_CMD_TEXT("the cube!",  0, 120, 100, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            }
        }
        case EMPTY: {
            if (fillTapTutorial) {
                abi_CMD_G2D_ADD_SPRITE(CIRCLE_QUARTER, false, 60, 60, 0xFF, 0, 0, MIRROR_BLANK);

                if (mascotTapReactAnimFlag) {
                    MascotTapReaction();
                    if ((angle % 180) == 0) {
                        abi_CMD_LINE(165,  86, 177,  86, 0xFF, 0xFF, 0xFF, 6, true)
                        abi_CMD_LINE( 86, 165,  86, 177, 0xFF, 0xFF, 0xFF, 6, true)
                        abi_CMD_LINE(146, 146, 156, 156, 0xFF, 0xFF, 0xFF, 6, true)
                    }
                    abi_CMD_G2D_ADD_SPRITE(CIRCLE_QUARTER_PUSH, false, 60, 60, 0xFF, 0, 0, MIRROR_BLANK);
                }
            }
        }
        case ICON: {
            abi_CMD_G2D_ADD_SPRITE(TAP_ICON, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
        }
        case WRONG_WAY: {
            abi_CMD_G2D_ADD_SPRITE(WRONG_TAP_ICON, false, 90, 120, 0xFF, 0, angle, MIRROR_BLANK);
            if (wrongTapRimFlag) {
                new step = (FixedCos(((wrongTapRimTimer += 10)) % 359) + 256) >> 1;
                abi_CMD_G2D_ADD_SPRITE(WRONG_TAP_RIM,  false, 90, 120, step, 0, angle, MIRROR_BLANK);
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
            abi_CMD_TEXT("not a soft", 0, 170, 120, SIMPLE_FONT_SIZE + 1, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("touch",      0, 210, 120, SIMPLE_FONT_SIZE + 1, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
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
            abi_CMD_G2D_ADD_SPRITE(RIGHT_TAP_RIM_1,  false, 120, 90, rightTapRimAlpha_1, 0, angle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(RIGHT_TAP_RIM_2,  false, 120, 90, rightTapRimAlpha_2, 0, angle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(RIGHT_TAP_RIM_3,  false, 120, 90, rightTapRimAlpha_3, 0, angle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(RIGHT_TAP_ICON, false, 120, 90, 0xFF, 0, angle, MIRROR_BLANK);
            abi_CMD_TEXT("but a light", 0, 120, 170, SIMPLE_FONT_SIZE + 1, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("knock",       0, 120, 210, SIMPLE_FONT_SIZE + 1, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
        case FINGER_ANIM: {
            FingerTapAnimation();
            abi_CMD_G2D_ADD_SPRITE(FINGER_SPRITE, false, 180, 200, 0xFF, 0, angle - 10 + fingerAngle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 120, 60 - mascotPosY, 0xFF, 0, angle - 10, MIRROR_BLANK);
            if ((tapAnimationFrame >= 5) && tapAnimationFrame <= 7) {
                abi_CMD_LINE(188, 163, 170, 155, 0xFF, 0xFF, 0xFF, 6, true);
                abi_CMD_LINE( 95, 180,  92, 195, 0xFF, 0xFF, 0xFF, 6, true);
                abi_CMD_LINE( 90, 160,  72, 168, 0xFF, 0xFF, 0xFF, 6, true);
            }
        }
        case BUBBLE_TEXT: {
            abi_CMD_G2D_ADD_SPRITE(SPEECH_BUBBLE_TAP_SPRITE, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
            abi_CMD_TEXT("double tap", 0, 140, 140, 9, angle, TEXT_ALIGN_CENTER, 0xFF, 0xA5, 0x52, true);
            abi_CMD_TEXT("the cube!",  0, 100, 140, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
        }
    }

    if (screen == 2) {
        alreadyCheckTapReaction = 0;
    }
}

UpdateTapTutorial(deltaTime) {
    if (abi_cubeN == 0) {
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
                abi_CMD_PLAYSND(TAP_STAGE_SUCCESS_SOUND, SOUND_VOLUME);
            }

            if(abi_MTD_GetTapsCount() >= 1) {
                ++tapTutorialStage;
                if (tapTutorialStage >= 5) {
                    abi_CMD_PLAYSND(TAPS_5_6_SOUND, SOUND_VOLUME);
                } else if (tapTutorialStage >= 3) {
                    abi_CMD_PLAYSND(TAPS_3_4_SOUND, SOUND_VOLUME);
                } else if (tapTutorialStage >= 0) {
                    abi_CMD_PLAYSND(TAPS_1_2_SOUND, SOUND_VOLUME);
                }
                mascotTapReactAnimFlag = 1;
                fulfillmentTimer = 0;
            }
        }
        if (!fillTapTutorial && beginTapTutorial && (abi_MTD_GetTapsCount() == 2)) {
            fillTapTutorial = 1;
        }
    }
}