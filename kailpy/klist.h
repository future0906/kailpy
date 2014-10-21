/*
    common.h file, contain common function and class
*/

#ifndef __H_KLIST_H_
#define __H_KLIST_H_
#include <iostream>
using std::cout;

//klist
template<class DataType>
struct KListNode
{
	DataType m_data;
	KListNode* m_next;
	KListNode* m_back;
};

//===========================================================
// Two way linked list with head node 
//             use template
//===========================================================
template<class DataType>
class KList
{
    friend class KListIter;
private:
	KListNode<DataType>* mp_head;
	KListNode<DataType>* mp_tail;
    int m_length;
public:
	KList();
    ~KList();
	KListNode<DataType>* addData(DataType);
	KListNode<DataType>* addNode(KListNode<DataType>*);
	void printList(int, bool);
	int getLength();
public:
    //================Iterator=================
    class KListIter{
    private:
        KListNode<DataType>* mp_cur;
        KList<DataType>* mp_list;
    public:
        KListIter(KList<DataType>* p_list, KListNode<DataType>* p_cur) {
            mp_list = p_list;
            mp_cur = p_cur;
        }

        //Next element
        KListNode<DataType>* next() {
	        KListNode<DataType>* tmp_po;
	        tmp_po = mp_cur->m_next;
	        if (tmp_po == mp_list->mp_head) {
		        tmp_po = NULL;
	        }
	        else {//if not at tail move mp_cur
		        mp_cur = tmp_po;
	        }
	        return tmp_po;
        }
        //Reset iterator
        void rewind() {
            mp_cur  = mp_list->mp_head;
        }
        friend class KList;
    };
    typedef KListIter iterator;
public:
    //Added method
    iterator begin() {
        return iterator(this, this->mp_head);
    }
    iterator end() {
        return iterator(this, this->mp_tail);
    }
public:
    int remove(iterator it);
};

template<typename DataType>
KList<DataType>::KList()
{
	//When list create, there is a EMPTY node at head(head node)
	KListNode<DataType>* p_node = new KListNode<DataType>;
	p_node->m_data = NULL;
	p_node->m_next = p_node;
	p_node->m_back = p_node;

	this->mp_head = p_node;
	this->mp_tail = p_node;
    
    m_length = 0;
}

template<typename DataType>
KListNode<DataType>* KList<DataType>::addData(DataType data)
{
	KListNode<DataType>* p_node = new KListNode<DataType>;
	__ASSERT(p_node);//Out of MEMORY
    p_node->m_data = data;
	this->addNode(p_node);
	return p_node;
}

template<typename DataType>
KListNode<DataType>* KList<DataType>::addNode(KListNode<DataType>* p_node)
{
	//Rewind pointer
	p_node->m_back = this->mp_tail;
	p_node->m_next = this->mp_head;
	this->mp_tail->m_next = p_node;
	this->mp_head->m_back = p_node;
	this->mp_tail = p_node;
	++m_length;
	return p_node;
}

template<typename DataType>
void KList<DataType>::printList(int order, bool address)
{
	KListNode<DataType>* p_cur = this->mp_head->m_next;
	while (p_cur != this->mp_head) {
		if (!address) {
			cout<<(p_cur->m_data);
		}
		else {
			cout<<*(DataType*)(p_cur->m_data);
		}
		p_cur = p_cur->m_next;
	}
}

template<typename DataType>
int KList<DataType>::getLength()
{
    return m_length;
}

template<typename DataType>
int KList<DataType>::remove(iterator it)
{
    it.mp_cur->m_back->m_next = it.mp_cur->m_next;
    it.mp_cur->m_next->m_back = it.mp_cur->m_back;
    mp_tail = mp_head->m_back;
    m_length--;
    return 1;
}

template<typename DataType>
KList<DataType>::~KList()
{
	KListNode<DataType>* p_cur = this->mp_head->m_next;
    KListNode<DataType>* p_next = p_cur->m_next;
	while (p_cur != this->mp_head) {
		delete p_cur;
        p_cur = p_next;
        p_next = p_next->m_next;
	}
    delete p_cur;
}
#endif//__H_KLIST_H_
