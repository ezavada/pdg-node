// -----------------------------------------------
// pdg_script_interface.h
//
// wrapper definitions for pdg classes 
// that are exposed to scripting languages without
// any specific language dependencies
//
// This file is parsed by tools/gen-script_interface.sh
// to generate an interface file
//
// This should only be done when the wrappers need 
// to change.
//
// Written by Ed Zavada, 2012-2013
// Copyright (c) 2013, Dream Rock Studios, LLC
// All Rights Reserved Worldwide
// -----------------------------------------------

#ifdef PDG_GENERATING_DOCS
#include "../../tools/js_docs_macros.h"
namespace pdg {
#else

#undef THIS // eliminate warning about Windows duplicate Macro
#include "pdg_script_macros.h"

%#ifndef PDG_INTERFACES_H_INCLUDED
%#define PDG_INTERFACES_H_INCLUDED

%#include "pdg_project.h"

%#include "pdg_script_impl.h"
%#include "pdg_script.h"

%#ifndef PDG_NO_APP_FRAMEWORK
%#define PDG_NO_APP_FRAMEWORK
%#endif
%#include "pdg/framework.h"

%#include <cstdlib>

namespace pdg {


#endif //!PDG_GENERATING_DOCS


// ========================================================================================
// Class Declarations
// ========================================================================================
//
// There are 4 types of classes:
//
// BINDING_CLASS - Native C++ instances can be instantiated by Javascript code.
// 					var a = new MyClass();
//					Lifecycle of the Native C++ instance is tied to lifecycle of
//					the Javascript object
// 
// SINGLETON_CLASS - there is only one Native instance, instantiated first time it is used in
//					Javascript or in C++. Javascript should never call new MySingletonClass(), 
//					instead it should call getMySingletonClass()
//
// WRAPPER_CLASS - Native C++ instances must be instantiated through some kind of 
//                  a factory call, often from another object. This is not a singleton,
//					so calling createMySingletonClass() multiple times will create
//					multiple Native C++ instances, however the lifecycle of the native
//					C++ object is not tied to the lifecycle of the Javascript object
//
// FACADE_CLASS	- There is no Native C++ instance. This is just a facade for a bunch of 
//					static function calls that we want grouped together

BINDING_CLASS(MemBlock)
  METHOD(MemBlock, GetData)
  METHOD(MemBlock, GetDataSize)
  METHOD(MemBlock, GetByte)
  METHOD(MemBlock, GetBytes)
DECL_END

SINGLETON_CLASS(ConfigManager)
  METHOD(ConfigManager, UseConfig)
  METHOD(ConfigManager, GetConfigString)
  METHOD(ConfigManager, GetConfigLong)
  METHOD(ConfigManager, GetConfigFloat)
  METHOD(ConfigManager, GetConfigBool)
  METHOD(ConfigManager, SetConfigString)
  METHOD(ConfigManager, SetConfigLong)
  METHOD(ConfigManager, SetConfigFloat)
  METHOD(ConfigManager, SetConfigBool)
DECL_END

SINGLETON_CLASS(LogManager)
  CONSTANT(LogManager, init_CreateUniqueNewFile)
  CONSTANT(LogManager, init_OverwriteExisting)
  CONSTANT(LogManager, init_AppendToExisting)
  CONSTANT(LogManager, init_StdOut)
  CONSTANT(LogManager, init_StdErr)
  PROPERTY(LogManager, LogLevel)
  METHOD(LogManager, Initialize)
  METHOD(LogManager, WriteLogEntry)
  METHOD(LogManager, BinaryDump)
DECL_END

BINDING_CLASS(IEventHandler)
DECL_END

#define EMITTER_METHODS(klass)  \
  METHOD(klass, AddHandler)     CR \
  METHOD(klass, RemoveHandler)  CR \
  METHOD(klass, Clear)          CR \
  METHOD(klass, BlockEvent)     CR \
  METHOD(klass, UnblockEvent)

BINDING_CLASS(EventEmitter)
  EMITTER_METHODS(EventEmitter)
//  METHOD(EventEmitter, PostEvent)
DECL_END

SINGLETON_CLASS(EventManager)
  EMITTER_METHODS(EventManager)
  METHOD(EventManager, IsKeyDown)
  METHOD(EventManager, IsRawKeyDown)
  METHOD(EventManager, IsButtonDown)
  METHOD(EventManager, GetDeviceOrientation)
DECL_END

SINGLETON_CLASS(ResourceManager)
  PROPERTY(ResourceManager, Language)
  METHOD(ResourceManager, OpenResourceFile)
  METHOD(ResourceManager, CloseResourceFile)
  METHOD(ResourceManager, GetImage)
  METHOD(ResourceManager, GetImageStrip)
%#ifndef PDG_NO_SOUND
  METHOD(ResourceManager, GetSound)
%#endif // !PDG_NO_SOUND
  METHOD(ResourceManager, GetString)
  METHOD(ResourceManager, GetResourceSize)
  METHOD(ResourceManager, GetResource)
  METHOD(ResourceManager, GetResourcePaths)
DECL_END

BINDING_CLASS(Serializer)
%#ifndef PDG_NO_64BIT
   METHOD(Serializer, Serialize_8)   // no 64 bit int in Javascript
   METHOD(Serializer, Serialize_8u)
%#endif
  METHOD(Serializer, Serialize_d)
  METHOD(Serializer, Serialize_f)
  METHOD(Serializer, Serialize_4)
  METHOD(Serializer, Serialize_4u)
  METHOD(Serializer, Serialize_3u)
  METHOD(Serializer, Serialize_2)
  METHOD(Serializer, Serialize_2u)
  METHOD(Serializer, Serialize_1)
  METHOD(Serializer, Serialize_1u)
  METHOD(Serializer, Serialize_bool)
  METHOD(Serializer, Serialize_uint)
  METHOD(Serializer, Serialize_color)
  METHOD(Serializer, Serialize_offset)
  METHOD(Serializer, Serialize_point)
  METHOD(Serializer, Serialize_vector)
  METHOD(Serializer, Serialize_rect)
  METHOD(Serializer, Serialize_rotr)
  METHOD(Serializer, Serialize_quad)
  METHOD(Serializer, Serialize_str)
  METHOD(Serializer, Serialize_mem)
  METHOD(Serializer, Serialize_obj)
  METHOD(Serializer, SerializedSize)
  METHOD(Serializer, GetDataSize)
  METHOD(Serializer, GetDataPtr)
DECL_END

BINDING_CLASS(Deserializer)
%#ifndef PDG_NO_64BIT
   METHOD(Deserializer, Deserialize_8)
   METHOD(Deserializer, Deserialize_8u)
%#endif
  METHOD(Deserializer, Deserialize_d)
  METHOD(Deserializer, Deserialize_f)
  METHOD(Deserializer, Deserialize_4)
  METHOD(Deserializer, Deserialize_4u)
  METHOD(Deserializer, Deserialize_3u)
  METHOD(Deserializer, Deserialize_2)
  METHOD(Deserializer, Deserialize_2u)
  METHOD(Deserializer, Deserialize_1)
  METHOD(Deserializer, Deserialize_1u)
  METHOD(Deserializer, Deserialize_bool)
  METHOD(Deserializer, Deserialize_uint)
  METHOD(Deserializer, Deserialize_color)
  METHOD(Deserializer, Deserialize_offset)
  METHOD(Deserializer, Deserialize_point)
  METHOD(Deserializer, Deserialize_vector)
  METHOD(Deserializer, Deserialize_rect)
  METHOD(Deserializer, Deserialize_rotr)
  METHOD(Deserializer, Deserialize_quad)
  METHOD(Deserializer, Deserialize_str)
  METHOD(Deserializer, Deserialize_strGetLen)
  METHOD(Deserializer, Deserialize_mem)
  METHOD(Deserializer, Deserialize_memGetLen)
  METHOD(Deserializer, Deserialize_obj)
  METHOD(Deserializer, SetDataPtr)
//  METHOD(Deserializer, RegisterClass)  STATIC
DECL_END

#define SERIALIZABLE_METHODS(klass)  \
  METHOD(klass, GetSerializedSize)  CR \
  METHOD(klass, Serialize)          CR \
  METHOD(klass, Deserialize)        CR \
  METHOD(klass, GetMyClassTag)

BINDING_CLASS(ISerializable)
DECL_END

#define IMAGE_METHODS(klass)  \
  PROPERTY(klass, TransparentColor) CR \
  PROPERTY(klass, Opacity)  		 CR \
  METHOD(klass, GetWidth)  		 CR \
  METHOD(klass, GetHeight)  		 CR \
  METHOD(klass, GetImageBounds)  	 CR \
  METHOD(klass, GetSubsection)  	 CR \
  METHOD(klass, SetEdgeClamping)    CR \
  METHOD(klass, RetainData)  		 CR \
  METHOD(klass, RetainAlpha)  		 CR \
  METHOD(klass, PrepareToRasterize) CR \
  METHOD(klass, GetAlphaValue)  	 CR \
  METHOD(klass, GetPixel)

BINDING_CLASS(Image)
  IMAGE_METHODS(Image)
DECL_END

BINDING_CLASS(ImageStrip)
  IMAGE_METHODS(ImageStrip)
  PROPERTY(ImageStrip, FrameWidth)
  PROPERTY(ImageStrip, NumFrames)
  METHOD(ImageStrip, GetFrame)
DECL_END

%#ifndef PDG_NO_GUI

SINGLETON_CLASS(GraphicsManager)
  METHOD(GraphicsManager, GetNumScreens)
  METHOD(GraphicsManager, GetNumSupportedScreenModes)
  METHOD(GraphicsManager, GetNthSupportedScreenMode)
  METHOD(GraphicsManager, GetCurrentScreenMode)
  METHOD(GraphicsManager, SetScreenMode)
  METHOD(GraphicsManager, CreateWindowPort)
  METHOD(GraphicsManager, CreateFullScreenPort)
  METHOD(GraphicsManager, CloseGraphicsPort)
  METHOD(GraphicsManager, CreateFont)
  METHOD(GraphicsManager, GetMainPort)
  METHOD(GraphicsManager, SwitchToFullScreenMode)
  METHOD(GraphicsManager, SwitchToWindowMode)
  METHOD(GraphicsManager, InFullScreenMode)
  METHOD(GraphicsManager, GetFPS)
  PROPERTY(GraphicsManager, TargetFPS)
  METHOD(GraphicsManager, GetMouse)
DECL_END

WRAPPER_CLASS(Font)
  METHOD(Font, GetFontName)
  METHOD(Font, GetFontHeight)
  METHOD(Font, GetFontLeading)
  METHOD(Font, GetFontAscent)
  METHOD(Font, GetFontDescent)
DECL_END

WRAPPER_CLASS(Port)
  PROPERTY(Port, ClipRect)
  PROPERTY(Port, Cursor)
  METHOD(Port, GetDrawingArea)
  METHOD(Port, FillRect)
  METHOD(Port, FrameRect)
  METHOD(Port, DrawLine)
  METHOD(Port, FrameOval)
  METHOD(Port, FillOval)
  METHOD(Port, FrameCircle)
  METHOD(Port, FillCircle)
  METHOD(Port, FrameRoundRect)
  METHOD(Port, FillRoundRect)
  METHOD(Port, FillRectEx)
  METHOD(Port, FrameRectEx)
  METHOD(Port, DrawLineEx)
  METHOD(Port, FillRectWithGradient)
  METHOD(Port, DrawText)
  METHOD(Port, DrawImage)
  METHOD(Port, DrawTexture)
  METHOD(Port, DrawTexturedSphere)
  METHOD(Port, GetTextWidth)
  METHOD(Port, GetCurrentFont)
  METHOD(Port, SetFont)
  METHOD(Port, SetFontForStyle)
  METHOD(Port, SetFontScalingFactor)
  METHOD(Port, StartTrackingMouse)
  METHOD(Port, StopTrackingMouse)
  METHOD(Port, ResetCursor)
DECL_END

%#endif // !PDG_NO_GUI

%#ifndef PDG_NO_SOUND
BINDING_CLASS(Sound)
  EMITTER_METHODS(Sound)
  PROPERTY(Sound, Volume)
  METHOD(Sound, Play)
  METHOD(Sound, Start)
  METHOD(Sound, Stop)
  METHOD(Sound, Pause)
  METHOD(Sound, Resume)
  METHOD(Sound, IsPaused)
  METHOD(Sound, SetLooping)
  METHOD(Sound, IsLooping)
  METHOD(Sound, SetPitch)
  METHOD(Sound, ChangePitch)
  METHOD(Sound, SetOffsetX)
  METHOD(Sound, ChangeOffsetX)
  METHOD(Sound, FadeOut)
  METHOD(Sound, FadeIn)
  METHOD(Sound, ChangeVolume)
  METHOD(Sound, Skip)
  METHOD(Sound, SkipTo)
DECL_END

SINGLETON_CLASS(SoundManager)
  METHOD(SoundManager, SetVolume)  // TODO: make property
  METHOD(SoundManager, SetMute)
  METHOD(SoundManager, Idle)
DECL_END
%#endif //! PDG_NO_SOUND

SINGLETON_CLASS(TimerManager)
  EMITTER_METHODS(TimerManager)
  METHOD(TimerManager, StartTimer)
  METHOD(TimerManager, CancelTimer)
  METHOD(TimerManager, CancelAllTimers)
  METHOD(TimerManager, DelayTimer)
  METHOD(TimerManager, DelayTimerUntil)
  METHOD(TimerManager, Pause)
  METHOD(TimerManager, Unpause)
  METHOD(TimerManager, IsPaused)
  METHOD(TimerManager, PauseTimer)
  METHOD(TimerManager, UnpauseTimer)
  METHOD(TimerManager, IsTimerPaused)
  METHOD(TimerManager, GetWhenTimerFiresNext)
  METHOD(TimerManager, GetMilliseconds)
DECL_END

FACADE_CLASS(FileManager)
  METHOD(FileManager, FindFirst)
  METHOD(FileManager, FindNext)
  METHOD(FileManager, FindClose)
  METHOD(FileManager, GetApplicationDataDirectory)
  METHOD(FileManager, GetApplicationDirectory)
  METHOD(FileManager, GetApplicationResourceDirectory)
DECL_END

BINDING_CLASS(IAnimationHelper)
DECL_END

#define ANIMATED_METHODS(klass) \
	PROPERTY(klass, Location) CR \
	PROPERTY(klass, Speed) CR \
	PROPERTY(klass, Velocity) CR \
	PROPERTY(klass, Width) CR \
	PROPERTY(klass, Height) CR \
	PROPERTY(klass, Rotation) CR \
	PROPERTY(klass, CenterOffset) CR \
	PROPERTY(klass, Spin) CR \
	PROPERTY(klass, Mass) CR \
	PROPERTY(klass, MoveFriction) CR \
	PROPERTY(klass, SpinFriction) CR \
	PROPERTY(klass, SizeFriction) CR \
	METHOD(klass, GetBoundingBox) CR \
	METHOD(klass, GetRotatedBounds) CR \
	METHOD(klass, Move) CR \
	METHOD(klass, MoveTo) CR \
	METHOD(klass, SetVelocityInRadians) CR \
	METHOD(klass, GetMovementDirectionInRadians) CR \
	METHOD(klass, StopMoving) CR \
	METHOD(klass, Accelerate) CR \
	METHOD(klass, AccelerateTo) CR \
	METHOD(klass, SetSize) CR \
	METHOD(klass, Grow) CR \
	METHOD(klass, Stretch) CR \
	METHOD(klass, StartGrowing) CR \
	METHOD(klass, StopGrowing) CR \
	METHOD(klass, StartStretching) CR \
	METHOD(klass, StopStretching) CR \
	METHOD(klass, Resize) CR \
	METHOD(klass, ResizeTo) CR \
	METHOD(klass, Rotate) CR \
	METHOD(klass, RotateTo) CR \
	METHOD(klass, StopSpinning) CR \
	METHOD(klass, ChangeCenter) CR \
	METHOD(klass, ChangeCenterTo) CR \
	METHOD(klass, Wait) CR \
	METHOD(klass, SetFriction) CR \
	METHOD(klass, ApplyForce) CR \
	METHOD(klass, ApplyTorque) CR \
	METHOD(klass, StopAllForces) CR \
	METHOD(klass, AddAnimationHelper) CR \
	METHOD(klass, RemoveAnimationHelper) CR \
	METHOD(klass, ClearAnimationHelpers)

BINDING_CLASS(Animated)
  ANIMATED_METHODS(Animated)
	METHOD(Animated, Animate)
// 	METHOD(Animated, OnLocationChanged)
// 	METHOD(Animated, OnSizeChanged)
// 	METHOD(Animated, OnRotationChanged)
// 	METHOD(Animated, OnCenterChanged)
DECL_END

%#ifdef PDG_USE_CHIPMUNK_PHYSICS
WRAPPER_CLASS(cpArbiter)
	METHOD(cpArbiter, IsFirstContact)
	METHOD(cpArbiter, GetCount)
	METHOD(cpArbiter, GetNormal)
	METHOD(cpArbiter, GetPoint)
	METHOD(cpArbiter, GetDepth)
DECL_END

WRAPPER_CLASS(cpConstraint)
	PROPERTY(cpConstraint, MaxForce)
	PROPERTY(cpConstraint, ErrorBias)
	PROPERTY(cpConstraint, MaxBias)
	PROPERTY(cpConstraint, Anchor)
	PROPERTY(cpConstraint, OtherAnchor)
	PROPERTY(cpConstraint, PinDist)
	PROPERTY(cpConstraint, SlideMinDist)
	PROPERTY(cpConstraint, SlideMaxDist)
	PROPERTY(cpConstraint, GrooveStart)
	PROPERTY(cpConstraint, GrooveEnd)
	PROPERTY(cpConstraint, SpringRestLength)
	PROPERTY(cpConstraint, SpringStiffness)
	PROPERTY(cpConstraint, SpringDamping)
	PROPERTY(cpConstraint, RotarySpringRestAngle)
	PROPERTY(cpConstraint, MinAngle)
	PROPERTY(cpConstraint, MaxAngle)
	PROPERTY(cpConstraint, RatchetAngle)
	PROPERTY(cpConstraint, RatchetPhase)
	PROPERTY(cpConstraint, RatchetInterval)
	PROPERTY(cpConstraint, GearRatio)
	PROPERTY(cpConstraint, GearInitialAngle)
	PROPERTY(cpConstraint, MotorSpinRate)
	METHOD(cpConstraint, GetType)	// return string of "pin, slide, etc..."
	METHOD(cpConstraint, ActivateBodies)
	METHOD(cpConstraint, GetImpulse)
	METHOD(cpConstraint, GetSprite)
	METHOD(cpConstraint, GetOtherSprite)
DECL_END

WRAPPER_CLASS(cpSpace)
	PROPERTY(cpSpace, IdleSpeedThreshold)
	PROPERTY(cpSpace, SleepTimeThreshold)
	PROPERTY(cpSpace, CollisionSlop)
	PROPERTY(cpSpace, CollisionBias)
	PROPERTY(cpSpace, CollisionPersistence)
	PROPERTY(cpSpace, EnableContactGraph)
	METHOD(cpSpace, UseSpatialHash)
	METHOD(cpSpace, ReindexStatic)
	METHOD(cpSpace, Step)
DECL_END
%#endif

BINDING_CLASS(ISpriteCollideHelper)
DECL_END

%#ifndef PDG_NO_GUI
BINDING_CLASS(ISpriteDrawHelper)
DECL_END
%#endif //!PDG_NO_GUI

BINDING_CLASS(Sprite)
  EMITTER_METHODS(Sprite)
  ANIMATED_METHODS(Sprite)
  SERIALIZABLE_METHODS(Sprite)
	PROPERTY(Sprite, WantsAnimLoopEvents)
	PROPERTY(Sprite, WantsAnimEndEvents)
	PROPERTY(Sprite, Opacity)
	PROPERTY(Sprite, CollisionRadius)
	PROPERTY(Sprite, Elasticity)
	METHOD(Sprite, SetWantsCollideWallEvents)
	METHOD(Sprite, GetFrameRotatedBounds)
	METHOD(Sprite, SetFrame)
	METHOD(Sprite, GetCurrentFrame)
    METHOD(Sprite, GetFrameCount)
	METHOD(Sprite, StartFrameAnimation)
	METHOD(Sprite, StopFrameAnimation)
	METHOD(Sprite, AddFramesImage)
%#ifdef PDG_SCML_SUPPORT
	METHOD(Sprite, HasAnimation)
	METHOD(Sprite, StartAnimation)
	METHOD(Sprite, SetEntityScale)
%#endif // PDG_SCML_SUPPORT
%#ifndef PDG_NO_GUI
	PROPERTY(Sprite, WantsMouseOverEvents)
	PROPERTY(Sprite, WantsClickEvents)
	PROPERTY(Sprite, MouseDetectMode)
	METHOD(Sprite, SetWantsOffscreenEvents)
	METHOD(Sprite, SetDrawHelper)
	METHOD(Sprite, SetPostDrawHelper)
%#endif //!PDG_NO_GUI
	METHOD(Sprite, ChangeFramesImage)
	METHOD(Sprite, OffsetFrameCenters)
	METHOD(Sprite, GetFrameCenterOffset)
	METHOD(Sprite, FadeTo)
	METHOD(Sprite, FadeIn)
	METHOD(Sprite, FadeOut)
	METHOD(Sprite, IsBehind)
	METHOD(Sprite, GetZOrder)
	METHOD(Sprite, MoveBehind)
	METHOD(Sprite, MoveInFrontOf)
	METHOD(Sprite, MoveToFront)
	METHOD(Sprite, MoveToBack)
	METHOD(Sprite, EnableCollisions)
	METHOD(Sprite, DisableCollisions)
	METHOD(Sprite, UseCollisionMask)
	METHOD(Sprite, SetCollisionHelper);
	METHOD(Sprite, SetUserData)
	METHOD(Sprite, FreeUserData)
	METHOD(Sprite, GetLayer)
%#ifdef PDG_USE_CHIPMUNK_PHYSICS
	PROPERTY(Sprite, CollideGroup)
	METHOD(Sprite, MakeStatic)
	METHOD(Sprite, GetFriction)
	METHOD(Sprite, PinJoint)
	METHOD(Sprite, SlideJoint)
	METHOD(Sprite, PivotJoint)
	METHOD(Sprite, GrooveJoint)
	METHOD(Sprite, SpringJoint)
	METHOD(Sprite, RotarySpring)
	METHOD(Sprite, RotaryLimit)
	METHOD(Sprite, Ratchet)
	METHOD(Sprite, Gear)
	METHOD(Sprite, Motor)
	METHOD(Sprite, RemoveJoint)
	METHOD(Sprite, Disconnect)
	METHOD(Sprite, MakeJointBreakable)
	METHOD(Sprite, MakeJointUnbreakable)
%#endif
DECL_END

#define SPRITE_LAYER_METHODS(klass) \
	METHOD(klass, SetSerializationFlags) CR \
	METHOD(klass, StartAnimations) CR \
	METHOD(klass, StopAnimations) CR \
	METHOD(klass, Hide) CR \
	METHOD(klass, Show) CR \
	METHOD(klass, IsHidden) CR \
	METHOD(klass, FadeIn) CR \
	METHOD(klass, FadeOut) CR \
	METHOD(klass, MoveBehind) CR \
	METHOD(klass, MoveInFrontOf) CR \
	METHOD(klass, MoveToFront) CR \
	METHOD(klass, MoveToBack) CR \
	METHOD(klass, GetZOrder) CR \
	METHOD(klass, MoveWith) CR \
	METHOD(klass, FindSprite) CR \
	METHOD(klass, GetNthSprite) CR \
	METHOD(klass, GetSpriteZOrder) CR \
	METHOD(klass, IsSpriteBehind) CR \
	METHOD(klass, HasSprite) CR \
	METHOD(klass, AddSprite) CR \
	METHOD(klass, RemoveSprite) CR \
	METHOD(klass, RemoveAllSprites) CR \
	METHOD(klass, EnableCollisions) CR \
	METHOD(klass, DisableCollisions) CR \
	METHOD(klass, EnableCollisionsWithLayer) CR \
	METHOD(klass, DisableCollisionsWithLayer) CR \
	METHOD(klass, CreateSprite)
//	METHOD(klass, CloneSprite)

%#ifndef PDG_NO_GUI
#define SPRITE_LAYER_GUI_METHODS(klass) \
	PROPERTY(klass, SpritePort) CR \
	PROPERTY(klass, Origin) CR \
	PROPERTY(klass, Zoom) CR \
	METHOD(klass, Zoom) CR \
	METHOD(klass, ZoomTo) CR \
	METHOD(klass, SetAutoCenter) CR \
	METHOD(klass, SetFixedMoveAxis) CR \
	METHOD(klass, LayerToPortPoint) CR \
	METHOD(klass, LayerToPortOffset) CR \
	METHOD(klass, LayerToPortVector) CR \
	METHOD(klass, LayerToPortRect) CR \
	METHOD(klass, LayerToPortQuad) CR \
	METHOD(klass, PortToLayerPoint) CR \
	METHOD(klass, PortToLayerOffset) CR \
	METHOD(klass, PortToLayerVector) CR \
	METHOD(klass, PortToLayerRect) CR \
	METHOD(klass, PortToLayerQuad)
%#endif

#define SPRITE_LAYER_CHIPMUNK_METHODS(klass) \
	METHOD(klass, SetUseChipmunkPhysics) CR \
	METHOD(klass, SetStaticLayer) CR \
	METHOD(klass, SetGravity) CR \
	METHOD(klass, SetKeepGravityDownward) CR \
	METHOD(klass, SetDamping) CR \
	METHOD(klass, GetSpace)

BINDING_CLASS(SpriteLayer)
  EMITTER_METHODS(SpriteLayer)
  ANIMATED_METHODS(SpriteLayer)
  SERIALIZABLE_METHODS(SpriteLayer)
  SPRITE_LAYER_METHODS(SpriteLayer)
%#ifndef PDG_NO_GUI
  SPRITE_LAYER_GUI_METHODS(SpriteLayer)
%#endif
%#ifdef PDG_USE_CHIPMUNK_PHYSICS
  SPRITE_LAYER_CHIPMUNK_METHODS(SpriteLayer)
%#endif
%#ifdef PDG_SCML_SUPPORT
	METHOD(SpriteLayer, CreateSpriteFromSCML)
	METHOD(SpriteLayer, CreateSpriteFromSCMLFile)
	METHOD(SpriteLayer, CreateSpriteFromSCMLEntity)
%#endif // PDG_SCML_SUPPORT
DECL_END

BINDING_CLASS(TileLayer)
  EMITTER_METHODS(TileLayer)
  ANIMATED_METHODS(TileLayer)
  SERIALIZABLE_METHODS(TileLayer)
  SPRITE_LAYER_METHODS(TileLayer)
%#ifndef PDG_NO_GUI
  SPRITE_LAYER_GUI_METHODS(TileLayer)
%#endif
%#ifdef PDG_USE_CHIPMUNK_PHYSICS
  SPRITE_LAYER_CHIPMUNK_METHODS(TileLayer)
%#endif
	PROPERTY(TileLayer, WorldSize)
	METHOD(TileLayer, GetWorldBounds)
	METHOD(TileLayer, DefineTileSet)
	METHOD(TileLayer, LoadMapData)
	METHOD(TileLayer, GetMapData)
	METHOD(TileLayer, GetTileSetImage)
	METHOD(TileLayer, GetTileSize)
	METHOD(TileLayer, GetTileTypeAt)
	METHOD(TileLayer, GetTileTypeAndFacingAt)
	METHOD(TileLayer, SetTileTypeAt)
	METHOD(TileLayer, CheckCollision)
DECL_END


FUNCTION_DECL(GetConfigManager)
FUNCTION_DECL(GetLogManager)
FUNCTION_DECL(GetEventManager)
FUNCTION_DECL(GetResourceManager)
FUNCTION_DECL(SetSerializationDebugMode)
FUNCTION_DECL(RegisterSerializableClass)
FUNCTION_DECL(GetGraphicsManager)
FUNCTION_DECL(GetSoundManager)
FUNCTION_DECL(GetFileManager)
FUNCTION_DECL(GetTimerManager)
FUNCTION_DECL(CreateSpriteLayer)
FUNCTION_DECL(CreateSpriteLayerFromSCMLFile)
FUNCTION_DECL(CleanupSpriteLayer)
FUNCTION_DECL(CreateTileLayer)
FUNCTION_DECL(Rand)
FUNCTION_DECL(GameCriticalRandom)
FUNCTION_DECL(Srand)
FUNCTION_DECL(RegisterEasingFunction)
FUNCTION_DECL(Idle)
FUNCTION_DECL(Run)
FUNCTION_DECL(Quit)
FUNCTION_DECL(IsQuitting)
FUNCTION_DECL(FinishedScriptSetup)


} // end pdg namespace

%#endif // PDG_INTERFACES_H_INCLUDED
