#include "NodeList.h"

NodeList::NodeList(): m_head(NULL) {}

NodeList::NodeList(size_t count, const DataType & value) {
	m_head = new Node[count];
	for(size_t i = 0; i < count; i++) {
		m_head[i] = value;
	}
}


NodeList::NodeList(const NodeList & other) {

}