/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Animation.h
Purpose: Clases for skeletal animation
Language: C++, MSC
Platform: Windows
Project: CS 560
Author: Tommy Walton
Creation date: 10/13/2014
- End Header -----------------------------------------------------*/
#pragma once
#include "Precompiled.h"
#include "File\ChunkReader.hpp"
#include "File\FileElements.hpp"
#include "VQS.h"

namespace KrakEngine
{
    //A Keyframe is a snapshot of a node / bone
    //at a point in time.
    struct KeyFrame
    {
        float time;
        VQS vqs;
    };

    //A Track is a set of keyframes that are in temporal
    //order from 0 to the animation duration
    struct Track
    {
        std::vector< KeyFrame > KeyFrames;
    };

    //Track data is used to help process the animation in this
    //case it is the last keyframe the track was on
    struct TrackData
    {
        UINT LastKey;
    };
    typedef std::vector<TrackData> TrackBuffer;

    //The matrix buffer is the result of processing an animation and
    //what is need to render the animated mesh
    typedef std::vector<XMFLOAT4X4> MatrixBuffer;


    //A Bone has two primary functions. It defines the parent/child
    //relationships between animation tracks and stores the model
    //to bone space transform used to perform vertex skinning.
    struct Bone
    {
        std::string Name;
        int BoneIndex;
        int ParentBoneIndex;

        //Not all the following data is needed but is
        //helpful to debug and understand how skinning works

        //VQS representation of bind position
        VQS BindVQS;

        //Transform of the bone in bind position
        XMFLOAT4X4 BindTransform;

        //VQS representation of transform from model space to bone space
        VQS ModelToBoneSpaceVQS;

        //Transform from model space to bone space
        //this is the inverse of the BindTransform
        XMFLOAT4X4 ModelToBoneSpace;

        std::vector<Bone*> Children;
    };
    
    //The animation stores the keyframes for each track and interpolate between them.
    //It does not contain the graph data (parent/child) or match tracks to individual 
    //transform nodes/bones (the skeleton contains this information). 
    //This is because there can be many animations related to one skeleton.
    class Animation
    {
    public:
        Animation();
        ~Animation();
        float Duration;
        std::vector< Track > m_Tracks;
        void CalculateTransform(float animTime, UINT trackIndex, VQS& vqs, TrackData& data);
        void ReadFrom(ChunkReader& file);
    };

    struct Rotation
    {
        Vector3 axis;
        float angle;
    };

    //The skeleton contains the bones which have the graph (parent/child) relationships and what animation tracks
    //correspond to what bones.
    class Skeleton
    {
    public:
        void Initialize();
        void ReadFrom(ChunkReader& reader);
        void ProcessAnimationGraph(float time, MatrixBuffer& matrixBuffer, Animation& anim, TrackBuffer& trackData);
        void RecursiveProcess(float time, Bone& bone, Animation& anim, MatrixBuffer& matrixBuffer, TrackBuffer& trackData, XMFLOAT4X4 matrix);
        void ProcessBindPose(MatrixBuffer& buffer);
        void ProcessIK(MatrixBuffer& buffer, XMFLOAT3 rootPos, XMFLOAT3 destPos);
        bool ProcessIK2D(MatrixBuffer& buffer, XMFLOAT2 rootPos, XMFLOAT2 destPos);
        void RenderSkeleton(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext) const;
        void RenderSkeleton2D(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext) const;
        XMFLOAT4X4& GetModelToBoneSpaceTransform(int boneIndex);
        std::vector<Bone> m_Bones;
        std::vector<Bone*> m_RootBones;
        std::vector<XMFLOAT3> m_JointPositions;
        std::vector<XMFLOAT2> m_JointPositions2D;
        std::vector<Rotation> m_JointRotations;
        std::vector<float> m_JointRotations2D;
        std::vector<VQS> m_JointVQS;
        std::vector<float> m_Links;
        XMFLOAT3 m_RootPosition;
        XMFLOAT2 m_RootPosition2D;
        XMFLOAT2 m_CurrentPosition2D;
        XMFLOAT3 CalculateCurrentPosition();
        XMFLOAT2 CalculateCurrentPosition2D();
        float m_IKEpsilon = 2.0f;
    private:
        float m_IKLinkLength = 6.f;
        UINT m_IKNumLinks = 6;
    };

    //Controls the animation for a animated model by tracking time and
    //using an animation and skeleton to generate a matrix buffer.
    class AnimationController
    {
    public:
        AnimationController();
        ~AnimationController();
        void Update(float dt);

        float m_AnimationTime;
        float m_AnimationSpeed;
        Skeleton * m_pSkeleton;
        Animation * m_pActiveAnimation;
        TrackBuffer m_AnimationTrackData;
        MatrixBuffer m_BoneMatrixBuffer;
        std::vector<Animation*> m_Animations;

        void ClearTrackData();
        void Process();
        void ProcessBindPose();
        void ProcessIK(XMFLOAT3 rootPos, XMFLOAT3 destPos);
        bool ProcessIK2D(XMFLOAT2 rootPos, XMFLOAT2 destPos);
        void SetRootPos(XMFLOAT3 rootPos){ m_pSkeleton->m_RootPosition = rootPos; m_pSkeleton->CalculateCurrentPosition(); }
        void SetRootPos2D(XMFLOAT2 rootPos){ m_pSkeleton->m_RootPosition2D = rootPos; m_pSkeleton->CalculateCurrentPosition(); }
        void RenderSkeleton(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext) const;
        void RenderSkeleton2D(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext) const;
        void SetSkeleton(Skeleton * pSkeleton);
        void AddAnimation(Animation * pAnimation);
    };
}
