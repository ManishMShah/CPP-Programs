//This program stores the data permanently in CSV format
#include <iostream>   // For input/output operations (cin, cout)
#include <vector>     // For std::vector to store multiple Library objects
#include <string>     // For std::string to handle text data (book names, author names)
#include <limits>     // For std::numeric_limits to clear input buffer
#include <algorithm>  // For std::remove_if and std::find_if (useful for delete/modify)
#include <fstream>    // For file stream operations (ifstream, ofstream)
#include <sstream>    // For std::stringstream to parse lines from file
// No longer explicitly using <random> or <chrono> for this specific auto-increment,
// but they are fine to keep if other random functionality is envisioned.

// Define the name of the file where library data will be stored (changed to .csv)
const std::string FILENAME = "LibraryManagement.csv";

// Define the Library struct to hold book details
struct Library
{
    int serialNumber; // Automatically assigned unique ID
    int bookCode;     // User-entered unique ID
    std::string bookName;
    std::string authorName;
    int cost;
    int qty;
    long int totalCost;

    // Default constructor to easily initialize a book
    Library() : serialNumber(0), bookCode(0), cost(0), qty(0), totalCost(0) {}
};

// Function to get book details (including bookCode) from user input
void getUserInputForBookDetailsAndCode(Library &book, const std::vector<Library>& existingBooks)
{
    std::cout << "Book Name: ";
    std::getline(std::cin, book.bookName);

    std::cout << "Author Name: ";
    std::getline(std::cin, book.authorName);

    std::cout << "Book Code: ";
    // Input validation for bookCode and check for uniqueness
    while (true) {
        if (!(std::cin >> book.bookCode)) {
            std::cout << "Invalid input. Please enter a number for Book Code: ";
            std::cin.clear(); // Clear error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
        } else {
            // Check for uniqueness of user-entered bookCode
            bool isDuplicate = false;
            for (const auto& existingBook : existingBooks) {
                // When modifying, the current book's bookCode might be the same, so exclude it from duplicate check
                // This check is primarily for adding new books.
                if (existingBook.serialNumber != book.serialNumber && existingBook.bookCode == book.bookCode) {
                    isDuplicate = true;
                    break;
                }
            }
            if (isDuplicate) {
                std::cout << "Error: Book Code " << book.bookCode << " already exists. Please enter a unique Book Code: ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer for next input
            } else {
                break; // Valid and unique bookCode entered
            }
        }
    }
    // Clear buffer after integer input, essential before next getline (if any)
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


    std::cout << "Price per book: ";
    // Input validation for cost
    while (!(std::cin >> book.cost)) {
        std::cout << "Invalid input. Please enter a number for Price: ";
        std::cin.clear(); // Clear error flags
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
    }
    // Clear buffer after integer input, essential before next getline (if any)
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Number of books purchased: ";
    // Input validation for quantity
    while (!(std::cin >> book.qty)) {
        std::cout << "Invalid input. Please enter a number for Quantity: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    // Clear buffer after integer input, essential before next getline (if any)
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    book.totalCost = static_cast<long int>(book.cost) * book.qty; // Cast to long int to prevent overflow
    std::cout << "Calculated Total Price: " << book.totalCost << std::endl;
}


// Function to display data for a SINGLE Library object
// Added an optional displaySerialNum parameter for the list order
void displaySingleBookData(const Library &book, int displaySerialNum = 0)
{
    std::cout << "\n----Book Details----" << std::endl;
    if (displaySerialNum > 0) { // Only display serial number if provided and valid (for list order)
        std::cout << "List Serial No: " << displaySerialNum << std::endl;
    }
    std::cout << "Internal Serial No: " << book.serialNumber << std::endl; // The auto-generated unique ID
    std::cout << "Book Code: " << book.bookCode << std::endl; // The user-entered ID
    std::cout << "Book Name: " << book.bookName << std::endl;
    std::cout << "Author Name: " << book.authorName << std::endl;
    std::cout << "Book Cost: " << book.cost << std::endl;
    std::cout << "Books Purchased: " << book.qty << std::endl;
    std::cout << "Total Price: " << book.totalCost << std::endl;
    std::cout << "--------------------" << std::endl;
}

// Function to save the entire collection to the file (overwrites existing file)
void saveBooksToFile(const std::vector<Library> &collection, const std::string &filename)
{
    std::ofstream outFile(filename); // Open file in truncation mode (overwrites)
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    // Write CSV header
    outFile << "Internal Serial No,Book Code,Book Name,Author Name,Cost,Qty,Total Cost" << std::endl;

    for (const auto &book : collection) {
        outFile << book.serialNumber << "," // Write auto-generated serialNumber
                << book.bookCode << ","     // Write user-entered bookCode
                << book.bookName << ","
                << book.authorName << ","
                << book.cost << ","
                << book.qty << ","
                << book.totalCost << std::endl;
    }
    outFile.close();
}

// Function to append a single book to the file
void appendBookToFile(const Library &book, const std::string &filename)
{
    // Open file in append mode (std::ios::app)
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for appending." << std::endl;
        return;
    }

    // Check if the file is empty (newly created or truncated) to write the header
    // This is a simple check; a more robust solution might check file size or content.
    outFile.seekp(0, std::ios::end); // Move to end of file
    if (outFile.tellp() == 0) { // If current position is 0, file was empty
        outFile << "Internal Serial No,Book Code,Book Name,Author Name,Cost,Qty,Total Cost" << std::endl;
    }

    outFile << book.serialNumber << "," // Write auto-generated serialNumber
            << book.bookCode << ","     // Write user-entered bookCode
            << book.bookName << ","
            << book.authorName << ","
            << book.cost << ","
            << book.qty << ","
            << book.totalCost << std::endl;
    outFile.close();
}

