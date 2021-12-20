#include "getstarted_vars.inc"

const MASCOT     = 0;
const EMPTY      = 1;
const ICON       = 2;
const SPEECH_BOX = 3;
const TEXT_BOX   = 4;
const ARROW      = 5;

new arrowIcons[MAX_ARROW_ICONS];

const MAX_TWIST_TUTORIAL_STAGES = 5;

const ARROW_ARRAY_SHIFT_TIME = 500;
new arrowAnimationTime = 0;

new twistTutorialStage = 0;

new getstarted_twistTutLayout_X{} = {
    ARROW, ARROW, SPEECH_BOX, MASCOT,
    ARROW, ARROW, SPEECH_BOX, MASCOT,
    ARROW, ARROW, ARROW, ARROW,
    ARROW, ARROW, ARROW, ARROW,
    ICON, EMPTY, TEXT_BOX, EMPTY,
    EMPTY, ICON, EMPTY, TEXT_BOX,
};
new getstarted_twistTutLayout_Y{} = {
    ARROW, MASCOT, SPEECH_BOX, ARROW,
    ARROW, MASCOT, SPEECH_BOX, ARROW,
    ICON, EMPTY, TEXT_BOX, EMPTY,
    EMPTY, ICON, EMPTY, TEXT_BOX,
    ARROW, ARROW, ARROW, ARROW,
    ARROW, ARROW, ARROW, ARROW,
};

new mascotTwistAnimTimer = 0;
new mascotTwistAngle = 270;
new mascotTwistAnimFlag = 1;

MascotTwistAnimUpdate(angle) {
    if (((mascotTwistAngle <= (angle + 90)) && (mascotTwistAngle >= angle))
     || ((mascotTwistAngle >= (angle - 90)) && (mascotTwistAngle <= angle))) {
        mascotTwistAngle -= 15;
    } else {
        mascotTwistAngle -= 30;
    }
    mascotTwistAngle %= 360;
    if (mascotTwistAngle == angle) {
        mascotTwistAnimFlag = 0;
    }
}

