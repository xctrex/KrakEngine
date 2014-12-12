/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or 
disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
File Name: PhysicsSystem.cpp
Purpose: System for Physics, Integrates Rigid Bodies, does collision
		 checks, calls for body resolution.
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: go_ninjs
Author: Cristina Pohlenz, p.cristina
- End Header -----------------------------------------------------*/

#include "Precompiled.h"
#include "PhysicsSystem.h"
#include "Collision.h"
#include "Resolution.h"
#include "RigidBody.h"
#include "MathLib.h"
#include "GameObject.h"
#include "GraphicsSystem.h"

namespace KrakEngine{

	PhysicsSystem * g_PHYSICSSYSTEM = NULL;

	PhysicsSystem::PhysicsSystem() : 
        m_PausePhysics(false),
        m_CurrentPhysicsSimulatonTime(0.)
    {
		g_PHYSICSSYSTEM = this;
		g_COLLISIONS = new Collisions();
		g_RESOLUTION = new Resolution();
		DebugDrawingActive = false;
		m_gravity = Vector3(0, -20.0f, 0);
	}

	PhysicsSystem::~PhysicsSystem(){}

	bool PhysicsSystem::Initialize(){
        m_x0.resize(NBODIES * RIGID_BODY_STATE_SIZE);
        m_xFinal.resize(NBODIES * RIGID_BODY_STATE_SIZE);

        InitializeBodies();
        BodiesToArray(m_xFinal);
		return true;
	}

	bool PhysicsSystem::Shutdown(){
		return true;
	}

	void PhysicsSystem::Update(float dt){
		if(m_PausePhysics)
			return;
		//Calculate semi-fixed step
		//time = 1.0f/60.0f;
		//accumulator += dt;

		//deltaTime = std::min<double>(accumulator,time);
        UpdateSimulation(dt);

			TimeStep(dt);
			//accumulator -=dt;
		
	}

	void PhysicsSystem::TimeStep(float dt){
		GetPositions();

		Integrate(dt);

		BroadPhaseCollision();

		ResolveCollision();

		SetPositions();
	}

	void PhysicsSystem::Integrate( float dt ){
		it = Bodies.begin();
		for (; it != Bodies.end(); ++it){
			it->collision = 0;
			Vector3 oldVelocity = Vector3(0, 0, 0);
			if(it->mass < 0){ //For particles not affected by outside forces
				//update positions
				it->position = it->position + ( it->velocity * dt );
			}
			if (it->mass > 0){
				oldVelocity = it->velocity;

				if(!it->m_noGravity)
					//apply gravity
					it->velocity = it->velocity + m_gravity * dt;


				//HACK!! THIS WILL CHANGE
				if (it->velocity.x < 0)
				{
					it->velocity.x += it->friction;
				}else if (it->velocity.x > 0)
				{
					it->velocity.x -= it->friction;
				}
				// UGLY CODE END

				//update positions
				it->position = it->position + ( ( oldVelocity + it->velocity ) * dt );
			}
		}
	}

	void PhysicsSystem::BroadPhaseCollision(){

		ObjectLinkList<RigidBody>::iterator it = Bodies.begin();
		
		for (; it != Bodies.end(); ++it){	

			ObjectLinkList<RigidBody>::iterator itB = it->Next;
			//ObjectLinkList<RigidBody>::iterator itB = it;
			

			for (; itB != Bodies.end(); ++itB){
				g_COLLISIONS->checkCollision((*it), (*itB));					
			}
		}
	}

	void PhysicsSystem::ResolveCollision(){
		g_RESOLUTION->Resolve();
	}

	void PhysicsSystem::GetPositions(){
		it = Bodies.begin();
        for(;it!=Bodies.end();++it)
		{
			it->GetPosition();
		}
	}
	void PhysicsSystem::SetPositions(){
		it = Bodies.begin();
        for(;it!=Bodies.end();++it)
		{
			it->SetPosition();
		}
	}

	void PhysicsSystem::RemovePhysicsComponent(int id)
	{
		it = Bodies.begin();
        for(;it!=Bodies.end();++it)
        {
			if((*it).GetOwnerID() == id)
			{
				Bodies.erase(it);
				return;	
			}
        }
	}



