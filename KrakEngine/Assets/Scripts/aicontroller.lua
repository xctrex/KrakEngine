CreateAt("object_NPC", "mime", 0,5,4)
ChangeSize(2,6,1);
SetState("MoveRight");
ChangeAction(1,"mimeup");
ChangeAction(2,"mimedown");
ChangeTexture("mime");

CreateAt("object_NPC", "fireman", 4,5,-4)
ChangeSize(3,4,1);
SetState("MoveRight");
ChangeAction(1,"fireman");
ChangeAction(2,"fireman2");
ChangeTexture("fireman");

CreateAt("floor", "hi", 0,0,0)
ChangeSize(50, 3, 10);

CreateAt("floor", "hi", 50,0,0)
ChangeSize(20, 3, 10);

CreateAt("floor", "hi", 55,10,0)
ChangeSize(5, 3, 10);
