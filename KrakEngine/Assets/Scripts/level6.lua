CreateAt("object_NPC", "mime", 20,5,4)
ChangeSize(3,4,0.1);
SetState("WanderRight");
ChangeAction(1,"mimeup");
ChangeAction(2,"mimedown");
AddAudio();
ChangeAudioFile(0, "Jump");
ChangeAudioFile(1, "Mime");
ChangeAudioFile(2, "Mime");
ChangeTexture("mime");

CreateAt("object_NPC", "fireman", -22,32,-4)ChangeSize(3,4,0.1);
SetState("NoState");
ChangeAction(1,"fireman");
ChangeAction(2,"fireman2");
AddAudio();
ChangeAudioFile(0, "Jump");
ChangeAudioFile(1, "Water");
ChangeAudioFile(2, "Ladder");
ChangeTexture("fireman");

CreateAt("object_static", "floor", 240,0,0)
ChangeSize(500, 3, 10);

CreateAt("object_static", "wall", -12,5,0)
ChangeSize(10, 10, 10);

CreateAt("object_static", "wall", -22, 5, 0)
ChangeSize(10, 20, 10);

CreateAt("object_static", "fire", 30, 5, 0)
ChangeSize(10, 1000, 10);

