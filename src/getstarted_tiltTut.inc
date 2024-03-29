#define GS_BALL_DATA .posX, .posY, .angle, .screenAngle, .module, .screen, .moduleT, .screenT, .collectables
#define GS_SELECTOR_DATA .accelX, .accelY, .screenAngle, .module, .screen, .moduleT, .screenT

#include "getstarted_vars.inc"

const TILTTUT_EMPTY       = 0;
const TILTTUT_ICON        = 1;
const TILTTUT_TEXT_BOX    = 2;
const COLLECT_CIRCLE      = 3;
const ARROW_HINT          = 4;
const FINISHED_TOP        = 5;
const FINISHED_DOUBLE_TAP = 6;
const FINISHED_EXCELLENT  = 7;
const SELECTOR_TOP        = 8;
const SELECTOR_TEXT_BOX   = 9;

const SELECTOR_SENSITIVITY = -38;

new getstarted_tiltTutCollect{} = {
    COLLECT_CIRCLE, COLLECT_CIRCLE, COLLECT_CIRCLE, COLLECT_CIRCLE,
    TILTTUT_EMPTY, TILTTUT_ICON, TILTTUT_EMPTY, TILTTUT_TEXT_BOX,
    TILTTUT_EMPTY, TILTTUT_ICON, ARROW_HINT, TILTTUT_TEXT_BOX,
    ARROW_HINT, TILTTUT_TEXT_BOX, TILTTUT_EMPTY, TILTTUT_ICON,
    TILTTUT_ICON, ARROW_HINT, TILTTUT_TEXT_BOX, TILTTUT_EMPTY,
    TILTTUT_TEXT_BOX, TILTTUT_EMPTY, TILTTUT_ICON, ARROW_HINT,
};

new getstarted_tiltTutFinished{} = {
    FINISHED_TOP, FINISHED_TOP, FINISHED_TOP, FINISHED_TOP,
    FINISHED_DOUBLE_TAP, TILTTUT_EMPTY, FINISHED_EXCELLENT, TILTTUT_EMPTY,
    FINISHED_DOUBLE_TAP, TILTTUT_EMPTY, FINISHED_EXCELLENT, TILTTUT_EMPTY,
    FINISHED_DOUBLE_TAP, TILTTUT_EMPTY, FINISHED_EXCELLENT, TILTTUT_EMPTY,
    FINISHED_DOUBLE_TAP, TILTTUT_EMPTY, FINISHED_EXCELLENT, TILTTUT_EMPTY,
    FINISHED_DOUBLE_TAP, TILTTUT_EMPTY, FINISHED_EXCELLENT, TILTTUT_EMPTY,
};

