#include <iostream>
#include <time.h>
#include <algorithm>

#include "CSVparser.hpp"

using namespace std;

// Forward declarations
double strToDouble(string str, char ch);

// Structure to hold bid information
struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};

// Structure for tree
struct Node {
	Bid bid;
	Node* left;
	Node* right;

	Node(){
		left = nullptr;
		right = nullptr;
	}
	Node(Bid aBid) : Node(){
		this->bid = aBid;
	}
};

// Defines class to hold data
class BinarySearchTree {

private:
    Node* root;

    void addNode(Node* node, Bid bid);
    void inOrder(Node* node);
    Node* removeNode(Node* node, string bidId);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void InOrder();
    void Insert(Bid bid);
    void Remove(string bidId);
    Bid Search(string bidId);
};

// Initialize variables
BinarySearchTree::BinarySearchTree() {
	root = nullptr;
}

BinarySearchTree::~BinarySearchTree() {
    // Deletes every node
}

// Goes through the tree in order
void BinarySearchTree::InOrder() {
	this->inOrder(root);
}

// Inserts a bid into the tree
void BinarySearchTree::Insert(Bid bid) {
	if(root == nullptr){
		root = new Node(bid);
	}
	else{
		this->addNode(root, bid);
	}
}

// Removes a bid from the tree
void BinarySearchTree::Remove(string bidId) {
	this->removeNode(root, bidId);
}

// Searches the tree for a bid
Bid BinarySearchTree::Search(string bidId) {
	Node* current = root;
	while(current != nullptr){
		if(current->bid.bidId.compare(bidId) == 0){
			return current->bid;
		}
		if(bidId.compare(current->bid.bidId) < 0){
			current = current->left;
		}
		else{
			current = current->right;
		}
	}

	Bid bid;
    return bid;
}

// Adds a bid to the tree
void BinarySearchTree::addNode(Node* node, Bid bid) {
	if(node->bid.bidId.compare(bid.bidId) > 0){
		if(node->left == nullptr){
			node->left = new Node(bid);
		}
		else{
			this->addNode(node->left, bid);
		}
	}
	else{
		if(node->right == nullptr){
			node->right = new Node(bid);
		}
		else{
			this->addNode(node->right, bid);
		}
	}
}

// Searches through the tree in order
void BinarySearchTree::inOrder(Node* node) {
	if (node != nullptr){
		inOrder(node->left);
		cout << node->bid.bidId << ": "
			 << node->bid.title << " | "
			 << node->bid.amount << " | "
		     << node->bid.fund << endl;
		inOrder(node->right);
	}
}

// Searches for then removes a node from the tree
Node* BinarySearchTree::removeNode(Node* node, string bidId){
	if(node=nullptr){
		return node;
	}
	if (bidId.compare(node->bid.bidId) < 0){
		node->left = removeNode(node->left, bidId);
	}
	else if (bidId.compare(node->bid.bidId) > 0){
		node->right = removeNode(node->right, bidId);
	}
	else{
		if (node->left == nullptr && node->right == nullptr){
			delete node;
			node = nullptr;
		}
		else if (node->left != nullptr && node->right == nullptr){
			Node* temp = node;
			node = node->left;
			delete temp;
		}
		else if (node->right != nullptr && node->left == nullptr){
			Node* temp = node;
			node = node->right;
			delete temp;
		}
		else{
			Node* temp = node->right;
			while (temp->left != nullptr){
				temp = temp->left;
			}
			node->bid = temp->bid;
			node->right = removeNode(node->right, temp->bid.bidId);
		}
	}
	return node;
}

// Displays bid info
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
            << bid.fund << endl;
    return;
}

// Loads and stores a CSV file
void loadBids(string csvPath, BinarySearchTree* bst) {
    cout << "Loading CSV file " << csvPath << endl;

    // Initializes the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // Reads and displays the header
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // Reads the CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Creates a data structure and adds bid info to the collection
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // Bid goes to end of list
            bst->Insert(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

// Converts a string to a double and removes unwanted characters
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

int main(int argc, char* argv[]) {

    // Processes command line arguments
    string csvPath, bidKey;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        bidKey = "98109";
        break;
    case 3:
        csvPath = argv[1];
        bidKey = argv[2];
        break;
    default:
        csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
        bidKey = "98109";
    }

    // Timer variable
    clock_t ticks;

    // Define binary search tree for bids
    BinarySearchTree* bst;

    Bid bid;

	// Displays user options and allows for user to pick an option
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

		// Arguments for above cases
        case 1:
            bst = new BinarySearchTree();

            // Timer variable before loading bids
            ticks = clock();

            // Calls to load bids
            loadBids(csvPath, bst);

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            bst->InOrder();
            break;

        case 3:
            ticks = clock();

            bid = bst->Search(bidKey);

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
            bst->Remove(bidKey);
            break;
        }
    }

    cout << "Good bye." << endl;

	return 0;
}
