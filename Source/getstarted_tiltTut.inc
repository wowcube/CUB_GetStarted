#define GS_BALL_DATA .posX, .posY, .angle, .screenAngle, .module, .screen, .moduleT, .screenT, .collectables
#define GS_SELECTOR_DATA .accelX, .accelY, .screenAngle, .module, .screen, .moduleT, .screenT

#include "getstarted_vars.inc"

const MASCOT         = 0;
const EMPTY          = 1;
const ICON           = 2;
const SPEECH_BOX     = 3;
const TEXT_BOX       = 4;
const COLLECT_CIRCLE = 5;
const ARROW_HINT     = 6;
const FINISHED_TOP        = 7;
const FINISHED_DOUBLE_TAP = 8;
const FINISHED_EXCELLENT  = 9;
const SELECTOR_TOP        = 10;
const SELECTOR_TEXT_BOX   = 11;


new getstarted_tiltTutorial{} = {
    TEXT_BOX, MASCOT, SPEECH_BOX, EMPTY,
    TEXT_BOX, MASCOT, SPEECH_BOX, EMPTY,
    EMPTY, ICON, EMPTY, TEXT_BOX,
    EMPTY, TEXT_BOX, EMPTY, ICON,
    ICON, EMPTY, TEXT_BOX, EMPTY,
    EMPTY, ICON, EMPTY, TEXT_BOX,
};

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

const COLLECTING_DROP_TIME = 10000;
new beginTiltTutorial = 0;
new finishTiltTutorial = 0;
new selectorTutorial = 0;
new collectables{FACES_MAX} = {1, 1, 1};
new collectablesAngles[FACES_MAX] = [0, 90, 270];

new ballPkt = 0;
new selectorPkt = 0;

