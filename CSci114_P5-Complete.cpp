#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

// Node class for representing memory blocks or allocations
class Node {
public:
    int data_size;  // Size of the block
    int id;         // Block ID or Request ID
    Node* next;     // Pointer to the next node

    Node() : data_size(0), id(-1), next(NULL) {}
    Node(int size, int id) : data_size(size), id(id), next(NULL) {}
};

// Class for managing free memory blocks
class FreeList {
    Node* head; // Head pointer of the free list

public:
    FreeList() : head(NULL) {}

    // Insert a new free block into the list
    void insertNode(int size, int id) {
        Node* newNode = new Node(size, id);
        if (!head || head->data_size > size) {
            newNode->next = head;
            head = newNode;
            return;
        }
        Node* temp = head;
        while (temp->next && temp->next->data_size <= size) {
            temp = temp->next;
        }
        newNode->next = temp->next;
        temp->next = newNode;
        mergeBlocks();
    }

    // Merge contiguous free blocks
    void mergeBlocks() {
        if (!head || !head->next) return;
        Node* current = head;
        while (current && current->next) {
            if (current->id + current->data_size == current->next->id) {
                current->data_size += current->next->data_size;
                Node* temp = current->next;
                current->next = current->next->next;
                delete temp;
            } else {
                current = current->next;
            }
        }
    }

    // Find the best fit block for allocation
    Node* findBestFit(int size) {
        Node* temp = head;
        Node* bestFit = NULL;
        while (temp) {
            if (temp->data_size >= size) {
                if (!bestFit || temp->data_size < bestFit->data_size) {
                    bestFit = temp;
                }
            }
            temp = temp->next;
        }
        return bestFit;
    }

    // Update the size of a block after allocation
    void updateNodeSize(Node* node, int size) {
        if (node->data_size > size) {
            node->data_size -= size;
        } else {
            deleteNode(node->id);
        }
    }

    // Delete a block with a given ID
    void deleteNode(int id) {
        if (!head) return;
        Node* temp = head;
        Node* prev = NULL;
        while (temp && temp->id != id) {
            prev = temp;
            temp = temp->next;
        }
        if (!temp) return;
        if (!prev) {
            head = temp->next;
        } else {
            prev->next = temp->next;
        }
        delete temp;
    }

    // Return a block to the free list and merge if necessary
    void returnNodeSize(int blockId, int size) {
        Node* temp = head;
        Node* prev = NULL;
        while (temp && temp->id < blockId) {
            prev = temp;
            temp = temp->next;
        }
        if (temp && temp->id == blockId) {
            temp->data_size += size;
            mergeBlocks();
            return;
        }
        Node* newNode = new Node(size, blockId);
        if (prev) {
            prev->next = newNode;
        } else {
            head = newNode;
        }
        newNode->next = temp;
        mergeBlocks();
    }

    // Get the head of the free list
    Node* getHead() const {
        return head;
    }

    // Print the free list
    void printList() const {
        Node* temp = head;
        while (temp) {
            cout << "Block ID: " << temp->id << ", Size: " << temp->data_size << endl;
            temp = temp->next;
        }
    }
};

// Class for managing allocated blocks
class AllocationList {
    Node* head; // Head pointer of the allocation list

public:
    AllocationList() : head(NULL) {}

    // Insert a new allocation into the list
    void insertNode(int size, int blockId, int requestId) {
        Node* newNode = new Node(size, requestId);
        newNode->id = blockId;
        if (!head) {
            head = newNode;
            return;
        }
        Node* temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
    }

    // Delete an allocation with a given request ID
    void deleteNode(int requestId) {
        if (!head) return;
        Node* temp = head;
        Node* prev = NULL;
        while (temp && temp->id != requestId) {
            prev = temp;
            temp = temp->next;
        }
        if (!temp) return;
        if (!prev) {
            head = temp->next;
        } else {
            prev->next = temp->next;
        }
        delete temp;
    }

    // Find an allocation by request ID
    Node* findNode(int requestId) const {
        Node* temp = head;
        while (temp && temp->id != requestId) {
            temp = temp->next;
        }
        return temp;
    }

    // Print the allocation list
    void printList() const {
        Node* temp = head;
        while (temp) {
            cout << "Request ID: " << temp->id << ", Block ID: " << temp->id << ", Size: " << temp->data_size << endl;
            temp = temp->next;
        }
    }
};

int main() {
    FreeList freeList;
    AllocationList allocatedList;

    // Initialize the free list with blocks of size 1024 bytes
    for (int i = 1; i <= 1024; ++i) {
        freeList.insertNode(1024, i);
    }

    ifstream file("requests-1.txt");
    if (!file) {
        cerr << "Unable to open file" << endl;
        return 1;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        char type;
        int id, size;
        iss >> type >> id;
        if (type == 'A') {
            iss >> size;
            Node* bestFitNode = freeList.findBestFit(size);
            if (!bestFitNode) {
                cout << "Allocation of " << size << " bytes failed for request " << id << endl;
            } else {
                cout << size << " bytes have been allocated at block " << bestFitNode->id << " for request " << id << endl;
                freeList.updateNodeSize(bestFitNode, size);
                allocatedList.insertNode(size, bestFitNode->id, id);
            }
        } else if (type == 'R') {
            Node* allocatedNode = allocatedList.findNode(id);
            if (!allocatedNode) {
                cout << "Release of request " << id << " failed" << endl;
            } else {
                cout << allocatedNode->data_size << " bytes have been returned back to block " << allocatedNode->id << " for request " << id << endl;
                freeList.returnNodeSize(allocatedNode->id, allocatedNode->data_size);
                allocatedList.deleteNode(id);
            }
        }
    }
    file.close();

    ofstream outputFile("final_size.txt");
    Node* temp = freeList.getHead();
    while (temp) {
        outputFile << temp->data_size << endl;
        temp = temp->next;
    }
    outputFile.close();

    return 0;
}