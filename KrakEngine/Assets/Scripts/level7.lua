CreateAt("object_NPC", "mime", 20,12,0)
ChangeSize(3,4,0.1);
SetState("NoState");
ChangeAction(1,"mimeup");
ChangeAction(2,"mimedown");
AddAudio();
ChangeAudioFile(0, "Jump");
ChangeAudioFile(1, "Mime");
ChangeAudioFile(2, "Mime");
ChangeTexture("mime");

CreateAt("object_NPC", "fireman", 20,5,-4)
ChangeSize(3,4,0.1);
SetState("NoState");
ChangeAction(1,"fireman");
ChangeAction(2,"fireman2");
AddAudio();
ChangeAudioFile(0, "Jump");
ChangeAudioFile(1, "Water");
ChangeAudioFile(2, "Ladder");
ChangeTexture("fireman");

CreateAt("object_static", "floor", 15,10,0)
ChangeSize(50, 3, 10);

CreateAt("object_static", "floor", 40,0,0)
ChangeSize(500, 3, 10);

CreateAt("object_static", "floor", -50,0,0)
ChangeSize(20, 500, 10);

CreateAt("object_static", "floor", 80,10,0)
ChangeSize(10, 3, 10);

CreateAt("object_static", "floor", -30,3,-5)
ChangeSize(5, 3, 10);
CreateAt("object_static", "floor", -25,6,-5)
ChangeSize(5, 3, 10);
CreateAt("object_static", "floor", -20,9,-5)
ChangeSize(5, 3, 10);


CreateAt("object_static", "fire", 60, 5, 0)
ChangeSize(10, 1000, 10);

