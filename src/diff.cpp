#include "table.h"
#include <functional>
#include <string>
#include <iostream>
#include<fstream>

using namespace std;

// First method I created to solve makeDiff
// Stores, byteSize chunks in hashtable
// Iterates through the new file and checks for continuous chunks and makes copy statement to diff
// If chunk is not found, make add statement to diff until the next chunk is found
void method1(const Table& t, string newSeq, int byteSize, ostream& fdiff) {
	int index = 0;
	// iterate through newSeq
	while (index < newSeq.size()) {
		// check first byte
		if (t.checkInTable(newSeq.substr(index, byteSize))) {
			int maxCopy = 1;
			string currSeq = newSeq.substr(index, byteSize);
			vector<int> positions = t.getIndices(currSeq);
			int startingIndex = positions[0];
			int nextIndex = index + byteSize;
			if (nextIndex < newSeq.size()) {
				string nextSeq = newSeq.substr(nextIndex, byteSize);
				// for all indices of first byte, check if the next byte is adjacent
				for (int i = 0; i < positions.size(); i++) {
					int tempLong = 1;
					int checkIndex = positions[i] + byteSize;
					bool repeat = t.atIndex(nextSeq, checkIndex);
					nextIndex += byteSize;
					checkIndex += byteSize;
					// while there is an adjacent byte, keep iterating
					while (repeat && nextIndex < newSeq.size()) {
						tempLong++;
						nextSeq = newSeq.substr(nextIndex, byteSize);
						repeat = t.atIndex(nextSeq, checkIndex);
						nextIndex += byteSize;
						checkIndex += byteSize;
					}
					// keep track of the longest copy sequence
					if (tempLong > maxCopy) {
						maxCopy = tempLong;
						startingIndex = positions[i];
					}
				}
			}
			// update diff
			fdiff << ("C" + to_string(maxCopy * byteSize) + "," + to_string(startingIndex));
			index += (maxCopy * byteSize);
		}
		// if the first byte is not in the table
		else {
			int count = 1;
			string addString = newSeq.substr(index, 1);
			index++;
			// iterate while the next byte is not in the table
			while (index < newSeq.size() && !t.checkInTable(newSeq.substr(index, byteSize))) {
				addString += newSeq.substr(index, 1);
				index++;
				count++;
			}
			// update diff
			fdiff << ("A" + to_string(count) + ":" + addString);
		}
	}
}

// Helper function finds the largest copyable path in a given string
// Returns the number of bytes in the path
// StartingIndex set to the first byte in the path's index
int longestPath(const Table t, const string totalSeq, int byteSize, int& startingIndex) {
	if (totalSeq.size() < byteSize) {
		return 0;
	}
	int currIndex = 0;
	string currSeq = totalSeq.substr(currIndex, byteSize);
	vector<int> positions = t.getIndices(currSeq);
	currIndex += byteSize;
	if (currIndex > totalSeq.size()) {
		startingIndex = positions[0];
		return 1;
	}
	string nextSeq = totalSeq.substr(currIndex, byteSize);
	int maxLength = 0;
	// check all indices of the first byte
	for (int i = 0; i < positions.size(); i++) {
		int tempLong = 1;
		int checkIndex = positions[i] + byteSize;
		bool repeat = t.atIndex(nextSeq, checkIndex);
		currIndex += byteSize;
		checkIndex += byteSize;
		// iterate whil the "path" continues
		while (repeat && currIndex < totalSeq.size()) {
			tempLong++;
			nextSeq = totalSeq.substr(currIndex, byteSize);
			repeat = t.atIndex(nextSeq, checkIndex);
			currIndex += byteSize;
			checkIndex += byteSize;
		}
		// keep track of the longest path and startingIndex
		if (tempLong > maxLength) {
			maxLength = tempLong;
			startingIndex = positions[i];
		}
	}
	return maxLength;
}

// Recursive function that returns the string to be added to the diff file
// Finds the longest copyable path in a seq, then does so on the left and right of the path
string makeOutput(const Table& t, const string searchSeq, int byteSize) {
	string myString = "";
	// if the current string is empty, return nothing
	if (searchSeq.size() == 0) {
		return myString;
	}
	// if the current string is not big enough to be a byte, return an add command contaning the entire string
	if (searchSeq.size() < byteSize) {
		myString = "A" + to_string(searchSeq.size()) + ":" + searchSeq;
		return myString;
	}
	int largestCopy = 0;
	int currSeqIndex = 0;
	int oldSeqIndex;
	// iterate though every possible byte in the string at every i
	for (int i = 0; i < searchSeq.size() - byteSize; i++) {
		string firstCheck = searchSeq.substr(i, byteSize);
		if (!t.checkInTable(firstCheck)) {
			continue;
		}
		string checkSeq = searchSeq.substr(i);
		int checkIndex;
		// finds the longest path at the given i
		int checkLongest = longestPath(t, checkSeq, byteSize, checkIndex);
		// keep track of the longest path
		if (checkLongest > largestCopy) {
			largestCopy = checkLongest;
			oldSeqIndex = checkIndex;
			currSeqIndex = i;
		}
	}
	// if no copy is found, return an add command
	if (largestCopy == 0) {
		myString = "A" + to_string(searchSeq.size()) + ":" + searchSeq;
		return myString;
	}
	// otherwise create a copy command for the longest path
	else {
		myString = "C" + to_string(largestCopy*byteSize) + "," + to_string(oldSeqIndex);
		string leftSeq = searchSeq.substr(0, currSeqIndex);
		string rightSeq = searchSeq.substr(currSeqIndex+largestCopy*byteSize);
		// split up the total string into left, right, and longest path
		// recursively call makeOutput on the left and right strings
		return makeOutput(t, leftSeq, byteSize) + myString + makeOutput(t, rightSeq, byteSize);
	}
}