	void PhysicsSystem::HandleMessages(Message* message){

		if (message->MessageId == MId::Debug)
			DebugDrawingActive = !DebugDrawingActive;

		else if(message->MessageId == MId::ObjectDestroyed)
		{
			ObjectDestroyedMessage * ODM = dynamic_cast<ObjectDestroyedMessage *>(message);
			RemovePhysicsComponent(ODM->GetID());
		}

		else if (message->MessageId == MId::ClearComponentLists)
		{			
			it = Bodies.begin();
			for(;it!=Bodies.end();++it)
			{
				Bodies.erase(it);
			}
		}
		//else if (message->MessageId == MId::TriggerZone)
	
	}

    float PhysicsSystem::SphereMomentOfInertia(float radius, float mass)
    {
        return 2.f * mass * pow(radius, 2) / 5.f;
    }
    
    void PhysicsSystem::InitializeBodies()
    {
        for (UINT i = 0; i < NBODIES; ++i)
        {
            m_rigidBodies[i].mass = 1.;

            // Moment of inertia is hard coded to a solid sphere of radius 1.
            float moi = SphereMomentOfInertia(1.f, m_rigidBodies[i].mass);

            m_rigidBodies[i].iBody = XMFLOAT3X3(moi, 0.f, 0.f, 
                                                0.f, moi, 0.f,
                                                0.f, 0.f, moi);

            // Pre-calculate the inverse of iBody
            XMStoreFloat3x3(&m_rigidBodies[i].iBodyInverse, XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat3x3(&m_rigidBodies[i].iBody)), XMLoadFloat3x3(&m_rigidBodies[i].iBody)));
        
            // Initial State

