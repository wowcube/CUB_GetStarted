#define GS_BALL_DATA .posX, .posY, .angle, .screenAngle, .module, .screen, .moduleT, .screenT, .collectables
#define GS_SELECTOR_DATA .accelX, .accelY, .screenAngle, .module, .screen, .moduleT, .screenT

#include "getstarted_vars.inc"

const EMPTY               = 0;
const ICON                = 1;
const TEXT_BOX            = 2;
const COLLECT_CIRCLE      = 3;
const ARROW_HINT          = 4;
const FINISHED_TOP        = 5;
const FINISHED_DOUBLE_TAP = 6;
const FINISHED_EXCELLENT  = 7;
const SELECTOR_TOP        = 8;
const SELECTOR_TEXT_BOX   = 9;

new getstarted_tiltTutCollect{} = {
    COLLECT_CIRCLE, COLLECT_CIRCLE, COLLECT_CIRCLE, COLLECT_CIRCLE,
    EMPTY, ICON, EMPTY, TEXT_BOX,
    EMPTY, ICON, ARROW_HINT, TEXT_BOX,
    ARROW_HINT, TEXT_BOX, EMPTY, ICON,
    ICON, ARROW_HINT, TEXT_BOX, EMPTY,
    TEXT_BOX, EMPTY, ICON, ARROW_HINT,
};

new getstarted_tiltTutFinished{} = {
    FINISHED_TOP, FINISHED_TOP, FINISHED_TOP, FINISHED_TOP,
    FINISHED_DOUBLE_TAP, EMPTY, FINISHED_EXCELLENT, EMPTY,
    FINISHED_DOUBLE_TAP, EMPTY, FINISHED_EXCELLENT, EMPTY,
    FINISHED_DOUBLE_TAP, EMPTY, FINISHED_EXCELLENT, EMPTY,
    FINISHED_DOUBLE_TAP, EMPTY, FINISHED_EXCELLENT, EMPTY,
    FINISHED_DOUBLE_TAP, EMPTY, FINISHED_EXCELLENT, EMPTY,
};

new getstarted_selectorTutorial{} = {
    SELECTOR_TOP, SELECTOR_TOP, SELECTOR_TOP, SELECTOR_TOP,
    EMPTY, ICON, EMPTY, SELECTOR_TEXT_BOX,
    EMPTY, ICON, EMPTY, SELECTOR_TEXT_BOX,
    EMPTY, SELECTOR_TEXT_BOX, EMPTY, ICON,
    ICON, EMPTY, SELECTOR_TEXT_BOX, EMPTY,
    EMPTY, ICON, EMPTY, SELECTOR_TEXT_BOX,
};

const BALL_SQUARE_RADIUS = 289;
const BALL_ORBIT = 107;
new tiltTutBall[GS_BALL_DATA];

new tiltTutSelector[GS_SELECTOR_DATA];

new finishTiltTutorial = 0;
new selectorTutorial = 0;
new collectables{SCREENS_MAX} = {1, 1, 1};
new collectablesAngles[SCREENS_MAX] = [0, 90, 270];

new ballPkt = 0;
new selectorPkt = 0;

new mascotTiltAnimFlag = 1;
new mascotTiltAnimAngle = 90;
new tiltAngleAnimChange = -4;
new mascotTiltAnimTimer = 0;

new selectorOutAnimation = 0;
new selectorInAnimation = 0;
new selectorYOffset = 0; 
new selectorXOffset = 0;


const PLUS_ONE_MAX_TICKS = 15;
new currentPlusOneTick = 0;
new collectedPlusOneEffectStep = 5;
new collectedPlusOneEffect = 0;
new backPlusOneOffset = 5;
new mainOneX = 75;
new mainOneY = 75;

