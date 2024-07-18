forward run(const pkt[], size, const src[]); // public Pawn function seen from C

#include "getstarted_start.inc"
#include "getstarted_twistTut.inc"
#include "getstarted_tapTut.inc"
#include "getstarted_tiltTut.inc"
#include "getstarted_shakeTut.inc"
#include "getstarted_success.inc"
#include "getstarted_firstLaunch.inc"

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
            tiltTutBall.pos = 2;
            tiltTutBall.moduleT = tiltTutBall.module = MODULES_MAX;
            tiltTutBall.screenT = tiltTutBall.screen = SCREENS_MAX;

            tiltTutSelector.pos = 3;
            tiltTutSelector.moduleT = tiltTutSelector.module = MODULES_MAX;
            tiltTutSelector.screenT = tiltTutSelector.screen = SCREENS_MAX;
        } else if (newState == FSM:successScreen) {
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
        bakeAppStateSpritesFlag = 1;
    }
}

BakeSpritesForCurrentAppState(currentAppState) {
    GFX_clearCache();
    switch (currentAppState) {
        case start: {
            StartBakeSprites();
        }
        case twistTutorial: {
            TwistTutorialBakeSprites();
        }
        case tapTutorial: {
            TapTutorialBakeSprites();
        }
        case tiltTutorial: {
            TiltTutorialBakeSprites();
        }
        case shakeTutorial: {
            ShakeTutorialBakeSprites();
        }
        case successScreen: {
        }
        case firstLaunch: {
            FirstLaunchBakeSprites();
        }
    }
    bakeAppStateSpritesFlag = 0;
}

GetMapping() {
    for (new moduleI = 0; moduleI < MODULES_MAX; ++moduleI) {
        for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
            new curPlace[TOPOLOGY_PLACE];
            curPlace = TOPOLOGY_getPlace(SetFacelet(moduleI, screenI), ORIENTATION_MODE_SPLASH);
            getStarted_AllScreensData[moduleI * SCREENS_MAX + screenI].face = TOPOLOGY_getFaceletOrientation(SetFacelet(moduleI, screenI));
            getStarted_AllScreensData[moduleI * SCREENS_MAX + screenI].pos = curPlace.position;
        }
    }
}

SendGeneralInfo(pktNumber) {
    new data[MAX_PACKET_SIZE / 4];

    new flags = beginTapTutorial
    | (finishTiltTutorial << 3) 
    | (selectorTutorial << 4) 
    | (beginShakeTutorial << 5)
    | (getstartedGreetingFlag << 6)
    | (fillTapTutorial << 7)
    | (flStartFlag << 8);

    new randomSoundOrder = 0;
    for (new soundI = 0; soundI < SCREENS_MAX; ++soundI) {
        randomSoundOrder |= (tiltTutCollectableSounds{soundI} << (soundI * 2));
    }
    data[0] = (previousAppState << 16) | (applicationState << 24);
    data[1] = tapTutorialStage | (sideTapIndicatorPos << 4) | (shakeTutorialStage << 8) | (twistTutorialStage << 16) | (randomSoundOrder << 24);
    data[2] = tutorialStartTimer;
    data[3] = pktNumber;
    data[4] = flags;

    broadcastPacket(PKT_GENERAL_DATA, data);
}

SendMapping() {
    new data[MAX_PACKET_SIZE / 4];

    new dataI = 0;
    for (new screenI = 3, offset = 0; screenI < MODULES_MAX * SCREENS_MAX; ++screenI, ++offset) {
        if (offset >= 5) {
            offset = 0;
            ++dataI;
        }
        data[dataI] |= (getStarted_AllScreensData[screenI].face << (5 * offset))
                     | (getStarted_AllScreensData[screenI].pos << (3 + (5 * offset)));
    }

    broadcastPacket(PKT_MAPPING, data);
}

public ON_PhysicsTick() {
}

public ON_Render() {
    if (bakeAppStateSpritesFlag) {
        BakeSpritesForCurrentAppState(applicationState);
    }

    for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
        GFX_setRenderTarget(screenI);
        
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
            case firstLaunch: {
                DrawFirstLaunch(screenI);
            }
        }
        GFX_render();
    }
}

