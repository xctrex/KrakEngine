CreateAt("object_NPC", "mime", 0,5,4)
ChangeSize(3,4,0.1);
SetState("WanderRight");
ChangeAction(1,"mimeup")
ChangeAction(2,"mimedown");
ChangeTexture("mime");

 CreateAt("object_static", "floor", 0,0,0)
 ChangeSize(40, 5, 10);

 CreateAt("object_static", "floor", 100,0,0)
 ChangeSize(40, 5, 10);

 CreateAt("car", "car", 10, 7, 2);
 SetState("WanderRight");