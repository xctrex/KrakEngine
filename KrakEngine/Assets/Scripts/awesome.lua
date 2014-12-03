

CreateAt("object_NPC", "mime", 0,5,0)

ChangeSize(3,4,1);
ChangeAction(1, "mimeup");
ChangeAction(2, "mimedown");
ChangeTexture("mime");

CreateAt("object_NPC", "fireman", 4,5,0)

ChangeSize(3,4,1);
ChangeAction(1, "fireman");
ChangeAction(2, "fireman2");
SetState("WanderLeft");
ChangeTexture("fireman");


CreateAt("floor", "floor", 0,0,0)
ChangeSize(30, 3, 10);

CreateAt("floor", "floor", 50,0,0)
ChangeSize(20, 3, 10);

CreateAt("floor", "floor", 55,10,0)
ChangeSize(5, 3, 10);



