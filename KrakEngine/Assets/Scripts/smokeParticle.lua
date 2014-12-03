cX, cY,cZ, bX, bY, bZ, minVX, minVY, minVZ, maxVX, maxVY, maxVZ, minT, maxT, rand, name, dir = GetParticle();

parentID = GetObjID();

math.randomseed(rand);
math.random();

x = math.random(cX - (bY / 8), cX + (bY / 8));

y = math.random(cY - (bY / 8) , cY + (bY / 4));

z = math.random(cZ - bZ, cZ + bZ);

CreateAt("Particle_smoke", "particle", x, y ,z);

x = math.random(minVX / 6, maxVX / 6) * 0.25;

y = math.random(minVY, maxVY);
z = math.random(minVZ, maxVZ);

ChangeVelocity(x, y);

lifetime = math.random(minT, maxT);

AddParticleTimer(lifetime, parentID);
