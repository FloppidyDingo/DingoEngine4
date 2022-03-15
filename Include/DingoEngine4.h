// DingoEngine4.h - Contains base engine stuff
#pragma once

#ifdef DINGOENGINE4_EXPORTS
#define DE4_API __declspec(dllexport)
#else
#define DE4_API __declspec(dllimport)
#endif

#define DE4_PLATFORMER 0
#define DE4_TOPDOWN 1
#define DE4_LIGHT_AMBIENT 1
#define DE4_LIGHT_POINT 2
#define DE4_MAX_INFOLOG_LENGTH 1024
#define DE4_VERSION "0.15.7b"

//DE4 key defines
#define DE4_MSE_RCLICK			   0
#define DE4_MSE_MCLICK			   1
#define DE4_MSE_LCLICK			   2
#define DE4_KEY_SPACE              32
#define DE4_KEY_APOSTROPHE         39  /* ' */
#define DE4_KEY_COMMA              44  /* , */
#define DE4_KEY_MINUS              45  /* - */
#define DE4_KEY_PERIOD             46  /* . */
#define DE4_KEY_SLASH              47  /* / */
#define DE4_KEY_0                  48
#define DE4_KEY_1                  49
#define DE4_KEY_2                  50
#define DE4_KEY_3                  51
#define DE4_KEY_4                  52
#define DE4_KEY_5                  53
#define DE4_KEY_6                  54
#define DE4_KEY_7                  55
#define DE4_KEY_8                  56
#define DE4_KEY_9                  57
#define DE4_KEY_SEMICOLON          59  /* ; */
#define DE4_KEY_EQUAL              61  /* = */
#define DE4_KEY_A                  65
#define DE4_KEY_B                  66
#define DE4_KEY_C                  67
#define DE4_KEY_D                  68
#define DE4_KEY_E                  69
#define DE4_KEY_F                  70
#define DE4_KEY_G                  71
#define DE4_KEY_H                  72
#define DE4_KEY_I                  73
#define DE4_KEY_J                  74
#define DE4_KEY_K                  75
#define DE4_KEY_L                  76
#define DE4_KEY_M                  77
#define DE4_KEY_N                  78
#define DE4_KEY_O                  79
#define DE4_KEY_P                  80
#define DE4_KEY_Q                  81
#define DE4_KEY_R                  82
#define DE4_KEY_S                  83
#define DE4_KEY_T                  84
#define DE4_KEY_U                  85
#define DE4_KEY_V                  86
#define DE4_KEY_W                  87
#define DE4_KEY_X                  88
#define DE4_KEY_Y                  89
#define DE4_KEY_Z                  90
#define DE4_KEY_LEFT_BRACKET       91  /* [ */
#define DE4_KEY_BACKSLASH          92  /* \ */
#define DE4_KEY_RIGHT_BRACKET      93  /* ] */
#define DE4_KEY_GRAVE_ACCENT       96  /* ` */
#define DE4_KEY_WORLD_1            161 /* non-US #1 */
#define DE4_KEY_WORLD_2            162 /* non-US #2 */
#define DE4_KEY_ESCAPE             256
#define DE4_KEY_ENTER              257
#define DE4_KEY_TAB                258
#define DE4_KEY_BACKSPACE          259
#define DE4_KEY_INSERT             260
#define DE4_KEY_DELETE             261
#define DE4_KEY_RIGHT              262
#define DE4_KEY_LEFT               263
#define DE4_KEY_DOWN               264
#define DE4_KEY_UP                 265
#define DE4_KEY_PAGE_UP            266
#define DE4_KEY_PAGE_DOWN          267
#define DE4_KEY_HOME               268
#define DE4_KEY_END                269
#define DE4_KEY_CAPS_LOCK          280
#define DE4_KEY_SCROLL_LOCK        281
#define DE4_KEY_NUM_LOCK           282
#define DE4_KEY_PRINT_SCREEN       283
#define DE4_KEY_PAUSE              284
#define DE4_KEY_F1                 290
#define DE4_KEY_F2                 291
#define DE4_KEY_F3                 292
#define DE4_KEY_F4                 293
#define DE4_KEY_F5                 294
#define DE4_KEY_F6                 295
#define DE4_KEY_F7                 296
#define DE4_KEY_F8                 297
#define DE4_KEY_F9                 298
#define DE4_KEY_F10                299
#define DE4_KEY_F11                300
#define DE4_KEY_F12                301
#define DE4_KEY_F13                302
#define DE4_KEY_F14                303
#define DE4_KEY_F15                304
#define DE4_KEY_F16                305
#define DE4_KEY_F17                306
#define DE4_KEY_F18                307
#define DE4_KEY_F19                308
#define DE4_KEY_F20                309
#define DE4_KEY_F21                310
#define DE4_KEY_F22                311
#define DE4_KEY_F23                312
#define DE4_KEY_F24                313
#define DE4_KEY_F25                314
#define DE4_KEY_KP_0               320
#define DE4_KEY_KP_1               321
#define DE4_KEY_KP_2               322
#define DE4_KEY_KP_3               323
#define DE4_KEY_KP_4               324
#define DE4_KEY_KP_5               325
#define DE4_KEY_KP_6               326
#define DE4_KEY_KP_7               327
#define DE4_KEY_KP_8               328
#define DE4_KEY_KP_9               329
#define DE4_KEY_KP_DECIMAL         330
#define DE4_KEY_KP_DIVIDE          331
#define DE4_KEY_KP_MULTIPLY        332
#define DE4_KEY_KP_SUBTRACT        333
#define DE4_KEY_KP_ADD             334
#define DE4_KEY_KP_ENTER           335
#define DE4_KEY_KP_EQUAL           336
#define DE4_KEY_LEFT_SHIFT         340
#define DE4_KEY_LEFT_CONTROL       341
#define DE4_KEY_LEFT_ALT           342
#define DE4_KEY_LEFT_SUPER         343
#define DE4_KEY_RIGHT_SHIFT        344
#define DE4_KEY_RIGHT_CONTROL      345
#define DE4_KEY_RIGHT_ALT          346
#define DE4_KEY_RIGHT_SUPER        347
#define DE4_KEY_MENU               348

