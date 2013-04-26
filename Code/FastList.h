#ifndef FAST_LIST_H
#define FAST_LIST_H
#ifndef __APPLE__
#include <unordered_map>
#else
#include <tr1/unordered_map>
using namespace std::tr1; 
#endif
using namespace std; 

template<class Type>
class FastList
{
	private:
	typedef struct Node
	{
		Type data; 
		uint32_t tag; 
		Node * previousNode; 
		Node * nextNode; 
	} Node; 
	
	Node * head;
	Node * tail; 
	unordered_map <uint32_t, Node *> nodeMap; 
	uint32_t size; 
	
	public: 
	
	uint32_t length()
	{
		return nodeMap.size(); 
	}
	
	void moveToFirst(uint32_t tag)
	{
		if(!contains(tag))
		{
			printf("Tag not Found when moving to Front %u\n", tag); 
			throw;  
		}
		
		Node * thisNode = nodeMap[tag]; 
		
		if(head == thisNode)
		{
			//Already at Front
			return; 
		}
		
		Node * tempPrevious = thisNode->previousNode; 
		Node * tempNext = thisNode->nextNode; 
		
		Node * oldFirst = head; 
		head = thisNode; 
		thisNode->previousNode = NULL; 
		thisNode->nextNode = oldFirst; 
		oldFirst->previousNode = thisNode; 
		
		tempPrevious->nextNode = tempNext; 
		 
		if(tempNext != NULL && tail != thisNode)
		{
			// Make sure we weren't the last node
			tempNext->previousNode = tempPrevious; 
		}
		else
		{
			tail = tempPrevious; 
		}
		
		if(head->nextNode != NULL && head->tag == head->nextNode->tag)
		{	
			printf("insert Head = head.next\n"); 
			throw;  
		}
	}
	
	void insertFirst(uint32_t tag, Type data)
	{
		if(contains(tag))
		{
			printf("Already contains tag \n"); 
			throw;  
		}
		
		Node * thisNode = new Node(); 
		
		thisNode->nextNode = head;
		head = thisNode; 
		
		if(thisNode->nextNode != NULL)
		{
			thisNode->nextNode->previousNode = thisNode; 
		}
		thisNode->previousNode = NULL;  
		thisNode->tag = tag; 
		thisNode->data = data; 
		
		if(tail == NULL)
		{
			tail = thisNode; 
		}
 
		nodeMap[tag] = thisNode; 
	}
	
	Type getFirst()
	{
		return head->data;
	}
	
	Type get(uint32_t index)
	{
		if(head == NULL)
		{	
			return Type(); 
		}
		Node * thisNode = head; 
		uint32_t ii = 1;
		while(thisNode->nextNode != NULL && ii < index)
		{ 
			thisNode = thisNode->nextNode; 
			ii++; 
		}
		
		return thisNode->data;
	}
	
	Type removeLast()
	{
		if(tail == NULL && head == NULL)
		{
			return Type(); 
		}
		// removes from Map 
	 	nodeMap.erase(tail->tag);
	 	Node * toDelete = tail; 
	 	Type oldData = tail->data;
	 	
		if(tail->previousNode != NULL)
		{
			tail->previousNode->nextNode = NULL; 
			//Removes from linked list
	 		tail = tail->previousNode;
		}
		else
		{
			tail = NULL; 
			head = NULL; 
		}
	 	//Removes data 
	 	delete toDelete; 
	 
	 	return oldData; 
	 	 
	}
	
	bool contains(uint32_t tag)
	{
		bool contains = (nodeMap.count(tag)) > 0;
		return contains; 
	}
	
	Type * load(uint32_t tag)
	{
		if(contains(tag))
		{
			return &(nodeMap[tag]->data);
		}
		else
		{
			throw; 
		} 
	}
	
	void removeAll()
	{
		while(removeLast() != NULL); 
	}
	
	FastList()
	{
		this->head = NULL; 
		this->tail = NULL; 
		nodeMap.clear(); 
	}
	~FastList()
	{
		//removeAll(); 
	}
};

#endif
