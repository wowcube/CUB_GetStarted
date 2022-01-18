#include "getstarted_vars.inc"

const MASCOT     = 0;
const EMPTY      = 1;
const ICON       = 2;
const SPEECH_BOX = 3;
const TEXT_BOX   = 4;
const TEXT_BOX_2 = 5;
const MASCOT_2   = 6;

new getstarted_shakeTutorial{} = {
    EMPTY, EMPTY, SPEECH_BOX, MASCOT,
    EMPTY, EMPTY, SPEECH_BOX, MASCOT,
    EMPTY, ICON, EMPTY, TEXT_BOX,
    EMPTY, TEXT_BOX, EMPTY, ICON,
    ICON, EMPTY, TEXT_BOX, EMPTY,
    EMPTY, ICON, EMPTY, TEXT_BOX,
};

new getstarted_shakeTutorialFill{} = {
    EMPTY, MASCOT_2, EMPTY, TEXT_BOX_2,
    EMPTY, EMPTY, EMPTY, TEXT_BOX_2,
    EMPTY, EMPTY, EMPTY, TEXT_BOX_2,
    EMPTY, EMPTY, EMPTY, TEXT_BOX_2,
    EMPTY, EMPTY, EMPTY, TEXT_BOX_2,
    EMPTY, EMPTY, EMPTY, TEXT_BOX_2,
};

new shakeTutorialStage = 0;
new beginShakeTutorial = 0;

new mascotShakeAnimFlag = 1;
new mascotShakePosX = 120;
new mascotShakeStep = -10;
new mascotShakeAnimTimer = 0;

MascotShakeAnimation() {
    mascotShakePosX += mascotShakeStep;
    if ((mascotShakePosX == 90) || (mascotShakePosX == 150)) {
        mascotShakeStep = -mascotShakeStep;
        ++mascotShakeAnimFlag;
    }

    if ((mascotShakeAnimFlag == 6) && (mascotShakePosX == 120)) {
        mascotShakeAnimFlag = 0;
        mascotShakeStep = -mascotShakeStep;
    }
}

GetShakeFillColor(shakeInPercents) {
    return (0xFF000000
    | 0xFA + ((0xFF - 0xFA) * (100 - shakeInPercents)) / 100 << 16
    | 0x46 + ((0xA5 - 0x46) * (100 - shakeInPercents)) / 100 << 8
    | 0x46 + ((0x52 - 0x46) * (100 - shakeInPercents)) / 100);
}

ShakeFillBackground(angle) {
    new shakeInPercents = (shakeTutorialStage * 100) / SENSITIVITY_MENU_CHANGE_SCRIPT;
    new startX = 0;
    new startY = 0;
    new endX = 240;
    new endY = 240;
    switch (angle) {
        case 0: {
            if (shakeInPercents <= 50) {
                startY = endY - (480 * shakeInPercents) / 100;
            }
        }
        case 90: {
            if (shakeInPercents >= 50) {
                endX = (480 * shakeInPercents) / 100 - endX;
            } else {
                endX = endY = 0;
            }
        }
        case 180: {
            if (shakeInPercents >= 50) {
                endY = (480 * shakeInPercents) / 100 - endY;
            } else {
                endX = endY = 0;
            }
        }
        case 270: {
            if (shakeInPercents <= 50) {
                startX = endX - (480 * shakeInPercents) / 100;
            }
        }
    }
    abi_CMD_G2D_ADD_RECTANGLE(startX, startY, endX, endY, GetShakeFillColor(shakeInPercents));
}

DrawShakeTutorial(screen) {
    new angle = getStarted_screenData[screen].angle;
    new screenLayout = (beginShakeTutorial) ? 
                       (getstarted_shakeTutorialFill{getStarted_screenData[screen].sideType * FACES_ON_PLANE_MAX + angle / 90}) : 
                       (getstarted_shakeTutorial{getStarted_screenData[screen].sideType * FACES_ON_PLANE_MAX + angle / 90});
    
    abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR);

    if (beginShakeTutorial) {
        ShakeFillBackground(angle);
    }
    switch (screenLayout) {
        case MASCOT: {
            if (mascotShakeAnimFlag) {
                MascotShakeAnimation();
            }
            abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, mascotShakePosX, 120, 0xFF, 0, angle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(DIALOGUE, false, 31, 120, 0xFF, 0, 90, MIRROR_BLANK);
        }
        case ICON: {
            abi_CMD_G2D_ADD_SPRITE(SHAKE_ICON, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
        }
        case SPEECH_BOX: {
            abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFFFFFF);
            InitCrossAnimationVars(0xFF5C00, 0xFFB576);
            DrawCrossAnimation();
            abi_CMD_TEXT("shake", 0, 120, 180, 18, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xA5, 0x52, true);
            abi_CMD_TEXT("the cube up and", 0, 120, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0x54, 0x26, 0x82, true);
            abi_CMD_TEXT("down to go back to", 0, 120, 90, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
            abi_CMD_TEXT("the main menu", 0, 120, 60, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0x54, 0x26, 0x82, true);
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
            abi_CMD_TEXT("shake", 0, posX_1, posY_1, 6, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("the cube up", 0, 120, 120, 6, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("and down",  0, posX_2, posY_2, 6, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
        case TEXT_BOX_2: {
            abi_CMD_TEXT("shake the", 0, 100, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("cube to go back", 0, 120, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("to main menu",  0, 140, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
        case MASCOT_2: {
            if (!shakeTutorialStage) {
                abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 210, 210, 0xFF, 0, 75, MIRROR_BLANK);
                abi_CMD_G2D_ADD_SPRITE(DIALOGUE_SMALL, false, 91, 120, 0xFF, 0, 90, MIRROR_BLANK);
                abi_CMD_TEXT("shake!", 0, 70, 130, 16, angle, TEXT_ALIGN_CENTER, 0xFF, 0xA5, 0x52, true);
            }
        }
    }
}

UpdateShakeTutorial(deltaTime) {
    if (!beginShakeTutorial && !mascotShakeAnimFlag) {
        if (mascotShakeAnimTimer >= MASCOT_WAIT_TIME) {
            mascotShakeAnimTimer = 0;
            mascotShakeAnimFlag = 1;
        } else {
            mascotShakeAnimTimer += deltaTime;
        }
    }

    if (abi_cubeN == 0) {
        if (beginShakeTutorial) {
            shakeTutorialStage = abi_MTD_GetShakesCount();

            if (!dataSaved) {
                SaveData();
            }

            abi_checkShake();
        }
        if (!beginShakeTutorial && (abi_MTD_GetShakesCount() > 0)) {
            beginShakeTutorial = 1;
        }
    }
}