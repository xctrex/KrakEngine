/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Animation.cpp
Purpose: Clases for skeletal animation
Language: C++, MSC
Platform: Windows
Project: CS 560
Author: Tommy Walton
Creation date: 10/13/2014
- End Header -----------------------------------------------------*/
#include "Precompiled.h"
#include "Animation.h"
#include "GraphicsSystem.h"

namespace KrakEngine
{
    inline XMFLOAT4 XMSlerp(XMFLOAT4& q1, XMFLOAT4& q2, float param)
    {
        //
        // XMFLOAT4ernion Interpolation With Extra Spins, pp. 96f, 461f
        // Jack Morrison, Graphics Gems III, AP Professional
        //

        XMFLOAT4 qt;

        float alpha, beta;
        float cosom = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
        float slerp_epsilon = 0.00001f;

        bool flip;

        if (flip = (cosom < 0))
            cosom = -cosom;

        if ((1.0 - cosom) > slerp_epsilon)
        {
            float omega = acos(cosom);
            float sinom = sin(omega);
            alpha = (float)(sin((1.0 - param) * omega) / sinom);
            beta = (float)(sin(param * omega) / sinom);
        }
        else
        {
            alpha = (float)(1.0 - param);
            beta = (float)param;
        }

        if (flip) beta = -beta;

        qt.x = (float)(alpha*q1.x + beta*q2.x);
        qt.y = (float)(alpha*q1.y + beta*q2.y);
        qt.z = (float)(alpha*q1.z + beta*q2.z);
        qt.w = (float)(alpha*q1.w + beta*q2.w);

        return qt;
    }

    Animation::Animation()
    {

    }

    Animation::~Animation()
    {

    }

    // This struct is needed for reading from the binary format, but will be converted to vqs for use in the engine
    struct Frame{
        float time;
        XMFLOAT3 T;
        XMFLOAT4 R;
    };

    void Animation::ReadFrom(ChunkReader& file)
    {
        GChunk animation = file.ReadChunkHeader();

        UINT NumberOfTracks = 0;
        file.Read(Duration);
        file.Read(NumberOfTracks);

        m_Tracks.resize(NumberOfTracks);
        for (UINT t = 0; t < NumberOfTracks; ++t)
        {
            UINT NumberOfKeyframes = 0;
            file.Read(NumberOfKeyframes);
            m_Tracks[t].KeyFrames.resize(NumberOfKeyframes);
            for (UINT f = 0; f < NumberOfKeyframes; ++f){
                // Read in the frame data of the keyframe
                Frame frame;
                file.Read(frame);
                m_Tracks[t].KeyFrames[f].time = frame.time;
                m_Tracks[t].KeyFrames[f].vqs.v = frame.T;
                m_Tracks[t].KeyFrames[f].vqs.q = frame.R;
            }
        }
    }

    void Animation::CalculateTransform(float animTime, UINT trackIndex, VQS& vqs, TrackData& data)
    {
        //Since keys are not spaced at regular intervals we need to search
        //for the keyframes that will be interpolated between.  The track data is
        //used to store what the last keyframe was to prevent searching the entire
        //track.

        int CurKey = data.LastKey;
        Track& CurPath = m_Tracks[trackIndex];

        //Search Forward in the keyframes for the interval
        while (CurKey != CurPath.KeyFrames.size() - 1 &&
            CurPath.KeyFrames[CurKey + 1].time < animTime)
            ++CurKey;

        //Search Backward in the keyframes for the interval
        while (CurKey != 0 && CurPath.KeyFrames[CurKey].time > animTime)
            --CurKey;

        if (CurKey == CurPath.KeyFrames.size() - 1)
        {
            //Past the last keyframe for this path so use the last frame and the transform data
            //so the animation is clamped to the last frame
            vqs = CurPath.KeyFrames[CurKey].vqs;
        }
        else
        {
            //Generate transform data by interpolating between the two keyframes
            KeyFrame& KeyOne = CurPath.KeyFrames[CurKey];
            KeyFrame& KeyTwo = CurPath.KeyFrames[CurKey + 1];

            float t1 = KeyOne.time;
            float t2 = KeyTwo.time;

            //Normalize the distance between the two keyframes
            float segLen = t2 - t1;
            float segStart = animTime - t1;
            float segNormalizedT = segStart / segLen;

            // Interpolate
            XMFLOAT4 R = XMSlerp(KeyOne.vqs.q.GetXMFloat4(), KeyTwo.vqs.q.GetXMFloat4(), segNormalizedT);
            XMFLOAT3 T = (1.0f - segNormalizedT) * KeyOne.vqs.v + segNormalizedT * KeyTwo.vqs.v;
            vqs = Interpolate(KeyOne.vqs, KeyTwo.vqs, segNormalizedT);
            //vqs.q = R;
            vqs.v = T;
        }

        //Remember the last keyframe
        data.LastKey = CurKey;
    }



