// -----------------------------------------------
// pdg-defs.js
//
// pure javascript implementation of 
// core application/system interface
//
// Written by Ed Zavada, 2004-2012
// Copyright (c) 2012, Dream Rock Studios, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// -----------------------------------------------

var all_events = 0;
var eventType_Startup = 1;
var eventType_Shutdown = 2;
var eventType_Timer = 3;
var eventType_KeyDown = 4;
var eventType_KeyUp = 5;
var eventType_KeyPress = 6;
var eventType_MouseDown = 7;
var eventType_MouseUp = 8;
var eventType_MouseMove = 9;
var eventType_MouseEnter = 22;
var eventType_MouseLeave = 23;
var eventType_PortResized = 15;
var eventType_PortDraw = 24;
var eventType_ScrollWheel = 16;
var eventType_SpriteTouch = 19;
var eventType_SpriteAnimate = 17;
var eventType_SpriteLayer = 18;
var eventType_SpriteCollide = 20;
var eventType_SpriteBreak = 21;
var eventType_SoundEvent = 14;
var soundEvent_DonePlaying = 0;
var soundEvent_Looping = 1;
var soundEvent_FailedToPlay = 2;
var key_Break = 1;
var key_Home = 2;
var key_End = 3;
var key_Clear = 4;
var key_Help = 5;
var key_Pause = 6;
var key_Mute = 7;
var key_Backspace = 8;
var key_Delete = 127;
var key_Tab = 9;
var key_PageUp = 11;
var key_PageDown = 12;
var key_Return = 13;
var key_Enter = 13;
var key_F1 = 14;
var key_F2 = 15;
var key_F3 = 16;
var key_F4 = 17;
var key_F5 = 18;
var key_F6 = 19;
var key_F7 = 20;
var key_F8 = 21;
var key_F9 = 22;
var key_F10 = 23;
var key_F11 = 24;
var key_F12 = 25;
var key_FirstF = 14;
var key_LastF = 25;
var key_Insert = 26;
var key_Escape = 27;
var key_LeftArrow = 28;
var key_RightArrow = 29;
var key_UpArrow = 30;
var key_DownArrow = 31;
var key_FirstPrintable = 32;
var screenPos_Normal = 0;
var screenPos_Rotated180 = 1;
var screenPos_Rotated90Clockwise = 2;
var screenPos_Rotated90CounterClockwise = 3;
var screenPos_FaceUp = 4;
var screenPos_FaceDown = 5;
var textStyle_Plain = 0;
var textStyle_Bold = 1;
var textStyle_Italic = 2;
var textStyle_Underline = 4;
var textStyle_Centered = 16;
var textStyle_LeftJustified = 0;
var textStyle_RightJustified = 32;
var fit_None = 0;
var fit_Height = 1;
var fit_Width = 2;
var fit_Inside = 3;
var fit_Fill = 4;
var fit_FillKeepProportions = 5;
var init_CreateUniqueNewFile = 0;
var init_OverwriteExisting = 1;
var init_AppendToExisting = 2;
var init_StdOut = 3;
var init_StdErr = 4;
var duration_Constant = -1;
var duration_Instantaneous = 0;
var animate_StartToEnd = 0;
var animate_EndToStart = 1;
var animate_Unidirectional = 0;
var animate_Bidirectional = 2;
var animate_NoLooping = 0;
var animate_Looping = 4;
var start_FromFirstFrame = -1;
var start_FromLastFrame = -2;
var all_Frames = 0;
var action_CollideSprite = 0;
var action_CollideWall = 1;
var action_Offscreen = 2;
var action_AnimationLoop = 8;
var action_AnimationEnd = 9;
var action_FadeComplete = 10;
var action_FadeInComplete = 11;
var action_FadeOutComplete = 12;
var action_JointBreak = 13;
var touch_MouseEnter = 20;
var touch_MouseLeave = 21;
var touch_MouseDown = 22;
var touch_MouseUp = 23;
var touch_MouseClick = 24;
var collide_None = 0;
var collide_Point = 1;
var collide_BoundingBox = 2;
var collide_CollisionRadius = 3;
var collide_AlphaChannel = 4;
var collide_Last = 4;
var action_ErasePort = 40;
var action_PreDrawLayer = 41;
var action_PostDrawLayer = 42;
var action_DrawPortComplete = 43;
var action_AnimationStart = 44;
var action_PreAnimateLayer = 45;
var action_PostAnimateLayer = 46;
var action_AnimationComplete = 47;
var action_ZoomComplete = 48;
var facing_North = 0;
var facing_East = 64;
var facing_South = 128;
var facing_West = 192;
var facing_Ignore = 256;
var flipped_None = 0;
var flipped_Horizontal = 64;
var flipped_Vertical = 128;
var flipped_Both = 192;
var flipped_Ignore = 256;
var timer_OneShot = 1;
var timer_Repeating = 0;
var timer_Never = -1;
var linearTween = 0;
var easeInQuad = 1;
var easeOutQuad = 2;
var easeInOutQuad = 3;
var easeInCubic = 4;
var easeOutCubic = 5;
var easeInOutCubic = 6;
var easeInQuart = 7;
var easeOutQuart = 8;
var easeInQuint = 10;
var easeOutQuint = 11;
var easeInOutQuint = 12;
var easeInSine = 13;
var easeOutSine = 14;
var easeInOutSine = 15;
var easeInExpo = 16;
var easeOutExpo = 17;
var easeInOutExpo = 18;
var easeInCirc = 19;
var easeOutCirc = 20;
var easeInOutCirc = 21;
var easeInBounce = 22;
var easeOutBounce = 23;
var easeInOutBounce = 24;
var easeInBack = 25;
var easeOutBack = 26;
var easeInOutBack = 27;
var lftTop = 0;
var rgtTop = 1;
var rgtBot = 2;
var lftBot = 3;
