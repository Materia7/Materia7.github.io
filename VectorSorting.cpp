#include <algorithm>
#include <iostream>
#include <time.h>

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

// Displays bid info
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
            << bid.fund << endl;
    return;
}

// Asks user for bid info
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

// Loads and holds a CSV file
vector<Bid> loadBids(string csvPath) {
    cout << "Loading CSV file " << csvPath << endl;

    // Defines a vector data structure to hold a collection of bids.
    vector<Bid> bids;

    // Initializes the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    try {
        // Reads the CSV file
        for (int i = 0; i < file.rowCount(); i++) {

            // Creates a data structure and adds bid info to the collection
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // Puts bid at the end of the list
            bids.push_back(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
    return bids;
}

// Implements a quick sort logic over bid.title
int partition(vector<Bid>& bids, int begin, int end) {
	int low = begin;
	int high = end;

	int pivot = begin + (end - begin) /2;
	bool done = false;
	while (!done){

		while (bids.at(low).title.compare(bids.at(pivot).title) < 0){
			++low;
		}

		while (bids.at(pivot).title.compare(bids.at(high).title) < 0){
			--high;
		}

		if(low >= high){
			done = true;
		}
		else{
			swap(bids.at(low), bids.at(high));


		++low;
		--high;
		}
	}
	return high;
}

// Implements a quick sort logic on bid.title
void quickSort(vector<Bid>& bids, int begin, int end) {
	int mid = 0;

	if (begin >= end){
		return;
	}

	mid = partition(bids, begin, end);
	quickSort(bids, begin, mid);
	quickSort(bids, mid + 1, end);
}

// Preforms a selection sort logic on bid.title
void selectionSort(vector<Bid>& bids) {
	int min;

	for (unsigned pos = 0; pos < bids.size(); ++pos){
		min = pos;

		for (unsigned j = pos + 1; j < bids.size(); ++j){
			if (bids.at(j).title.compare(bids.at(min).title) < 0){
				min = j;
			}
		}
		if (min != pos){
			swap(bids.at(pos), bids.at(min));
		}
	}
}

// Converts a string to a double and removes unwanted characters
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

int main(int argc, char* argv[]) {

    // Process command line arguments
    string csvPath;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        break;
    default:
        csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
    }

    // Defines a vector to hold the bids
    vector<Bid> bids;

    // Timer variable
    clock_t ticks;

	//Displays user choices and asks for a choice
    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Selection Sort All Bids" << endl;
        cout << "  4. Quick Sort All Bids" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

		// Command line arguments
        case 1:
            // Initialize a timer variable before loading bids
            ticks = clock();

            // Calls to load the bids
            bids = loadBids(csvPath);

            cout << bids.size() << " bids read" << endl;

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 2:
            // Loop and display the bids
            for (int i = 0; i < bids.size(); ++i) {
                displayBid(bids[i]);
            }
            cout << endl;

            break;

        // Initializes selection sort and report timing results
        case 3:
            // Initialize a timer variable before loading bids
            ticks = clock();

            // Calls and loads the bids
            selectionSort(bids);

            cout << bids.size() << " bids read" << endl;

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;
        // Initalizes the quick sort and reports timing results
        case 4:
            // Timer variable before loading bids
            ticks = clock();

            // Calls and loads the bids
            quickSort(bids, 0, bids.size() - 1);

            cout << bids.size() << " bids read" << endl;

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
