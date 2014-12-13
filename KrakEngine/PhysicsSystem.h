/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or 
disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
File Name: PhysicsSystem.h
Purpose: Header for PhysicsSystem.cpp
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: go_ninja
Author: Cristina Pohlenz, p.cristina
- End Header -----------------------------------------------------*/

#pragma once
#include "Precompiled.h"
#include "ISystem.h"
#include "ObjectLinkedList.h"
#include "RBody.h"

#define NBODIES 27
#define CUBE_WIDTH 3
#define CUBE_HEIGHT 3
#define CUBE_DEPTH 3
#define CLOTH_WIDTH 2
#define CLOTH_HEIGHT 2
#define RIGID_BODY_STATE_SIZE 18

namespace KrakEngine{

	class RigidBody;
    
    
    class PhysicsSystem : public ISystem{
	
		public:
		PhysicsSystem();
		~PhysicsSystem();

		bool Initialize();
		bool Shutdown();
		void Update(float dt);
        void HandleMessages(Message* message);
		void PausePhysics() { m_PausePhysics = true; }
		void UnpausePhysics() { m_PausePhysics = false; }

	private:
		void TimeStep (float dt);
		void Integrate(float dt);
		void GetPositions();
		void SetPositions();
		void RemovePhysicsComponent(int id);
		void BroadPhaseCollision();
		void ResolveCollision();

	public:
		ObjectLinkList<RigidBody> Bodies;
		bool DebugDrawingActive;

	private:
		bool m_PausePhysics;
		XMFLOAT3 m_gravity;
		ObjectLinkList<RigidBody>::iterator it;
		double time;
		double accumulator;
		double deltaTime;
        
        float SphereMomentOfInertia(float radius, float mass);

        void InitializeBodies();

        void StateToArray(rBody *rb, double *y);
        void ArrayToState(rBody *rb, const double *y);
        void BodiesToArray(std::vector<double> &x);
        XMFLOAT3X3 Star(XMFLOAT3 a);

        std::vector<double> m_x0;
        std::vector<double> m_xFinal;
        double m_CurrentPhysicsSimulatonTime;

        // CS 560 Cube
        XMFLOAT3 CubeForce(UINT i, UINT j, UINT k, double t);
        XMFLOAT3 HookesLaw(UINT aIndex, UINT bIndex, float springLength, float springStiffness);
        XMFLOAT3 CubeForceSpring(UINT i, UINT j, UINT k, double t);
        XMFLOAT3 CubeForceAdjacent(UINT i, UINT j, UINT k, double t);
        XMFLOAT3 CubeForceDiagonal(UINT i, UINT j, UINT k, double t);
        XMFLOAT3 CubeForceBend(UINT i, UINT j, UINT k, double t);
        XMFLOAT3 CubeForceGravity(UINT i, UINT j, UINT k, double t);
        XMFLOAT3 CubeForceDampeners(UINT i, UINT j, UINT k, double t);

        void ComputeForceAndTorque(double t, rBody *rb, UINT index);

        typedef void(*DerivativeFunction)(double t, const std::vector<double> &x, std::vector<double> &xDot);

        void ode(const std::vector<double> &x0, std::vector<double> &xEnd, int length, double t0, double t1);

        void dxdt(double t, const std::vector<double> &x, std::vector<double> &xdot);

        // Returns the index i of the cloth given the 2D i,j coordiantes
        UINT Get1DClothIndex(UINT i, UINT j);

        // Returns the i,j coordinates of the cloth given the 1D index
        XMUINT2 Get2DClothIndex(UINT index1D);

        UINT Get1DCubeIndex(UINT i, UINT j, UINT k);
        XMUINT3 Get3DCubeIndex(UINT index1D);

        

    public:
        void UpdateSimulation(float dt);
        void ArrayToBodies(const std::vector<double> &x);
        void ddtStateToArray(rBody *rb, double *xdot);

        rBody m_rigidBodies[NBODIES];

        float m_ClothSpringStiffness = 1.f;
        float m_CubeSpringStiffness = 100.f;
        float m_CubeSpringLength = 1.f;
        float m_CubeDiagonalSpringLength;
        float m_Dampen = 0.0999f;

        void DrawSpringsAtPoint(UINT i, UINT j, UINT k, const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext);

        bool IsAnchor(UINT index);
        std::list<UINT> m_AnchorPoints;
        UINT m_MaxAnchorPoints = 12;
	};

	extern PhysicsSystem* g_PHYSICSSYSTEM;

}