forward run(const pkt[], size, const src[]); // public Pawn function seen from C

#include "getstarted_start.inc"
#include "getstarted_twistTut.inc"
#include "getstarted_tapTut.inc"
#include "getstarted_tiltTut.inc"
#include "getstarted_shakeTut.inc"
#include "getstarted_success.inc"

SaveData() {
    new saveData [2] = [0,...];
    saveData[0] = alreadyLaunched + 1;
    saveState(saveData);
    dataSaved = 1;
}

SetApplicationState(newState) {
    if (applicationState != newState) {
        if (newState == FSM:start) {
            tutorialStartTimer = TUTORIAL_COUNTDOWN_TIME;
            currentTangibleIcon = tutorialStartTimer / DISPLAY_ONE_ICON_TIME;
            tangibleIcons[0] = SHAKE_ICON;
            tangibleIcons[1] = TILT_ICON;
            tangibleIcons[2] = TAP_ICON;
            tangibleIcons[3] = TWIST_ICON;
        } else if (newState == FSM:twistTutorial) {
            arrowIcons[0] = 0x19;
            arrowIcons[1] = 0x4C;
            arrowIcons[2] = 0xFF;
        } else if (newState == FSM:tapTutorial) {
            tapTutorialStage = 0;
            beginTapTutorial = 1;
            fillTapTutorial  = 0;
        } else if (newState == FSM:tiltTutorial) {
            tiltTutBall.angle = 45;
            tiltTutBall.screenAngle = 180;
            tiltTutBall.moduleT = tiltTutBall.module = MODULES_MAX;
            tiltTutBall.screenT = tiltTutBall.screen = SCREENS_MAX;

            tiltTutSelector.screenAngle = 90;
            tiltTutSelector.moduleT = tiltTutSelector.module = MODULES_MAX;
            tiltTutSelector.screenT = tiltTutSelector.screen = SCREENS_MAX;
        } else if (newState == FSM:successScreen) {
            currentMascotSprite = MASCOT_SUCCESS_SPRITE;
            
            successAnimationEffectTimer = 0;
            successAnimationEffectFlag = 0;
            sucAnimEffectTimePercent = 0;

            smallStarX = 120;
            smallStarY = 120;
            bigStarX = 120;
            bigStarY = 120;
        }
        previousAppState = applicationState;
        applicationState = newState;
        SetDefaultMascot();
    }
}

SendGeneralInfo(pktNumber) {
    new data[4];

    new flags = beginTapTutorial
    | (finishTiltTutorial << 3) 
    | (selectorTutorial << 4) 
    | (beginShakeTutorial << 5)
    | (getstartedGreetingFlag << 6)
    | (fillTapTutorial << 7);

    new randomSoundOrder = 0;
    for (new soundI = 0; soundI < SCREENS_MAX; ++soundI) {
        randomSoundOrder |= (tiltTutCollectableSounds{soundI} << (soundI * 2));
    }
    data[0] = PKT_GENERAL_DATA | (flags << 8) | (previousAppState << 16) | (applicationState << 24);
    data[1] = tapTutorialStage | (shakeTutorialStage << 8) | (twistTutorialStage << 16) | (randomSoundOrder << 24);
    data[2] = tutorialStartTimer;
    data[3] = pktNumber;

    sendMessage(0, NET_BROADCAST_TTL_MAX, data);
    sendMessage(1, NET_BROADCAST_TTL_MAX, data);
    sendMessage(2, NET_BROADCAST_TTL_MAX, data);
}

public ON_PhysicsTick() {
}

public ON_Render() {
    for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
        GFX_updateDisplay(screenI);
        
        switch (applicationState) {
            case start: {
                DrawStartCountdown(screenI);
            }
            case twistTutorial: {
                DrawTwistTutorial(screenI);
            }
            case tapTutorial: {
                DrawTapTutorial(screenI);
            }
            case tiltTutorial: {
                DrawTiltTutorial(screenI);
            }
            case shakeTutorial: {
                DrawShakeTutorial(screenI);
            }
            case successScreen: {
                DrawSuccessScreen(screenI);
            }
        }
        GFX_render();
    }
}