    AnimationController::AnimationController()
    {
        m_AnimationTime = 0.0f;
        m_AnimationSpeed = 1.0f;
        m_pSkeleton = NULL;
        m_pActiveAnimation = NULL;
    }

    AnimationController::~AnimationController()
    {
        SafeDelete(m_pSkeleton);
        for (UINT i = 0; i < m_Animations.size(); ++i)
            SafeDelete(m_Animations[i]);
    }

    void AnimationController::Update(float dt)
    {
        m_AnimationTime += dt * m_AnimationSpeed;
        //Just loop forever
        if (m_AnimationTime > m_pActiveAnimation->Duration)
        {
            m_AnimationTime = 0.0f;
            ClearTrackData();
        }
    }

    void AnimationController::Process()
    {
        m_pSkeleton->ProcessAnimationGraph(m_AnimationTime, m_BoneMatrixBuffer, *m_pActiveAnimation, m_AnimationTrackData);
    }

    void AnimationController::ProcessBindPose()
    {
        m_pSkeleton->ProcessBindPose(m_BoneMatrixBuffer);
    }

    void AnimationController::ProcessIK(XMFLOAT3 rootPos, XMFLOAT3 destPos)
    {
        m_pSkeleton->ProcessIK(m_BoneMatrixBuffer, rootPos, destPos);
    }

    void AnimationController::RenderSkeleton(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext) const
    {
        m_pSkeleton->RenderSkeleton(spD2DDeviceContext);
    }

    void AnimationController::SetSkeleton(Skeleton * pSkeleton)
    {
        m_pSkeleton = pSkeleton;
        m_BoneMatrixBuffer.resize(m_pSkeleton->m_Bones.size());
        m_AnimationTrackData.resize(m_pSkeleton->m_Bones.size());
    }

    void AnimationController::AddAnimation(Animation * pAnimation)
    {
        m_Animations.push_back(pAnimation);
        m_pActiveAnimation = pAnimation;
    }

    void AnimationController::ClearTrackData()
    {
        //Reset all the keys back to zero
        for (UINT i = 0; i < m_AnimationTrackData.size(); ++i)
            m_AnimationTrackData[i].LastKey = 0;

    }

    void Skeleton::ReadFrom(ChunkReader& file)
    {
        GChunk skeletonChunk = file.ReadChunkHeader();

        UINT NumberOfBones = 0;
        file.Read(NumberOfBones);

        m_Bones.resize(NumberOfBones);
        m_JointPositions.resize(m_IKNumLinks);
        m_JointRotations.resize(m_IKNumLinks);
        m_JointVQS.resize(m_IKNumLinks);
        m_Links.resize(m_IKNumLinks);
        for (size_t i = 0; i < m_JointVQS.size(); ++i)
        {
            m_JointVQS[i] = VQS(XMFLOAT3(0.f, m_IKLinkLength, 0.f), XMFLOAT4(0.f, 0.f, 0.f, 1.f), 1.f);
        }

        for (UINT i = 0; i < NumberOfBones; ++i)
        {
            file.Read(m_Bones[i].Name);
            file.Read(m_Bones[i].ParentBoneIndex);

            // Read the bind pose
            XMFLOAT3 T;
            file.Read(T);
            XMFLOAT4 R;
            file.Read(R);
            m_Bones[i].BindVQS = VQS(T, R, 1.0f);

            // Read the model to bone space VQS
            file.Read(T);
            file.Read(R);
            m_Bones[i].ModelToBoneSpaceVQS = VQS(T, R, 1.0f);
        }
    }

    void Skeleton::Initialize()
    {
        for (UINT i = 0; i < m_Bones.size(); ++i)
        {
            Bone& bone = m_Bones[i];
            bone.BoneIndex = i;

            if (bone.ParentBoneIndex != -1)
                m_Bones[bone.ParentBoneIndex].Children.push_back(&bone);
            else
                m_RootBones.push_back(&bone);

            bone.ModelToBoneSpace = bone.ModelToBoneSpaceVQS.GetMatrix();
            bone.BindTransform = bone.BindVQS.GetMatrix();
        }
    }

