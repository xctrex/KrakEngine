x, y, z = GetTransform();
SetCurrentObject("Cursor");
Cx, Cy, Cz = GetTransform();

DistX = Cx - x;
DistY = Cy - y;

CreateAt("object_misc", "mimebox", Cx, Cy, z)
ChangeSize(3,3,0)
ChangeMass(0)
AddTimer(4000)


