SelectCurrentControlledObject();
Px, Py, Pz = GetTransform();
dir = GetDirection();

SetCenter(Px + (2.5*dir), Py, Pz);
SetParticleScript("waterParticle");
