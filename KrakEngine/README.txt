Tommy is testing merging to master with this message.
/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Purpose: Explain Game Controllers, Gameplay, and Grading Details
Language: English
Platform: NA
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/13/2013
- End Header -----------------------------------------------------*/

	============
		INFO
	============
	This is the Frankengine, for I am Dr. Frankenstein and this 
	is my creation from multiple parts.
	It might be an abomination, but it just wants 	to be a 
	normal human(engine) and be loved.

	(NOTE: I am not actually Dr. Frankenstein, but since my game is
	not fun I thought I could make a fun reference instead, you know)
	
	============
	 CONTROLERS
	============

	UP		-> 		Move Ship UP
	DOWN	-> 		Move Ship DOWN
	SPACE	->		Shoot
	C		->		Shoot bullet barrier
	
	ESC / Q ->		Quit Game
	
	D		->		Toggle Debug on/off
	
	============
	  GAMEPLAY
	============
	
	To win you must survive through the level either by destroying or 
	avoiding all enemies.
	
	If an enemy or enemy bullet touches the ship, you lose.
	
	
	============
	  FEATURES
	============
	
	Debug Drawing
		Basic Debug Drawing (Press D):			Object Collision Bodies
	
	Object Architecture:
		Basic C++ Objects
		Component Based
		Game Object Ids, Garbage Collector:		Using ObjectFactory from Sample Engine
		
	Communication:
		Basic Events:							Input Management and Messages
		
	Data Driven Design:
		Basic Text Serialization
		Creation of objects from Data Files
		Level Files
		Archetypes								Inside Factory (CreateAt(string,float,float))
		
	Graphics:
		Sprite Rendering (textured quads):		DirectX 11
		Basic Sprite Transformation
		
	Physics:
		Basic Collision Detection between circles
		Basic Collision Response:				Objects destroy
		Collision between different body types:	circle/rectangle
		
	Game Requirements:
		Control of Ship
		Game runs at (over) 30fps
		2 different types of enemies
		2 different types of player attacks
		Collision between projectiles, enemies, and player
		Control Screen
		Win/Lose Condition