MovePlusOneEffect(angle) {
    new offsetX = 0;
    new offsetY = 0;
    if (angle == 0) {
        mainOneY = 100 - (currentPlusOneTick * collectedPlusOneEffectStep);
        mainOneX = 50;
        offsetX = -backPlusOneOffset;
        offsetY = -backPlusOneOffset;
    } else if (angle == 90) {
        mainOneX = 50 + (currentPlusOneTick * collectedPlusOneEffectStep);
        mainOneY = 100;
        offsetX =  backPlusOneOffset;
        offsetY = -backPlusOneOffset;
    } else if (angle == 270) {
        mainOneX = 100 - (currentPlusOneTick * collectedPlusOneEffectStep);
        mainOneY = 100;
        offsetX = -backPlusOneOffset;
        offsetY =  backPlusOneOffset;
    }

    abi_CMD_TEXT("+1", 0, mainOneX + offsetX, mainOneY + offsetY, 20, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0x80, 0x0B, true);
    abi_CMD_TEXT("+1", 0, mainOneX, mainOneY, 20, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);

    ++currentPlusOneTick;

    if (currentPlusOneTick > PLUS_ONE_MAX_TICKS) {
        collectedPlusOneEffect = 0;
    }
}

MascotTiltAnimation() {
    mascotTiltAnimAngle += tiltAngleAnimChange;
    if ((mascotTiltAnimAngle == 70) || (mascotTiltAnimAngle == 90)) {
        tiltAngleAnimChange = -tiltAngleAnimChange;
    }
    if (mascotTiltAnimAngle == 90) {
        mascotTiltAnimFlag = 0;
    }
}

CheckCollectables() {
    new alreadyCollected;
    for (new item = 0; item < SCREENS_MAX; ++item) {
        alreadyCollected |= collectables{item} << item;
    }
    return alreadyCollected;
}

SendBall(pktNumber) {
    new data[4];

    data[0] = PKT_BALL_TILT_TUT | (tiltTutBall.module << 8) | (tiltTutBall.screen << 16) | (CheckCollectables() << 24);
    data[1] = tiltTutBall.angle;
    data[2] = tiltTutBall.screenAngle;
    data[3] = pktNumber;

    abi_CMD_NET_TX(0, NET_BROADCAST_TTL_MAX, data);
    abi_CMD_NET_TX(1, NET_BROADCAST_TTL_MAX, data);
    abi_CMD_NET_TX(2, NET_BROADCAST_TTL_MAX, data);
}

SendSelector(pktNumber) {
    new data[4];

    data[0] = PKT_SELECTOR_TILT_TUT | (tiltTutSelector.module << 8) | (tiltTutSelector.screen << 16);
    data[1] = tiltTutSelector.screenAngle;
    data[2] = pktNumber;
    data[3] = (selectorOutAnimation == 1) ? (2) : (1);

    abi_CMD_NET_TX(0, NET_BROADCAST_TTL_MAX, data);
    abi_CMD_NET_TX(1, NET_BROADCAST_TTL_MAX, data);
    abi_CMD_NET_TX(2, NET_BROADCAST_TTL_MAX, data);
}

FindPosDependOnAngle(angle, &posX_1, &posY_1, &posX_2, &posY_2) {
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
}

SelectorLogic() {
    if (tiltTutSelector.module >= MODULES_MAX) {
        for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
            if (getStarted_screenData[screenI].sideType == TOPOLOGY_location:LOCATION_UP) {
                if (getStarted_screenData[screenI].angle == tiltTutSelector.screenAngle) {
                    tiltTutSelector.module = abi_cubeN;
                    tiltTutSelector.screen = screenI;
                    break;
                }
            }
        }
    }

    if (tiltTutSelector.module == abi_cubeN) {
        for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
            if (tiltTutSelector.screen == screenI) {
                tiltTutSelector.moduleT = abi_cubeN;
                tiltTutSelector.screenT = screenI;

                new accelX = -abi_MTD_GetFaceAccelX(screenI);
                new accelY = -abi_MTD_GetFaceAccelY(screenI);

                tiltTutSelector.accelX = tiltTutSelector.accelY = 0;

                accelX *= (accelX <= 0);
                accelY *= (accelY <= 0);
                if (ABS(accelX) > ABS(accelY)) {
                    tiltTutSelector.accelX = accelX >> 1;
                } else {
                    tiltTutSelector.accelY = accelY >> 1;
                }
                
                tiltTutSelector.screenAngle = getStarted_screenData[screenI].angle;
                
                if (accelX < -35) {
                    neighbor = TOPOLOGY_getAdjacentFacelet(SetFacelet(abi_cubeN, screenI), TOPOLOGY_neighbor:NEIGHBOR_LEFT);
                    if (neighbor.connected) {
                        tiltTutSelector.module = neighbor.module;
                        tiltTutSelector.screen = neighbor.screen;
                        tiltTutSelector.screenAngle = (getStarted_screenData[screenI].angle - 90) % 360;
                        selectorOutAnimation = 1;
                    }
                } else if (accelY < -35) {
                    neighbor = TOPOLOGY_getAdjacentFacelet(SetFacelet(abi_cubeN, screenI), TOPOLOGY_neighbor:NEIGHBOR_TOP);
                    if (neighbor.connected) {
                        tiltTutSelector.module = neighbor.module;
                        tiltTutSelector.screen = neighbor.screen;
                        tiltTutSelector.screenAngle = (getStarted_screenData[screenI].angle + 90) % 360;
                        selectorOutAnimation = 2;
                    }
                }

                selectorPkt = ++selectorPkt % 0xFFFF;
                SendSelector(selectorPkt);
                
                break;
            }
        }
    } else if (tiltTutSelector.moduleT == abi_cubeN) {
        SendSelector(selectorPkt);
    }
}

