cX, cY,cZ, bX, bY, bZ, minVX, minVY, minVZ, maxVX, maxVY, maxVZ, minT, maxT, rand, name, dir = GetParticle();

parentID = GetObjID();

math.randomseed(rand);
math.random();

x = math.random(cX - bX, cX + bX);

y = math.random(cY - bY, cY + bY);

z = math.random(cZ - bZ, cZ + bZ);

CreateAt("Particle_wind", "particle", x, y ,z);

x = math.random(minVX, maxVX) * dir;

y = math.random(minVY, maxVY) * 2;
z = math.random(minVZ, maxVZ);

ChangeVelocity(x, y);

lifetime = math.random(minT, maxT);

AddParticleTimer(lifetime, parentID);