    XMFLOAT4X4& Skeleton::GetModelToBoneSpaceTransform(int boneIndex)
    {
        return m_Bones[boneIndex].ModelToBoneSpace;
    }

    void Skeleton::ProcessAnimationGraph(float time, MatrixBuffer& buffer, Animation& anim, TrackBuffer& trackData)
    {
        //Linear Form This only works because the bone parent is always guaranteed to be in front
        //of its children (breath first order)
        XMFLOAT4X4 identity;
        XMStoreFloat4x4(&identity, XMMatrixIdentity());
        for (UINT boneIndex = 0; boneIndex < m_Bones.size(); ++boneIndex)
        {
            Bone& bone = m_Bones[boneIndex];
            VQS vqs;
            anim.CalculateTransform(time, boneIndex, vqs, trackData[boneIndex]);
            XMFLOAT4X4 parentTransform = bone.ParentBoneIndex != -1 ? buffer[bone.ParentBoneIndex] : identity;
            XMFLOAT4X4 localTransform = vqs.GetMatrix();
            XMFLOAT4X4 modelTransform; // = localTransform  * parentTransform;
            XMStoreFloat4x4(&modelTransform, XMMatrixMultiply(XMLoadFloat4x4(&localTransform), XMLoadFloat4x4(&parentTransform)));
            buffer[boneIndex] = modelTransform;
        }
    }

    void Skeleton::ProcessIK(MatrixBuffer& buffer, XMFLOAT3 rootPos, XMFLOAT3 destPos)
    {        
        m_RootPosition = rootPos;

        XMFLOAT4X4 identity;
        XMStoreFloat4x4(&identity, XMMatrixIdentity());
        
        XMFLOAT3 currentPos;
        XMFLOAT3 newPos;
        for (int i = m_JointVQS.size() - 1; i >= 0; --i)
        {
            currentPos = CalculateCurrentPosition();
            // If the distance between the current and destination points < epsilon, exit
            if (Mag(destPos - currentPos) < m_IKEpsilon)
            {
                return;
            }
            Vector3 Vci;
            Vector3 Vdi;
            if (i > 0)
            {
                Vci = currentPos - m_JointPositions[i - 1]; // The way the code is currently constructed, joint positions really represents the point at the end of the link, not the end of the joint
                Vdi = destPos - m_JointPositions[i - 1];
            }
            else
            {
                Vci = currentPos - m_RootPosition;
                Vdi = destPos - m_RootPosition;
            }
            m_JointRotations[i].angle = AngleInRadians(Vci, Vdi);
            // Constrain rotation
            if (m_JointRotations[i].angle > PI / 6.f) m_JointRotations[i].angle = PI / 6.f;
            if (m_JointRotations[i].angle < -PI / 6.f) m_JointRotations[i].angle = -PI / 6.f;
            
            float s = sin(m_JointRotations[i].angle / 2.f);
            // Convert from angle-axis notation to quaternions
            m_JointVQS[i] = VQS(
                XMFLOAT3(0.f, m_IKLinkLength, 0.f),
                XMFLOAT4(m_JointRotations[i].axis.x * s, m_JointRotations[i].axis.y * s, m_JointRotations[i].axis.z * s, cos(m_JointRotations[i].angle / 2.f)),
                1.f);



            // Update modelTransform

            /*Bone& bone = m_Bones[boneIndex];
            VQS vqs;
            anim.CalculateTransform(time, boneIndex, vqs, trackData[boneIndex]);
            XMFLOAT4X4 parentTransform = bone.ParentBoneIndex != -1 ? buffer[bone.ParentBoneIndex] : identity;
            XMFLOAT4X4 localTransform = vqs.GetMatrix();
            XMFLOAT4X4 modelTransform; // = localTransform  * parentTransform;
            XMStoreFloat4x4(&modelTransform, XMMatrixMultiply(XMLoadFloat4x4(&localTransform), XMLoadFloat4x4(&parentTransform)));
            buffer[boneIndex] = modelTransform;*/

            newPos = CalculateCurrentPosition();
            if (Mag(newPos - currentPos) < m_IKEpsilon)
            {
                return;
            }

            ComPtr<ID2D1DeviceContext> spD2DDeviceContext = g_GRAPHICSSYSTEM->GetD2DDeviceContext();
            spD2DDeviceContext->BeginDraw();
            RenderSkeleton(spD2DDeviceContext);
            DXThrowIfFailed(spD2DDeviceContext->EndDraw());
        }
    }