new getstarted_selectorTutorial{} = {
    SELECTOR_TOP, SELECTOR_TOP, SELECTOR_TOP, SELECTOR_TOP,
    TILTTUT_EMPTY, TILTTUT_ICON, TILTTUT_EMPTY, SELECTOR_TEXT_BOX,
    TILTTUT_EMPTY, TILTTUT_ICON, TILTTUT_EMPTY, SELECTOR_TEXT_BOX,
    TILTTUT_EMPTY, SELECTOR_TEXT_BOX, TILTTUT_EMPTY, TILTTUT_ICON,
    TILTTUT_ICON, TILTTUT_EMPTY, SELECTOR_TEXT_BOX, TILTTUT_EMPTY,
    TILTTUT_EMPTY, TILTTUT_ICON, TILTTUT_EMPTY, SELECTOR_TEXT_BOX,
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

new COLLECTED_MASCOT_ANIM_TIME = 500;
new collectedTokens = 0;
new collectedMascotEyesAnimFlag = 0;
new collectedMascotEyesAnimTimer = 0;

const BAKED_TILTTUT_WAIT_SPRITE = 0;
const BAKED_TILTTUT_MAIN_N_SPRITE = 1;
const BAKED_TILTTUT_MAIN_V_SPRITE = 2;
const BAKED_TILTTUT_SUCCESS_SPRITE = 3;
const BAKED_TILTTUT_WAIT_END_SPRITE = 4;

TiltTutorialBakeSprites() {
    GFX_clearCache();
    GFX_bakeImage(BAKED_TILTTUT_WAIT_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(205, 20, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
    GFX_drawImageXY(195, 20, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_WAIT_EYES_SPRITE);
    GFX_drawImageXY(205, 70, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_WAIT_MOUNTH_SPRITE);
    GFX_drawImage([ 75,  75], 0xFF, 0, 0, MIRROR_BLANK, COLLECTED);
    GFX_drawImage([120, 147], 0xFF, 0, 0, MIRROR_BLANK, DIALOGUE_SMALL);
    GFX_drawText([120, 145], SIMPLE_FONT_SIZE + 1, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "collect");
    GFX_drawText([120, 185], SIMPLE_FONT_SIZE + 1, 0, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "all shapes");
    GFX_render();
    GFX_bakeImage(BAKED_TILTTUT_MAIN_N_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(205, 20, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
    GFX_drawImageXY(195, 20, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
    GFX_drawImageXY(205, 70, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
    GFX_drawImage([ 75,  75], 0xFF, 0, 0, MIRROR_BLANK, COLLECTED);
    GFX_drawImage([120, 147], 0xFF, 0, 0, MIRROR_BLANK, DIALOGUE_SMALL);
    GFX_drawText([120, 145], SIMPLE_FONT_SIZE + 1, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "collect");
    GFX_drawText([120, 185], SIMPLE_FONT_SIZE + 1, 0, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "all shapes");
    GFX_render();
    GFX_bakeImage(BAKED_TILTTUT_MAIN_V_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(205, 20, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
    GFX_drawImageXY(195, 20, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_MAIN_EYES_CUTE_SPRITE);
    GFX_drawImageXY(205, 70, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
    GFX_drawImage([ 75,  75], 0xFF, 0, 0, MIRROR_BLANK, COLLECTED);
    GFX_drawImage([120, 147], 0xFF, 0, 0, MIRROR_BLANK, DIALOGUE_SMALL);
    GFX_drawText([120, 145], SIMPLE_FONT_SIZE + 1, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "collect");
    GFX_drawText([120, 185], SIMPLE_FONT_SIZE + 1, 0, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "all shapes");
    GFX_render();
    GFX_bakeImage(BAKED_TILTTUT_SUCCESS_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(205, 20, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_SUCCESS_BODY_SPRITE);
    GFX_drawImageXY(195, 20, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_SUCCESS_EYES_SPRITE);
    GFX_drawImageXY(205, 65, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_SUCCESS_MOUNTH_SPRITE);
    GFX_drawImage([120, 147], 0xFF, 0, 0, MIRROR_BLANK, DIALOGUE_SMALL);
    GFX_drawText([125, 135], 10, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFF5252, "excellent!");
    GFX_drawText([120, 170], SIMPLE_FONT_SIZE, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "double tap");
    GFX_drawText([120, 200], SIMPLE_FONT_SIZE, 0, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "to continue");
    GFX_render();
    GFX_bakeImage(BAKED_TILTTUT_WAIT_END_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(205, 20, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
    GFX_drawImageXY(195, 20, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_WAIT_EYES_SPRITE);
    GFX_drawImageXY(205, 65, 0xFF, 0, 350, MIRROR_BLANK, MASCOT_WAIT_MOUNTH_SPRITE);
    GFX_drawImage([120, 147], 0xFF, 0, 0, MIRROR_BLANK, DIALOGUE_SMALL);
    GFX_drawText([125, 135], 10, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFF5252, "excellent!");
    GFX_drawText([120, 170], SIMPLE_FONT_SIZE, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "double tap");
    GFX_drawText([120, 200], SIMPLE_FONT_SIZE, 0, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "to continue");
    GFX_render();
}

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

    GFX_drawText(GetGFXPoint(mainOneX + offsetX, mainOneY + offsetY), 20, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFF8008, "+1");
    GFX_drawText(GetGFXPoint(mainOneX, mainOneY), 20, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "+1");

    ++currentPlusOneTick;

    if (currentPlusOneTick > PLUS_ONE_MAX_TICKS) {
        collectedPlusOneEffect = 0;
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
    new data[MESSAGE_SIZE];

    data[0] = PKT_BALL_TILT_TUT | (tiltTutBall.module << 8) | (tiltTutBall.screen << 16) | (CheckCollectables() << 24);
    data[1] = tiltTutBall.angle;
    data[2] = tiltTutBall.screenAngle;
    data[3] = pktNumber;

    broadcastMessage(data);
}

SendSelector(pktNumber) {
    new data[MESSAGE_SIZE];

    data[0] = PKT_SELECTOR_TILT_TUT | (tiltTutSelector.module << 8) | (tiltTutSelector.screen << 16);
    data[1] = tiltTutSelector.screenAngle;
    data[2] = pktNumber;
    data[3] = (selectorOutAnimation == 1) ? (2) : (1);

    broadcastMessage(data);
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
            if (getStarted_screenData[screenI].sideType == TOPOLOGY_orientation:ORIENTATION_UP) {
                if (getStarted_screenData[screenI].angle == tiltTutSelector.screenAngle) {
                    tiltTutSelector.module = SELF_ID;
                    tiltTutSelector.screen = screenI;
                    break;
                }
            }
        }
    }

    if (tiltTutSelector.module == SELF_ID) {
        for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
            if (tiltTutSelector.screen == screenI) {
                tiltTutSelector.moduleT = SELF_ID;
                tiltTutSelector.screenT = screenI;

                new accelX = -MS_getFaceAccelX(screenI);
                new accelY = -MS_getFaceAccelY(screenI);

                tiltTutSelector.accelX = tiltTutSelector.accelY = 0;

                accelX *= (accelX <= 0);
                accelY *= (accelY <= 0);
                if (ABS(accelX) > ABS(accelY)) {
                    tiltTutSelector.accelX = accelX >> 1;
                } else {
                    tiltTutSelector.accelY = accelY >> 1;
                }
                
                tiltTutSelector.screenAngle = getStarted_screenData[screenI].angle;
                
                if (accelX < SELECTOR_SENSITIVITY) {
                    neighbor = TOPOLOGY_getAdjacentFacelet(SetFacelet(SELF_ID, screenI), TOPOLOGY_neighbor:NEIGHBOR_LEFT);
                    if (neighbor.connected) {
                        tiltTutSelector.module = neighbor.module;
                        tiltTutSelector.screen = neighbor.screen;
                        tiltTutSelector.screenAngle = (getStarted_screenData[screenI].angle - 90) % 360;
                        selectorOutAnimation = 1;
                    }
                } else if (accelY < SELECTOR_SENSITIVITY) {
                    neighbor = TOPOLOGY_getAdjacentFacelet(SetFacelet(SELF_ID, screenI), TOPOLOGY_neighbor:NEIGHBOR_TOP);
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
    } else if (tiltTutSelector.moduleT == SELF_ID) {
        SendSelector(selectorPkt);
    }
}

BallLogic() {
    if (tiltTutBall.module >= MODULES_MAX) {
        for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
            if (getStarted_screenData[screenI].sideType == TOPOLOGY_orientation:ORIENTATION_UP) {
                if (getStarted_screenData[screenI].angle == tiltTutBall.screenAngle) {
                    tiltTutBall.module = SELF_ID;
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
    if (collectedTokens < allCollected) {
        collectedMascotEyesAnimFlag = 1;
        collectedMascotEyesAnimTimer = 0;
        collectedTokens = allCollected;
    }
    if ((allCollected == SCREENS_MAX) && (finishTiltTutorial == 0)) {
        finishTiltTutorial = 1;
    }

    if (tiltTutBall.module == SELF_ID) {
        for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
            if (tiltTutBall.screen == screenI) {
                tiltTutBall.moduleT = SELF_ID;
                tiltTutBall.screenT = screenI;

                new accelX = MS_getFaceAccelX(screenI) >> 2;
                new accelY = MS_getFaceAccelY(screenI) >> 2;

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
                            SND_play(tiltTutCollectableSounds{item}, SOUND_VOLUME);
                        }
                    }
                }

                if (tiltTutBall.posX < -HALF_MODULE_GAP) {
                    neighbor = TOPOLOGY_getAdjacentFacelet(SetFacelet(SELF_ID, screenI), TOPOLOGY_neighbor:NEIGHBOR_LEFT);
                    if (neighbor.connected) {
                        tiltTutBall.module = neighbor.module;
                        tiltTutBall.screen = neighbor.screen;
                        tiltTutBall.angle = 352;
                        tiltTutBall.screenAngle = (getStarted_screenData[tiltTutBall.screen].angle - 90) % 360;
                    }
                } else if (tiltTutBall.posY < -HALF_MODULE_GAP) {
                    neighbor = TOPOLOGY_getAdjacentFacelet(SetFacelet(SELF_ID, screenI), TOPOLOGY_neighbor:NEIGHBOR_TOP);
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
    } else if (tiltTutBall.moduleT == SELF_ID) {
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
    
    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);

    switch (screenLayout) {
        case TILTTUT_ICON: {
            if (tiltTutSelector.screenAngle != 270) {
                GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, TILT_ICON);
            } else {
                GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, TAP_ICON);
            }
        }
        case TILTTUT_TEXT_BOX: {
            new posX_1 = 120;
            new posY_1 = 100;
            new posX_2 = 120;
            new posY_2 = 140;
            FindPosDependOnAngle(angle, posX_1, posY_1, posX_2, posY_2);
            
            GFX_drawText(GetGFXPoint(posX_1, posY_1), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "tilt the cube");
            GFX_drawText([120, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "and collect");
            GFX_drawText(GetGFXPoint(posX_2, posY_2), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "all shapes");
        }
        case ARROW_HINT: {
            GFX_drawImage([120, 120], 0xFF, 0, 0, MIRROR_BLANK, ARROW_TILT);
        }
        case COLLECT_CIRCLE: {
            GFX_drawArc([-15, -15],  95, 4, 0, 90, 0xFFFFC28A);
            GFX_drawArc([-15, -15], 160, 4, 0, 90, 0xFFFFC28A);

            new curMascotSprite = BAKED_TILTTUT_MAIN_N_SPRITE;
            if (mascotWaitModeFlag) {
                curMascotSprite = BAKED_TILTTUT_WAIT_SPRITE;
            } else if (collectedMascotEyesAnimFlag) {
                curMascotSprite = BAKED_TILTTUT_MAIN_V_SPRITE;
            }
            if (angle == 0) {
                GFX_drawBakedImage([120, 120], 0xFF, 0, 0, MIRROR_BLANK, curMascotSprite);
                if (collectables{0}) {
                    GFX_drawImage([ 75,  75], 0xFF, 0, angle, MIRROR_BLANK, COLLECTABLE);
                }
            } else if (angle == 90) {
                if (curMascotSprite == BAKED_TILTTUT_WAIT_SPRITE) {
                    GFX_drawImage([-50, 205], 0xFF, 0, 80, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
                } else {
                    GFX_drawImage([-50, 205], 0xFF, 0, 80, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
                }
                GFX_drawImage([ 75,  75], 0xFF, 0, 0, MIRROR_BLANK, (collectables{1}) ? (COLLECTABLE) : (COLLECTED));
            } else if (angle == 180) {
                GFX_drawImage([176, 208], 0xFF, 0, angle, MIRROR_BLANK, COUNT_BAR);
                for (new item = 0; item < SCREENS_MAX; ++item) {
                    if (!collectables{item}) {
                        GFX_drawImage(GetGFXPoint(200 - (24 * item), 208), 0xFF, 0, angle, MIRROR_BLANK, COLLECTED_CHECK);
                    }
                }
            } else if (angle == 270) {
                GFX_drawText([210, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "tilt the cube");
                GFX_drawImage([ 75,  75], 0xFF, 0, 0, MIRROR_BLANK, (collectables{2}) ? (COLLECTABLE) : (COLLECTED));
            }

            if (collectedPlusOneEffect) {
                MovePlusOneEffect(angle);
            }

            if ((tiltTutBall.module == SELF_ID) && (tiltTutBall.screen == screen)) {
                GFX_drawImage(GetGFXPoint(tiltTutBall.posX, tiltTutBall.posY), 0xFF, 0, 0, MIRROR_BLANK, BALL);
            }
        }
        case FINISHED_TOP: {
            GFX_drawArc([-15, -15],  95, 4, 0, 90, 0xFFFF5252);
            GFX_drawArc([-15, -15], 160, 4, 0, 90, 0xFFFF5252);

            new curMascotSprite = BAKED_TILTTUT_SUCCESS_SPRITE;
            if (mascotWaitModeFlag) {
                curMascotSprite = BAKED_TILTTUT_WAIT_END_SPRITE;
            }
            if (angle != 180) {
                GFX_drawImage([75, 75], 0xFF, 0, 0, MIRROR_BLANK, COLLECTED);
            }
            if (angle == 0) {
                GFX_drawBakedImage([120, 120], 0xFF, 0, 0, MIRROR_BLANK, curMascotSprite);
                if (collectables{0}) {
                    GFX_drawImage([ 75,  75], 0xFF, 0, angle, MIRROR_BLANK, COLLECTABLE);
                }
            } else if (angle == 90) {
                if (curMascotSprite == BAKED_TILTTUT_WAIT_END_SPRITE) {
                    GFX_drawImage([-50, 205], 0xFF, 0, 80, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
                } else {
                    GFX_drawImage([-50, 205], 0xFF, 0, 80, MIRROR_BLANK, MASCOT_SUCCESS_BODY_SPRITE);
                }
            } else if (angle == 180) {
                GFX_drawImage([176, 208], 0xFF, 0, angle, MIRROR_BLANK, COUNT_BAR);
                for (new item = 0; item < SCREENS_MAX; ++item) {
                    GFX_drawImage(GetGFXPoint(200 - (24 * item), 208), 0xFF, 0, angle, MIRROR_BLANK, COLLECTED_CHECK_RED);
                }
            }

            if (collectedPlusOneEffect) {
                MovePlusOneEffect(angle);
            }

            if ((tiltTutBall.module == SELF_ID) && (tiltTutBall.screen == screen)) {
                GFX_drawImage(GetGFXPoint(tiltTutBall.posX, tiltTutBall.posY), 0xFF, 0, 0, MIRROR_BLANK, BALL);
            }
        }
        case FINISHED_DOUBLE_TAP: {
            new posX_1 = 120;
            new posY_1 = 100;
            new posX_2 = 120;
            new posY_2 = 140;
            FindPosDependOnAngle(angle, posX_1, posY_1, posX_2, posY_2);
            GFX_drawText(GetGFXPoint(posX_1, posY_1), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFFFFFF, "double tap");
            GFX_drawText(GetGFXPoint(posX_2, posY_2), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "to continue");
        }
        case FINISHED_EXCELLENT: {
            GFX_drawText([120, 120], SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "excellent!");
        }
        case SELECTOR_TOP: {
            if ((angle == 0) && (tiltTutSelector.module != SELF_ID)) {
                if (tiltTutSelector.screenAngle != 270) {
                    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], 0xFFFFFFFF);
                    InitCrossAnimationVars(0xFF5C00, 0xFFB576);
                    DrawCrossAnimation();
                    GFX_drawText([120,  80], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "put the white");
                    GFX_drawText([120, 105], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "hightlight on me");
                    GFX_drawText([120, 130], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "and tap the");
                    GFX_drawText([120, 155], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "cube 2 times");
                }
            } else if ((angle == 180) && (tiltTutSelector.module != SELF_ID)) {
                InitCrossAnimationVars(0xFF5C00, 0x513469);
                DrawCrossAnimation();
                if (tiltTutSelector.screenAngle == 270) {
                    GFX_drawText([120, 130], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "tap the cube");
                    GFX_drawText([120, 110], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "2 times now");
                } else {
                    GFX_drawText([120, 155], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "tilt the cube");
                    GFX_drawText([120, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "for navigation");
                    GFX_drawText([120,  80], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "or selection");
                }
            } else if (angle == 270) {
                if (!mascotWaitModeFlag) {
                    GFX_drawImageXY(120, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
                    GFX_drawImageXY(120, 115, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
                    GFX_drawImageXY(170, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
                } else {
                    GFX_drawImageXY(120, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
                    GFX_drawImageXY(120, 115, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_WAIT_EYES_SPRITE);
                    GFX_drawImageXY(170, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_WAIT_MOUNTH_SPRITE);
                }
                if (tiltTutSelector.screenAngle % 270) {
                    GFX_drawImage([120, 31], 0xFF, 0, 180, MIRROR_BLANK, DIALOGUE);
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
                GFX_drawImage(GetGFXPoint(85 - selectorXOffset, 85 - selectorYOffset), 0xFF, 0, 0, MIRROR_BLANK, SELECTOR);
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
                GFX_drawImage(GetGFXPoint(-90 + selectorXOffset, -90 + selectorYOffset), 0xFF, 0, 0, MIRROR_BLANK, SELECTOR);
                if ((selectorXOffset > 210) || (selectorYOffset > 210)) {
                    selectorOutAnimation = selectorInAnimation = selectorYOffset = selectorXOffset = 0;
                    SND_play(SELECTOR_MENU_SOUND, SOUND_VOLUME);
                }
            } else if ((tiltTutSelector.module == SELF_ID) && (tiltTutSelector.screen == screen)) {
                GFX_drawImage(GetGFXPoint(120 + tiltTutSelector.accelX, 120 + tiltTutSelector.accelY), 0xFF, 0, 0, MIRROR_BLANK, SELECTOR);
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
            GFX_drawText(GetGFXPoint(staticCenterPos * isStaticX + changingPos * isStaticY, staticCenterPos * isStaticY + changingPos * isStaticX), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "put the");
            GFX_drawText(GetGFXPoint(staticCenterPos * isStaticX + (changingPos + posStep * 1) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 1) * isStaticX), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "highlight on");
            GFX_drawText(GetGFXPoint(staticCenterPos * isStaticX + (changingPos + posStep * 2) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 2) * isStaticX), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "the little");
            GFX_drawText(GetGFXPoint(staticCenterPos * isStaticX + (changingPos + posStep * 3) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 3) * isStaticX), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "cube and");
            GFX_drawText(GetGFXPoint(staticCenterPos * isStaticX + (changingPos + posStep * 4) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 4) * isStaticX), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "double tap");
        }
    }

}

UpdateTiltTutorial(m_deltaTime) {
    if (selectorTutorial) {
        SelectorLogic();
    } else {
        BallLogic();
        if (collectedMascotEyesAnimFlag) {
            if (collectedMascotEyesAnimTimer >= COLLECTED_MASCOT_ANIM_TIME) {
                collectedMascotEyesAnimFlag = 0;
            } else {
                collectedMascotEyesAnimTimer += m_deltaTime;
            }
        }
    }
}