//Main Engine Functions----------------------------------------------------------------------------------------------
DE4_API void DE4Start(bool debug, int resx, int resy, bool profile, int framerate, void (*init)(void), int argc, char** argv);
DE4_API void DE4SetScene(unsigned int sceneID);
DE4_API void DE4SetSFXVolume(float volume);
DE4_API float DE4GetSFXVolume();
DE4_API void DE4SetMusicVolume(float volume);
DE4_API float DE4GetMusicVolume();
DE4_API void DE4SetFullScreen(bool fullScreen);
DE4_API void DE4SetGlobalScale(float scale);
DE4_API float DE4GetGlobalScale();
DE4_API void DE4SetTitle(const char title[]);
DE4_API void DE4Exit();
DE4_API void DE4SetPhysicsEnabled(bool enabled);
DE4_API void DE4AssignThreadContext();
DE4_API void DE4ReleaseThreadContext();
//DE4_API void DE4

//Pysics Functions-----------------------------------------------------------------------------------------------------
DE4_API void PHYSetMode(unsigned int mode);
DE4_API void PHYSetTerminalVelocity(float vel);
DE4_API void PHYSetGravity(float grav);
DE4_API void PHYSetCamPostion(float cam[]);
DE4_API void PHYGetCamPosition(float cam[]);
DE4_API void PHYSetCamVector(float vec[]);
DE4_API void PHYGetCamVector(float vec[]);
DE4_API unsigned int PHYAddNoCollide(unsigned int groupA, unsigned int groupB);
DE4_API void PHYRemoveNoCollide(unsigned int id);
DE4_API unsigned int* PHYGetNoCollide(unsigned int id);
DE4_API void PHYClearNoCollides();
//DE4_API void PHY

