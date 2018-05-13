#ifndef CHEL_DBL_LK_LST_HPP
#define CHEL_DBL_LK_LST_HPP

#include "CDblLkLst.h"

#define Node SDLkNode

template<class T> Node<T>* CDblLkLst<T>::knownNodeClosestToIndex(int index, int& ldResultIndex) const {
#define abs(a) (a) < 0 ? -a : a
	Node<T>* result = m_pFirst;
	int min = index;
	ldResultIndex = 0;
	
	if (m_pLastReferenced) {
		int differenceToLastReferenced = abs(index - m_iLastReferencedIndex);
		if (differenceToLastReferenced <= min) {
			min = differenceToLastReferenced;
			result = m_pLastReferenced;
			ldResultIndex = m_iLastReferencedIndex;
		}
	} else {
		m_pLastReferenced = m_pFirst;
		m_iLastReferencedIndex = 0;
	}
	
	int differenceToEnd = m_iLength - index - 1;
	if (differenceToEnd < min) {
		min = differenceToEnd;
		result = m_pLast;
		ldResultIndex = m_iLength - 1;
	}
	
	return result;
#undef abs
}

template<class T> Node<T>* CDblLkLst<T>::getNode(int index) const {
	//AssertTrue(index >= 0 && index < m_iLength && m_iLength > 0, "Valid CDblLkLst index");
	
	int 		iStartIndex;
	Node<T>* 	result = knownNodeClosestToIndex(index, iStartIndex);
	
	bool bForward = iStartIndex < index;
	if (bForward) {
		while (iStartIndex != index) {
			result = result->m_post;
			iStartIndex++;
		}
	} else {
		while (iStartIndex != index) {
			result = result->m_pre;
			iStartIndex--;
		}
	}
	
	m_iLastReferencedIndex = index;
	m_pLastReferenced = result;
	return result;
}

template<class T> void CDblLkLst<T>::add(int index, T value) {
	//AssertTrue(index >= 0 && index <= m_iLength, "Valid index for adding to CDblLkLst");
	if (index != m_iLength && m_iLength > 0) {
		Node<T>* pExistingNode = getNode(index);
		Node<T>* pBeforeExisting = pExistingNode->m_pre;
		
		//build a new node
		Node<T>* pNewNode = new Node<T>(pBeforeExisting, pExistingNode, value);
		if (index == 0)
			m_pFirst = pNewNode;
		
		//now change the references in the previous nodes
		if(pBeforeExisting)
			pBeforeExisting->m_post = pNewNode;
		pExistingNode->m_pre = pNewNode;
		
		m_iLastReferencedIndex++;
	} else if (index == m_iLength && m_iLength > 0) {
		Node<T>* pNewNode = new Node<T>(m_pLast, nullptr, value);
		m_pLast->m_post = pNewNode;
		m_pLast = pNewNode;
	} else {
		Node<T>* pNewNode = new Node<T>(nullptr, nullptr, value);
		m_pFirst = m_pLast = m_pLastReferenced = pNewNode;
		m_iLastReferencedIndex = 0;
	}
	m_iLength++;
}

template<class T> T CDblLkLst<T>::remove(int index) {
	//AssertTrue(m_iLength, "No removing from empty CDblLkLst");
	//AssertTrue(index >= 0 && index < m_iLength, "Valid remove index from CDblLkLst");
	
	Node<T>* removeMe = getNode(index);
	Node<T>* preRemove = removeMe->m_pre;
	Node<T>* postRemove = removeMe->m_post;
	T removedValue = removeMe->m_value;
	
	if (preRemove)
		preRemove->m_post = postRemove;
	else
		m_pFirst = postRemove;
	
	if(postRemove)
		postRemove->m_pre = preRemove;
	else
		m_pLast = preRemove;
	
	delete removeMe;
	m_pLastReferenced = nullptr;
	
	m_iLength--;
	
	return removedValue;
}

template<class T> void CDblLkLst<T>::flush() {
	while (!isEmpty())
		pop();
}

template<class T> bool CDblLkLst<T>::contains(const T& value) const {
	Node<T>* pNode = m_pFirst;
	bool bFound = false;

	while (!bFound && pNode) {
		bFound = pNode->m_value == value;
		pNode = pNode->m_post;
	}

	return bFound;
}

template<class T> int CDblLkLst<T>::indexOf(const T& value) const {
	Node<T>* pNode = m_pFirst;
	int foundIndex = -1;
	int i = 0;
	while (pNode) {
		if (pNode->m_value == value) {
			foundIndex = i;
		}
		pNode = pNode->m_post;
		i++;
	}

	return foundIndex;
}

template<class T> bool CDblLkLst<T>::operator ==(const CDblLkLst<T>& other) const {
	bool bEqual = this->length() == other.length();
	for (int i = 0; bEqual && i < length(); i++) {
		bEqual = this->get(i) == other.get(i);
	}
	return bEqual;
}


#undef Node
#endif //CHEL_DBL_LK_LST_HPP