BallLogic() {
    if (tiltTutBall.module >= MODULES_MAX) {
        for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
            if (getStarted_screenData[screenI].sideType == TOPOLOGY_location:LOCATION_UP) {
                if (getStarted_screenData[screenI].angle == tiltTutBall.screenAngle) {
                    tiltTutBall.module = abi_cubeN;
                    tiltTutBall.screen = screenI;
                    break;
                }
            }
        }
    }

    if (tiltTutBall.module >= MODULES_MAX) {
        return;
    }

    new allCollected = 0;
    for (new item = 0; item < SCREENS_MAX; ++item) {
        allCollected += (collectables{item} == 0);
    }
    if (allCollected == SCREENS_MAX) {
        finishTiltTutorial = 1;
    }

    if (tiltTutBall.module == abi_cubeN) {
        for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
            if (tiltTutBall.screen == screenI) {
                tiltTutBall.moduleT = abi_cubeN;
                tiltTutBall.screenT = screenI;

                new accelX = abi_MTD_GetFaceAccelX(screenI) >> 2;
                new accelY = abi_MTD_GetFaceAccelY(screenI) >> 2;

                tiltTutBall.angle += accelX - accelY;

                if (tiltTutBall.angle < 0) {
                    tiltTutBall.angle = 360 + tiltTutBall.angle;
                }
                tiltTutBall.angle %= 360;
                                    
                tiltTutBall.posX = BALL_ORBIT * FixedCos(tiltTutBall.angle) >> FP;
                tiltTutBall.posY = BALL_ORBIT * FixedSin(tiltTutBall.angle) >> FP;

                tiltTutBall.screenAngle = getStarted_screenData[tiltTutBall.screen].angle;

                for (new item = 0; item < SCREENS_MAX; ++item) {
                    if (collectables{item} && (collectablesAngles[item] == tiltTutBall.screenAngle)) {
                        new distance = CheapDistance(75 - tiltTutBall.posX, 75 - tiltTutBall.posY);
                        if (distance < BALL_SQUARE_RADIUS) {
                            collectables{item} = 0;
                            collectedPlusOneEffect = 1;
                            abi_CMD_PLAYSND(tiltTutCollectableSounds{item}, SOUND_VOLUME);
                        }
                    }
                }

                if (tiltTutBall.posX < -HALF_MODULE_GAP) {
                    neighbor = TOPOLOGY_getAdjacentFacelet(SetFacelet(abi_cubeN, screenI), TOPOLOGY_neighbor:NEIGHBOR_LEFT);
                    if (neighbor.connected) {
                        tiltTutBall.module = neighbor.module;
                        tiltTutBall.screen = neighbor.screen;
                        tiltTutBall.angle = 352;
                        tiltTutBall.screenAngle = (getStarted_screenData[tiltTutBall.screen].angle - 90) % 360;
                    }
                } else if (tiltTutBall.posY < -HALF_MODULE_GAP) {
                    neighbor = TOPOLOGY_getAdjacentFacelet(SetFacelet(abi_cubeN, screenI), TOPOLOGY_neighbor:NEIGHBOR_TOP);
                    if (neighbor.connected) {
                        tiltTutBall.module = neighbor.module;
                        tiltTutBall.screen = neighbor.screen;
                        tiltTutBall.angle = 98;
                        tiltTutBall.screenAngle = (getStarted_screenData[tiltTutBall.screen].angle + 90) % 360;
                    }
                }

                ballPkt = ++ballPkt % 0xFFFF;
                SendBall(ballPkt);

                break;
            }
        }
    } else if (tiltTutBall.moduleT == abi_cubeN) {
        SendBall(ballPkt);
    }
}