DrawTwistTutorial(screen) {
    new angle = getStarted_screenData[screen].angle;
    new currentStage = twistTutorialStage % 2;
    new screenLayout = (currentStage) ? 
                       (getstarted_twistTutLayout_Y{getStarted_screenData[screen].sideType * FACES_ON_PLANE_MAX + angle / 90}) : 
                       (getstarted_twistTutLayout_X{getStarted_screenData[screen].sideType * FACES_ON_PLANE_MAX + angle / 90});
    abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR);
    switch (screenLayout) {
        case MASCOT: {
            if (twistTutorialStage) {
                if (currentStage) {
                    abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
                    abi_CMD_G2D_ADD_SPRITE(DIALOGUE, false, 120, 31, 0xFF, 0, 180, MIRROR_BLANK);
                } else {
                    abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 120, 120, 0xFF, 0, angle, MIRROR_Y);
                    abi_CMD_G2D_ADD_SPRITE(DIALOGUE, false, 31, 120, 0xFF, 0, 90, MIRROR_BLANK);
                }
            } else {
                if (mascotTwistAnimFlag) {
                    MascotTwistAnimUpdate(angle);
                }
                abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 120, 120, 0xFF, 0, mascotTwistAngle, MIRROR_Y);
                abi_CMD_G2D_ADD_SPRITE(DIALOGUE, false, 31, 120, 0xFF, 0, 90, MIRROR_BLANK);
            }
        }
        case ICON: {
            abi_CMD_G2D_ADD_SPRITE(TWIST_ICON, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
        }
        case SPEECH_BOX: {
            abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFFFFFF);
            InitCrossAnimationVars(0xFF5C00, 0xFFB576);
            DrawCrossAnimation();
            if (twistTutorialStage > 1) {
                abi_CMD_TEXT("twist", 0, 120, 160, 17, angle, TEXT_ALIGN_TOP_CENTER, 0x54, 0x26, 0x82, true);
                abi_CMD_TEXT("twist", 0, 120,  70, 17, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xA5, 0x52, true);
            } else if (twistTutorialStage == 1) {
                abi_CMD_TEXT("good!",            0, 120, 130, 17, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xA5, 0x52, true);
                abi_CMD_TEXT("one more time!", 0, 120,  98, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0x54, 0x26, 0x82, true);
                abi_CMD_TEXT("twist another",  0, 120,  78,  SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0x54, 0x26, 0x82, true);
                abi_CMD_TEXT("side",           0, 120,  58,  SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0x54, 0x26, 0x82, true);
            } else {
                abi_CMD_TEXT("twist", 0, 120, 140, 17, angle, TEXT_ALIGN_CENTER, 0xFF, 0xA5, 0x52, true);
                abi_CMD_TEXT("the cube once", 0, 120, 100, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
            }
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
            abi_CMD_TEXT("twist the", 0, posX_1, posY_1, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("cube once", 0, posX_2, posY_2, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
        case ARROW: {
            new posX = 0;
            new posY = 0;
            new isMainAxisX = ((angle % 180) == 0);
            if (isMainAxisX) {
                posX = 120;
                posY = 80;
                angle = 0;
                if (currentStage) {
                    posY = 120;
                    posX = 80;
                    angle = 90;
                    isMainAxisX = 0;
                }
            } else {
                posY = 120;
                posX = 80;
                angle = 90;
                if (currentStage) {
                    posX = 120;
                    posY = 80;
                    angle = 0;
                    isMainAxisX = 1;
                }
            }
            if (!isMainAxisX) {
                //abi_CMD_G2D_ADD_SPRITE(arrowIcons[0], false, posX, posY, 0xFF, 0, angle, MIRROR_BLANK);
                //abi_CMD_G2D_ADD_SPRITE(arrowIcons[1], false, posX + 40 * !isMainAxisX, posY + 40 * isMainAxisX, 0xFF, 0, angle, MIRROR_BLANK);
                //abi_CMD_G2D_ADD_SPRITE(arrowIcons[2], false, posX + 80 * !isMainAxisX, posY + 80 * isMainAxisX, 0xFF, 0, angle, MIRROR_BLANK);
                abi_CMD_G2D_ADD_SPRITE(ARROW_1, false, posX, posY, arrowIcons[0], 0, angle, MIRROR_BLANK);
                abi_CMD_G2D_ADD_SPRITE(ARROW_1, false, posX + 40 * !isMainAxisX, posY + 40 * isMainAxisX, arrowIcons[1], 0, angle, MIRROR_BLANK);
                abi_CMD_G2D_ADD_SPRITE(ARROW_1, false, posX + 80 * !isMainAxisX, posY + 80 * isMainAxisX, arrowIcons[2], 0, angle, MIRROR_BLANK);
            } else {
                //abi_CMD_G2D_ADD_SPRITE(arrowIcons[2], false, posX, posY, 0xFF, 0, angle, MIRROR_BLANK);
                //abi_CMD_G2D_ADD_SPRITE(arrowIcons[1], false, posX + 40 * !isMainAxisX, posY + 40 * isMainAxisX, 0xFF, 0, angle, MIRROR_BLANK);
                //abi_CMD_G2D_ADD_SPRITE(arrowIcons[0], false, posX + 80 * !isMainAxisX, posY + 80 * isMainAxisX, 0xFF, 0, angle, MIRROR_BLANK);
                abi_CMD_G2D_ADD_SPRITE(ARROW_1, false, posX, posY, arrowIcons[2], 0, angle, MIRROR_BLANK);
                abi_CMD_G2D_ADD_SPRITE(ARROW_1, false, posX + 40 * !isMainAxisX, posY + 40 * isMainAxisX, arrowIcons[1], 0, angle, MIRROR_BLANK);
                abi_CMD_G2D_ADD_SPRITE(ARROW_1, false, posX + 80 * !isMainAxisX, posY + 80 * isMainAxisX, arrowIcons[0], 0, angle, MIRROR_BLANK);
            }
        }
    }
}

UpdateTwistTutorial(deltaTime) {
    arrowAnimationTime += deltaTime;
    if (arrowAnimationTime >= ARROW_ARRAY_SHIFT_TIME) {
        arrowAnimationTime = 0;
        new lastArrowElement = arrowIcons[MAX_ARROW_ICONS - 1];
        for(new arrowI = MAX_ARROW_ICONS - 1; arrowI >= 1; --arrowI) {
            arrowIcons[arrowI] = arrowIcons[arrowI - 1];
        }
        arrowIcons[0] = lastArrowElement;
    }

    if (!twistTutorialStage && !mascotTwistAnimFlag) {
        if (mascotTwistAnimTimer >= MASCOT_WAIT_TIME) {
            mascotTwistAnimTimer = 0;
            mascotTwistAnimFlag = 1;
        } else {
            mascotTwistAnimTimer += deltaTime;
        }
    }

    if (abi_cubeN == 0) {
        if (twistTutorialStage >= 5) {
            SetApplicationState(FSM:successScreen);
        }
    }
}