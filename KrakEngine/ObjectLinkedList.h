///////////////////////////////////////////////////////////////////////////////
///
///	\file ObjectLinkedList.h
///	
///	Authors: Chris Peters
/// 		 Cristina Pohlenz
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

namespace KrakEngine
{
	///	Intrusively linked list for objects. This enables the ability to store 
	/// objects in an efficient linked list structure without duplicating code. 
	/// Object to be used in this container must have Prev and Next pointers as
	/// members.
	template< typename type >
	class ObjectLinkList
	{
	public:
		typedef type * pointer;
		typedef type& reference;

		ObjectLinkList()
			:First(NULL), Last(NULL), ObjectCount(0)
		{
		}

		//Pop the front element. If the list is empty return NULL.
		pointer pop_front()
		{

			if (First == NULL)
			{
				//List is empty
				return NULL;
			}
			
			//get the first object
			type * first = First;
			First = first->Next;

			//Update linked list pointers
			if (First)
			{
				//more elements in list
				First->Prev = NULL;
			}
			else
			{
				//popped last object in list
				Last = NULL;
			}

			--ObjectCount;
			return first;
		}

		void push_back(pointer object)
		{
			++ObjectCount;

			if (Last == NULL)
			{
				//list was empty
				First = object;
				Last = object;
				object->Next = NULL;
				object->Prev = NULL;
			}
			else
			{
				object->Prev = Last;
				object->Next = NULL;
				Last->Next = object;
				Last = object;
			}
		}

		void erase(pointer object)
		{
			//**************NEW CODE***************//
			--ObjectCount;

			if(First==object)
			{
				First = object->Next;
			}
			if(Last == object)
			{
				Last = object->Prev;
			}

			if(object->Prev)
			{
				object->Prev->Next = object->Next;
			}
			if(object->Next)
			{
				object->Next->Prev = object->Prev;
			}
			
			//delete object;

			/*****ORIGINAL CODE************
			if (object->Next == NULL && object->Prev == NULL)
			{
				//Only object in list make list empty
				Last = NULL;
				First = NULL;
			}
			else if (object->Next == NULL)
			{
				//Object is last update Last
				Last = object->Prev;
				if (Last) Last->Next = NULL;

			}
			else if (object->Prev == NULL)
			{
				//Object is first update first
				First = object->Next;
				if (First) First->Prev = NULL;
			}
			else
			{
				//Object is in middle just update pointers
				object->Prev->Next = object->Next;
				object->Next->Prev = object->Prev;
			}
			//*/
		}

		///Intrusive linked list iterator
		class iterator
		{
		public:
			friend class ObjectLinkList;
			iterator(){};
			iterator(pointer p) : item(p) {}
			void operator--(){ item = item->Prev; }
			void operator++(){ item = item->Next; }
			reference operator*(){ return *item; }
			pointer operator->(){ return item; }
			bool operator==(const iterator& i){ return item == i.item; }
			bool operator!=(const iterator& i){ return item != i.item; }
			operator bool(){ return item != NULL; }
			operator pointer(){ return item; }
			pointer GetPtr(){ return item; }
		private:
			pointer item;
		};

		iterator begin(){ return First; }
		iterator end(){ return NULL; }
		pointer last(){ return Last; }
	private:
		pointer First;
		pointer Last;
		unsigned ObjectCount;
	};
}
