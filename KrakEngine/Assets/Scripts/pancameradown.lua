--[[

x, y, z = GetTransform();

if y > 15 then
	MoveCamera(y, y);
else
	MoveCamera(10, 10);
end
--]]

id = GetObjID();

SelectCurrentControlledObject();
x, y, z = GetTransform();
keepgoing = PanCameraDown(y, y);

SetCurrentObjByID(id);

Continue(true);