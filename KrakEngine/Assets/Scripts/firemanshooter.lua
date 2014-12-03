CreateAt("object_NPC", "fireman", 5,5,-4)
ChangeSize(3,4,0.1);
SetState("WanderRight");
ChangeAction(1,"fireman");
ChangeAction(2,"fireman2");
AddAudio();
ChangeAudioFile(1, "SoundEffect");
ChangeTexture("fireman");

CreateAt("object_static", "floor", 0,0,0)
ChangeSize(100, 5, 10);

math.randomseed(os.time());
x = math.random(20);
y = math.random(10);

x = x - 10;
y = y + 10;

CreateAt("object_static", "target", x, y, 0)
ChangeSize(2, 2, 6);

x = math.random(20);
y = math.random(10);

x = x - 10;
y = y + 10;

CreateAt("object_static", "target", x, y, 0)
ChangeSize(2, 2, 6);

x = math.random(20);
y = math.random(10);
x = x - 10;
y = y + 10;

CreateAt("object_static", "target", x, y, 0)
ChangeSize(2, 2, 6);

x = math.random(20);
y = math.random(10);

x = x - 10;
y = y + 10;
CreateAt("object_static", "target", x, y, 0)
ChangeSize(2, 2, 6);