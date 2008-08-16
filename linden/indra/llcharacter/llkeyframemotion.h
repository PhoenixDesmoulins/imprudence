/** 
 * @file llkeyframemotion.h
 * @brief Implementation of LLKeframeMotion class.
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

#ifndef LL_LLKEYFRAMEMOTION_H
#define LL_LLKEYFRAMEMOTION_H

//-----------------------------------------------------------------------------
// Header files
//-----------------------------------------------------------------------------

#include <string>

#include "llassetstorage.h"
#include "llassoclist.h"
#include "llbboxlocal.h"
#include "llhandmotion.h"
#include "lljointstate.h"
#include "llmotion.h"
#include "llptrskipmap.h"
#include "llquaternion.h"
#include "v3dmath.h"
#include "v3math.h"
#include "llapr.h"

class LLKeyframeDataCache;
class LLVFS;
class LLDataPacker;

#define MIN_REQUIRED_PIXEL_AREA_KEYFRAME (40.f)
#define MAX_CHAIN_LENGTH (4)

const S32 KEYFRAME_MOTION_VERSION = 1;
const S32 KEYFRAME_MOTION_SUBVERSION = 0;

//-----------------------------------------------------------------------------
// class LLKeyframeMotion
//-----------------------------------------------------------------------------
class LLKeyframeMotion :
	public LLMotion
{
	friend class LLKeyframeDataCache;
public:
	// Constructor
	LLKeyframeMotion(const LLUUID &id);

	// Destructor
	virtual ~LLKeyframeMotion();

public:
	//-------------------------------------------------------------------------
	// functions to support MotionController and MotionRegistry
	//-------------------------------------------------------------------------

	// static constructor
	// all subclasses must implement such a function and register it
	static LLMotion *create(const LLUUID& id);

public:
	//-------------------------------------------------------------------------
	// animation callbacks to be implemented by subclasses
	//-------------------------------------------------------------------------

	// motions must specify whether or not they loop
	virtual BOOL getLoop() { 
		if (mJointMotionList) return mJointMotionList->mLoop; 
		else return FALSE;
	}

	// motions must report their total duration
	virtual F32 getDuration() { 
		if (mJointMotionList) return mJointMotionList->mDuration; 
		else return 0.f;
	}

	// motions must report their "ease in" duration
	virtual F32 getEaseInDuration() { 
		if (mJointMotionList) return mJointMotionList->mEaseInDuration; 
		else return 0.f;
	}

	// motions must report their "ease out" duration.
	virtual F32 getEaseOutDuration() { 
		if (mJointMotionList) return mJointMotionList->mEaseOutDuration; 
		else return 0.f;
	}

	// motions must report their priority
	virtual LLJoint::JointPriority getPriority() { 
		if (mJointMotionList) return mJointMotionList->mBasePriority; 
		else return LLJoint::LOW_PRIORITY;
	}

	virtual LLMotionBlendType getBlendType() { return NORMAL_BLEND; }

	// called to determine when a motion should be activated/deactivated based on avatar pixel coverage
	virtual F32 getMinPixelArea() { return MIN_REQUIRED_PIXEL_AREA_KEYFRAME; }

	// run-time (post constructor) initialization,
	// called after parameters have been set
	// must return true to indicate success and be available for activation
	virtual LLMotionInitStatus onInitialize(LLCharacter *character);

	// called when a motion is activated
	// must return TRUE to indicate success, or else
	// it will be deactivated
	virtual BOOL onActivate();

	// called per time step
	// must return TRUE while it is active, and
	// must return FALSE when the motion is completed.
	virtual BOOL onUpdate(F32 time, U8* joint_mask);

	// called when a motion is deactivated
	virtual void onDeactivate();

	virtual void setStopTime(F32 time);

	static void setVFS(LLVFS* vfs) { sVFS = vfs; }

	static void onLoadComplete(LLVFS *vfs,
							   const LLUUID& asset_uuid,
							   LLAssetType::EType type,
							   void* user_data, S32 status);

public:
	U32		getFileSize();
	BOOL	serialize(LLDataPacker& dp) const;
	BOOL	deserialize(LLDataPacker& dp);
	void	writeCAL3D(apr_file_t* fp);
	BOOL	isLoaded() { return mJointMotionList != NULL; }


	// setters for modifying a keyframe animation
	void setLoop(BOOL loop);

	F32 getLoopIn() {
		return (mJointMotionList) ? mJointMotionList->mLoopInPoint : 0.f;
	}

	F32 getLoopOut() {
		return (mJointMotionList) ? mJointMotionList->mLoopOutPoint : 0.f;
	}
	
	void setLoopIn(F32 in_point);

	void setLoopOut(F32 out_point);

	void setHandPose(LLHandMotion::eHandPose pose) {
		if (mJointMotionList) mJointMotionList->mHandPose = pose;
	}

	LLHandMotion::eHandPose getHandPose() { 
		return (mJointMotionList) ? mJointMotionList->mHandPose : LLHandMotion::HAND_POSE_RELAXED;
	}

	void setPriority(S32 priority);

	void setEmote(const LLUUID& emote_id);

	void setEaseIn(F32 ease_in);

	void setEaseOut(F32 ease_in);

	F32 getLastUpdateTime() { return mLastLoopedTime; }

	const LLBBoxLocal& getPelvisBBox();

	static void flushKeyframeCache();

	typedef enum e_constraint_type
	{
		TYPE_POINT,
		TYPE_PLANE
	} EConstraintType;

	typedef enum e_constraint_target_type
	{
		TYPE_BODY,
		TYPE_GROUND
	} EConstraintTargetType;

protected:
	//-------------------------------------------------------------------------
	// JointConstraintSharedData
	//-------------------------------------------------------------------------
	class JointConstraintSharedData
	{
	public:
		JointConstraintSharedData() :
			mChainLength(0),
			mEaseInStartTime(0.f), 
			mEaseInStopTime(0.f),
			mEaseOutStartTime(0.f),
			mEaseOutStopTime(0.f), 
			mUseTargetOffset(FALSE),
			mConstraintType(TYPE_POINT),
			mConstraintTargetType(TYPE_BODY) {};
		~JointConstraintSharedData() { delete [] mJointStateIndices; }

		S32						mSourceConstraintVolume;
		LLVector3				mSourceConstraintOffset;
		S32						mTargetConstraintVolume;
		LLVector3				mTargetConstraintOffset;
		LLVector3				mTargetConstraintDir;
		S32						mChainLength;
		S32*					mJointStateIndices;
		F32						mEaseInStartTime;
		F32						mEaseInStopTime;
		F32						mEaseOutStartTime;
		F32						mEaseOutStopTime;
		BOOL					mUseTargetOffset;
		EConstraintType			mConstraintType;
		EConstraintTargetType	mConstraintTargetType;
	};

	//-----------------------------------------------------------------------------
	// JointConstraint()
	//-----------------------------------------------------------------------------
	class JointConstraint
	{
	public:
		JointConstraint(JointConstraintSharedData* shared_data);
		~JointConstraint();

		JointConstraintSharedData*	mSharedData;
		F32							mWeight;
		F32							mTotalLength;
		LLVector3					mPositions[MAX_CHAIN_LENGTH];
		F32							mJointLengths[MAX_CHAIN_LENGTH];
		F32							mJointLengthFractions[MAX_CHAIN_LENGTH];
		BOOL						mActive;
		LLVector3d					mGroundPos;
		LLVector3					mGroundNorm;
		LLJoint*					mSourceVolume;
		LLJoint*					mTargetVolume;
		F32							mFixupDistanceRMS;
	};

	void applyKeyframes(F32 time);

	void applyConstraints(F32 time, U8* joint_mask);

	void activateConstraint(JointConstraint* constraintp);

	void initializeConstraint(JointConstraint* constraint);

	void deactivateConstraint(JointConstraint *constraintp);

	void applyConstraint(JointConstraint* constraintp, F32 time, U8* joint_mask);

	BOOL	setupPose();

public:
	enum AssetStatus { ASSET_LOADED, ASSET_FETCHED, ASSET_NEEDS_FETCH, ASSET_FETCH_FAILED, ASSET_UNDEFINED };

	enum InterpolationType { IT_STEP, IT_LINEAR, IT_SPLINE };

	//-------------------------------------------------------------------------
	// ScaleKey
	//-------------------------------------------------------------------------
	class ScaleKey
	{
	public:
		ScaleKey() { mTime = 0.0f; }
		ScaleKey(F32 time, const LLVector3 &scale) { mTime = time; mScale = scale; }

		F32			mTime;
		LLVector3	mScale;
	};

	//-------------------------------------------------------------------------
	// RotationKey
	//-------------------------------------------------------------------------
	class RotationKey
	{
	public:
		RotationKey() { mTime = 0.0f; }
		RotationKey(F32 time, const LLQuaternion &rotation) { mTime = time; mRotation = rotation; }

		F32				mTime;
		LLQuaternion	mRotation;
	};

	//-------------------------------------------------------------------------
	// PositionKey
	//-------------------------------------------------------------------------
	class PositionKey
	{
	public:
		PositionKey() { mTime = 0.0f; }
		PositionKey(F32 time, const LLVector3 &position) { mTime = time; mPosition = position; }

		F32			mTime;
		LLVector3	mPosition;
	};

	//-------------------------------------------------------------------------
	// ScaleCurve
	//-------------------------------------------------------------------------
	class ScaleCurve
	{
	public:
		ScaleCurve();
		~ScaleCurve();
		LLVector3 getValue(F32 time, F32 duration);
		LLVector3 interp(F32 u, ScaleKey& before, ScaleKey& after);

		InterpolationType	mInterpolationType;
		S32					mNumKeys;
		LLPtrSkipMap<F32, ScaleKey*>			mKeys;
		ScaleKey			mLoopInKey;
		ScaleKey			mLoopOutKey;
	};

	//-------------------------------------------------------------------------
	// RotationCurve
	//-------------------------------------------------------------------------
	class RotationCurve
	{
	public:
		RotationCurve();
		~RotationCurve();
		LLQuaternion getValue(F32 time, F32 duration);
		LLQuaternion interp(F32 u, RotationKey& before, RotationKey& after);

		InterpolationType	mInterpolationType;
		S32					mNumKeys;
		LLPtrSkipMap<F32, RotationKey*>		mKeys;
		RotationKey		mLoopInKey;
		RotationKey		mLoopOutKey;
	};

	//-------------------------------------------------------------------------
	// PositionCurve
	//-------------------------------------------------------------------------
	class PositionCurve
	{
	public:
		PositionCurve();
		~PositionCurve();
		LLVector3 getValue(F32 time, F32 duration);
		LLVector3 interp(F32 u, PositionKey& before, PositionKey& after);

		InterpolationType	mInterpolationType;
		S32					mNumKeys;
		LLPtrSkipMap<F32, PositionKey*>		mKeys;
		PositionKey		mLoopInKey;
		PositionKey		mLoopOutKey;
	};

	//-------------------------------------------------------------------------
	// JointMotion
	//-------------------------------------------------------------------------
	class JointMotion
	{
	public:
		PositionCurve	mPositionCurve;
		RotationCurve	mRotationCurve;
		ScaleCurve		mScaleCurve;
		std::string		mJointName;
		U32				mUsage;
		LLJoint::JointPriority	mPriority;

		void update(LLJointState *joint_state, F32 time, F32 duration);
	};
	
	//-------------------------------------------------------------------------
	// JointMotionList
	//-------------------------------------------------------------------------
	class JointMotionList
	{
	public:
		U32						mNumJointMotions;
		JointMotion*			mJointMotionArray;
		F32						mDuration;
		BOOL					mLoop;
		F32						mLoopInPoint;
		F32						mLoopOutPoint;
		F32						mEaseInDuration;
		F32						mEaseOutDuration;
		LLJoint::JointPriority	mBasePriority;
		LLHandMotion::eHandPose mHandPose;
		LLJoint::JointPriority  mMaxPriority;
		LLLinkedList<JointConstraintSharedData> mConstraints;
		LLBBoxLocal				mPelvisBBox;
	public:
		JointMotionList() : mNumJointMotions(0), mJointMotionArray(NULL) {};
		~JointMotionList() { mConstraints.deleteAllData(); delete [] mJointMotionArray; }
		U32 dumpDiagInfo();
	};


protected:
	static LLVFS*				sVFS;

	//-------------------------------------------------------------------------
	// Member Data
	//-------------------------------------------------------------------------
	JointMotionList*				mJointMotionList;
	LLJointState*					mJointStates;
	LLJoint*						mPelvisp;
	LLCharacter*					mCharacter;
	std::string						mEmoteName;
	LLLinkedList<JointConstraint>	mConstraints;
	U32								mLastSkeletonSerialNum;
	F32								mLastUpdateTime;
	F32								mLastLoopedTime;
	AssetStatus						mAssetStatus;
};

class LLKeyframeDataCache
{
public:
	// *FIX: implement this as an actual singleton member of LLKeyframeMotion
	LLKeyframeDataCache(){};
	~LLKeyframeDataCache();

	typedef std::map<LLUUID, class LLKeyframeMotion::JointMotionList*> LLKeyframeDataMap; 
	static LLKeyframeDataMap sKeyframeDataMap;

	static void addKeyframeData(const LLUUID& id, LLKeyframeMotion::JointMotionList*);
	static LLKeyframeMotion::JointMotionList* getKeyframeData(const LLUUID& id);

	static void removeKeyframeData(const LLUUID& id);

	//print out diagnostic info
	static void dumpDiagInfo();
	static void clear();
};

#endif // LL_LLKEYFRAMEMOTION_H
