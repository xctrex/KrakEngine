cX, cY,cZ, bX, bY, bZ, minVX, minVY, minVZ, maxVX, maxVY, maxVZ, minT, maxT, rand, name = GetParticle();

parentID = GetObjID();

math.randomseed(rand);
math.random();

x = math.random(cX - bX, cX + bX);

y = math.random(cY - bY, cY + bY);

z = math.random(cZ - bZ, cZ + bZ);

CreateAt("object_fire", "particle", x , y ,z);

x = math.random(minVX, maxVX);

y = math.random(minVY, maxVY);
z = math.random(minVZ, maxVZ);

ChangeVelocity(x, y);

lifetime = math.random(minT, maxT);

AddParticleTimer(lifetime, parentID);