//Entity Methods-----------------------------------------------------------------------------------------------------
DE4_API void ENTAssign(unsigned int code);
DE4_API unsigned int ENTCreate();
DE4_API void ENTDestroy(unsigned int code);
DE4_API void ENTDestroyAll();
DE4_API float ENTGetX();
DE4_API float ENTGetY();
DE4_API float ENTGetWidth();
DE4_API float ENTGetHeight();
DE4_API void ENTSetX(float x);
DE4_API void ENTSetY(float y);
DE4_API void ENTSetPosition(float x, float y);
DE4_API void ENTGetDir(float vec[]);
DE4_API float ENTGetDirX();
DE4_API float ENTGetDirY();
DE4_API void ENTSetDir(float vec[]);
DE4_API void ENTSetDir(float dx, float dy);
DE4_API void ENTSetDirX(float dirx);
DE4_API void ENTSetDirY(float diry);
DE4_API void ENTSetFrictionProfile(float top, float bottom, float left, float right);
DE4_API void ENTApplyForce(float dx, float dy);
DE4_API void ENTApplyForce(float vec[]);
DE4_API void ENTSetTileSheet(unsigned int tileid);
DE4_API void ENTSetTileSheet();
DE4_API void ENTAddAnimation(unsigned int aniid);
DE4_API void ENTAddAnimation();
DE4_API void ENTRemoveAnimation(unsigned int aniid);
DE4_API void ENTRemoveAnimation();
DE4_API void ENTClearAnimations();
DE4_API void ENTStopAllAnimations();
DE4_API void ENTSetFrame(unsigned int frame);
DE4_API void ENTSetCollisionGroup(unsigned int group);
DE4_API unsigned int ENTGetCollisionGroup();
DE4_API void ENTSetMass(float mass);
DE4_API float ENTGetMass();
DE4_API void ENTSetVisible(bool vis);
DE4_API bool ENTIsVisible();
DE4_API void ENTSetSolid(bool solid);
DE4_API bool ENTIsSolid();
DE4_API void ENTSetScale(float scale);
DE4_API void ENTSetScaleX(float scale);
DE4_API void ENTSetScaleY(float scale);
DE4_API float ENTGetScale();
DE4_API float ENTGetScaleX();
DE4_API float ENTGetScaleY();
DE4_API void ENTSetOpacity(float opac);
DE4_API float ENTGetOpacity();
DE4_API char* ENTGetID();
DE4_API void ENTSetID(const char id[]);
DE4_API void ENTSetInvertX(bool invert);
DE4_API void ENTSetInvertY(bool invert);
DE4_API bool ENTIsInvertX();
DE4_API bool ENTIsInvertY();
DE4_API unsigned int ENTGetIDCode();
//DE4_API void ENT();

//Scene--------------------------------------------------------------------------------------------------------------
DE4_API void SCNAssign(unsigned int code);
DE4_API unsigned int SCNCreate();
DE4_API void SCNDestroy(unsigned int code);
DE4_API void SCNDestroyAll();
DE4_API void SCNAddEntity();
DE4_API void SCNAddEntity(unsigned int entCode);
DE4_API void SCNAddLight();
DE4_API void SCNAddLight(unsigned int lightCode);
DE4_API void SCNAddGUIEntity();
DE4_API void SCNAddGUIEntity(unsigned int entCode);
DE4_API void SCNAddGUIText(unsigned int textCode);
DE4_API void SCNAddGUIText();
DE4_API void SCNAddTrigger();
DE4_API void SCNAddTrigger(unsigned int trigCode);
DE4_API void SCNRemoveEntity();
DE4_API void SCNRemoveEntity(unsigned int entCode);
DE4_API void SCNRemoveLight();
DE4_API void SCNRemoveLight(unsigned int lightCode);
DE4_API void SCNRemoveGUIEntity();
DE4_API void SCNRemoveGUIEntity(unsigned int entCode);
DE4_API void SCNRemoveGUIText(unsigned int textCode);
DE4_API void SCNRemoveGUIText();
DE4_API void SCNRemoveTrigger();
DE4_API void SCNRemoveTrigger(unsigned int trigCode);
DE4_API void SCNClearEntities();
DE4_API void SCNClearGUI();
DE4_API void SCNClearLights();
DE4_API void SCNClearTriggers();
DE4_API void SCNClearAll();
//DE4_API void SCN