public ON_Tick() {
    currentTime = getTime();
    deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    UpdateCrossAnimation(deltaTime);
    UpdateMascotAnimation(deltaTime);

    switch(applicationState) {
        case start: {
            UpdateStartCountdown(deltaTime);
        }
        case twistTutorial: {
            UpdateTwistTutorial(deltaTime);
        }
        case tapTutorial: {
            UpdateTapTutorial(deltaTime);
        }
        case tiltTutorial: {
            UpdateTiltTutorial(deltaTime);
        }
        case shakeTutorial: {
            UpdateShakeTutorial(deltaTime);
        }
        case successScreen: {
            UpdateSuccessScreen(deltaTime);
        }
    }

    if (SELF_ID == 0) {
        generalDataPkt = ++generalDataPkt % 0x7FFFFFFF;
        SendGeneralInfo(generalDataPkt);
    }
}

public ON_Init() {
    loadState();

    previousTime = getTime();

    ARROW_TWIST = GFX_getId("arrow_twist.png");
    
    ARROW_TILT    = GFX_getId("arrow_tilt.png");
    BALL          = GFX_getId("ball.png");
    COLLECTABLE   = GFX_getId("collectable.png");
    COMPLETE_ICON = GFX_getId("complete.png");
    COUNT_BAR     = GFX_getId("count_bar.png");

    DIALOGUE              = GFX_getId("dialogue.png");
    DIALOGUE_SMALL        = GFX_getId("speechSmall.png");

    COLLECTED_CHECK       = GFX_getId("checked.png");
    COLLECTED_CHECK_RED   = GFX_getId("checked_red.png");

    MASCOT_MAIN_SPRITE    = GFX_getId("mas_main.png");
    MASCOT_SUCCESS_SPRITE = GFX_getId("mas_success.png");
    MASCOT_WAIT_SPRITE    = GFX_getId("mas_wait.png");
    CIRCLE_QUARTER        = GFX_getId("quarter.png");
    SELECTOR              = GFX_getId("selector.png");
    SHAKE_ICON            = GFX_getId("shake_icon.png");
    TAP_ICON              = GFX_getId("tap_icon.png");
    TILT_ICON             = GFX_getId("tilt_icon.png");
    TWIST_ICON            = GFX_getId("twist_icon.png");

    SMALL_STAR            = GFX_getId("star_small.png");
    BIG_STAR              = GFX_getId("star_big.png");

    CIRCLE_QUARTER_PUSH   = GFX_getId("quarterPush.png");

    FINGER_SPRITE         = GFX_getId("finger.png");
    WRONG_TAP_ICON        = GFX_getId("wrongTap.png");
    WRONG_TAP_RIM         = GFX_getId("wrongRim.png");
    RIGHT_TAP_ICON        = GFX_getId("rightTap.png");
    RIGHT_TAP_RIM_1       = GFX_getId("rightRim1.png");
    RIGHT_TAP_RIM_2       = GFX_getId("rightRim2.png");
    RIGHT_TAP_RIM_3       = GFX_getId("rightRim3.png");
    
    SPEECH_BUBBLE_TAP_SPRITE = GFX_getId("speechTap.png");

    MASCOT_MAIN_EMPTY_SPRITE       = GFX_getId("masMainE.png");
    MASCOT_MAIN_EYES_NORMAL_SPRITE = GFX_getId("masEyesN.png");
    MASCOT_MAIN_MOUNTH_O_SPRITE    = GFX_getId("masMouthO.png");

    // Sounds
    ACTION_SOUND      = SND_getId("action.wav");
    GOOD_SOUND        = SND_getId("good.wav");
    EXCELLENT_1_SOUND = SND_getId("excellent_1.wav");
    EXCELLENT_2_SOUND = SND_getId("excellent_2.wav");
    
    SELECTOR_MENU_SOUND = SND_getId("selec_menu.wav");
    
    TAPS_1_2_SOUND = SND_getId("taps_1-2.wav");
    TAPS_3_4_SOUND = SND_getId("taps_3-4.wav");
    TAPS_5_6_SOUND = SND_getId("taps_5-6.wav");
    TAP_STAGE_SUCCESS_SOUND = SND_getId("tapsSuccess.wav");

    PLUS_1_SHAPE_COLLECT_1_SOUND = SND_getId("collect_1.wav");
    PLUS_1_SHAPE_COLLECT_2_SOUND = SND_getId("collect_2.wav");
    PLUS_1_SHAPE_COLLECT_3_SOUND = SND_getId("collect_3.wav");

    tiltTutCollectableSounds{0} = PLUS_1_SHAPE_COLLECT_1_SOUND;
    tiltTutCollectableSounds{1} = PLUS_1_SHAPE_COLLECT_2_SOUND;
    tiltTutCollectableSounds{2} = PLUS_1_SHAPE_COLLECT_3_SOUND;

    if (SELF_ID == 0) {
        for (new soundI = 0; soundI < SCREENS_MAX; ++soundI) {
            new j = random (0, SCREENS_MAX - 1);
            new temp = tiltTutCollectableSounds{j};
            tiltTutCollectableSounds{j} = tiltTutCollectableSounds{soundI};
            tiltTutCollectableSounds{soundI} = temp;
        }
    }

    for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
        getStarted_screenData[screenI].sideType = TOPOLOGY_getFaceletLocation(SetFacelet(SELF_ID, screenI));
        getStarted_screenData[screenI].angle = TOPOLOGY_getAngle(SetFacelet(SELF_ID, screenI), TOPOLOGY_orientation:ORIENTATION_SPLASH);
    }

    SetApplicationState(FSM:start);
}

