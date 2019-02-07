#include <algorithm>
#include <climits>
#include <iostream>
#include <string>
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

const unsigned int DEFAULT_SIZE = 179;

// Forward declarations
double strToDouble(string str, char ch);

// Bid information holding
struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};

// Structure to hold bids
class HashTable {

private:
	struct Node {
		Bid bid;
		unsigned key;
		Node* next;

		Node(){
			key = UINT_MAX;
			next = nullptr;
		}
		Node(Bid aBid) : Node(){
			bid = aBid;
		}
		Node(Bid aBid, unsigned aKey) : Node(aBid){
			key = aKey;
		}
	};

	vector<Node> nodes;
	unsigned tableSize = DEFAULT_SIZE;
    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned size);
    virtual ~HashTable();
    void Insert(Bid bid);
    void PrintAll();
    void Remove(string bidId);
    Bid Search(string bidId);
};

// Initializes bid structure
HashTable::HashTable() {
	nodes.resize(tableSize);
}
HashTable::HashTable(unsigned size){
	this->tableSize = size;
	nodes.resize(tableSize);
}

// Removes bid info
HashTable::~HashTable() {
	nodes.erase(nodes.begin());
}

// Calculates hash value
unsigned int HashTable::hash(int key) {
	return key % tableSize;
}

// Logic for inserting a bid
void HashTable::Insert(Bid bid) {
	unsigned key = hash(atoi(bid.bidId.c_str()));
	Node* oldNode = &(nodes.at(key));

	if (oldNode == nullptr){
		Node* newNode = new Node(bid, key);
		nodes.insert(nodes.begin() + key, (*newNode));
	}
	else {
		if (oldNode->key == UINT_MAX){
			oldNode->key = key;
			oldNode->bid = bid;
			oldNode->next = nullptr;
		}
		else {
			while (oldNode->next != nullptr){
				oldNode = oldNode->next;
			}
			oldNode->next = new Node(bid, key);
		}
	}
}

// Print all bids
void HashTable::PrintAll() {
    cout << bid.bidID << " | " << bid.title << " | " << bid.amount << " | " << bid.fund << endl;
    return;
}

// Removes bid info
void HashTable::Remove(string bidId) {
	unsigned key = hash(atoi(bidId.c_str()));
	Node* node = &(nodes.at(key));
	if (node->bid.bidId.compare(bidId) == 0 && node->next == nullptr){
		nodes.erase(nodes.begin() + key);
		return;
	}
	if (node->bid.bidId.compare(bidId) == 0 && node->next != nullptr){
		Node* tempNode = node->next;
		Node* tempNode = new Node(node->next->bid, node->next->key, node->next->next);
		nodes.erase(nodes.begin() + key);
		nodes.insert(nodes.begin() + key, (*tempNode));
		return;
	}
	Node* prevNode = node;
	node = node->next;
	while(node != nullptr){
		if (node->bid.bidId.compare(bidId) == 0){
			prevNode->next = node->next;
			delete node;
			node = nullptr;
			return;
		}
		prevNode = node;
		node = node->next;
	}
}

// Searches then returns bid info
Bid HashTable::Search(string bidId) {
    Bid bid;

    unsigned key = hash(atoi(bidID.c_str()));
    Node* node = &(nodes.at(key));
    if (node == nullptr || node->key == UINT_MAX){
    	return bid;
    }
    if (node != nullptr && node->key != UINT_MAX && node-> bid.bidId.compare(bidId) == 0){
    	return node->bid;
    }
    while (node != nullptr){
    	if (node->key != UINT_MAX && node->bid.bidId.compare(bidId) == 0){
    		return node->bid;
    	}
    	node = node->next;
    }

    return bid;
}

// Displays bid info
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
            << bid.fund << endl;
    return;
}

// Load and store CSV files
void loadBids(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    // Initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // Read and display header
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // Loop to read CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Data structure to add bids to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push bid to the end
            hashTable->Insert(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

// Coverts a string to a double and strips unwanted characters
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

int main(int argc, char* argv[]) {

    // Processes command line arguments
    string csvPath, searchValue;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        searchValue = "98109";
        break;
    case 3:
        csvPath = argv[1];
        searchValue = argv[2];
        break;
    default:
        csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
        searchValue = "98109";
    }

    // Timer variable
    clock_t ticks;

    // Hash table to hold all the bid info
    HashTable* bidTable;

    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Find Bid" << endl;
        cout << "  4. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            bidTable = new HashTable();

            // Initializes timer before loading bids
            ticks = clock();

            // Loads bids
            loadBids(csvPath, bidTable);

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

		// Arguments for different options
        case 2:
            bidTable->PrintAll();
            break;

        case 3:
            ticks = clock();

            bid = bidTable->Search(searchValue);

            ticks = clock() - ticks;

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
                cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 4:
            bidTable->Remove(bidKey);
            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
