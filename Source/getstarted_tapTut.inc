#include "getstarted_vars.inc"

const MASCOT     = 0;
const EMPTY      = 1;
const ICON       = 2;
const SPEECH_BOX = 3;
const TEXT_BOX   = 4;
const TEXT_BOX_2 = 5;
const TAP_CIRCLE = 6;

new getstarted_tapTutorial{} = {
    TEXT_BOX, MASCOT, SPEECH_BOX, EMPTY,
    TEXT_BOX, MASCOT, SPEECH_BOX, EMPTY,
    EMPTY, ICON, EMPTY, TEXT_BOX,
    EMPTY, TEXT_BOX, EMPTY, ICON,
    ICON, EMPTY, TEXT_BOX, EMPTY,
    EMPTY, ICON, EMPTY, TEXT_BOX,
};

new getstarted_tapTutorialFill{} = {
    TAP_CIRCLE, TAP_CIRCLE, TAP_CIRCLE, TAP_CIRCLE,
    EMPTY, ICON, EMPTY, TEXT_BOX_2,
    EMPTY, ICON, EMPTY, TEXT_BOX_2,
    EMPTY, ICON, EMPTY, TEXT_BOX_2,
    EMPTY, ICON, EMPTY, TEXT_BOX_2,
    EMPTY, ICON, EMPTY, TEXT_BOX_2,
};

const FULFILLMENT_DROP_TIME = 10000;
const TAPS_TO_FINISH_TUTORIAL = 6;
new fulfillmentTimer = 0;
new tapTutorialStage = 0;
new beginTapTutorial = 0;
new finishTapTutorial = 0;

new mascotTapReactAnimFlag = 0;
new mascotTapReactAngle = 75;
new tapAngleAnimChange = 4;

MascotTapReaction() {
    mascotTapReactAngle += tapAngleAnimChange;
    if ((mascotTapReactAngle == 87) || (mascotTapReactAngle == 75)) {
        tapAngleAnimChange = -tapAngleAnimChange;
    }
    if (mascotTapReactAngle == 75) {
        mascotTapReactAnimFlag = 0;
    }
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
    abi_CMD_G2D_ADD_RECTANGLE(startX, startY, endX, endY, 0xFF68398D);
}

DrawTapTutorial(screen) {
    new angle = getStarted_screenData[screen].angle;
    new screenLayout = (beginTapTutorial) ? 
                       (getstarted_tapTutorialFill{getStarted_screenData[screen].sideType * FACES_ON_PLANE_MAX + angle / 90}) : 
                       (getstarted_tapTutorial{getStarted_screenData[screen].sideType * FACES_ON_PLANE_MAX + angle / 90});
    
    abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR);

    if (beginTapTutorial) {
        TapFillBackground(TopologyGetAngle(abi_cubeN, screen, topology_projection:projection_gravity));
    }
    switch (screenLayout) {
        case MASCOT: {
            abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(DIALOGUE, false, 120, 31, 0xFF, 0, 180, MIRROR_BLANK);
        }
        case ICON: {
            if (!finishTapTutorial) {
                abi_CMD_G2D_ADD_SPRITE(TAP_ICON, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
            }
        }
        case SPEECH_BOX: {
            abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFFFFFF);
            InitCrossAnimationVars(0xFF5C00, 0xFFB576);
            DrawCrossAnimation();
            abi_CMD_TEXT("let us continue", 0, 120, 142, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0x54, 0x26, 0x82, true);
            abi_CMD_TEXT("with the tap", 0, 120, 98, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0x54, 0x26, 0x82, true);
        }
        case TEXT_BOX: {
            new posX_1 = 120;
            new posY_1 = 100;
            new posX_2 = 120;
            new posY_2 = 140;
            if (angle == 90) {
                posX_1 = 140;
                posY_1 = 120;
                posX_2 = 100;
                posY_2 = 120;
            } else if (angle == 180) {
                posX_1 = 120;
                posY_1 = 140;
                posX_2 = 120;
                posY_2 = 100;
            } else if (angle == 270) {
                posX_1 = 100;
                posY_1 = 120;
                posX_2 = 140;
                posY_2 = 120;
            }
            if ((getStarted_screenData[screen].sideType == topology_location:location_bottom) ||
                (getStarted_screenData[screen].sideType == topology_location:location_top)) {
                InitCrossAnimationVars(0xFF5C00, 0x513469);
                DrawCrossAnimation();
            }
            abi_CMD_TEXT("just lightly", 0, posX_1, posY_1, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("tap the cube", 0, 120, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("to continue",  0, posX_2, posY_2, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
        case TEXT_BOX_2: {
            if (finishTapTutorial) {
                abi_CMD_TEXT("excellent!", 0, 210, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
            } else {
                abi_CMD_TEXT("tap to fill", 0, 103, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("the cube", 0, 145, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
            }
        }
        case TAP_CIRCLE: {
            abi_CMD_G2D_ADD_SPRITE(CIRCLE_QUARTER, false, 60, 60, 0xFF, 0, 0, MIRROR_BLANK);
            if (angle == 90) {
                if (mascotTapReactAnimFlag) {
                    MascotTapReaction();
                }
                abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 210, 210, 0xFF, 0, mascotTapReactAngle, MIRROR_BLANK);
            } else if (angle == 270) {
                if (finishTapTutorial) {
                    abi_CMD_TEXT("excellent!", 0, 210, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
                } else {
                    abi_CMD_TEXT("tap to fill", 0, 168, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xFF, 0xFF, true);
                    abi_CMD_TEXT("the cube", 0, 210, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
                }
            } else if ((finishTapTutorial) && (angle == 0)) {
                abi_CMD_TEXT("double tap", 0, 120, 168, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("to continue", 0, 120, 210, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
            }
        }
        case EMPTY: {
            if ((finishTapTutorial) && (angle == 0)) {
                abi_CMD_TEXT("double tap", 0, 120, 168, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("to continue", 0, 120, 210, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
            }
        }
    }
}

UpdateTapTutorial(deltaTime) {
    if (abi_cubeN == 0) {
        if (finishTapTutorial) {
            if (abi_MTD_GetTapsCount() >= 2) {
                SetApplicationState(FSM:successScreen);
            }
        } else {
            if (beginTapTutorial) {
                if (fulfillmentTimer >= FULFILLMENT_DROP_TIME) {
                    tapTutorialStage = 0;
                    fulfillmentTimer = 0;
                } else {
                    fulfillmentTimer += deltaTime;
                }
                if(abi_MTD_GetTapsCount() >= 1) {
                    //tapTutorialStage = abi_MTD_GetTapsCount();
                    ++tapTutorialStage;
                    if (tapTutorialStage > TAPS_TO_FINISH_TUTORIAL) {
                        tapTutorialStage = TAPS_TO_FINISH_TUTORIAL;
                    }
                    fulfillmentTimer = 0;
                    //SetDefaultMascot();
                    if (tapTutorialStage >= TAPS_TO_FINISH_TUTORIAL) {
                        finishTapTutorial = 1;
                    }
                }
            }
            if ((!beginTapTutorial) && (abi_MTD_GetTapsCount() >= 1)) {
                ++beginTapTutorial;
            }
        }
    }
}