//Animation----------------------------------------------------------------------------------------------------------
DE4_API void ANIAssign(unsigned int code);
DE4_API unsigned int ANICreate();
DE4_API void ANIDestroy(unsigned int code);
DE4_API void ANIDestroyAll();
DE4_API void ANISetID(const char id[]);
DE4_API char* ANIGetID();
DE4_API void ANISetFrameSpacing(unsigned int space);
DE4_API unsigned int ANIGetFrameSpacing();
DE4_API void ANISetRunning(bool running);
DE4_API bool ANIIsRunning();
DE4_API void ANISetSinglePlay(bool single);
DE4_API bool ANIIsSinglePlay();
DE4_API unsigned int ANIGetCurrentFrame();
DE4_API unsigned int ANIGetCurrentTile();
DE4_API void ANIAddFrame(unsigned int frame);
DE4_API void ANIClearFrames();
//DE4_API void ANI

//Tilesheet----------------------------------------------------------------------------------------------------------
DE4_API void TLSAssign(unsigned int codeID);
DE4_API unsigned int TLSCreate();
DE4_API void TLSDestroy(unsigned int codeID);
DE4_API void TLSDestroyAll();
DE4_API void TLSGenSheet(const char url_base[], const char url_def[]);
DE4_API float TLSGetFrameWidth(unsigned int frame);
DE4_API float TLSGetFrameHeight(unsigned int frame);
//DE4_API void TLS

//Lights-------------------------------------------------------------------------------------------------------------
DE4_API void LGTAssign(unsigned int codeID);
DE4_API unsigned int LGTCreate();
DE4_API void LGTDestroy(unsigned int codeID);
DE4_API void LGTDestroyAll();
DE4_API void LGTSetX(float x);
DE4_API void LGTSetY(float y);
DE4_API void LGTSetPosition(float x, float y);
DE4_API void LGTSetType(unsigned int type);
DE4_API unsigned int LGTGetType();
DE4_API void LGTSetRadius(float radius);
DE4_API float LGTGetRadius();
DE4_API void LGTSetBrightness(float bright);
DE4_API float LGTGetBrightness();
DE4_API void LGTSetColor(float red, float green, float blue);
DE4_API void LGTSetID(const char id[]);
DE4_API char* LGTGetID();
//DE4_API void LGT

//Event Handler------------------------------------------------------------------------------------------------------
DE4_API void EVTSetUpdate(void (*func)());
DE4_API void EVTSetCollision(void (*func)(unsigned int idA, unsigned int idB));
DE4_API void EVTSetKeyPressed(void (*func)(const char ID[]));
DE4_API void EVTSetKeyReleased(void (*func)(const char ID[]));
DE4_API void EVTSetKeyboardEvent(void (*func)());
DE4_API void EVTSetMousePressed(void (*func)(unsigned int button, float x, float y));
DE4_API void EVTSetMouseReleased(void (*func)(unsigned int button, float x, float y));
DE4_API void EVTSetPostPhysicsTick(void (*func)());
DE4_API void EVTAddKey(const char name[], int key);
DE4_API void EVTRemoveKey(const char name[]);
DE4_API void EVTClearKeys();
DE4_API void EVTGetMousePos(float pos[]);
DE4_API void EVTSetTriggerCallback(void (*func)(const char ID[], unsigned int entityID));
//DE4_API void EVT

//Triggers-----------------------------------------------------------------------------------------------------------
DE4_API void TRGAssign(unsigned int codeID);
DE4_API unsigned int TRGCreate();
DE4_API void TRGDestroy(unsigned int codeID);
DE4_API void TRGDestroyAll();
DE4_API void TRGSetSize(float width, float height);
DE4_API void TRGSetX(float x);
DE4_API void TRGSetY(float y);
DE4_API void TRGSetPosition(float x, float y);
DE4_API void TRGSetID(const char id[]);
DE4_API float TRGGetX();
DE4_API float TRGGetY();
DE4_API float TRGGetWidth();
DE4_API float TRGGetHeight();
DE4_API char* TRGGetID();
DE4_API void TRGSetEnabled(bool en);
DE4_API bool TRGIsEnabled();
//DE4_API void TRG

