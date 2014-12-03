--[[

x, y, z = GetTransform();

if y > 15 then
	MoveCamera(y, y);
else
	MoveCamera(10, 10);
end
--]]

Debug();

id = GetObjID();

SelectCurrentControlledObject();
x, y, z = GetTransform();
keepgoing = MoveCamera(y, y);

SetCurrentObjByID(id);

Continue(true);