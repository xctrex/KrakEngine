

x, y, z = GetTransform();


function Ladder (xoff, yoff)

CreateAt("object_misc", "ladder", xoff, yoff, z)
ChangeSize(2,2,1)
ChangeMass(0)

end

SetCurrentObject("Cursor");
Cx, Cy, Cz = GetTransform();
DistX = Cx - x;

xo = 7
yo = -3


if DistX>0 then
--Ladder(x + xo, y + yo)
for i = 1,5 do
	Ladder(x + xo, y + yo)
	xo = xo + 2
	yo = yo + 2
	end
else
--Ladder(x - xo, y - yo)
	for i = 1,5 do
	Ladder(x - xo, y + yo)
	xo = xo - 2
	yo = yo - 2
	end
end
