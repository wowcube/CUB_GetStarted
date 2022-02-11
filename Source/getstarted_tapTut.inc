#include "getstarted_vars.inc"

const MASCOT     = 0;
const EMPTY      = 1;
const ICON       = 2;
const TEXT_BOX   = 3;

new getstarted_tapTutorial{} = {
    EMPTY, MASCOT, EMPTY, TEXT_BOX,
    EMPTY, MASCOT, EMPTY, TEXT_BOX,
    EMPTY, EMPTY, EMPTY, TEXT_BOX,
    EMPTY, EMPTY, EMPTY, TEXT_BOX,
    EMPTY, EMPTY, EMPTY, TEXT_BOX,
    EMPTY, EMPTY, EMPTY, TEXT_BOX,
};

const FULFILLMENT_DROP_TIME = 3000;
const TAPS_TO_FINISH_TUTORIAL = 6;
new fulfillmentTimer = 0;
new tapTutorialStage = 0;
new beginTapTutorial = 0;

new alreadyCheckTapReaction = 0;

new mascotTapReactAnimFlag = 0;
new mascotTapReactAngle = 75;
new tapAngleAnimChange = 4;

const START_TAP_TUT_TIME = 5000;
new startTapTutTimer = 0;

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

DrawTapTutorial(screen) {
    new angle = getStarted_screenData[screen].angle;
    
    new screenLayout = (getstarted_tapTutorial{getStarted_screenData[screen].sideType * FACES_ON_PLANE_MAX + angle / 90});

    abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR);

    if (beginTapTutorial) {
        TapFillBackground(TopologyGetAngle(abi_cubeN, screen, topology_projection:projection_gravity));
    }
    switch (screenLayout) {
        case MASCOT: {
            abi_CMD_G2D_ADD_SPRITE(CIRCLE_QUARTER, false, 60, 60, 0xFF, 0, 0, MIRROR_BLANK);

            if (mascotTapReactAnimFlag) {
                MascotTapReaction();
                abi_CMD_G2D_ADD_SPRITE(CIRCLE_QUARTER_PUSH, false, 60, 60, 0xFF, 0, 0, MIRROR_BLANK);
            }
            abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 210, 210, 0xFF, 0, mascotTapReactAngle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(DIALOGUE_SMALL, false, 91, 120, 0xFF, 0, 90, MIRROR_BLANK);
            if (beginTapTutorial) {
                abi_CMD_TEXT("tap!", 0, 55, 130, 17, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xA5, 0x52, true);
            } else {
                abi_CMD_TEXT("moving on", 0, 90, 120, SIMPLE_FONT_SIZE + 1, angle, TEXT_ALIGN_TOP_CENTER, 0x54, 0x26, 0x82, true);
                abi_CMD_TEXT("to the tap", 0, 45, 120, SIMPLE_FONT_SIZE + 1, angle, TEXT_ALIGN_BOTTOM_CENTER, 0x54, 0x26, 0x82, true);
            }
        }
        case TEXT_BOX: {
            abi_CMD_G2D_ADD_SPRITE(CIRCLE_QUARTER, false, 60, 60, 0xFF, 0, 0, MIRROR_BLANK);
            
            if (mascotTapReactAnimFlag) {
                MascotTapReaction();
                abi_CMD_G2D_ADD_SPRITE(CIRCLE_QUARTER_PUSH, false, 60, 60, 0xFF, 0, 0, MIRROR_BLANK);
            }

            abi_CMD_TEXT("tap to fill", 0, 170, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("the cube", 0, 210, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
        case EMPTY: {
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

    if (screen == 2) {
        alreadyCheckTapReaction = 0;
    }
}

UpdateTapTutorial(deltaTime) {
    if (abi_cubeN == 0) {
        if (!beginTapTutorial) {
            startTapTutTimer += deltaTime;
        } else {
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
        if ((startTapTutTimer >= START_TAP_TUT_TIME) || (abi_MTD_GetTapsCount() == 1)) {
            beginTapTutorial = 1;
        }
    }
}