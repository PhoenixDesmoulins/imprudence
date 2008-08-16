/** 
 * @file lldir_mac.h
 * @brief Definition of directory utilities class for Mac OS X 
 *
 * Copyright (c) 2000-2007, Linden Research, Inc.
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

#ifndef LL_LLDIR_MAC_H
#define LL_LLDIR_MAC_H

#include "lldir.h"

#include <stdio.h>
#include <dirent.h>

class LLDir_Mac : public LLDir
{
public:
	LLDir_Mac();
	virtual ~LLDir_Mac();

	virtual void initAppDirs(const std::string &app_name);
public:	
	virtual S32 deleteFilesInDir(const std::string &dirname, const std::string &mask);
	virtual std::string getCurPath();
	virtual U32 countFilesInDir(const std::string &dirname, const std::string &mask);
	virtual BOOL getNextFileInDir(const std::string &dirname, const std::string &mask, std::string &fname, BOOL wrap);
	virtual void getRandomFileInDir(const std::string &dirname, const std::string &ask, std::string &fname);
	virtual BOOL fileExists(const std::string &filename);

private:
	int mCurrentDirIndex;
	int mCurrentDirCount;
	std::string mCurrentDir;
};

#endif // LL_LLDIR_MAC_H

