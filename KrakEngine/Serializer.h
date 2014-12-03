/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Serializer.h
Header for serializer.cpp
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 1/14/2014
- End Header -----------------------------------------------------*/

#pragma once

#include "Precompiled.h"
#include "Vertex.h"

#include <list>

namespace KrakEngine
{

	class GameObject;

    void LoadXMLAttribute(tinyxml2::XMLElement *txmlElement, float &output, char* attributeName);
    void LoadXMLAttribute(tinyxml2::XMLElement *txmlElement, int &output, char* attributeName);
    void LoadXMLAttribute(tinyxml2::XMLElement *txmlElement, VertexType &output, char* attributeName);

	class Serializer
	{
	public:
		Serializer() : File(new tinyxml2::XMLDocument), CurrentElement(NULL), CurrentNode(NULL), MainElement(NULL) {};
		~Serializer() {
			//delete File;
		};

		bool LoadFile(std::string objName);

		void PublishLevel();
		void SetFileTypeForWriting(const char * name);
		void WriteGameObjectToFile(GameObject * go);
		void SaveFile();

		void GetFirstChild();

		void PutFirstChildOnNode();
		void SetElement();

		/**GetElementName
		Gets the name of the current element

		@return const char * the name of the element <THIS></>
		*/
		const char * GetElementName() { return CurrentElement->Name(); };

		tinyxml2::XMLDocument * GetFile() { return File; };

		bool NextNode();

		void PutFirstChildOnElement();

		//grab information
		int ReadInt();
		bool ReadBool();
		float ReadFloat();
		std::string ReadString();

		bool CheckAttribute(const char * type);
		bool CheckAttribute(const char * attribute, const char * value);

		tinyxml2::XMLElement * GetElement() { return CurrentElement; };

		//gets the information from attributes
		int GetAttributeInt(const char * attribute);
		bool GetAttributeBool(const char * attribute);
		float GetAttributeFloat(const char * attribute);
		std::string GetAttributeString(const char * attribute);

		//loads the first node of the file
		void LoadFirstItem();

		//reads a level file
		std::string ReadLevel();

		//checks if node has children
		bool HasNoChildren() const;

		void ClearFile();

	private:
		tinyxml2::XMLDocument * File;

		std::string filepath;

		//const char * filepath;

		tinyxml2::XMLNode * CurrentNode;

		tinyxml2::XMLElement * CurrentElement;

		tinyxml2::XMLElement * MainElement;
	};

};