            // Position
            m_rigidBodies[i].x = XMFLOAT3((float)i, 10.f, (float)i);
            // Velocity
            m_rigidBodies[i].R = XMFLOAT3X3(0.f, 0.f, 0.f,
                                            0.f, 0.f, 0.f,
                                            0.f, 0.f, 0.f);
            // Linear Momentum
            m_rigidBodies[i].P = XMFLOAT3(0.f, 0.f, 0.f);
            // Angular Momentum
            m_rigidBodies[i].L = XMFLOAT3(0.f, 0.f, 0.f);
        }
    }

    void PhysicsSystem::UpdateSimulation(float dt)
    {
        // Copy xFinal back to x0
        for (UINT i = 0; i < RIGID_BODY_STATE_SIZE * NBODIES; ++i)
        {
            m_x0[i] = m_xFinal[i];
        }

        ode(m_x0, m_xFinal, RIGID_BODY_STATE_SIZE * NBODIES, m_CurrentPhysicsSimulatonTime, m_CurrentPhysicsSimulatonTime + dt, dxdt);

        // Copy d/dt X(t + 1/24) into state variables
        ArrayToBodies(m_xFinal);

        //g_GRAPHICSSYSTEM->DrawBodies(m_rigidBodies, NBODIES);
    }

    void PhysicsSystem::StateToArray(rBody *rb, double *y)
    {
        // Copy position
        *y++ = rb->x.x;
        *y++ = rb->x.y;
        *y++ = rb->x.z;

        // Copy velocity
        for (UINT i = 0; i < 3; ++i)
        {
            for (UINT j = 0; j < 3; j++)
            {
                *y++ = rb->R.m[i][j];
            }
        }

        // Copy linear momentum
        *y++ = rb->P.x;
        *y++ = rb->P.y;
        *y++ = rb->P.z;

        // Copy angular momentum
        *y++ = rb->L.x;
        *y++ = rb->L.y;
        *y++ = rb->L.z;
    }

    void PhysicsSystem::ArrayToState(rBody *rb, const double *y)
    {
        // Copy position
        rb->x.x = *y++;
        rb->x.y = *y++;
        rb->x.z = *y++;

        // Copy velocity
        for (UINT i = 0; i < 3; ++i)
        {
            for (UINT j = 0; j < 3; ++j)
            {
                rb->R.m[i][j] = *y++;
            }
        }

        // Copy linear momentum
        rb->P.x = *y++;
        rb->P.y = *y++;
        rb->P.z = *y++;
        
        // Copy angular momentum
        rb->L.x = *y++;
        rb->L.y = *y++;
        rb->L.z = *y++;

        // Compute auxiliary variables
        // v(t) = P(t)/M
        rb->v = rb->P / rb->mass;

        // I-1(t) = R(T) iBodyInv R(t)T
        XMStoreFloat3x3(&rb->iInv, XMLoadFloat3x3(&rb->R) * XMLoadFloat3x3(&rb->iBodyInverse) * XMMatrixTranspose(XMLoadFloat3x3(&rb->R)));

        // w(t) = I-1(t) L(t)
        XMStoreFloat3(&rb->omega, XMVector3Transform(XMLoadFloat3(&rb->L), XMLoadFloat3x3(&rb->iInv)));
    }
    
    void PhysicsSystem::ArrayToBodies(const std::vector<double> &x)
    {
        for (UINT i = 0; i < NBODIES; ++i)
        {
            ArrayToState(&m_rigidBodies[i], &x[i * RIGID_BODY_STATE_SIZE]);
        }
    }

    void PhysicsSystem::BodiesToArray(std::vector<double> &x)
    {
        for (UINT i = 0; i < NBODIES; ++i)
        {
            StateToArray(&m_rigidBodies[i], &x[i * RIGID_BODY_STATE_SIZE]);
        }
    }


    void PhysicsSystem::ddtStateToArray(rBody *rb, double *xdot)
    {
        // Copy d/dt x(t) = v(t) into xdot
        *xdot++ = rb->v.x;
        *xdot++ = rb->v.y;
        *xdot++ = rb->v.z;

        // Compute Rdot(t) = w(t) * R(t)
        XMFLOAT3X3 Rdot;
        XMStoreFloat3x3(&Rdot, XMMatrixMultiply(XMLoadFloat3x3(&Star(rb->omega)), XMLoadFloat3x3(&rb->R)));

        // Copy Rdot(t) into xdot
        for (UINT i = 0; i < 3; ++i)
        {
            for (UINT j = 0; j < 3; ++j)
            {
                *xdot++ = Rdot.m[i][j];
            }
        }

        // Copy d/dt P(t) = F(t) into xdot
        *xdot++ = rb->force.x;
        *xdot++ = rb->force.y;
        *xdot++ = rb->force.z;

        // Copy d/dt L(t) = t(t) into xdot
        *xdot++ = rb->torque.x;
        *xdot++ = rb->torque.y;
        *xdot++ = rb->torque.z;
    }

    XMFLOAT3X3 PhysicsSystem::Star(XMFLOAT3 a)
    {
        return XMFLOAT3X3(0.f, -a.z, a.y,
                          a.z, 0.f, -a.x,
                          -a.y, a.x, 0.f);
    }
    
    void ode(const std::vector<double> &x0, std::vector<double> &xEnd, int length, double t0, double t1, DerivativeFunction der)
    {
        std::vector<double> xdot(x0.size());
        dxdt(t1 - t0, x0, xdot);

        for (size_t i = 0; i < x0.size(); ++i)
        {
            // Euller
            xEnd[i] = x0[i] + xdot[i] * (t1 - t0);
        }
    }
    
    void ComputeForceAndTorque(double t, rBody *rb)
    {
        // TODO
        rb->force = XMFLOAT3(0.f, -9.8f, 0.f);
        rb->torque = XMFLOAT3(0.f, 0.f, 0.f);
    }

    void dxdt(double t, const std::vector<double> &x, std::vector<double> &xdot)
    {
        // Put data from x into m_rigidBodies
        g_PHYSICSSYSTEM->ArrayToBodies(x);

        for (UINT i = 0; i < NBODIES; ++i)
        {
            ComputeForceAndTorque(t, &g_PHYSICSSYSTEM->m_rigidBodies[i]);
            g_PHYSICSSYSTEM->ddtStateToArray(&g_PHYSICSSYSTEM->m_rigidBodies[i], &xdot[i * RIGID_BODY_STATE_SIZE]);
        }
    }
}