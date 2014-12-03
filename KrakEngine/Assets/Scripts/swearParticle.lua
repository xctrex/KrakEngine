cX, cY,cZ, bX, bY, bZ, minVX, minVY, minVZ, maxVX, maxVY, maxVZ, minT, maxT, rand, name, dir = GetParticle();

parentID = GetObjID();

math.randomseed(rand);
math.random();

x = math.random(cX - (bX/2), cX + bX);

y = math.random(cY , cY );

z = math.random(cZ - bZ, cZ + bZ);

CreateAt("Particle_swear", "particle", x, y ,z);

x = math.random(minVX, maxVX) * 0.5;

y = math.random(minVY, maxVY) * 2.5;
z = math.random(minVZ, maxVZ);

ChangeVelocity(x, y);

lifetime = math.random(minT, maxT);

AddParticleTimer(lifetime, parentID);