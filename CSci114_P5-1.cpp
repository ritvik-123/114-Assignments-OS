// Ritvik, Assignment-5, 301894867
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

class node {
public:
    int data_size;
    int id;
    node* next;

    node() {
        data_size = 1024;
        id = -1; // -1 to indicate an uninitialized ID
        next = NULL;
    }

    node(int data, int id) {
        this->data_size = data;
        this->id = id;
        this->next = NULL;
    }
};

class linkedlist {
    node* head;

public:
    linkedlist() {
        head = NULL;
    }

    node* getHead() {
        return head;
    }

    void insertNode(int size, int id) {
        node* newNode = new node(size, id);
        if (head == NULL) {
            head = newNode;
            return;
        }
        node* temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
        sortList();
    }

    void printList() {
        node* temp = head;
        while (temp != NULL) {
            cout << "Block ID: " << temp->id << ", Size: " << temp->data_size << endl;
            temp = temp->next;
        }
    }

    void deleteNode(int id) {
        if (head == NULL) {
            cout << "List empty" << endl;
            return;
        }
        node* temp = head;
        node* prev = NULL;
        while (temp != NULL && temp->id != id) {
            prev = temp;
            temp = temp->next;
        }
        if (temp == NULL) {
            cout << "Item not found" << endl;
            return;
        }
        if (prev == NULL) {
            head = temp->next;
        } else {
            prev->next = temp->next;
        }
        delete temp;
        sortList();
    }

    void sortList() {
        if (head == NULL || head->next == NULL) {
            return;
        }
        for (node* i = head; i->next != NULL; i = i->next) {
            for (node* j = i->next; j != NULL; j = j->next) {
                if (i->data_size > j->data_size) {
                    swap(i->data_size, j->data_size);
                    swap(i->id, j->id);
                }
            }
        }
    }

    node* findBestFit(int size) {
        node* temp = head;
        node* bestFit = NULL;
        while (temp != NULL) {
            if (temp->data_size >= size) {
                if (bestFit == NULL || temp->data_size < bestFit->data_size) {
                    bestFit = temp;
                }
            }
            temp = temp->next;
        }
        return bestFit;
    }

    void updateNodeSize(node* n, int size) {
        n->data_size -= size;
        sortList();
    }

    void returnNodeSize(int blockId, int size) {
        node* temp = head;
        while (temp != NULL && temp->id != blockId) {
            temp = temp->next;
        }
        if (temp != NULL) {
            temp->data_size += size;
            sortList();
        }
    }
};

class allocationList {
    node* head;

public:
    allocationList() {
        head = NULL;
    }

    node* getHead() {
        return head;
    }

    void insertNode(int size, int blockId, int requestId) {
        node* newNode = new node(size, requestId);
        newNode->id = blockId;
        if (head == NULL) {
            head = newNode;
            return;
        }
        node* temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }

    void deleteNode(int requestId) {
        if (head == NULL) {
            return;
        }
        node* temp = head;
        node* prev = NULL;
        while (temp != NULL && temp->id != requestId) {
            prev = temp;
            temp = temp->next;
        }
        if (temp == NULL) {
            return;
        }
        if (prev == NULL) {
            head = temp->next;
        } else {
            prev->next = temp->next;
        }
        delete temp;
    }

    node* findNode(int requestId) {
        node* temp = head;
        while (temp != NULL && temp->id != requestId) {
            temp = temp->next;
        }
        return temp;
    }

    void printList() {
        node* temp = head;
        while (temp != NULL) {
            cout << "Request ID: " << temp->id << ", Block ID: " << temp->id << ", Size: " << temp->data_size << endl;
            temp = temp->next;
        }
    }
};

int main() {
    linkedlist freeList;
    allocationList allocatedList;

    // Initialize the free list with 1024 blocks of size 1024 bytes
    for (int i = 1; i <= 1024; ++i) {
        freeList.insertNode(1024, i);
    }

    ifstream file("requests-1.txt");
    if (!file) {
        cout << "Unable to open file" << endl;
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
            node* bestFitNode = freeList.findBestFit(size);
            if (bestFitNode == NULL) {
                cout << "Allocation of " << size << " bytes failed for request " << id << endl;
            } else {
                cout << size << " bytes have been allocated at block " << bestFitNode->id << " for request " << id << endl;
                freeList.updateNodeSize(bestFitNode, size);
                allocatedList.insertNode(size, bestFitNode->id, id);
            }
        } else if (type == 'R') {
            node* allocatedNode = allocatedList.findNode(id);
            if (allocatedNode == NULL) {
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
    node* temp = freeList.getHead();
    while (temp != NULL) {
        outputFile << temp->data_size << endl;
        temp = temp->next;
    }
    outputFile.close();

    return 0;
}