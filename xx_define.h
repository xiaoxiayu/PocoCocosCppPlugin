#ifndef _XX_AUTOTEST_DEFINE_H
#define _XX_AUTOTEST_DEFINE_H

#define _XX_AUTOTEST_

#include <string>
#include "cocos2d.h"

#ifdef WIN32
#define XX_SERVER_PORT 9091
#else
#define XX_SERVER_PORT 9090
#endif
#define MAX_PACKET 4096 

#if COCOS2D_VERSION >= 0x00020100 && COCOS2D_VERSION < 0x00030000
#define COCOS2D_CPP_2X

#define XXCCLog CCLog
#define XXCCNode CCNode
#define XXCCTexture2D CCTexture2D
#define XXCCScene	CCScene
#define XXCCLayer	CCLayer
#define XXCCDirector CCDirector
#define XXCCSize CCSize
#define XXCCPoint CCPoint
#define XXCCRect CCRect
#define XXCCSprite CCSprite
#define XXCCMenuItem CCMenuItem
#define XXCCAtlasNode CCAtlasNode
#define XXCCLabelTTF CCLabelTTF
#define XXCCLabelAtlas CCLabelAtlas
#define XXCCMenuItemLabel CCMenuItemLabel
#define XXCCMenuItemSprite CCMenuItemSprite
#define XXCCMenuItemToggle CCMenuItemToggle
#define XXCCSpriteBatchNode CCSpriteBatchNode
#define XXCCScale9Sprite extension::CCScale9Sprite
#define XXCCTextureCache CCTextureCache

#define XXCCSize CCSize
#define XXCCRenderTexture CCRenderTexture

#define funcDirectorGetInstance sharedDirector
#define funcNodeGetZOder getZOrder
#define funcGetString getString
#define funcGetBoundingBox boundingBox
#define funcGetOffsetPosition() getOffsetPosition()
#define funcPoint ccp

#define XXCCVec2 kmVec2

#elif COCOS2D_VERSION < 0x00040000
#define COCOS2D_CPP_3X

#define XXCCLog log
#define XXCCNode Node
#define XXCCTexture2D Texture2D
#define XXCCScene	Scene
#define XXCCLayer Layer
#define XXCCDirector Director
#define XXCCSize Size
#define XXCCPoint Point
#define XXCCRect Rect
#define XXCCSprite Sprite
#define XXCCMenuItem MenuItem
#define XXCCAtlasNode AtlasNode
#define XXCCLabelTTF LabelTTF
#define XXCCLabelAtlas LabelAtlas
#define XXCCMenuItemLabel MenuItemLabel
#define XXCCMenuItemSprite MenuItemSprite
#define XXCCMenuItemToggle MenuItemToggle
#define XXCCSpriteBatchNode SpriteBatchNode
#define XXCCScale9Sprite extension::Scale9Sprite
#define XXCCTextureCache TextureCache
#define XXCCLabel Label
#define XXCCApplication CCApplication

#define XXCCSize Size
#define XXCCRenderTexture RenderTexture

#define funcDirectorGetInstance getInstance
#define funcNodeGetZOder getLocalZOrder
#define funcGetString getString().c_str
#define funcGetBoundingBox getBoundingBox
#define funcGetOffsetPosition() getBoundingBox().origin
#define funcPoint Point

#define XXCCVec2 Vec2

#if COCOS2D_VERSION >= 0x00030100 && COCOS2D_VERSION < 0x00030200
#define COCOS2D_CPP_31
#endif
#if COCOS2D_VERSION >= 0x00030200
#define COCOS2D_CPP_32
#define COCOS2D_CPP_32_ABOVE
#endif
#if COCOS2D_VERSION < 0x00030100
#define COCOS2D_CPP_30
#endif

#endif 


#endif //_XX_AUTOTEST_DEFINE_H