public ON_Shake(const count) {
    if ((SELF_ID == 0) && (count >= SENSITIVITY_MENU_CHANGE_SCRIPT) && alreadyLaunched) {
        quit();
    }
    if (count > 0) {
        SetDefaultMascot();
    }
    if ((SELF_ID == 0) && (applicationState == FSM:shakeTutorial)) {
        if (beginShakeTutorial) {
            shakeTutorialStage = count;

            if (!dataSaved) {
                SaveData();
            }

            if (count >= SENSITIVITY_MENU_CHANGE_SCRIPT) {
                quit();
            }
        }
        if (!beginShakeTutorial && (count > 0)) {
            beginShakeTutorial = 1;
        }
    }
}

public ON_Tap(const count, const display, const bool:opposite) {
    if (count > 0) {
        SetDefaultMascot();
    }
    if (SELF_ID == 0) {
        switch(applicationState) {
            case tapTutorial: {
                if (fillTapTutorial) {
                    if(count >= 1) {
                        ++tapTutorialStage;
                        if (tapTutorialStage >= 5) {
                            SND_play(TAPS_5_6_SOUND, SOUND_VOLUME);
                        } else if (tapTutorialStage >= 3) {
                            SND_play(TAPS_3_4_SOUND, SOUND_VOLUME);
                        } else if (tapTutorialStage >= 0) {
                            SND_play(TAPS_1_2_SOUND, SOUND_VOLUME);
                        }
                        mascotTapReactAnimFlag = 1;
                        fulfillmentTimer = 0;
                    }
                }
                if (!fillTapTutorial && beginTapTutorial && (count == 2)) {
                    fillTapTutorial = 1;
                }
            }
            case tiltTutorial: {
                if (count >= 2) {
                    if (selectorTutorial) {
                        if (tiltTutSelector.screenAngle == 270) {
                            SetApplicationState(FSM:shakeTutorial);
                            SND_play(EXCELLENT_2_SOUND, SOUND_VOLUME);
                        }
                    } else if (finishTiltTutorial) {
                        SND_play(ACTION_SOUND, SOUND_VOLUME);
                        finishTiltTutorial = 0;
                        selectorTutorial = 1;
                    }
                }
            }
            case successScreen: {
                if ((previousAppState == FSM:tapTutorial) && (count >= 2)) {
                    SetApplicationState(FSM:tiltTutorial);
                    SND_play(ACTION_SOUND, SOUND_VOLUME);
                }
            }
        }
    }
}