    void Skeleton::RenderSkeleton(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext) const{
        if (m_JointPositions.size() < 2) { return; }

        /*spD2DDeviceContext->BeginDraw();*/

        // Get the screen coords
        XMFLOAT2 points[2];
        points[0] = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(m_RootPosition, g_GRAPHICSSYSTEM->GetWorldTransform());
        points[1] = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(m_JointPositions[0], g_GRAPHICSSYSTEM->GetWorldTransform());

        // Draw the root point
        spD2DDeviceContext->DrawEllipse(
            D2D1::Ellipse(D2D1::Point2F(points[0].x, points[0].y), 10.f, 10.f),
            g_GRAPHICSSYSTEM->GetD2DBrush(ColorRed).Get());

        // Draw a line from the root to the end of the root link
        spD2DDeviceContext->DrawLine(
            D2D1::Point2F(points[0].x, points[0].y),
            D2D1::Point2F(points[1].x, points[1].y),
            g_GRAPHICSSYSTEM->GetD2DBrush(ColorRed).Get()
            );

        // Draw the current point
        spD2DDeviceContext->DrawEllipse(
            D2D1::Ellipse(D2D1::Point2F(points[1].x, points[1].y), 10.f, 10.f),
            g_GRAPHICSSYSTEM->GetD2DBrush(ColorRed).Get());

        for (UINT i = 1; i < m_JointPositions.size(); ++i)
        {
            // Get the screen coords
            points[0] = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(m_JointPositions[i - 1], g_GRAPHICSSYSTEM->GetWorldTransform());
            points[1] = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(m_JointPositions[i], g_GRAPHICSSYSTEM->GetWorldTransform());

            // Draw a line from the current point to the previous point
            spD2DDeviceContext->DrawLine(
                D2D1::Point2F(points[0].x, points[0].y),
                D2D1::Point2F(points[1].x, points[1].y),
                g_GRAPHICSSYSTEM->GetD2DBrush(ColorRed).Get()
                );

            // Draw the current point
            spD2DDeviceContext->DrawEllipse(
                D2D1::Ellipse(D2D1::Point2F(points[1].x, points[1].y), 10.f, 10.f),
                g_GRAPHICSSYSTEM->GetD2DBrush(ColorRed).Get());
        }

        /*DXThrowIfFailed(
            spD2DDeviceContext->EndDraw());*/
    }

    XMFLOAT3 Skeleton::CalculateCurrentPosition()
    {
        if (m_JointPositions.size() < 2) return XMFLOAT3(0.f, 0.f, 0.f);

        XMStoreFloat3(&m_JointPositions[0], XMVector3TransformCoord(XMLoadFloat3(&m_RootPosition), XMLoadFloat4x4(&m_JointVQS[0].GetMatrix())));
        // Walk through each link and apply the transform
        for (size_t i = 1; i < m_JointPositions.size(); ++i)
        {
            XMStoreFloat3(&m_JointPositions[i], XMVector3TransformCoord(XMLoadFloat3(&m_JointPositions[i - 1]), XMLoadFloat4x4(&m_JointVQS[i].GetMatrix())));
        }
        return m_JointPositions.back();
    }

    void Skeleton::ProcessBindPose(MatrixBuffer& buffer)
    {
        for (UINT i = 0; i < m_Bones.size(); ++i)
            buffer[i] = m_Bones[i].BindTransform;
    }

    void Skeleton::RecursiveProcess(float time, Bone& bone, Animation& anim, MatrixBuffer& buffer, TrackBuffer& trackData, XMFLOAT4X4 parentTransform)
    {
        VQS vqs;

        anim.CalculateTransform(time, bone.BoneIndex, vqs, trackData[bone.BoneIndex]);

        XMFLOAT4X4 localTransform = vqs.GetMatrix();
        XMFLOAT4X4 modelTransform; // = localTransform  * parentTransform;
        XMStoreFloat4x4(&modelTransform, XMMatrixMultiply(XMLoadFloat4x4(&localTransform), XMLoadFloat4x4(&parentTransform)));
        buffer[bone.BoneIndex] = modelTransform;

        for (UINT i = 0; i < bone.Children.size(); ++i)
        {
            RecursiveProcess(time, *bone.Children[i], anim, buffer, trackData, modelTransform);
        }
    }
}
