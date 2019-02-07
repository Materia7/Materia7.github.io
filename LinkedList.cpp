#include <algorithm>
#include <iostream>
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

// forward declarations
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

// Class to hold data
class LinkedList {

private:
    // Structure for list entries
	struct Node {
		Bid bid;
		Node* next;

		Node() {
			next = nullptr;
		}

		Node(Bid aBid) {
			bid = aBid;
			next = nullptr;
		}
	};

	Node* head;
	Node* tail;
	int size = 0;

public:
    LinkedList();
    virtual ~LinkedList();
    void Append(Bid bid);
    void Prepend(Bid bid);
    void PrintList();
    void Remove(string bidId);
    Bid Search(string bidId);
    int Size();
};

LinkedList::LinkedList() {
    //Initializes list variables
	head = tail = nullptr;
}

LinkedList::~LinkedList() {
}

// Adds new bid to end of bid list
void LinkedList::Append(Bid bid) {
	Node* node = new Node(bid);

	if (head == nullptr) {
		head = node;
	}
	else {
		if (tail != nullptr) {
			tail->next = node;
		}
	}
	tail = node;

	size++;
}

// Add a bid to the start of the list
void LinkedList::Prepend(Bid bid) {
	Node* node = new Node(bid);

	if (head != nullptr) {
		node->next = head;
	}

	head = node;

	size++;
}

// Prints bid info
void LinkedList::PrintList() {
	Node* current = head;

	while (current != nullptr) {
		cout << current->bid.bidId << ": " << current->bid.title << " | " << current->bid.amount << " | " << current->bid.fund << endl;
		current = current->next:
	}
}

// Removes a chosen bid
void LinkedList::Remove(string bidId) {
	if (head != nullptr) {
		if (head->bid.bidId.compare(bidId) == 0) {
			Node* tempNode = head->next;
			delete head;
			head = tempNode;
		}
	}

	Node* current = head;
	
	// Searches for the correct bid to delete
	while (current->next != nullptr) {
		if (current->next->bid.bidId.compare(bidId) == 0) {
			Node* tempNode = current->next;
			current->next = tempNode->next;
			delete tempNode;

			size--;

			return;
		}
			current = current->next:
	}
}

// Searches for a bid
Bid LinkedList::Search(string bidId) {
	Node* current = head;

	while (current != nullptr) {
		if (current->bid.bidId.compare(bidId) == 0) {
			return current->bid;
		}
			current = current->next:
	}
}

// Returns the number of bids in the list
int LinkedList::Size() {
    return size;
}

// Displays bid info
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount
         << " | " << bid.fund << endl;
    return;
}

// Propts user to input bid info
Bid getBid() {
    Bid bid;

    cout << "Enter Id: ";
    cin.ignore();
    getline(cin, bid.bidId);

    cout << "Enter title: ";
    getline(cin, bid.title);

    cout << "Enter fund: ";
    cin >> bid.fund;

    cout << "Enter amount: ";
    cin.ignore();
    string strAmount;
    getline(cin, strAmount);
    bid.amount = strToDouble(strAmount, '$');

    return bid;
}

// Load and store a CSV file
void loadBids(string csvPath, LinkedList *list) {
    cout << "Loading CSV file " << csvPath << endl;

    // Initialize the CSV Parser
    csv::Parser file = csv::Parser(csvPath);

    try {
        // Reads a CSV file
        for (int i = 0; i < file.rowCount(); i++) {

            // Initializes a bid
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << bid.bidId << ": " << bid.title << " | " << bid.fund << " | " << bid.amount << endl;

            // Adds bid to the end
            list->Append(bid);
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

    clock_t ticks;

    LinkedList bidList;

    Bid bid;

	// Displays user choices and reads a choice
    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Enter a Bid" << endl;
        cout << "  2. Load Bids" << endl;
        cout << "  3. Display All Bids" << endl;
        cout << "  4. Find Bid" << endl;
        cout << "  5. Remove Bid" << endl;
        cout << "  6. Prepend a bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

		// Command line arguments
        switch (choice) {
        case 1:
            bid = getBid();
            bidList.Append(bid);
            displayBid(bid);

            break;

        case 2:
            ticks = clock();

            loadBids(csvPath, &bidList);

            cout << bidList.Size() << " bids read" << endl;

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " milliseconds" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 3:
            bidList.PrintList();

            break;

        case 4:
            ticks = clock();

            bid = bidList.Search(bidKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
            	cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 5:
            bidList.Remove(bidKey);

            break;

        case 6:
        	 bid = getBid();
        	 bidList.Prepend(bid);
        	 displayBid(bid);

        	 break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
