#include "table.h"
#include <vector>
#include <functional>
#include <iostream>

using namespace std;

// Constructor
Table::Table(int N, int len) {
	m_numBuckets = 2 * (len / N);
	m_numItems = 0;
	m_buckets = new Node*[m_numBuckets];
	for (int i = 0; i < m_numBuckets; i++) {
		m_buckets[i] = nullptr;
	}
}

// Copy constructor
Table::Table(const Table& other) {
	this->m_numBuckets = other.m_numBuckets;
	this->m_numItems = other.m_numItems;
	this->m_buckets = new Node*[m_numBuckets];
	for (int i = 0; i < m_numBuckets; i++) {
		Node* temp = other.m_buckets[i];
		Node* copy;
		if (temp == nullptr) {
			m_buckets[i] = nullptr;
			continue;
		}
		copy = new Node;
		copy->seq = temp->seq;
		copy->spots = temp->spots;
		m_buckets[i] = copy;
		temp = temp->next;
		while (temp != nullptr) {
			Node* n = new Node;
			copy->next = n;
			copy = n;
			copy->seq = temp->seq;
			copy->spots = temp->spots;
			temp = temp->next;
		}
	}
}

// Destructor
Table::~Table() {
	for (int i = 0; i < m_numBuckets; i++) {
		Node* p = m_buckets[i];
		while (p != nullptr) {
			Node* temp = p->next;
			delete p;
			p = temp;
		}
	}
	delete[] m_buckets;
}

// Helper function to hash a string
// Returns the hashed value of input string
unsigned int Table::myHash(const string seq) const{
	hash<string> str_hash;
	unsigned int hashVal = str_hash(seq);
	unsigned int bucketVal = hashVal % m_numBuckets;
	return bucketVal;
}

// Adds the seq and index to table if it is not already there
void Table::add(string seq, int index) {
	unsigned int bucketNum = myHash(seq);
	// Check if the bucket is empty
	if (m_buckets[bucketNum] == nullptr) {
		m_buckets[bucketNum] = new Node(seq, index);
	}
	else {
		Node* temp = m_buckets[bucketNum];
		// Check if the seq is already in the table
		// If so, just add the new index
		if (temp->seq.compare(seq) == 0) {
			temp->spots.push_back(index);
			return;
		}
		// Otherwise, go to the end of the list and create a new Node
		while (temp->next != nullptr) {
			if (temp->seq.compare(seq) == 0) {
				temp->spots.push_back(index);
				return;
			}
			temp = temp->next;
		}
		temp->next = new Node(seq, index);
	}
	m_numItems++;
}

// Return true if input seq is in the table, false otherwise
bool Table::checkInTable(const string seq) const{
	unsigned int bucketNum = myHash(seq);
	Node* temp = m_buckets[bucketNum];
	while (temp != nullptr) {
		if (temp->seq == seq) {
			return true;
		}
		else {
			temp = temp->next;
		}
	}
	return false;
}

// Returns a vector of all the indices of input seq
vector<int> Table::getIndices(const string seq) const {
	vector<int> empty;
	if (!checkInTable(seq)) {
		return empty;
	}
	unsigned int bucketNum = myHash(seq);
	Node* temp = m_buckets[bucketNum];
	while (temp != nullptr) {
		if (temp->seq == seq) {
			return temp->spots;
		}
		else {
			temp = temp->next;
		}
	}
	
	return empty;
}

// Returns true if input seq occurs at input index, false otherwise
bool Table::atIndex(const string seq, int index) const{
	vector<int> positions = getIndices(seq);
	for (int i = 0; i < positions.size(); i++) {
		if (positions[i] == index) {
			return true;
		}
	}
	return false;
}

// Helper function displays hashtable
void Table::display() const{
	for (int i = 0; i < m_numBuckets; i++) {
		Node* temp = m_buckets[i];
		if (temp != nullptr) {
			string output;
			while (temp != nullptr) {
				output += ("Seq = " + temp->seq + " Index = ");
				for (int i = 0; i < temp->spots.size(); i++) {
					output += to_string(temp->spots[i]) + " ";
				}
				output += " || ";
				temp = temp->next;
			}
			cerr << output << endl;
		}
	}
}