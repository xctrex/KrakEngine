/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Serializer.cpp
Serializer wrapped around tinyxml. Handles serialization and deserialization. 
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 1/14/2014
- End Header -----------------------------------------------------*/

#include "Precompiled.h"
#include "serializer.h"
#include <iostream>
#include "Factory.h"

#include "GameObject.h"

//testing serialization
#include "Model.h"

namespace KrakEngine
{
    void LoadXMLAttribute(tinyxml2::XMLElement *txmlElement, float &output, char* attributeName){
        // If the attribute exists
        if (txmlElement->Attribute(attributeName))
        {
            output = txmlElement->FloatAttribute(attributeName);
        }
    }

    void LoadXMLAttribute(tinyxml2::XMLElement *txmlElement, int &output, char* attributeName){
        // If the attribute exists
        if (txmlElement->Attribute(attributeName))
        {
            output = txmlElement->IntAttribute(attributeName);
        }
    }

    void LoadXMLAttribute(tinyxml2::XMLElement *txmlElement, VertexType &output, char* attributeName){
        // If the attribute exists
        if (txmlElement->Attribute(attributeName))
        {
            std::string vertexTypeName = txmlElement->Attribute(attributeName);
            if(vertexTypeName == "AlbedoModel")
                output = VertexTypeAlbedoModel;
            else if(vertexTypeName == "TexturedModel")
                output = VertexTypeTexturedModel;
            else
                ThrowErrorIf(true, "VertexType described in XML not found");
        }
    }

	/**LoadFile
	Loads the file for serialization
	
	@param name a string that contains the file name to load (without the .xml)
	@return bool returns true if file successfully loaded and false if not.
	*/
	//--------------------------------------------------------------------------------
	bool Serializer::LoadFile(std::string name)
	{
		//format file name
		std::string filename = name + ".xml";
		filepath = filename;

		//open file
		return(File->LoadFile(filepath.c_str()) == tinyxml2::XML_NO_ERROR);
	}

	/**GetFirstChild
	Puts the firstchild of the file on the current node.
	*/
	//--------------------------------------------------------------------------------
	void Serializer::GetFirstChild()
	{
		CurrentNode = File->FirstChild();
	}

	/**PutFirstChildOnNode
	Sets the current node as the first child of the node. (goes down one level)
	*/
	//--------------------------------------------------------------------------------
	void Serializer::PutFirstChildOnNode()
	{
		CurrentNode = CurrentNode->FirstChild();
	}

	/**SetElement
	Puts the current node's element on the current element.
	*/
	//--------------------------------------------------------------------------------
	void Serializer::SetElement()
	{
		CurrentElement = CurrentNode->ToElement();
	}

	/**PutFirstChildOnElement
	Grabs the first child of the node and puts the element 
	*/
	//--------------------------------------------------------------------------------
	void Serializer::PutFirstChildOnElement()
	{
		PutFirstChildOnNode();
		SetElement();
	}

	/**NextNode
	Grabs the next node and puts it on the serializer. Returns true if there is 
	a next node and false if NULL.
	
	@return bool if there is a next node to set to and false if not.
	*/
	//--------------------------------------------------------------------------------
	bool Serializer::NextNode()
	{
		//if(!CurrentNode) return false;

		//std::cout<<CurrentElement->Name()<<" "<<"from next node"<<std::endl;
		//if(CurrentElement == CurrentNode->Parent()->LastChildElement())
		//	CurrentNode = CurrentNode->Parent();

		//CurrentNode = CurrentNode->NextSibling();
		//if(!CurrentNode) return false;
		//SetElement();

		//return true;
		tinyxml2::XMLNode * NextNode = CurrentNode->NextSibling();
		if(!NextNode)
		{
			CurrentNode = CurrentNode->Parent();
			SetElement();
			return false;
		}

		CurrentNode = NextNode;
		SetElement();
		return true;
	}

	/**ReadInt
	Reads an integer from the xml file, or defaults to 0 if not successful.

	@return int an int value between the tags
	*/
	//--------------------------------------------------------------------------------
	int Serializer::ReadInt()
	{
		const char * hi = CurrentElement->GetText();
		int i = std::stoi(hi, 0, 10);
		return i;
	}