// This is my second attempt at using the hash tables
// Uses recursion to continually find the longest path available in the given string
void method2(const Table& t, const string totalSeq, int byteSize, ostream& fdiff) {
	fdiff << makeOutput(t, totalSeq, byteSize);
}

// Uses string manipulation to find the longest possible substring at each index
void method3(string oldSeq, string newSeq, ostream& fdiff) {
	// iterate through every index of the new string
	for (int newIndex = 0; newIndex < newSeq.size(); newIndex++) {
		int findLength = 0;
		int findIndex;
		// iterate through every length
		for (int length = 1; newIndex + length - 1 < newSeq.size(); length++) {
			string checkString = newSeq.substr(newIndex, length);
			// check if the current substring is in the old string
			if (oldSeq.find(checkString) == -1) {
				// if it is not break the loop and set findLength to the last "good" length
				findLength = length - 1;
				break;
			}
			else {
				findLength = length;
				findIndex = oldSeq.find(checkString);
			}
		}
		// if findLength is 0, then no substring was found
		// make an add command to add as many char in a row that are not in the old string
		if (findLength == 0) {
			int addLength = 1;
			int addIndex = newIndex + 1;
			string addCheck = newSeq.substr(addIndex, 1);
			// iterate until a char that is in old string is found
			while (addIndex < newSeq.size() && oldSeq.find(addCheck) == -1) {
				addLength++;
				addIndex++;
			}
			string s = newSeq.substr(newIndex,addLength);
			fdiff << "A" + to_string(addLength) + ":" + s;
			findLength = addLength;
		}
		// otherwise make a copy command with the largest possible substring
		else {
			fdiff << "C" + to_string(findLength) + "," + to_string(findIndex);
		}
		newIndex += findLength - 1;
	}
}

// Creates a diff file from an old and new file
void createDiff(istream& fold, istream& fnew, ostream& fdiff) {
	if (!fold || !fnew || !fdiff) {
		cerr << "Error: Cannot open files" << endl;
		return;
	}
	char c;
	string oldSeq;
	string newSeq;
	// read in the old file into a string
	while (fold.get(c)) {
		 oldSeq+= c;
	}
	// read in the new file into a string
	while (fnew.get(c)) {
		newSeq += c;
	}
	const int oldSize = oldSeq.size();
	int byteSize = 0;
	// choose the byteSize and method based on the old file's size
	if (oldSize < 200) {
		byteSize = 4;
		Table t(byteSize, oldSize);
		int index = 0;
		while (index < oldSize - byteSize) {
			t.add(oldSeq.substr(index, byteSize), index);
			index += byteSize;
		}
		method2(t, newSeq, byteSize, fdiff);
	}
	else {
		byteSize = 16;
		Table t(byteSize, oldSize);
		int index = 0;
		while (index < oldSize - byteSize) {
			t.add(oldSeq.substr(index, byteSize), index);
			index += byteSize;
		}
		method3(oldSeq, newSeq, fdiff);
	}
}

bool getInt(istream& inf, int& n)
{
	char ch;
	if (!inf.get(ch) || !isascii(ch) || !isdigit(ch))
		return false;
	inf.unget();
	inf >> n;
	return true;
}

bool getCommand(istream& inf, char& cmd, int& length, int& offset)
{
	if (!inf.get(cmd))
	{
		cmd = 'x';  // signals end of file
		return true;
	}
	char ch;
	switch (cmd)
	{
	case 'A':
		offset = -1;
		return getInt(inf, length) && inf.get(ch) && ch == ':';
	case 'C':
		return getInt(inf, length) && inf.get(ch) && ch == ',' && getInt(inf, offset);
	case '\r':
	case '\n':
		return true;
	}
	return false;
}

// Returns true if a valid diff file is input, false otherwise
// Creates a new file based off the old file and the diff file
bool applyDiff(istream&fold, istream& fdiff, ostream& fnew) {
	if (!fold || !fnew || !fdiff) {
		cerr << "Error: Cannot open files" << endl;
		return false;
	}
	char input;
	string oldSeq;
	// read in the old file
	while (fold.get(input)) {
		oldSeq += input;
	}
	string newString = "";
	char c;
	int length = 0;
	int offset = -1;
	// iterate through the diff file
	while (getCommand(fdiff, c, length, offset) && c != 'x') {
		if (length == 0) {
			return false;
		}
		// if offset is -1, this means it is an add command
		if (offset == -1) {
			int tempLength = 0;
			while (tempLength <length && fdiff.get(c)) {
				newString += c;
				tempLength++;
			}
		}
		else {
			newString += oldSeq.substr(offset, length);
		}
	}
	fnew << newString;
	return true;
}

int main() {
	string before_file = "old_code.txt";
	string after_file = "current_code.txt";
	string txt1 = "../Input Text Files/" + before_file;
    string txt2 = "../Input Text Files/" + after_file;

	ifstream fold(txt1);
	ifstream fnew(txt2);
	ofstream fdiff("../Output Text Files/diff.txt");
	ofstream foutput("../Output Text Files/newTest.txt");

	createDiff(fold, fnew, fdiff);
	fold.close();
	fnew.close();
	fdiff.close();

	ifstream fNewOld(txt1);
	ifstream fNewDiff("../Output Text Files/diff.txt");

	applyDiff(fNewOld, fNewDiff, foutput);
	fNewOld.close();
	fNewDiff.close();
	foutput.close();
	system("pause");
}