// Function to load books from the file into the collection
void loadBooksFromFile(std::vector<Library> &collection, const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        // File might not exist yet, which is fine for the first run
        std::cout << "No existing library data found. Starting with an empty library." << std::endl;
        return;
    }

    collection.clear(); // Clear existing data in case of multiple loads (though not typical here)
    std::string line;

    // Skip the header line
    std::getline(inFile, line);
    if (inFile.eof() && line.empty()) { // Handle case where file is empty except for header
        std::cout << "Library file is empty (contains only header)." << std::endl;
        return;
    }


    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        std::string segment;
        Library book;

        // Read serialNumber
        std::getline(ss, segment, ','); // Changed delimiter to comma
        try {
            book.serialNumber = std::stoi(segment);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error parsing serialNumber: '" << segment << "' - " << e.what() << std::endl;
            continue; // Skip this line and try next
        } catch (const std::out_of_range& e) {
            std::cerr << "Error parsing serialNumber (out of range): '" << segment << "' - " << e.what() << std::endl;
            continue;
        }

        // Read bookCode
        std::getline(ss, segment, ','); // Changed delimiter to comma
        try {
            book.bookCode = std::stoi(segment);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error parsing bookCode: '" << segment << "' - " << e.what() << std::endl;
            continue;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error parsing bookCode (out of range): '" << segment << "' - " << e.what() << std::endl;
            continue;
        }

        // Read bookName
        std::getline(ss, book.bookName, ','); // Changed delimiter to comma

        // Read authorName
        std::getline(ss, book.authorName, ','); // Changed delimiter to comma

        // Read cost
        std::getline(ss, segment, ','); // Changed delimiter to comma
        try {
            book.cost = std::stoi(segment);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error parsing cost: '" << segment << "' - " << e.what() << std::endl;
            continue;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error parsing cost (out of range): '" << segment << "' -  " << e.what() << std::endl;
            continue;
        }

        // Read qty
        std::getline(ss, segment, ','); // Changed delimiter to comma
        try {
            book.qty = std::stoi(segment);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error parsing qty: '" << segment << "' - " << e.what() << std::endl;
            continue;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error parsing qty (out of range): '" << segment << "' - " << e.what() << std::endl;
            continue;
        }

        // Read totalCost (last segment, no delimiter needed)
        std::getline(ss, segment); // Read till end of line
        try {
            book.totalCost = std::stoll(segment); // Use stoll for long long
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error parsing totalCost: '" << segment << "' - " << e.what() << std::endl;
            continue;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error parsing totalCost (out of range): '" << segment << "' - " << e.what() << std::endl;
            continue;
        }

        collection.push_back(book);
    }
    inFile.close();
    std::cout << "Library data loaded successfully from " << filename << std::endl;
}