new mascotTiltAnimFlag = 1;
new mascotTiltAnimAngle = 90;
new tiltAngleAnimChange = -4;
new mascotTiltAnimTimer = 0;

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
    for (new item = 0; item < FACES_MAX; ++item) {
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
    if (tiltTutSelector.module >= CUBES_MAX) {
        for (new screenI = 0; screenI < FACES_MAX; ++screenI) {
            if (getStarted_screenData[screenI].sideType == topology_location:location_top) {
                if (getStarted_screenData[screenI].angle == tiltTutSelector.screenAngle) {
                    tiltTutSelector.module = abi_cubeN;
                    tiltTutSelector.screen = screenI;
                    break;
                }
            }
        }
    }

    if (tiltTutSelector.module == abi_cubeN) {
        for (new screenI = 0; screenI < FACES_MAX; ++screenI) {
            if (tiltTutSelector.screen == screenI) {
                tiltTutSelector.moduleT = abi_cubeN;
                tiltTutSelector.screenT = screenI;

                new accelX = -abi_MTD_GetFaceAccelX(screenI);
                new accelY = -abi_MTD_GetFaceAccelY(screenI);

                accelX *= (accelX <= 0);
                accelY *= (accelY <= 0);
                tiltTutSelector.accelX = accelX >> 1;
                tiltTutSelector.accelY = accelY >> 1;
                
                tiltTutSelector.screenAngle = getStarted_screenData[screenI].angle;
                
                if (accelX < -35) {
                    if (abi_leftCubeN(abi_cubeN, screenI) < CUBES_MAX) {
                        tiltTutSelector.module = abi_leftCubeN(abi_cubeN, screenI);
                        tiltTutSelector.screen = abi_leftFaceN(abi_cubeN, screenI);
                        tiltTutSelector.screenAngle = (getStarted_screenData[screenI].angle - 90) % 360;
                    }
                } else if (accelY < -35) {
                    if (abi_topCubeN(abi_cubeN, screenI) < CUBES_MAX) {
                        tiltTutSelector.module = abi_topCubeN(abi_cubeN, screenI);
                        tiltTutSelector.screen = abi_topFaceN(abi_cubeN, screenI);
                        tiltTutSelector.screenAngle = (getStarted_screenData[screenI].angle + 90) % 360;
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
    if (tiltTutBall.module >= CUBES_MAX) {
        for (new screenI = 0; screenI < FACES_MAX; ++screenI) {
            if (getStarted_screenData[screenI].sideType == topology_location:location_top) {
                if (getStarted_screenData[screenI].angle == tiltTutBall.screenAngle) {
                    tiltTutBall.module = abi_cubeN;
                    tiltTutBall.screen = screenI;
                    break;
                }
            }
        }
    }

    if (tiltTutBall.module >= CUBES_MAX) {
        return;
    }

    new allCollected = 0;
    for (new item = 0; item < FACES_MAX; ++item) {
        allCollected += (collectables{item} == 0);
    }
    if (allCollected == FACES_MAX) {
        finishTiltTutorial = 1;
        beginTiltTutorial = 0;
    }

    if (tiltTutBall.module == abi_cubeN) {
        for (new screenI = 0; screenI < FACES_MAX; ++screenI) {
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

                for (new item = 0; item < FACES_MAX; ++item) {
                    if (collectables{item} && (collectablesAngles[item] == tiltTutBall.screenAngle)) {
                        new distance = CheapDistance(75 - tiltTutBall.posX, 75 - tiltTutBall.posY);
                        if (distance < BALL_SQUARE_RADIUS) {
                            collectables{item} = 0;
                        }
                    }
                }

                if (tiltTutBall.posX < -HALF_MODULE_GAP) {
                    if (abi_leftCubeN(abi_cubeN, screenI) < CUBES_MAX) {
                        tiltTutBall.module = abi_leftCubeN(abi_cubeN, screenI);
                        tiltTutBall.screen = abi_leftFaceN(abi_cubeN, screenI);
                        tiltTutBall.angle = 355;
                        tiltTutBall.screenAngle = (getStarted_screenData[tiltTutBall.screen].angle - 90) % 360;
                    }
                } else if (tiltTutBall.posY < -HALF_MODULE_GAP) {
                    if (abi_topCubeN(abi_cubeN, screenI) < CUBES_MAX) {
                        tiltTutBall.module = abi_topCubeN(abi_cubeN, screenI);
                        tiltTutBall.screen = abi_topFaceN(abi_cubeN, screenI);
                        tiltTutBall.angle = 95;
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
    new layoutElement = getStarted_screenData[screen].sideType * FACES_ON_PLANE_MAX + angle / 90;
    new screenLayout = getstarted_tiltTutorial{layoutElement};
    if (beginTiltTutorial) {
        screenLayout = getstarted_tiltTutCollect{layoutElement};
    } else if (finishTiltTutorial) {
        screenLayout = getstarted_tiltTutFinished{layoutElement};
    } else if (selectorTutorial) {
        screenLayout = getstarted_selectorTutorial{layoutElement};
    }
    
    abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR);

    switch (screenLayout) {
        case MASCOT: {
            if (mascotTiltAnimFlag) {
                MascotTiltAnimation();
            }
            abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 120, 120, 0xFF, 0, mascotTiltAnimAngle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(DIALOGUE, false, 120, 31, 0xFF, 0, 180, MIRROR_BLANK);
        }
        case ICON: {
            abi_CMD_G2D_ADD_SPRITE((beginTiltTutorial) ? (TILT_ICON) : (TAP_ICON), false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
        }
        case SPEECH_BOX: {
            abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFFFFFF);
            InitCrossAnimationVars(0xFF5C00, 0xFFB576);
            DrawCrossAnimation();
            abi_CMD_TEXT("next thing", 0, 120, 150, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0x54, 0x26, 0x82, true);
            abi_CMD_TEXT("to learn", 0, 120, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
            abi_CMD_TEXT("is tilt", 0, 120, 90, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0x54, 0x26, 0x82, true);
        }
        case TEXT_BOX: {
            new posX_1 = 120;
            new posY_1 = 100;
            new posX_2 = 120;
            new posY_2 = 140;
            FindPosDependOnAngle(angle, posX_1, posY_1, posX_2, posY_2);
            if (beginTiltTutorial) {
                abi_CMD_TEXT("tilt the cube", 0, posX_1, posY_1, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("and collect", 0, 120, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("all shapes",  0, posX_2, posY_2, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            } else {
                if ((getStarted_screenData[screen].sideType == topology_location:location_bottom) ||
                    (getStarted_screenData[screen].sideType == topology_location:location_top)) {
                    InitCrossAnimationVars(0xFF5C00, 0x513469);
                    DrawCrossAnimation();
                }
                abi_CMD_TEXT("double tap", 0, posX_1, posY_1, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("to continue", 0, posX_2, posY_2, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            }
        }
        case ARROW_HINT: {
            abi_CMD_G2D_ADD_SPRITE(ARROW_TILT, false, 120, 120, 0xFF, 0, 0, MIRROR_BLANK);
        }
        case COLLECT_CIRCLE: {
            abi_CMD_ARC(0, 0, 80, 4, 0, 90, 0xFF, 0xC2, 0x8A, true);
            abi_CMD_ARC(0, 0, 135, 4, 0, 90, 0xFF, 0xC2, 0x8A, true);
            
            if (angle == 0) {
                abi_CMD_TEXT("collect", 0, 120, 168, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("all shapes", 0, 120, 210, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_G2D_ADD_SPRITE((collectables{0}) ? (COLLECTABLE) : (COLLECTED), false, 75, 75, 0xFF, 0, 0, MIRROR_BLANK);
            } else if (angle == 90) {
                abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 210, 210, 0xFF, 0, 75, MIRROR_BLANK);
                abi_CMD_G2D_ADD_SPRITE((collectables{1}) ? (COLLECTABLE) : (COLLECTED), false, 75, 75, 0xFF, 0, 0, MIRROR_BLANK);
            } else if (angle == 180) {
                abi_CMD_G2D_ADD_SPRITE(COUNT_BAR, false, 176, 208, 0xFF, 0, angle, MIRROR_BLANK);
                for (new item = 0; item < FACES_MAX; ++item) {
                    if (!collectables{item}) {
                        abi_CMD_G2D_ADD_SPRITE(COLLECTED_CHECK, false, 200 - (24 * item), 208, 0xFF, 0, angle, MIRROR_BLANK);
                    }
                }
            } else if (angle == 270) {
                abi_CMD_TEXT("tilt the cube", 0, 210, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_G2D_ADD_SPRITE((collectables{2}) ? (COLLECTABLE) : (COLLECTED), false, 75, 75, 0xFF, 0, 0, MIRROR_BLANK);
            }

            if ((tiltTutBall.module == abi_cubeN) && (tiltTutBall.screen == screen)) {
                abi_CMD_G2D_ADD_SPRITE(BALL, false, tiltTutBall.posX, tiltTutBall.posY, 0xFF, 0, 0, MIRROR_BLANK);
            }
        }
        case FINISHED_TOP: {
            abi_CMD_ARC(0, 0, 80, 4, 0, 90, 0xFF, 0xC2, 0x8A, true);
            abi_CMD_ARC(0, 0, 135, 4, 0, 90, 0xFF, 0xC2, 0x8A, true);
            
            if (angle != 180) {
                abi_CMD_G2D_ADD_SPRITE(COLLECTED, false, 75, 75, 0xFF, 0, 0, MIRROR_BLANK);
            }
            if (angle == 0) {
                abi_CMD_TEXT("double tap", 0, 120, 168, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("to continue", 0, 120, 210, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
            } else if (angle == 90) {
                abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 210, 210, 0xFF, 0, 75, MIRROR_BLANK);
            } else if (angle == 180) {
                abi_CMD_G2D_ADD_SPRITE(COUNT_BAR, false, 176, 208, 0xFF, 0, angle, MIRROR_BLANK);
                for (new item = 0; item < FACES_MAX; ++item) {
                    abi_CMD_G2D_ADD_SPRITE(COLLECTED_CHECK, false, 200 - (24 * item), 208, 0xFF, 0, angle, MIRROR_BLANK);
                }
            } else if (angle == 270) {
                abi_CMD_TEXT("excellent!", 0, 210, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_TOP_CENTER, 0xFF, 0xFF, 0xFF, true);
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
            abi_CMD_TEXT("double tap", 0, posX_1, posY_1, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("to continue", 0, posX_2, posY_2, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
        case FINISHED_EXCELLENT: {
            abi_CMD_TEXT("excellent!", 0, 120, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
        case SELECTOR_TOP: {
            if ((angle == 0) && (tiltTutSelector.module != abi_cubeN)) {
                abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFFFFFF);
                InitCrossAnimationVars(0xFF5C00, 0xFFB576);
                DrawCrossAnimation();
                abi_CMD_TEXT("put the white", 0, 120, 80, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
                abi_CMD_TEXT("hightlight on me", 0, 120, 105, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
                abi_CMD_TEXT("and tap the", 0, 120, 130, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
                abi_CMD_TEXT("cube 2 times", 0, 120, 155, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
            } else if ((angle == 180) && (tiltTutSelector.module != abi_cubeN)) {
                InitCrossAnimationVars(0xFF5C00, 0x513469);
                DrawCrossAnimation();
                abi_CMD_TEXT("tilt the cube",  0, 120, 155, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("for navigation", 0, 120, 120, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("or selection",   0, 120,  80, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            } else if (angle == 270) {
                abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 120, 120, 0xFF, 0, angle, MIRROR_Y);
                if (tiltTutSelector.screenAngle % 270) {
                    abi_CMD_G2D_ADD_SPRITE(DIALOGUE, false, 120, 31, 0xFF, 0, 180, MIRROR_BLANK);
                }
            }

            if ((tiltTutSelector.module == abi_cubeN) && (tiltTutSelector.screen == screen)) {
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
            abi_CMD_TEXT("put the ",        0, staticCenterPos * isStaticX + changingPos * isStaticY, staticCenterPos * isStaticY + changingPos * isStaticX, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("highlight on",    0, staticCenterPos * isStaticX + (changingPos + posStep * 1) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 1) * isStaticX, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("the little cube", 0, staticCenterPos * isStaticX + (changingPos + posStep * 2) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 2) * isStaticX, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            abi_CMD_TEXT("and double tap",  0, staticCenterPos * isStaticX + (changingPos + posStep * 3) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 3) * isStaticX, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
        }
    }

}

UpdateTiltTutorial(deltaTime) {
    if (selectorTutorial) {
        SelectorLogic();
    } else if((beginTiltTutorial) || (finishTiltTutorial)) {
        BallLogic();
    }

    if (!beginTiltTutorial && !mascotTiltAnimFlag) {
        if (mascotTiltAnimTimer >= MASCOT_WAIT_TIME) {
            mascotTiltAnimTimer = 0;
            mascotTiltAnimFlag = 1;
        } else {
            mascotTiltAnimTimer += deltaTime;
        }
    }

    if (abi_cubeN == 0) {
        if (abi_MTD_GetTapsCount() >= 2) {
            if (selectorTutorial) {
                if (tiltTutSelector.screenAngle == 270) {
                    SetApplicationState(FSM:successScreen);
                }
            } else if (!finishTiltTutorial && !beginTiltTutorial) {
                beginTiltTutorial = 1;
            } else if (finishTiltTutorial) {
                finishTiltTutorial = 0;
                selectorTutorial = 1;
            }
        }
    }
}