public ON_Tick() {
    currentTime = getTime();
    deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    CheckAcceleration();
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
        case firstLaunch: {
            UpdateFirstLaunch(deltaTime);
        }
    }

    if (SELF_ID == 0) {
        generalDataPkt = ++generalDataPkt % 0x7FFFFFFF;
        SendGeneralInfo(generalDataPkt);
        SendMapping();
    }
}

public ON_Init(id, size, const pkt[]) {
    loadState();

    previousTime = getTime();

    ARROW_TWIST = GFX_getAssetId("arrow_twist.png");
    
    ARROW_TILT    = GFX_getAssetId("arrow_tilt.png");
    BALL          = GFX_getAssetId("ball.png");
    COLLECTED     = GFX_getAssetId("collected.png");
    COLLECTABLE   = GFX_getAssetId("collectable.png");
    COMPLETE_ICON = GFX_getAssetId("complete.png");
    COUNT_BAR     = GFX_getAssetId("count_bar.png");

    DIALOGUE              = GFX_getAssetId("dialogue.png");
    DIALOGUE_SMALL        = GFX_getAssetId("speechSmall.png");

    COLLECTED_CHECK       = GFX_getAssetId("checked.png");
    COLLECTED_CHECK_RED   = GFX_getAssetId("checked_red.png");

    CIRCLE_QUARTER        = GFX_getAssetId("quarter.png");
    SELECTOR              = GFX_getAssetId("selector.png");
    SHAKE_ICON            = GFX_getAssetId("shake_icon.png");
    TAP_ICON              = GFX_getAssetId("tap_icon.png");
    TILT_ICON             = GFX_getAssetId("tilt_icon.png");
    TWIST_ICON            = GFX_getAssetId("twist_icon.png");

    SMALL_STAR            = GFX_getAssetId("star_small.png");
    BIG_STAR              = GFX_getAssetId("star_big.png");

    CIRCLE_QUARTER_PUSH   = GFX_getAssetId("quarterPush.png");

    FINGER_SPRITE         = GFX_getAssetId("finger.png");
    WRONG_TAP_ICON        = GFX_getAssetId("wrongTap.png");
    WRONG_TAP_RIM         = GFX_getAssetId("wrongRim.png");
    RIGHT_TAP_ICON        = GFX_getAssetId("rightTap.png");
    RIGHT_TAP_RIM_1       = GFX_getAssetId("rightRim1.png");
    RIGHT_TAP_RIM_2       = GFX_getAssetId("rightRim2.png");
    RIGHT_TAP_RIM_3       = GFX_getAssetId("rightRim3.png");
    
    SPEECH_BUBBLE_TAP_SPRITE = GFX_getAssetId("speechTap.png");

    MASCOT_MAIN_EMPTY_SPRITE       = GFX_getAssetId("masMainE.png");
    MASCOT_MAIN_EYES_NORMAL_SPRITE = GFX_getAssetId("masEyesN.png");
    MASCOT_MAIN_EYES_CUTE_SPRITE   = GFX_getAssetId("masEyesV.png");
    MASCOT_MAIN_EYES_X_SPRITE      = GFX_getAssetId("masEyesX.png");
    MASCOT_MAIN_MOUNTH_O_SPRITE    = GFX_getAssetId("masMouthO.png");

    MASCOT_WAIT_BODY_SPRITE   = GFX_getAssetId("masWait.png");
    MASCOT_WAIT_EYES_SPRITE   = GFX_getAssetId("masWaitEyes.png");
    MASCOT_WAIT_MOUNTH_SPRITE = GFX_getAssetId("mWaitMouth.png");

    MASCOT_WAIT_BODY_SPRITE   = GFX_getAssetId("masWait.png");
    MASCOT_WAIT_EYES_SPRITE   = GFX_getAssetId("masWaitEyes.png");
    MASCOT_WAIT_MOUNTH_SPRITE = GFX_getAssetId("mWaitMouth.png");

    MASCOT_SUCCESS_BODY_SPRITE   = GFX_getAssetId("masSuccess.png");
    MASCOT_SUCCESS_EYES_SPRITE   = GFX_getAssetId("masSucEyes.png");
    MASCOT_SUCCESS_MOUNTH_SPRITE = GFX_getAssetId("mSucMouth.png");

    HI_SPRITE          = GFX_getAssetId("hi.png");
    OFF_HAND_SPRITE    = GFX_getAssetId("off_hand.png");
    MAIN_HAND_1_SPRITE = GFX_getAssetId("hand_main1.png");
    MAIN_HAND_2_SPRITE = GFX_getAssetId("hand_main2.png");
    MAIN_HAND_3_SPRITE = GFX_getAssetId("hand_main3.png");
    
    mascotHandWaveAnimFrames{0} = MAIN_HAND_1_SPRITE;
    mascotHandWaveAnimFrames{1} = MAIN_HAND_2_SPRITE;
    mascotHandWaveAnimFrames{2} = MAIN_HAND_3_SPRITE;
    mascotHandWaveAnimFrames{3} = MAIN_HAND_2_SPRITE;

    HE_GREEN_SPRITE  = GFX_getAssetId("heGreen.png");
    HE_ORANGE_SPRITE = GFX_getAssetId("heOrange.png");
    HE_PINK_SPRITE   = GFX_getAssetId("hePink.png");
    HE_PURPLE_SPRITE = GFX_getAssetId("hePurple.png");
    HE_WHITE_SPRITE  = GFX_getAssetId("heWhite.png");
    LLO_GREEN_SPRITE  = GFX_getAssetId("lloGreen.png");
    LLO_ORANGE_SPRITE = GFX_getAssetId("lloOrange.png");
    LLO_PINK_SPRITE   = GFX_getAssetId("lloPink.png");
    LLO_PURPLE_SPRITE = GFX_getAssetId("lloPurple.png");
    LLO_WHITE_SPRITE  = GFX_getAssetId("lloWhite.png");

    ARC_1_SPRITE = GFX_getAssetId("arc1.png");
    ARC_2_SPRITE = GFX_getAssetId("arc2.png");
    ARC_3_SPRITE = GFX_getAssetId("arc3.png");
    ARC_4_SPRITE = GFX_getAssetId("arc4.png");
    ARC_5_SPRITE = GFX_getAssetId("arc5.png");

    arcsInCurtain[0].sprite = ARC_1_SPRITE;
    arcsInCurtain[1].sprite = ARC_2_SPRITE;
    arcsInCurtain[2].sprite = ARC_3_SPRITE;
    arcsInCurtain[3].sprite = ARC_4_SPRITE;
    arcsInCurtain[4].sprite = ARC_5_SPRITE;

    TWIST_PURPLE_ICON_1_SPRITE = GFX_getAssetId("twistP1.png");
    TWIST_PURPLE_ICON_2_SPRITE = GFX_getAssetId("twistP2.png");

    TAP_SIDE_ICON = GFX_getAssetId("tapSideicon.png");
    SIDE_TAP_INDICATOR = GFX_getAssetId("sideTapInd.png");
    SILUETTE_SPRITE = GFX_getAssetId("siluette.png");

    // Sounds
    ACTION_SOUND      = SND_getAssetId("action.mp3");
    GOOD_SOUND        = SND_getAssetId("good.mp3");
    EXCELLENT_1_SOUND = SND_getAssetId("excellent_1.mp3");
    EXCELLENT_2_SOUND = SND_getAssetId("excellent_2.mp3");
    
    SELECTOR_MENU_SOUND = SND_getAssetId("selec_menu.mp3");
    
    TAPS_1_2_SOUND = SND_getAssetId("taps_1-2.wav");
    TAPS_3_4_SOUND = SND_getAssetId("taps_3-4.wav");
    TAPS_5_6_SOUND = SND_getAssetId("taps_5-6.wav");
    TAP_STAGE_SUCCESS_SOUND = SND_getAssetId("tapsSuccess.mp3");

    PLUS_1_SHAPE_COLLECT_1_SOUND = SND_getAssetId("collect_1.mp3");
    PLUS_1_SHAPE_COLLECT_2_SOUND = SND_getAssetId("collect_2.mp3");
    PLUS_1_SHAPE_COLLECT_3_SOUND = SND_getAssetId("collect_3.mp3");

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

    GetMapping();
    
    SetApplicationState(FSM:firstLaunch);
}