//Audio Engine-------------------------------------------------------------------------------------------------------
DE4_API unsigned int AUDCreateSource();
DE4_API void AUDAssignSource(unsigned int codeID);
DE4_API void AUDDestroySource(unsigned int id);
DE4_API void AUDDestroyAllSources();
DE4_API unsigned int AUDLoadBuffer(const char path[]);
DE4_API void AUDDestroyBuffer(unsigned int id);
DE4_API void AUDPlaySound(unsigned int bufferID, bool loop);
DE4_API void AUDStopSound();
DE4_API void AUDPauseSound();
DE4_API void AUDResetSound();
DE4_API void AUDSetTimeStamp(float time);
DE4_API void AUDSetLOC(float x, float y);
DE4_API void AUDSetX(float x);
DE4_API void AUDSetY(float y);
DE4_API void AUDSetSpacial(bool spacial);
DE4_API void AUDSetVolume(float volume);
DE4_API void AUDSetBalance(float balance);
DE4_API float AUDGetBalance();
DE4_API float AUDGetVolume();
DE4_API float AUDGetTimeStamp();
DE4_API float AUDGetX();
DE4_API float AUDGetY();
DE4_API bool AUDisPlaying();
DE4_API bool AUDisSpatial();
//DE4_API void AUD

//Map Generation-----------------------------------------------------------------------------------------------------
DE4_API void MAPSetTileFolder(const char path[]);
DE4_API void MAPStartTagCallback(void(*func)(const char tag[]));
DE4_API void MAPVariableCallback(void(*func)(const char id[], const char value[]));
DE4_API void MAPEntityCreationCallback(bool(*func)(unsigned int codeID));
DE4_API void MAPTriggerCreationCallback(bool(*func)(unsigned int codeID));
DE4_API void MAPLightCreationCallback(bool(*func)(unsigned int codeID));
DE4_API void MAPGenerate(const char path[], unsigned int sceneID);
//DE4_API void MAP

//Utilities----------------------------------------------------------------------------------------------------------
DE4_API float UTILRange(unsigned int idA, unsigned int idB);
DE4_API bool UTILIntersect(unsigned int idA, unsigned int idB);
//DE4_API void UTIL

//Game States--------------------------------------------------------------------------------------------------------
DE4_API void GMSTAssign(unsigned int id);
DE4_API unsigned int GMSTCreate();
DE4_API unsigned int GMSTCreate(void (*func)());
DE4_API void GMSTDestroy(unsigned int id);
DE4_API void GMSTDestroyAll();
DE4_API void GMSTSetUpdate(void (*func)());
DE4_API void GMSTSetKeyPress(void (*func)(const char* id));
DE4_API void GMSTSetKeyRelease(void (*func)(const char* id));
DE4_API void GMSTSetCollision(void (*func)(unsigned int ida, unsigned int idb));
DE4_API void GMSTSetKeyboardEvent(void (*func)());
DE4_API void GMSTSetMousePressed(void (*func)(unsigned int button, float x, float y));
DE4_API void GMSTSetMouseReleased(void (*func)(unsigned int button, float x, float y));
DE4_API void GMSTSeTrigger(void (*func)(const char* id, unsigned int codeID));
DE4_API void GMSTSetOnEnabledChange(void (*func)(bool enabled));
DE4_API void GMSTSetEnabled(bool enabled);
DE4_API void GMSTDisableAll();
//DE4_API void GMST

//Text---------------------------------------------------------------------------------------------------------------
DE4_API void TXTAssign(unsigned int id);
DE4_API unsigned int TXTCreate();
DE4_API void TXTDestroy(unsigned int id);
DE4_API void TXTDestroyAll();
DE4_API void TXTSetText(const char text[]);
DE4_API void TXTSetSheet(unsigned int sheet);
DE4_API void TXTSetID(const char id[]);
DE4_API void TXTSetX(float x);
DE4_API void TXTSetY(float y);
DE4_API void TXTSetPosition(float x, float y);
DE4_API void TXTSetScale(float scale);
DE4_API void TXTSetFont(unsigned int font);
DE4_API void TXTSetVisible(bool visible);
DE4_API char* TXTGetText();
DE4_API char* TXTGetID();
DE4_API float TXTGetX();
DE4_API float TXTGetY();
DE4_API float TXTGetScale();
DE4_API unsigned int TXTGetFont();
DE4_API bool TXTIsVisible();
DE4_API float TXTGetWidth();
DE4_API float TXTGetHeight();
//DE4_API void TXT
