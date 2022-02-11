#include "getstarted_vars.inc"

const MASCOT     = 0;
const GREETING   = 1;
const ICONS      = 2;
const SPEECH_BOX = 3;
const TEXT_BOX   = 4;
const COUNTDOWN  = 5;

const TUTORIAL_COUNTDOWN_TIME = 5000;
const DISPLAY_ONE_ICON_TIME = 1250;

new tutorialStartTimer = TUTORIAL_COUNTDOWN_TIME - 1;

new getstarted_startLayout{} = {
    SPEECH_BOX, MASCOT, GREETING, ICONS,
    SPEECH_BOX, MASCOT, GREETING, ICONS,
    COUNTDOWN, COUNTDOWN, TEXT_BOX, COUNTDOWN,
    TEXT_BOX, COUNTDOWN, COUNTDOWN, COUNTDOWN,
    COUNTDOWN, TEXT_BOX, COUNTDOWN, COUNTDOWN,
    COUNTDOWN, COUNTDOWN, TEXT_BOX, COUNTDOWN,
};

new tangibleIcons[MAX_TANGIBLE_ICONS];
new currentTangibleIcon = 0;

const GREETING_TIME = 1000;

new getstartedGreetingFlag = 1;
new getstartedGreetingTimer = GREETING_TIME;

DrawStartCountdown(screen) {
    new angle = getStarted_screenData[screen].angle;
    new screenLayout = getstarted_startLayout{getStarted_screenData[screen].sideType * FACES_ON_PLANE_MAX + angle / 90};
    abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR);
    switch (screenLayout) {
        case MASCOT: {
            abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(DIALOGUE, false, 31, 120, 0xFF, 0, angle, MIRROR_BLANK);
        }
        case GREETING: {
            InitCrossAnimationVars(0xFF5C00, 0x513469);
            DrawCrossAnimation();
            if (getstartedGreetingFlag) {
                abi_CMD_TEXT("hello", 0, 120, 120, 15, angle, TEXT_ALIGN_CENTER, 0xFF, 0xA5, 0x52, true);
            } else {
                abi_CMD_TEXT("we will", 0, 120, 170, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("start in", 0, 120, 130, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT_ITOA(tutorialStartTimer / 1000, 0, 120, 70, 20, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xA5, 0x52, true);
            }
        }
        case ICONS: {
            abi_CMD_G2D_ADD_SPRITE(tangibleIcons[currentTangibleIcon], false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
        }
        case SPEECH_BOX: {
            abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFFFFFF);
            InitCrossAnimationVars(0xFF5C00, 0xFFB576);
            DrawCrossAnimation();
            abi_CMD_TEXT("here you can", 0, 120,  90, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
            abi_CMD_TEXT("learn how to", 0, 120, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
            abi_CMD_TEXT("use wowcube", 0, 120, 150, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
        }
        case TEXT_BOX: {
            if (!getstartedGreetingFlag) {
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
                abi_CMD_TEXT("we will", 0, posX_1, posY_1, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("start in", 0, posX_2, posY_2, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
            }
        }
        case COUNTDOWN: {
            if (!getstartedGreetingFlag) {
                abi_CMD_TEXT_ITOA(tutorialStartTimer / 1000, 0, 120, 120, 25, angle, TEXT_ALIGN_CENTER, 0xFF, 0xA5, 0x52, true);
            }
        }
    }
}

UpdateStartCountdown(deltaTime) {    
    if (abi_cubeN == 0) {
        if (getstartedGreetingFlag) {
            getstartedGreetingTimer -= deltaTime;
        } else {
            if ((abi_leftCubeN(abi_cubeN, 0) < CUBES_MAX) 
            && (abi_topCubeN(abi_cubeN, 0) < CUBES_MAX) 
            && (abi_topCubeN(abi_cubeN, abi_rightFaceN(abi_cubeN, 0)) < CUBES_MAX)) {
                tutorialStartTimer -= deltaTime;
            }
            if (tutorialStartTimer <= 0) {
                SetApplicationState(FSM:twistTutorial);
                abi_CMD_PLAYSND(ACTION_SOUND, SOUND_VOLUME);
            }
        }
        if (getstartedGreetingTimer <= 0) {
            getstartedGreetingFlag = 0;
        }
    }
    currentTangibleIcon = (tutorialStartTimer / DISPLAY_ONE_ICON_TIME) % MAX_TANGIBLE_ICONS;
}