// Global variable to keep track of the next available serial number for automatic generation
int nextAutoSerialNumber = 1;

// Function to add a new book to the library collection (automatic serialNumber, user input for rest)
void addBook(std::vector<Library> &collection)
{
    Library newBook; // Create a new Library object

    // Assign an automatic, unique serial number
    newBook.serialNumber = nextAutoSerialNumber++;
    std::cout << "\n--- Adding New Book ---" << std::endl;
    std::cout << "Assigned Internal Serial Number: " << newBook.serialNumber << std::endl;

    // Get the rest of the book details (including user-entered bookCode) from the user
    getUserInputForBookDetailsAndCode(newBook, collection); // Pass collection for duplicate bookCode check

    // No need for duplicate check on newBook.serialNumber here, as it's auto-incremented.
    // The duplicate check for newBook.bookCode is handled inside getUserInputForBookDetailsAndCode.

    collection.push_back(newBook); // Add the new book to the vector
    appendBookToFile(newBook, FILENAME); // Append only the new book to the file
    std::cout << "\nBook added successfully with Internal Serial Number: " << newBook.serialNumber << std::endl;
}

// Function to delete a book from the library collection by book code
void deleteBook(std::vector<Library> &collection)
{
    if (collection.empty()) {
        std::cout << "\nLibrary is empty. No books to delete." << std::endl;
        return;
    }

    int codeToDelete;
    std::cout << "\nEnter Book Code of the book to delete: ";
    while (!(std::cin >> codeToDelete)) {
        std::cout << "Invalid input. Please enter a number for Book Code: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    // Use std::remove_if to logically remove elements, then erase to physically remove
    // Search by bookCode (user-entered)
    auto it = std::remove_if(collection.begin(), collection.end(),
                             [codeToDelete](const Library &book) {
                                 return book.bookCode == codeToDelete;
                             });

    if (it != collection.end()) {
        collection.erase(it, collection.end());
        std::cout << "\nBook with Code " << codeToDelete << " deleted successfully!" << std::endl;
        saveBooksToFile(collection, FILENAME); // Rewrite the entire file after deletion
    } else {
        std::cout << "\nBook with Code " << codeToDelete << " not found." << std::endl;
    }
}

// Function to modify an existing book's details by book code
void modifyBook(std::vector<Library> &collection)
{
    if (collection.empty()) {
        std::cout << "\nLibrary is empty. No books to modify." << std::endl;
        return;
    }

    int codeToModify;
    std::cout << "\nEnter Book Code of the book to modify: ";
    while (!(std::cin >> codeToModify)) {
        std::cout << "Invalid input. Please enter a number for Book Code: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    // Find the book by book code
    auto it = std::find_if(collection.begin(), collection.end(),
                           [codeToModify](const Library &book) {
                               return book.bookCode == codeToModify;
                           });

    if (it != collection.end()) {
        std::cout << "\nBook found. Enter new details for Book Code " << codeToModify << ":" << std::endl;
        // Store the original serial number, as it should not change during modification
        int originalSerialNumber = it->serialNumber;
        // Get new data for the found book, overwriting its current details (excluding serialNumber)
        // Pass the entire collection for duplicate bookCode check, but exclude the book being modified itself.
        std::vector<Library> tempCollection = collection;
        tempCollection.erase(std::remove_if(tempCollection.begin(), tempCollection.end(),
                                            [&](const Library& b){ return b.serialNumber == originalSerialNumber; }),
                              tempCollection.end());

        getUserInputForBookDetailsAndCode(*it, tempCollection); // Pass collection excluding current book
        it->serialNumber = originalSerialNumber; // Restore original serial number

        std::cout << "\nBook with Code " << codeToModify << " modified successfully!" << std::endl;
        saveBooksToFile(collection, FILENAME); // Rewrite the entire file after modification
    } else {
        std::cout << "\nBook with Code " << codeToModify << " not found." << std::endl;
    }
}

// Function to search for books by author name
void searchByAuthor(const std::vector<Library> &collection)
{
    if (collection.empty()) {
        std::cout << "\nLibrary is empty. No books to search." << std::endl;
        return;
    }

    std::string searchAuthorName;
    std::cout << "\nEnter Author Name to search: ";
    std::getline(std::cin, searchAuthorName);

    bool foundBooks = false;
    std::cout << "\n--- Search Results for Author: " << searchAuthorName << " ---" << std::endl;
    int serialCounter = 1; // Initialize serial counter for search results
    for (const auto &book : collection) {
        // Case-sensitive comparison for simplicity. For case-insensitive, convert both to lowercase.
        if (book.authorName == searchAuthorName) {
            displaySingleBookData(book, serialCounter++); // Pass display serial number
            foundBooks = true;
        }
    }

    if (!foundBooks) {
        std::cout << "No books are available for this author: " << searchAuthorName << std::endl;
    }
    std::cout << "-----------------------------------------------" << std::endl;
}

// Function to display the main menu options
void displayMenu() {
    std::cout << "\n--- Library Management System ---" << std::endl;
    std::cout << "1. Add New Book" << std::endl;
    std::cout << "2. Delete Book" << std::endl;
    std::cout << "3. Modify Book Details" << std::endl;
    std::cout << "4. Search Books by Author" << std::endl;
    std::cout << "5. Display All Books" << std::endl;
    std::cout << "6. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

int main()
{
    std::vector<Library> libraryCollection; // Vector to store all books in RAM
    int choice;

    // Load existing data from file when the program starts
    loadBooksFromFile(libraryCollection, FILENAME);

    // After loading, ensure nextAutoSerialNumber is higher than any existing serialNumber
    // If no books are loaded, nextAutoSerialNumber remains 1.
    if (!libraryCollection.empty()) {
        int maxSerialNumber = 0;
        for (const auto& book : libraryCollection) {
            if (book.serialNumber > maxSerialNumber) {
                maxSerialNumber = book.serialNumber;
            }
        }
        nextAutoSerialNumber = maxSerialNumber + 1;
    }


    do {
        displayMenu(); // Show the menu
        // Input validation for menu choice
        while (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number (1-6): ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer after integer input

        switch (choice) {
            case 1:
                addBook(libraryCollection);
                break;
            case 2:
                deleteBook(libraryCollection);
                break;
            case 3:
                modifyBook(libraryCollection);
                break;
            case 4:
                searchByAuthor(libraryCollection);
                break;
            case 5:
                if (libraryCollection.empty()) {
                    std::cout << "\nLibrary is currently empty." << std::endl;
                } else {
                    std::cout << "\n--- All Books in Library ---" << std::endl;
                    // Use a loop with an index to provide serial numbers for display order
                    for (size_t i = 0; i < libraryCollection.size(); ++i) {
                        displaySingleBookData(libraryCollection[i], static_cast<int>(i + 1));
                    }
                }
                break;
            case 6:
                std::cout << "\nExiting Library Management System. Goodbye!" << std::endl;
                break;
            default:
                std::cout << "\nInvalid choice. Please enter a number between 1 and 6." << std::endl;
                break;
        }
        std::cout << "\nPress Enter to continue..." << std::endl;
        std::cin.get(); // Wait for user to press Enter before displaying menu again

    } while (choice != 6); // Loop until user chooses to exit

    return 0;
}
