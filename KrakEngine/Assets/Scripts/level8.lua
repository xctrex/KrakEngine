CreateAt("object_Enemy", "burglar", 20,14,0)
ChangeSize(3,4,0.1);
SetState("Burglar");
SetLeftDestination(20,0,0);
SetRightDestination(40,0,0);
AddAudio();
ChangeAudioFile(0, "Jump");
ChangeAudioFile(1, "Mime");
ChangeAudioFile(2, "Mime");
ChangeTexture("mime");

CreateAt("object_NPC", "policeman", 15,14,0)
ChangeSize(3,4,0.1);
SetState("NoState");
SetLeftDestination(20,0,0);
SetRightDestination(40,0,0);
AddAudio();
ChangeAudioFile(0, "Jump");
ChangeAudioFile(1, "Mime");
ChangeAudioFile(2, "Mime");
ChangeTexture("fireman");

CreateAt("object_static", "floor", 15,10,0)
ChangeSize(150, 3, 10);