	/**ReadBool
	Reads a boolean from the xml file, or defaults to 0 if not successful.
	
	@return bool a bool value between the tags
	*/
	//--------------------------------------------------------------------------------
	bool Serializer::ReadBool()
	{
		bool b = false;
		const char * hi = CurrentElement->GetText();
		int i = std::stoi(hi, 0, 10);
		if (i == 1)
			b = true;
		return b;
	}

	/**ReadFloat
	Reads a float from the xml file, or defaults to 0.0f if not successful.
	
	@return float a flaot between the tags.
	*/
	//--------------------------------------------------------------------------------
	float Serializer::ReadFloat()
	{
		const char * hi = CurrentElement->GetText();
		float f = std::stof(hi, 0);
		return f;
	}

	/**ReadString
	Reads a string from the xml file and gets whatever is in the text
	
	@return string the text of the element between the tags <element>THIS</element>
	*/
	//--------------------------------------------------------------------------------
	std::string Serializer::ReadString()
	{
		const char * hi = CurrentElement->GetText();
		return hi;
	}
	
	/**CheckAttribute
	Checks the attribute value of the current element, returns true
	if it matches the given parameter and false if it does not (or does not
	exist for this element).

	@param type the type to check (like gameobject or component)
	@return bool true if the type matches, false if not
	*/
	//--------------------------------------------------------------------------------
	bool Serializer::CheckAttribute(const char * attribute)
	{
		return (CurrentElement->Attribute(attribute) != NULL);
	}

	bool Serializer::CheckAttribute(const char * attribute, const char * value)
	{
		return (CurrentElement->Attribute(attribute, value) != NULL);
	}

	/**LoadFirstItem
	Gets the first item in the file and loads it onto the current element
	*/
	//--------------------------------------------------------------------------------
	void Serializer::LoadFirstItem()
	{
		//the name of the object/level/whatever
		GetFirstChild();
		SetElement();

		PutFirstChildOnElement();
	}

	/**ReadLevel
	Grabs all game objects in a level file and creates them
	*/
	//--------------------------------------------------------------------------------
	std::string Serializer::ReadLevel()
	{
		LoadFirstItem();
		
		std::string levelscript;

		do {
			if(CheckAttribute("type", "gameobject") || CheckAttribute("type", "archetype"))
				g_FACTORY->Create(*this, GetElementName());
			else if(CheckAttribute("type", "script"))
				levelscript = GetAttributeString("name");
		} while(NextNode());

		return levelscript;
	}

	//--------------------------------------------------------------------------------
	int Serializer::GetAttributeInt(const char * attribute)
	{
		return CurrentElement->IntAttribute(attribute);
	}

	float Serializer::GetAttributeFloat(const char * attribute)
	{
		return CurrentElement->FloatAttribute(attribute);
	}

	bool Serializer::GetAttributeBool(const char * attribute)
	{
		return CurrentElement->BoolAttribute(attribute);
	}

	std::string Serializer::GetAttributeString(const char * attribute)
	{
		return CurrentElement->Attribute( attribute );	
	}


	bool Serializer::HasNoChildren() const
	{
		return CurrentElement->NoChildren();
	}

	void Serializer::SetFileTypeForWriting(const char * name)
	{
		MainElement = File->NewElement(name);
		File->InsertFirstChild(MainElement);
	}

	void Serializer::PublishLevel()
	{
		SetFileTypeForWriting("level");

		WriteGameObjectToFile(g_FACTORY->Create("FPS"));
		WriteGameObjectToFile(g_FACTORY->Create("Cursor"));
	}

	void Serializer::SaveFile()
	{
		File->SaveFile(filepath.c_str());
	}

	//--------------------------------------------------------------------------------
	void Serializer::WriteGameObjectToFile(GameObject * go)
	{	
		tinyxml2::XMLElement * hi = File->NewElement((go->GetName()).c_str());

		hi->SetAttribute("type", "gameobject");

		hi->SetAttribute("objtype", go->GetType().c_str());

		hi->SetAttribute("ID", go->GetID());

		CurrentElement = hi;

		go->WriteObjectToSerializer(*this);
		
		if(MainElement)
			MainElement->InsertEndChild(hi);
		else
			File->InsertFirstChild(hi);
	}

	void Serializer::ClearFile()
	{
		File->DeleteChildren();
	}

};