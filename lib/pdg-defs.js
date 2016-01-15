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

exports.all_events = 0;
exports.eventType_Startup = 1;
exports.eventType_Shutdown = 2;
exports.eventType_Timer = 3;
exports.eventType_KeyDown = 4;
exports.eventType_KeyUp = 5;
exports.eventType_KeyPress = 6;
exports.eventType_MouseDown = 7;
exports.eventType_MouseUp = 8;
exports.eventType_MouseMove = 9;
exports.eventType_MouseEnter = 22;
exports.eventType_MouseLeave = 23;
exports.eventType_PortResized = 15;
exports.eventType_PortDraw = 24;
exports.eventType_ScrollWheel = 16;
exports.eventType_SpriteTouch = 19;
exports.eventType_SpriteAnimate = 17;
exports.eventType_SpriteLayer = 18;
exports.eventType_SpriteCollide = 20;
exports.eventType_SpriteBreak = 21;
exports.eventType_SoundEvent = 14;
exports.soundEvent_DonePlaying = 0;
exports.soundEvent_Looping = 1;
exports.soundEvent_FailedToPlay = 2;
exports.key_Break = 1;
exports.key_Home = 2;
exports.key_End = 3;
exports.key_Clear = 4;
exports.key_Help = 5;
exports.key_Pause = 6;
exports.key_Mute = 7;
exports.key_Backspace = 8;
exports.key_Delete = 127;
exports.key_Tab = 9;
exports.key_PageUp = 11;
exports.key_PageDown = 12;
exports.key_Return = 13;
exports.key_Enter = 13;
exports.key_F1 = 14;
exports.key_F2 = 15;
exports.key_F3 = 16;
exports.key_F4 = 17;
exports.key_F5 = 18;
exports.key_F6 = 19;
exports.key_F7 = 20;
exports.key_F8 = 21;
exports.key_F9 = 22;
exports.key_F10 = 23;
exports.key_F11 = 24;
exports.key_F12 = 25;
exports.key_FirstF = 14;
exports.key_LastF = 25;
exports.key_Insert = 26;
exports.key_Escape = 27;
exports.key_LeftArrow = 28;
exports.key_RightArrow = 29;
exports.key_UpArrow = 30;
exports.key_DownArrow = 31;
exports.key_FirstPrintable = 32;
exports.screenPos_Normal = 0;
exports.screenPos_Rotated180 = 1;
exports.screenPos_Rotated90Clockwise = 2;
exports.screenPos_Rotated90CounterClockwise = 3;
exports.screenPos_FaceUp = 4;
exports.screenPos_FaceDown = 5;
exports.textStyle_Plain = 0;
exports.textStyle_Bold = 1;
exports.textStyle_Italic = 2;
exports.textStyle_Underline = 4;
exports.textStyle_Centered = 16;
exports.textStyle_LeftJustified = 0;
exports.textStyle_RightJustified = 32;
exports.fit_None = 0;
exports.fit_Height = 1;
exports.fit_Width = 2;
exports.fit_Inside = 3;
exports.fit_Fill = 4;
exports.fit_FillKeepProportions = 5;
exports.init_CreateUniqueNewFile = 0;
exports.init_OverwriteExisting = 1;
exports.init_AppendToExisting = 2;
exports.init_StdOut = 3;
exports.init_StdErr = 4;
exports.duration_Constant = -1;
exports.duration_Instantaneous = 0;
exports.animate_StartToEnd = 0;
exports.animate_EndToStart = 1;
exports.animate_Unidirectional = 0;
exports.animate_Bidirectional = 2;
exports.animate_NoLooping = 0;
exports.animate_Looping = 4;
exports.start_FromFirstFrame = -1;
exports.start_FromLastFrame = -2;
exports.all_Frames = 0;
exports.action_CollideSprite = 0;
exports.action_CollideWall = 1;
exports.action_Offscreen = 2;
exports.action_AnimationLoop = 8;
exports.action_AnimationEnd = 9;
exports.action_FadeComplete = 10;
exports.action_FadeInComplete = 11;
exports.action_FadeOutComplete = 12;
exports.action_JointBreak = 13;
exports.touch_MouseEnter = 20;
exports.touch_MouseLeave = 21;
exports.touch_MouseDown = 22;
exports.touch_MouseUp = 23;
exports.touch_MouseClick = 24;
exports.collide_None = 0;
exports.collide_Point = 1;
exports.collide_BoundingBox = 2;
exports.collide_CollisionRadius = 3;
exports.collide_AlphaChannel = 4;
exports.collide_Last = 4;
exports.action_ErasePort = 40;
exports.action_PreDrawLayer = 41;
exports.action_PostDrawLayer = 42;
exports.action_DrawPortComplete = 43;
exports.action_AnimationStart = 44;
exports.action_PreAnimateLayer = 45;
exports.action_PostAnimateLayer = 46;
exports.action_AnimationComplete = 47;
exports.action_ZoomComplete = 48;
exports.facing_North = 0;
exports.facing_East = 64;
exports.facing_South = 128;
exports.facing_West = 192;
exports.facing_Ignore = 256;
exports.flipped_None = 0;
exports.flipped_Horizontal = 64;
exports.flipped_Vertical = 128;
exports.flipped_Both = 192;
exports.flipped_Ignore = 256;
exports.timer_OneShot = 1;
exports.timer_Repeating = 0;
exports.timer_Never = -1;
exports.linearTween = 0;
exports.easeInQuad = 1;
exports.easeOutQuad = 2;
exports.easeInOutQuad = 3;
exports.easeInCubic = 4;
exports.easeOutCubic = 5;
exports.easeInOutCubic = 6;
exports.easeInQuart = 7;
exports.easeOutQuart = 8;
exports.easeInQuint = 10;
exports.easeOutQuint = 11;
exports.easeInOutQuint = 12;
exports.easeInSine = 13;
exports.easeOutSine = 14;
exports.easeInOutSine = 15;
exports.easeInExpo = 16;
exports.easeOutExpo = 17;
exports.easeInOutExpo = 18;
exports.easeInCirc = 19;
exports.easeOutCirc = 20;
exports.easeInOutCirc = 21;
exports.easeInBounce = 22;
exports.easeOutBounce = 23;
exports.easeInOutBounce = 24;
exports.easeInBack = 25;
exports.easeOutBack = 26;
exports.easeInOutBack = 27;
exports.lftTop = 0;
exports.rgtTop = 1;
exports.rgtBot = 2;
exports.lftBot = 3;
