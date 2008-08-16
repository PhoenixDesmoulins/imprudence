/** 
 * @file llvostars.h
 * @brief LLVOStars class header file
 *
 * Copyright (c) 2001-2007, Linden Research, Inc.
 * 
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlife.com/developers/opensource/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at http://secondlife.com/developers/opensource/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 */

#ifndef LL_LLVOSTARS_H
#define LL_LLVOSTARS_H

#include "stdtypes.h"
#include "v4color.h"
#include "llviewerobject.h"
#include "llframetimer.h"

class LLFace;

class LLVOStars : public LLStaticViewerObject
{
public:
	LLVOStars(const LLUUID &id, const LLPCode pcode, LLViewerRegion *regionp);
	virtual ~LLVOStars();

	/*virtual*/ BOOL idleUpdate(LLAgent &agent, LLWorld &world, const F64 &time);
	/*virtual*/ LLDrawable* createDrawable(LLPipeline *pipeline);
	/*virtual*/ BOOL updateGeometry(LLDrawable *drawable);

	LLColor4* getStarColors() { return mStarColors; }

protected:
	void initStars();
	void updateStarColors();
	BOOL updateStarGeometry(LLDrawable *drawable);

protected:
	LLVector3			*mStarVertices;				// Star verticies
	LLColor4			*mStarColors;				// Star colors
	F32					*mStarIntensities;			// Star intensities
	LLFace				*mFace;
};

#endif