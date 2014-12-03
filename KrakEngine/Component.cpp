//component.h

//the component base class that all components inherit, has functionality
//for all components.

//Team: Go Ninja
//Project: GAM 541 game
//Author: Jeff Kitts
//--------------------------------------------------------------------------------

#include "Component.h"
#include "GameObject.h"

namespace KrakEngine
{
	void Component::SetParent(GameObject * GO) {
			m_Parent = GO;
			m_ObjectID = GO->GetID();
	};

	void Component::InitializeDataMember(Serializer & Seri, int &DataMember, const char * AttributeName) { 
		if (Seri.CheckAttribute(AttributeName))
			DataMember = Seri.GetAttributeInt(AttributeName);
	}

	void Component::InitializeDataMember(Serializer & Seri, bool &DataMember, const char * AttributeName) { 
		if (Seri.CheckAttribute(AttributeName))
			DataMember = Seri.GetAttributeBool(AttributeName);
	}

	void Component::InitializeDataMember(Serializer & Seri, float &DataMember, const char * AttributeName) { 
		if (Seri.CheckAttribute(AttributeName))
			DataMember = Seri.GetAttributeFloat(AttributeName);
	}
		
	void Component::InitializeDataMember(Serializer & Seri, std::string &DataMember, const char * AttributeName) { 
		if (Seri.CheckAttribute(AttributeName))
			DataMember = Seri.GetAttributeString(AttributeName);
	}

    void Component::InitializeDataMember(Serializer & Seri, VertexType &DataMember, const char * AttributeName) { 
		if (Seri.CheckAttribute(AttributeName))
        {
			std::string vertexTypeName = Seri.GetAttributeString(AttributeName);
            if (vertexTypeName == "FBXBinModel")
                DataMember = VertexTypeFBXBinModel;
            else if (vertexTypeName == "FBXBinSkinnedModel")
                DataMember = VertexTypeFBXBinSkinnedModel;
            else if(vertexTypeName == "AlbedoModel")
                DataMember = VertexTypeAlbedoModel;
            else if(vertexTypeName == "TexturedModel")
                DataMember = VertexTypeTexturedModel;
            else
                ThrowErrorIf(true, "VertexType described in XML not found");
        }
	}
}