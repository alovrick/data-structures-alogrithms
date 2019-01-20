#include <iostream>
#include <iomanip>
using namespace std;

template <class T>
class ThreadedTree {

	struct Node {

		T data_;
		Node* left_;
		Node* right_;

		//flags for threads
		bool leftThread;
		bool rightThread;

		//assign data value and child nodes, thread flags set to true by default
		Node(const T& data = T{}, Node* left = nullptr, Node* right = nullptr) {
			data_ = data;
			left_ = left;
			right_ = right;
			leftThread = true;
			rightThread = true;
		}

	};

	Node* root_;

	//sentinel nodes for front and back of the tree
	Node* front_;
	Node* back_;

public:
	class const_iterator {
	protected:
		Node* curr_;
		const_iterator(Node* c) { curr_ = c; }
	public:
		const_iterator() {
			curr_ = nullptr;
		}
		//iterator advances to next biggest node in tree if iterator is not currently at end()
		//postfix operator returns iterator to node before the increment
		const_iterator operator++(int) {

			const_iterator temp = *this;

			if (this->curr_ != nullptr) {

				//moving right by threads
				if (this->curr_->rightThread) {

					this->curr_ = this->curr_->right_;

				}
				//moving right by link
				else {

					this->curr_ = this->curr_->right_;

					while (this->curr_->leftThread == false) {
						this->curr_ = this->curr_->left_;
					}
				}
			}

			return temp;
		}
		//iterator goes to the next smallest node in the tree
		//postfix operator returns iterator to node before the decrement.
		const_iterator operator--(int) {
			const_iterator temp = *this;

			if (this->curr_ != nullptr) {

				if (this->curr_->leftThread) {

					this->curr_ = this->curr_->left_;

				}
				else { //if (!this->curr_->leftThread) 

					this->curr_ = this->curr_->left_;

					while (this->curr_->rightThread == false) {
						this->curr_ = this->curr_->right_;
					}
				}
			}


			return temp;
		}
		//iterator advances to next biggest node in tree if iterator is not currently at end()
		//prefix operator returns iterator to current node
		const_iterator operator++() {
			if (this->curr_ != nullptr) {

				if (this->curr_->rightThread) {//moving right by thread

					this->curr_ = this->curr_->right_;

				}
				else {//moving right by link

					this->curr_ = this->curr_->right_;

					while (this->curr_->leftThread == false) {
						this->curr_ = this->curr_->left_;
					}
				}
			}

			return *this;
		}
		//iterator goes to the next smallest node in the tree
		//prefix operator returns iterator to current node.
		const_iterator operator--() {
			if (this->curr_ != nullptr) {

				//move by left threads
				if (this->curr_->leftThread) { 

					this->curr_ = this->curr_->left_;

				}
				//move by left link
				else {

					this->curr_ = this->curr_->left_;

					while (this->curr_->rightThread == false) {
						this->curr_ = this->curr_->right_;
					}
				}
			}

			return *this;
		}
		//returns a reference to data in the node referred to by the iterator.
		const T& operator*() const {
			return curr_->data_;
		}
		//function returns true if rhs and current object refer to the same node
		bool operator==(const const_iterator& rhs) const {
			return curr_ == rhs.curr_;
		}
		//function returns true if rhs and current object does not refer to the same node
		bool operator!=(const const_iterator& rhs) const {
			return curr_ != rhs.curr_;
		}
		friend class ThreadedTree;
	};
	//all iterator operators behave exactly the same as cont_iterator equivalents
	class iterator :public const_iterator {
		iterator(Node* c) :const_iterator(c) {}
	public:
		iterator() :const_iterator() {}
		const T& operator*() const {
			return this->curr_->data_;
		}
		T& operator*() {
			return this->curr_->data_;
		}
		iterator operator++(int) {
			iterator temp = *this;

			if (this->curr_ != nullptr) {

				if (this->curr_->rightThread) {

					this->curr_ = this->curr_->right_;

				}
				else {

					this->curr_ = this->curr_->right_;

					while (this->curr_->leftThread == false) {
						this->curr_ = this->curr_->left_;
					}
				}
			}

			return temp;
		}
		iterator operator--(int) {
			iterator temp = *this;

			if (this->curr_ != nullptr) {

				if (this->curr_->leftThread) {

					this->curr_ = this->curr_->left_;

				}
				else { 

					this->curr_ = this->curr_->left_;

					while (this->curr_->rightThread == false) {
						this->curr_ = this->curr_->right_;
					}
				}
			}

			return temp;
		}
		iterator operator++() {
			if (this->curr_ != nullptr) {

				if (this->curr_->rightThread) {

					this->curr_ = this->curr_->right_;

				}
				else {

					this->curr_ = this->curr_->right_;

					while (this->curr_->leftThread == false) {
						this->curr_ = this->curr_->left_;
					}
				}
			}

			return *this;
		}
		iterator operator--() {

			if (this->curr_ != nullptr) {

				if (this->curr_->leftThread) {

					this->curr_ = this->curr_->left_;

				}
				else {

					this->curr_ = this->curr_->left_;

					while (this->curr_->rightThread == false) {
						this->curr_ = this->curr_->right_;
					}
				}
			}
			return *this;
		}