DrawTiltTutorial(screen) {
    new angle = getStarted_screenData[screen].angle;
    new layoutElement = getStarted_screenData[screen].sideType * TOPOLOGY_POSITIONS_MAX + angle / 90;
    new screenLayout = getstarted_tiltTutCollect{layoutElement};
    if (finishTiltTutorial) {
        screenLayout = getstarted_tiltTutFinished{layoutElement};
    } else if (selectorTutorial) {
        screenLayout = getstarted_selectorTutorial{layoutElement};
    }
    
    abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR);

    switch (screenLayout) {
        case ICON: {
            if (tiltTutSelector.screenAngle != 270) {
                abi_CMD_G2D_ADD_SPRITE(TILT_ICON, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
            } else {
                abi_CMD_G2D_ADD_SPRITE(TAP_ICON, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
            }
        }
        case TEXT_BOX: {
            new posX_1 = 120;
            new posY_1 = 100;
            new posX_2 = 120;
            new posY_2 = 140;
            FindPosDependOnAngle(angle, posX_1, posY_1, posX_2, posY_2);
            
            abi_CMD_TEXT("tilt the cube", 0, posX_1, posY_1, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("and collect", 0, 120, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("all shapes",  0, posX_2, posY_2, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
        case ARROW_HINT: {
            abi_CMD_G2D_ADD_SPRITE(ARROW_TILT, false, 120, 120, 0xFF, 0, 0, MIRROR_BLANK);
        }
        case COLLECT_CIRCLE: {
            abi_CMD_ARC(-15, -15,  95, 4, 0, 90, 0xFF, 0xC2, 0x8A, true);
            abi_CMD_ARC(-15, -15, 160, 4, 0, 90, 0xFF, 0xC2, 0x8A, true);
            
            if (angle == 0) {
                abi_CMD_G2D_ADD_SPRITE((collectables{0}) ? (COLLECTABLE) : (COLLECTED), false, 75, 75, 0xFF, 0, 0, MIRROR_BLANK);
                abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 205, 20, 0xFF, 0, 350, MIRROR_BLANK);
                abi_CMD_G2D_ADD_SPRITE(DIALOGUE_SMALL, false, 120, 147, 0xFF, 0, 0, MIRROR_BLANK);
                abi_CMD_TEXT("collect",    0, 120, 145, SIMPLE_FONT_SIZE + 1, angle, TEXT_ALIGN_TOP_CENTER, 0x54, 0x26, 0x82, true);
                abi_CMD_TEXT("all shapes", 0, 120, 185, SIMPLE_FONT_SIZE + 1, angle, TEXT_ALIGN_BOTTOM_CENTER, 0x54, 0x26, 0x82, true);
            } else if (angle == 90) {
                abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, -50, 205, 0xFF, 0, 80, MIRROR_BLANK);
                abi_CMD_G2D_ADD_SPRITE((collectables{1}) ? (COLLECTABLE) : (COLLECTED), false, 75, 75, 0xFF, 0, 0, MIRROR_BLANK);
            } else if (angle == 180) {
                abi_CMD_G2D_ADD_SPRITE(COUNT_BAR, false, 176, 208, 0xFF, 0, angle, MIRROR_BLANK);
                for (new item = 0; item < SCREENS_MAX; ++item) {
                    if (!collectables{item}) {
                        abi_CMD_G2D_ADD_SPRITE(COLLECTED_CHECK, false, 200 - (24 * item), 208, 0xFF, 0, angle, MIRROR_BLANK);
                    }
                }
            } else if (angle == 270) {
                abi_CMD_TEXT("tilt the cube", 0, 210, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_G2D_ADD_SPRITE((collectables{2}) ? (COLLECTABLE) : (COLLECTED), false, 75, 75, 0xFF, 0, 0, MIRROR_BLANK);
            }

            if (collectedPlusOneEffect) {
                MovePlusOneEffect(angle);
            }

            if ((tiltTutBall.module == abi_cubeN) && (tiltTutBall.screen == screen)) {
                abi_CMD_G2D_ADD_SPRITE(BALL, false, tiltTutBall.posX, tiltTutBall.posY, 0xFF, 0, 0, MIRROR_BLANK);
            }
        }
        case FINISHED_TOP: {
            abi_CMD_ARC(-15, -15,  95, 4, 0, 90, 0xFF, 0x52, 0x52, true);
            abi_CMD_ARC(-15, -15, 160, 4, 0, 90, 0xFF, 0x52, 0x52, true);
            
            if (angle != 180) {
                abi_CMD_G2D_ADD_SPRITE(COLLECTED, false, 75, 75, 0xFF, 0, 0, MIRROR_BLANK);
            }
            if (angle == 0) {
                abi_CMD_G2D_ADD_SPRITE(MASCOT_SUCCESS_SPRITE, false, 205, 20, 0xFF, 0, 350, MIRROR_BLANK);
                abi_CMD_G2D_ADD_SPRITE(DIALOGUE_SMALL, false, 120, 147, 0xFF, 0, 0, MIRROR_BLANK);
                abi_CMD_TEXT("excellent!", 0, 120, 135, 10, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0x52, 0x52, true);
                abi_CMD_TEXT("double tap", 0, 120, 170, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0x54, 0x26, 0x82, true);
                abi_CMD_TEXT("to continue", 0, 120, 200, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0x54, 0x26, 0x82, true);
            } else if (angle == 90) {
                abi_CMD_G2D_ADD_SPRITE(MASCOT_SUCCESS_SPRITE, false, -50, 205, 0xFF, 0, 80, MIRROR_BLANK);
            } else if (angle == 180) {
                abi_CMD_G2D_ADD_SPRITE(COUNT_BAR, false, 176, 208, 0xFF, 0, angle, MIRROR_BLANK);
                for (new item = 0; item < SCREENS_MAX; ++item) {
                    abi_CMD_G2D_ADD_SPRITE(COLLECTED_CHECK_RED, false, 200 - (24 * item), 208, 0xFF, 0, angle, MIRROR_BLANK);
                }
            }

            if (collectedPlusOneEffect) {
                MovePlusOneEffect(angle);
            }

            if ((tiltTutBall.module == abi_cubeN) && (tiltTutBall.screen == screen)) {
                abi_CMD_G2D_ADD_SPRITE(BALL, false, tiltTutBall.posX, tiltTutBall.posY, 0xFF, 0, 0, MIRROR_BLANK);
            }
        }
        case FINISHED_DOUBLE_TAP: {
            new posX_1 = 120;
            new posY_1 = 100;
            new posX_2 = 120;
            new posY_2 = 140;
            FindPosDependOnAngle(angle, posX_1, posY_1, posX_2, posY_2);
            abi_CMD_TEXT("double tap", 0, posX_1, posY_1, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("to continue", 0, posX_2, posY_2, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
        case FINISHED_EXCELLENT: {
            abi_CMD_TEXT("excellent!", 0, 120, 120, SIMPLE_FONT_SIZE + 1, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
        case SELECTOR_TOP: {
            if ((angle == 0) && (tiltTutSelector.module != abi_cubeN)) {
                if (tiltTutSelector.screenAngle != 270) {
                    abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFFFFFF);
                    InitCrossAnimationVars(0xFF5C00, 0xFFB576);
                    DrawCrossAnimation();
                    abi_CMD_TEXT("put the white", 0, 120, 80, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
                    abi_CMD_TEXT("hightlight on me", 0, 120, 105, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
                    abi_CMD_TEXT("and tap the", 0, 120, 130, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
                    abi_CMD_TEXT("cube 2 times", 0, 120, 155, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
                }
            } else if ((angle == 180) && (tiltTutSelector.module != abi_cubeN)) {
                InitCrossAnimationVars(0xFF5C00, 0x513469);
                DrawCrossAnimation();
                if (tiltTutSelector.screenAngle == 270) {
                    abi_CMD_TEXT("tap the cube",  0, 120, 130, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                    abi_CMD_TEXT("2 times now",   0, 120, 110, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                } else {
                    abi_CMD_TEXT("tilt the cube",  0, 120, 155, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                    abi_CMD_TEXT("for navigation", 0, 120, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                    abi_CMD_TEXT("or selection",   0, 120,  80, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                }
            } else if (angle == 270) {
                abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 120, 120, 0xFF, 0, angle, MIRROR_Y);
                if (tiltTutSelector.screenAngle % 270) {
                    abi_CMD_G2D_ADD_SPRITE(DIALOGUE, false, 120, 31, 0xFF, 0, 180, MIRROR_BLANK);
                }
            }

            if (selectorOutAnimation) {
                if (selectorOutAnimation == 1) {
                    selectorXOffset += 30;
                    selectorYOffset = -35;
                } else {
                    selectorYOffset += 30;
                    selectorXOffset = -35;
                }
                abi_CMD_G2D_ADD_SPRITE(SELECTOR, false, 85 - selectorXOffset, 85 - selectorYOffset, 0xFF, 0, 0, MIRROR_BLANK);
                if ((-selectorXOffset < -180) || (-selectorYOffset < -180)) {
                    selectorInAnimation = selectorOutAnimation = selectorYOffset = selectorXOffset = 0;
                }
            } else if (selectorInAnimation) {
                if (selectorInAnimation == 1) {
                    selectorXOffset += 30;
                    selectorYOffset = 210;
                } else {
                    selectorYOffset += 30;
                    selectorXOffset = 210;
                }
                abi_CMD_G2D_ADD_SPRITE(SELECTOR, false, -90 + selectorXOffset, -90 + selectorYOffset, 0xFF, 0, 0, MIRROR_BLANK);
                if ((selectorXOffset > 210) || (selectorYOffset > 210)) {
                    selectorOutAnimation = selectorInAnimation = selectorYOffset = selectorXOffset = 0;
                    abi_CMD_PLAYSND(SELECTOR_MENU_SOUND, SOUND_VOLUME);
                }
            } else if ((tiltTutSelector.module == abi_cubeN) && (tiltTutSelector.screen == screen)) {
                abi_CMD_G2D_ADD_SPRITE(SELECTOR, false, 120 + tiltTutSelector.accelX, 120 + tiltTutSelector.accelY, 0xFF, 0, 0, MIRROR_BLANK);
            }
        }
        case SELECTOR_TEXT_BOX: {
            new staticCenterPos = 120;
            new changingPos = 80;
            new posStep = 20;
            new isStaticX = 1;
            new isStaticY = 0;
            if (angle == 90) {
                changingPos = 140;
                posStep = -posStep;
                isStaticX = 0;
                isStaticY = 1;
            } else if (angle == 180) {
                changingPos = 140;
                posStep = -posStep;
            } else if (angle == 270) {
                isStaticX = 0;
                isStaticY = 1;
            }
            abi_CMD_TEXT("put the ",     0, staticCenterPos * isStaticX + changingPos * isStaticY, staticCenterPos * isStaticY + changingPos * isStaticX, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("highlight on", 0, staticCenterPos * isStaticX + (changingPos + posStep * 1) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 1) * isStaticX, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("the little",   0, staticCenterPos * isStaticX + (changingPos + posStep * 2) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 2) * isStaticX, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("cube and",     0, staticCenterPos * isStaticX + (changingPos + posStep * 3) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 3) * isStaticX, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("double tap",   0, staticCenterPos * isStaticX + (changingPos + posStep * 4) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 4) * isStaticX, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
    }

}

UpdateTiltTutorial(deltaTime) {
    if (selectorTutorial) {
        SelectorLogic();
    } else {
        BallLogic();
    }

    if (abi_cubeN == 0) {
        if (abi_MTD_GetTapsCount() >= 2) {
            if (selectorTutorial) {
                if (tiltTutSelector.screenAngle == 270) {
                    SetApplicationState(FSM:shakeTutorial);
                    abi_CMD_PLAYSND(EXCELLENT_2_SOUND, SOUND_VOLUME);
                }
            } else if (finishTiltTutorial) {
                abi_CMD_PLAYSND(ACTION_SOUND, SOUND_VOLUME);
                finishTiltTutorial = 0;
                selectorTutorial = 1;
            }
        }
    }
}