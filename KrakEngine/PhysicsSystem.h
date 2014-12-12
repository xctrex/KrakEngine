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

#define NBODIES 16
#define CLOTH_WIDTH 4
#define CLOTH_LENGTH 4
#define RIGID_BODY_STATE_SIZE 18

namespace KrakEngine{

	class RigidBody;

    typedef void(*DerivativeFunction)(double t, const std::vector<double> &x, std::vector<double> &xDot);

    void ComputeForceAndTorque(double t, rBody *rb);
    
    void ode(const std::vector<double> &x0, std::vector<double> &xEnd, int length, double t0, double t1, DerivativeFunction dxdt);

    void dxdt(double t, const std::vector<double> &x, std::vector<double> &xdot);


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

        // CS 560 Cloth
        void ClothForce(size_t i, size_t j, float t);
        void ClothForceSpring(size_t i, size_t j, float t);
        void ClothForceGravity(size_t i, size_t j, float t);
        void ClothForceDampeners(size_t i, size_t j, float t);
        void ClothForceStretch(size_t i, size_t j, float t);
        void ClothForceShear(size_t i, size_t j, float t);
        void ClothForceBend(size_t i, size_t j, float t);

        float SphereMomentOfInertia(float radius, float mass);

        void InitializeBodies();

        void StateToArray(rBody *rb, double *y);
        void ArrayToState(rBody *rb, const double *y);
        void BodiesToArray(std::vector<double> &x);
        XMFLOAT3X3 Star(XMFLOAT3 a);

        std::vector<double> m_x0;
        std::vector<double> m_xFinal;
        double m_CurrentPhysicsSimulatonTime;

    public:
        void UpdateSimulation(float dt);
        void ArrayToBodies(const std::vector<double> &x);
        void ddtStateToArray(rBody *rb, double *xdot);

        rBody m_rigidBodies[NBODIES];
	};

	extern PhysicsSystem* g_PHYSICSSYSTEM;

}