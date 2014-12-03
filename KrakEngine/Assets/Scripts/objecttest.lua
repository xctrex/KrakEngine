
x = 0
y = 0
square = 25

CreateAt("floor", "f", 0, -5, 0);
ChangeSize(30, 2, 1);	

while x < square do
	while y < square do
	CreateAt("floor", "f", x, y, 0);
	ChangeSize(0.5, 0.5, 0.5);		
	y = y + 1;
	end
x = x + 1;
y = 0;
end