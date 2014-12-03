#include "Precompiled.h"

#include "Resolution.h"
#include "GameObject.h"
#include "MathLib.h"


namespace KrakEngine{

	Resolution * g_RESOLUTION = NULL;

	Resolution::Resolution(){
		g_RESOLUTION = this;
	}

	void Resolution::AddCollisionManifold(Manifold m){
		collisionsList.push_back(m);
	}

	void Resolution::FlushCollisions(){
		collisionsList.clear();
	}

	void Resolution::Resolve(){
		if (collisionsList.size() == 0)
			return;
		it = collisionsList.begin();
		for (;it != collisionsList.end();++it){
			ResolvePosition(&(*it));
			ResolveCollision(&(*it));
			//ResetFlags(&(*it));
			
		}
		FlushCollisions();
	}

	void Resolution::ResetFlags(Manifold* m){
		m->A->collision = 0;
		m->B->collision = 0;
	}

	void Resolution::ResolvePosition(Manifold* m){
		float percent = 0.8f;
		float slop = 0.01f;
		float sumInvMass =  m->A->invMass + m->B->invMass;
		Vector3 correction = (max(m->penetration - slop,0.0f) /
							(sumInvMass)) * m->ContactNormal * percent;
		
		//Vector3 correction = ((m->penetration / sumInvMass) * m->ContactNormal) * percent;
		
		m->A->position = m->A->position + (correction * m->A->invMass);
		m->B->position = m->B->position + (correction * -m->B->invMass);
		
	}

	void Resolution::ResolveCollision(Manifold* m){
		Vector3 relVel = m->A->velocity - m->B->velocity;

		float velNormal = Dot(relVel,m->ContactNormal);
		float sumInvMass =  m->A->invMass + m->B->invMass;

		if (velNormal > 0)
			return;

		float scalar = m->restitution * -velNormal;
		scalar -= velNormal;
		//scalar /= sumInvMass; 

		float impulse = scalar / sumInvMass;
		Vector3 massImpulse = m->ContactNormal * impulse;
		m->A->velocity = m->A->velocity + massImpulse * m->A->invMass;
		m->B->velocity = m->B->velocity + massImpulse * -m->B->invMass;
		
		//friction

		relVel = m->A->velocity - m->B->velocity;

		Vector3 tangent = relVel - Dot(relVel,m->ContactNormal) * m->ContactNormal;

		Normalize(tangent);

		float tangentScalar = -Dot(relVel,tangent);
		tangentScalar = tangentScalar / sumInvMass;


		Vector3 frictionImpulse = Vector3(0,0,0);
		if (abs(tangentScalar) < impulse * m->friction)

			frictionImpulse = tangentScalar * tangent;

		m->A->velocity = m->A->velocity + frictionImpulse * m->A->invMass;
		m->B->velocity = m->B->velocity + frictionImpulse * -m->B->invMass;

	}
}