public ON_Twist(twist[TOPOLOGY_TWIST_INFO]) {
    for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
        getStarted_screenData[screenI].sideType = TOPOLOGY_getFaceletLocation(SetFacelet(SELF_ID, screenI));
        getStarted_screenData[screenI].angle = TOPOLOGY_getAngle(SetFacelet(SELF_ID, screenI), TOPOLOGY_orientation:ORIENTATION_SPLASH);
    }

    if (applicationState == FSM:twistTutorial) {
        SetDefaultMascot();
    }
    
    if (SELF_ID == 0) {
        if (applicationState == FSM:twistTutorial) {
            ++twistTutorialStage;
            if (twistTutorialStage == 1) {
                SND_play(GOOD_SOUND, SOUND_VOLUME);
            }
            if (twistTutorialStage >= MAX_TWIST_TUTORIAL_STAGES) {
                SetApplicationState(FSM:successScreen);
                SND_play(EXCELLENT_1_SOUND, SOUND_VOLUME);
            }
        } else if ((applicationState == FSM:successScreen) && (previousAppState == FSM:twistTutorial)) {
            SetApplicationState(FSM:tapTutorial);
        }
    }

    if (applicationState == FSM:tiltTutorial) {
        tiltTutBall.module = MODULES_MAX;
        tiltTutSelector.module = MODULES_MAX;
    }
}

public ON_Load(const pkt[]) {
    if (parseByte(pkt, 1) == 0) {
        return;
    }
    alreadyLaunched = pkt[1];
}

public ON_Message(const pkt[]) {
    switch (parseByte(pkt, 4)) {
        case PKT_GENERAL_DATA: {
            new packetNumberReceived = pkt[4];
            if ((generalDataPkt < packetNumberReceived) || ((generalDataPkt - packetNumberReceived) > (0x7FFFFFFF >> 1))) {
                generalDataPkt = packetNumberReceived;
                SetApplicationState(parseByte(pkt, 7));
                new flags = parseByte(pkt, 5);
                beginTapTutorial = flags & 0x1;
                finishTiltTutorial = (flags >> 3) & 0x1;
                selectorTutorial = (flags >> 4) & 0x1;
                beginShakeTutorial = (flags >> 5) & 0x1;
                getstartedGreetingFlag = (flags >> 6) & 0x1;
                fillTapTutorial = (flags >> 7) & 0x1;
                if (parseByte(pkt, 8) > tapTutorialStage) {
                    mascotTapReactAnimFlag = 1;
                }
                for (new soundI = 0; soundI < SCREENS_MAX; ++soundI) {
                    tiltTutCollectableSounds{soundI} = (parseByte(pkt, 11) >> (soundI * 2)) & 0x3;
                }
                tapTutorialStage = parseByte(pkt, 8);
                shakeTutorialStage = parseByte(pkt, 9);
                twistTutorialStage = parseByte(pkt, 10);
                tutorialStartTimer = pkt[3];
            }
        }
        case PKT_BALL_TILT_TUT: {
            new packetNumberReceived = pkt[4];
            if ((ballPkt < packetNumberReceived) || ((ballPkt - packetNumberReceived) > (0xFFFFF >> 1))) {
                ballPkt = packetNumberReceived;
                tiltTutBall.module = parseByte(pkt, 5);
                tiltTutBall.screen = parseByte(pkt, 6);
                tiltTutBall.angle = pkt[2];
                tiltTutBall.screenAngle = pkt[3];
                for (new item = 0; item < SCREENS_MAX; ++item) {
                    collectables{item} = (parseByte(pkt, 7) >> item) & 0x1;
                }
            }
        }
        case PKT_SELECTOR_TILT_TUT: {
            new packetNumberReceived = pkt[3];
            if ((selectorPkt < packetNumberReceived) || ((selectorPkt - packetNumberReceived) > (0xFFFFF >> 1))) {
                selectorPkt = packetNumberReceived;
                tiltTutSelector.module = parseByte(pkt, 5);
                if (tiltTutSelector.module == SELF_ID) {
                    selectorInAnimation = pkt[4];
                }
                tiltTutSelector.screen = parseByte(pkt, 6);
                tiltTutSelector.screenAngle = pkt[2];
            }
        }
    }

}