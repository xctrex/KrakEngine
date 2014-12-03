
CreateAt("object_NPC", "mime", 70,5,4)
ChangeSize(3,4,0.1);
SetState("WanderRight");
ChangeAction(1,"mimeup");
ChangeAction(2,"mimedown");
AddAudio();
ChangeAudioFile(0, "Jump");
ChangeAudioFile(1, "Mime");
ChangeAudioFile(2, "Mime");
ChangeTexture("mime");


CreateAt("object_NPC", "fireman", 5,5,-4)
ChangeSize(3,4,0.1);
SetState("WanderRight");
ChangeAction(1,"fireman");
ChangeAction(2,"fireman2");
AddAudio();
ChangeAudioFile(0, "Jump");
ChangeAudioFile(1, "Water");
ChangeAudioFile(2, "Ladder");
ChangeTexture("fireman");

CreateAt("object_static", "floor", 240,0,0)
ChangeSize(500, 5, 10);

 CreateAt("object_static", "wall", -15,0,0)
 ChangeSize(10, 4, 10);

 CreateAt("object_static_fire", "fire", 40, 0, 0)
ChangeSize(10, 100, 10);
 
 CreateAt("object_static", "wall", 30,0,0)
 ChangeSize(10, 4, 10);
 
 CreateAt("object_static", "wall", 90,5,0)
 ChangeSize(10, 20, 10);



-- CreateAt("floor", "hi", 55,10,0)
-- ChangeSize(5, 3, 10);
