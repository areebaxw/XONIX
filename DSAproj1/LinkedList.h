#pragma once
#include <iostream>
using namespace std;

class Node {	
public:
	int data;
	Node* next;
	Node(int d) {
		data = d;
		next = NULL;
	}
};

class LinkedList {
private:
	Node* head;
public:
	int rows;
	int columns;

	LinkedList() {
		head = NULL;
	}

	void insert(int d) {
		Node* newNode = new Node(d);
		if (head == NULL) {
			head = newNode;
		}
		else {
			Node* temp = head;
			while (temp->next != NULL) {
				temp = temp->next;
			}
			temp->next = newNode;
		}
	}
	void display() {
		Node* temp = head;
		while (temp != NULL) {
			cout << temp->data << " ";
			temp = temp->next;
		}
		cout << endl;
	}

	void convert2D(int arr[35][55], int rows, int columns) {
		head = nullptr;          // start fresh
		Node* tail = nullptr;    // to track the end of the list

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < columns; j++) {
				Node* newNode = new Node(arr[i][j]);

				if (!head) {
					head = newNode;
					tail = newNode;
				}
				else {
					tail->next = newNode;
					tail = newNode;
				}
			}
		}

		this->rows = rows;
		this->columns = columns;
	}

	void convertTo2D(int arr[35][55]) {
		if (!head) return;

		Node* temp = head;
		for (int i = 0; i < 35; i++)
		{
			for (int j = 0; j < 55; j++) 
			{
				if (temp)
				{
					arr[i][j] = temp->data;
					temp = temp->next;
				}
			}
		}
	}

	Node* getHead() {
		return head;
	}
};