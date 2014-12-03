maxtime = 800;
AddTimer(maxtime);

curtime = GetTime();

ChangeVelocity((maxtime-curtime) * 0.8, 0);
