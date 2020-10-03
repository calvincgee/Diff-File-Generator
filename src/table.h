#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <vector>

// Hashtable class
class Table {
public:
	Table(int N, int len);
	Table(const Table& other);
	~Table();
	int numBuckets() const { return m_numBuckets; }
	int numItems() const { return m_numItems; }
	void add(std::string seq, int index);
	bool checkInTable(const std::string seq) const;
	std::vector<int> getIndices(const std::string seq) const;
	bool atIndex(const std::string seq, int index) const;
	void display() const;

private:
	// Nodes of the linked list
	struct Node {
		std::string seq;	// contains the seq
		Node* next;			// pointer to next Node, nullptr if none
		std::vector<int> spots;		// vector containing all indices of seq

		Node() {
			seq = "";
			next = nullptr;
		}
		Node(std::string s, int index) {
			this->seq = s;
			this->spots.push_back(index);
			next = nullptr;
		}
	};

	int m_numBuckets;
	int m_numItems;
	Node** m_buckets;

	unsigned int myHash(const std::string seq) const;
};

#endif // !TABLE_H