public ON_Quit() {
}

public ON_Shake(const count) {
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
                    if(count > 1) {
                        ++tapTutorialStage;
                        if (tapTutorialStage >= 2) {
                            SND_play(TAPS_5_6_SOUND, SOUND_VOLUME);
                        } else if (tapTutorialStage >= 1) {
                            SND_play(TAPS_3_4_SOUND, SOUND_VOLUME);
                        } else if (tapTutorialStage >= 0) {
                            SND_play(TAPS_1_2_SOUND, SOUND_VOLUME);
                        }
                        mascotTapReactAnimFlag = 1;
                    }
                }
                if (!fillTapTutorial && beginTapTutorial && (count == 2)) {
                    fillTapTutorial = 1;
                }
            }
            case tiltTutorial: {
                if (count >= 2) {
                    if (selectorTutorial) {
                        if (tiltTutSelector.pos == 1) {
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
    if (SELF_ID == 0) {
        GetMapping();
    }

    SetDefaultMascot();
    
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
        } else if (applicationState == FSM:firstLaunch) {
            SetApplicationState(FSM:start);
        }
    }

    if (applicationState == FSM:tiltTutorial) {
        tiltTutBall.module = MODULES_MAX;
        tiltTutSelector.module = MODULES_MAX;
    }
}

public ON_Load(id, size, const pkt[]) {
    if (size == 0) {
        return;
    }
    alreadyLaunched = pkt[0];
}

public ON_Packet(type, size, const pkt[]) {
    switch (type) {
        case PKT_GENERAL_DATA: {
            new packetNumberReceived = pkt[3];
            if ((generalDataPkt < packetNumberReceived) || ((generalDataPkt - packetNumberReceived) > (0x7FFFFFFF >> 1))) {
                generalDataPkt = packetNumberReceived;
                SetApplicationState(parseByte(pkt, 3));
                new flags = pkt[4];
                beginTapTutorial = flags & 0x1;
                finishTiltTutorial = (flags >> 3) & 0x1;
                selectorTutorial = (flags >> 4) & 0x1;
                beginShakeTutorial = (flags >> 5) & 0x1;
                getstartedGreetingFlag = (flags >> 6) & 0x1;
                fillTapTutorial = (flags >> 7) & 0x1;
                flStartFlag = (flags >> 8) & 0x1;
                if ((parseByte(pkt, 4) & 0xF) > tapTutorialStage) {
                    mascotTapReactAnimFlag = 1;
                }
                for (new soundI = 0; soundI < SCREENS_MAX; ++soundI) {
                    tiltTutCollectableSounds{soundI} = (parseByte(pkt, 7) >> (soundI * 2)) & 0x3;
                }
                tapTutorialStage = parseByte(pkt, 4) & 0xF;
                shakeTutorialStage = parseByte(pkt, 5);
                twistTutorialStage = parseByte(pkt, 6);
                sideTapIndicatorPos = (parseByte(pkt, 4) >> 4) & 0xF;
                tutorialStartTimer = pkt[2];
            }
        }
        case PKT_BALL_TILT_TUT: {
            new packetNumberReceived = pkt[3];
            if ((ballPkt < packetNumberReceived) || ((ballPkt - packetNumberReceived) > (0xFFFFF >> 1))) {
                ballPkt = packetNumberReceived;
                tiltTutBall.module = parseByte(pkt, 1);
                tiltTutBall.screen = parseByte(pkt, 2);
                tiltTutBall.angle = pkt[1];
                tiltTutBall.pos = pkt[2];
                for (new item = 0; item < SCREENS_MAX; ++item) {
                    collectables{item} = (parseByte(pkt, 3) >> item) & 0x1;
                }
            }
        }
        case PKT_SELECTOR_TILT_TUT: {
            new packetNumberReceived = pkt[2];
            if ((selectorPkt < packetNumberReceived) || ((selectorPkt - packetNumberReceived) > (0xFFFFF >> 1))) {
                selectorPkt = packetNumberReceived;
                tiltTutSelector.module = parseByte(pkt, 1);
                if (tiltTutSelector.module == SELF_ID) {
                    selectorInAnimation = pkt[3];
                }
                tiltTutSelector.screen = parseByte(pkt, 2);
                tiltTutSelector.pos = pkt[1];
            }
        }
        case PKT_MAPPING: {
            new dataI = 0;
            for (new screenI = 3, offset = 0; screenI < MODULES_MAX * SCREENS_MAX; ++screenI, ++offset) {
                if (offset >= 5) {
                    offset = 0;
                    ++dataI;
                }
                getStarted_AllScreensData[screenI].face = (pkt[dataI] >> (5 * offset)) & 0x7;
                getStarted_AllScreensData[screenI].pos = (pkt[dataI] >> (3 + (5 * offset))) & 0x3;
            }
        }
    }

}