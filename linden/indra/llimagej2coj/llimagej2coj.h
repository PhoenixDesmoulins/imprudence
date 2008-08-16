/** 
 * @file llimagej2coj.h
 * @brief This is an implementation of JPEG2000 encode/decode using OpenJPEG.
 *
 * Copyright (c) 2006-2007, Linden Research, Inc.
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

#ifndef LL_LLIMAGEJ2COJ_H
#define LL_LLIMAGEJ2COJ_H

#include "llimagej2c.h"

class LLImageJ2COJ : public LLImageJ2CImpl
{	
public:
	LLImageJ2COJ();
	virtual ~LLImageJ2COJ();

protected:
	/*virtual*/ BOOL getMetadata(LLImageJ2C &base);
	/*virtual*/ BOOL decodeImpl(LLImageJ2C &base, LLImageRaw &raw_image, F32 decode_time, S32 first_channel, S32 max_channel_count);
	/*virtual*/ BOOL encodeImpl(LLImageJ2C &base, const LLImageRaw &raw_image, const char* comment_text, F32 encode_time=0.0);
	int ceildivpow2(int a, int b)
	{
		// Divide a by b to the power of 2 and round upwards.
		return (a + (1 << b) - 1) >> b;
	}

	// Temporary variables for in-progress decodes...
	LLImageRaw *mRawImagep;
};

#endif