		friend class ThreadedTree;
	};

	//initialize sentinels to empty nodes that point to each other
	ThreadedTree() {
		root_ = nullptr;
		front_ = new Node();
		back_ = new Node();
		front_->right_ = back_;
		back_->left_ = front_;
	}

	//This function accepts a piece of data. The function inserts the data into the tree
	void insert(const T& data) {

		if (root_) {
			Node* node;

			node = root_;
			bool done = false;

			//flags to track if the node being inserted is the right/left most node respectively
			bool largest = true;
			bool smallest = true;

			while (!done) {

				//move left if data is smaller and leftThread flag has not been flipped
				if (data < node->data_ && node->leftThread == false) {
					node = node->left_;
					largest = false; //this node is not the right most node
				}
				//move right if data is larger and rightThread flag has not been flipped
				else if (data > node->data_ && node->rightThread == false) {
					node = node->right_;
					smallest = false; //this node is not the left most node
				}
				//handle duplicates
				else if (data == node->data_) {
					done = true;
				}
				//node no longer has to be moved, check if we are inserting left or right
				else {
					//insert left 
					if (data < node->data_) {
						//insert new node to with children linked to previous nodes in the tree
						Node* nn = new Node(data, node->left_, node); 
						node->left_ = nn;

						//trip leftThread flag
						node->leftThread = false;
						done = true;

						//assign front to inserted node if node was not moved right
						if (smallest) {
							front_->left_ = nn;
						}
					}
					//insert right
					else if (data > node->data_) {
						//insert new node to with children linked to previous nodes in the tree
						Node* nn = new Node(data, node, node->right_);
						node->right_ = nn;

						//trip rightThread flag
						node->rightThread = false;
						done = true;

						//assign back to inserted node if node was not moved left
						if (largest) {
							back_->right_ = nn->right_;
							back_->left_ = nn;
						}
					}
				}
			}
		}
		//insert at root if the tree is empty
		else {
			root_ = new Node(data);
			front_->left_ = root_;
			back_->left_ = root_;
			root_->right_ = back_;
			root_->left_ = front_;
		}
		
	}
	//this function accepts a piece of data and returns an iterator to the node containing that data. 
	//If data is not found, function returns end();
	iterator find(const T& data) {
		//start search from root
		iterator it(root_);

		while (it != nullptr) {

			//node found, exit
			if (data == it.curr_->data_) {
				return it;
			}
			//search left
			else if (data < it.curr_->data_) {
				it.curr_ = it.curr_->left_;
			}
			//search right
			else {
				it.curr_ = it.curr_->right_;
			}
		}
	}
	//cont_iterator equivalent of find function
	const_iterator find(const T& data) const {

		const_iterator it(root_);

		while (it != nullptr) {

			if (data == it.curr->data) {
				return it;
			}
			else if (data < it.curr_->data_) {
				it.curr_ = it.curr_->left_;
			}
			else {
				it.curr_ = it.curr_->right_;
			}
		}
	}
	//This function returns the appropriate iterator to the smallest node in the tree
	iterator begin() {
		iterator it(front_->left_);
		return it;
	}
	//This function returns the appropriate iterator to the node to the right of the node with biggest value 
	iterator end() {
		iterator it(back_->right_);
		return it++;
	}
	const_iterator cbegin()const {
		const_iterator it(front_->left_);
		return it;
	}
	const_iterator cend() const {
		const_iterator it(back_->right_);
		return it++;
	}
	//recursive function to delete the whole tree upon destruction
	void destructTree(Node* curr_) {

		if (curr_) {

			if (!curr_->leftThread) {
				destructTree(curr_->left_);
			}

			if (!curr_->rightThread) {
				destructTree(curr_->right_);
			}

			delete curr_;
			curr_ = nullptr;
		}
	}

	~ThreadedTree() {
		//delete tree
		destructTree(root_);
		//delete sentinels
		delete front_;
		delete back_;
		front_ = nullptr;
		back_ = nullptr;
	}
	
};