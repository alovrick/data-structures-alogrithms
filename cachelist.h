template <typename T>

class CacheList {
	struct Node {

		T data_;
		int access_;
		Node* next_;
		Node* prev_;

		//3 arg constructor, fills out all fields in a node except for access_ which always defaults to 0
		Node(const T& data = T{}, Node* nx = nullptr, Node* pr = nullptr) {
			data_ = data;
			next_ = nx;
			prev_ = pr;
			access_ = 0;
		}
	};

	Node* front_;
	Node* back_;

public:
	class const_iterator {
		friend class CacheList<T>;
		
	protected:
		const CacheList* list_;
		Node* curr_;

	public:
		const_iterator() {
			list_ = nullptr;
			this->curr_ = nullptr;
		}
		const_iterator(Node* nn) { 
			curr_ = nn; 
		}
		const_iterator(Node* curr, const CacheList* list) {
			list_ = list;
			curr_ = curr;
		}
		const_iterator operator++() {
			curr_ = curr_->next_;
			return *this;
		}
		const_iterator operator++(int) {
			const_iterator old = *this;
			curr_ = curr_->next_;
			return old;
		}
		const_iterator operator--() {
			if (curr_) {
				curr_ = curr_->prev_;
			}
			else {
				if (list_) {
					curr_ = list_->back_;
				}
			}
			return *this;
		}
		const_iterator operator--(int) {
			const_iterator old = *this;
			curr_ = curr_->prev_;
			return old;
		}
		bool operator==(const_iterator rhs) {
			bool rc = false;
			if (list_ == rhs.list_ && curr_ == rhs.curr_)
				rc = true;
			return rc;
		}
		bool operator!=(const_iterator rhs) {
			if (list_ != rhs.list_ || curr_ != rhs.curr_) {
				return true;
			}
			else {
				return false;
			}
		}
		const T& operator*()const {
			return curr_->data_;
		}
	};
	class iterator :public const_iterator {
		friend class CacheList<T>;

	public:
		iterator() {
			this->list_ = nullptr;
			this->curr_ = nullptr;
		}
		iterator(Node* nn) { this->curr_ = nn; }
		iterator(Node* curr, CacheList* list) :const_iterator(curr, list) {
			this->list_ = list;
			this->curr_ = curr;
		}
		iterator operator++() {
			this->curr_ = this->curr_->next_;
			return *this;
		}
		iterator operator++(int) {
			iterator old = *this;
			this->curr_ = this->curr_->next_;
			return old;
		}
		iterator operator--() {
			if (this->curr_ != nullptr)
				this->curr_ = this->curr_->prev_;
			else {
				if (this->list_) {
					this->curr_ = this->list_->back_;
				}
			}
			return *this;
		}
		iterator operator--(int) {
			iterator old = *this;
			this->curr_ = this->curr_->prev_;
			return old;
		}
		T& operator*() {
			return this->curr_->data_;
		}
		const T& operator*()const {
			return this->curr_->data_;
		}
	};
	CacheList();
	~CacheList();
	CacheList(const CacheList& rhs);
	CacheList& operator=(const CacheList& rhs);
	CacheList(CacheList&& rhs);
	CacheList& operator=(CacheList&& rhs);
	iterator begin() {
		return iterator(front_->next_, this);
	}
	iterator end() {
		return iterator(back_, this);
	}
	const_iterator cbegin() const {
		return const_iterator(front_->next_, this);
	}
	const_iterator cend() const {
		return const_iterator(back_, this);
	}
	void insert(const T& data);
	iterator search(const T& data);
	iterator erase(iterator it);
	iterator erase(iterator first, iterator last);
	void sortByAccess(const_iterator it);
	bool empty() const;
	int size() const;
};

//Default constructor for a sentinel linked list, 
//two emtpy nodes are created at the front and back and linked together
template <typename T>
CacheList<T>::CacheList() {
	front_ = new Node();
	back_ = new Node();
	front_->next_ = back_;
	back_->prev_ = front_;
}
//Destructor, deletes all nodes in the list
template <typename T>
CacheList<T>::~CacheList() {

	Node* curr = front_;

	while (curr) {
		Node* rm = curr;
		curr = curr->next_;
		delete rm;
	}

}
//Copy constructor, only performs operations if the received reference is not empty
template <typename T>
CacheList<T>::CacheList(const CacheList& rhs) {

	front_ = new Node();
	back_ = new Node();
	front_->next_ = back_;
	back_->prev_ = front_;

	for (const_iterator i = rhs.cbegin(); i != rhs.cend(); i++) {

		Node* nn = new Node(i.curr_->data_, back_, back_->prev_);

		if (back_->prev_ == front_) {
			back_->prev_ = nn;
			front_->next_ = nn;
		}
		else {
			back_->prev_->next_ = nn;
			back_->prev_ = nn;
		}
	}
}
//copy assignment operator
template <typename T>
CacheList<T>& CacheList<T>::operator=(const CacheList& rhs) {

	//erase the current contents of the list before proceeding 
	iterator first = begin();
	iterator last = end();
	erase(first, last);

	iterator bckIt = rhs.back_->prev_;
	iterator fIt = rhs.front_;

	while (bckIt != fIt)
	{
		Node* nn = new Node(*bckIt);
		Node* nxtNode = front_->next_;

		nn->next_ = nxtNode;
		nn->prev_ = nxtNode->prev_;

		nxtNode->prev_->next_ = nn;
		nxtNode->prev_ = nn;

		bckIt--;
	}
	return *this;
}
//move constructor
template <typename T>
CacheList<T>::CacheList(CacheList&& rhs) {

	//copy the sentinels from the reference and then empty them
	front_ = rhs.front_;
	back_ = rhs.back_;
	rhs.back_ = new Node();
	rhs.front_ = new Node();
}
//copy assignment operator
template <typename T>
CacheList<T>& CacheList<T>::operator=(CacheList&& rhs) {
	if (this != &rhs) {
		front_ = rhs.front_;
		back_ = rhs.back_;

		rhs.back_ = new Node();
		rhs.front_ = new Node();
	}
	return *this;
}
//Creates new node holding data at the back of the linked list
template <typename T>
void CacheList<T>::insert(const T& data) {

	Node* nn = new Node(data, back_, back_->prev_); //node to be inserted
	back_->prev_->next_ = nn;
	back_->prev_ = nn;

}
//Find and return an iterator to the node containing a matching data value
template <typename T>
typename CacheList<T>::iterator CacheList<T>::search(const T& data) {

	iterator it = begin(); //node we are checking, being at the front of the list

	//Iterate through the list. 
	//If the data values match, increase the access_ value and sort the list by access values highest to lowest
	while (it != end()) {

		if (data == *it) {
			++it.curr_->access_;		
			sortByAccess(it);		
			return it; //matching iterator is returned if found
		}

		it++;
	}
	
	return end(); //end() is returned if a matching node is not found

}
//sorts the list by the access_ data memeber, highest to lowest
template <typename T>
void CacheList<T>::sortByAccess(const_iterator it) {

	//iterate through the list. reorder the nodes accordingly if the nodes are not sorted from highest to lowest
	const_iterator tmp = begin();
	while (tmp.curr_ != back_) {
		if (tmp.curr_->access_ <= it.curr_->access_ && tmp.curr_ != it.curr_)
		{
			it.curr_->prev_->next_ = it.curr_->next_;
			it.curr_->next_->prev_ = it.curr_->prev_;
			tmp.curr_->prev_->next_ = it.curr_;
			it.curr_->prev_ = tmp.curr_->prev_;
			it.curr_->next_ = tmp.curr_;
			tmp.curr_->prev_ = it.curr_;
			break;
		}
		else
		{
			tmp++;
		}
	}
	
}
//removes a node referred to by it. returns the node following it
template <typename T>
typename CacheList<T>::iterator CacheList<T>::erase(iterator it) {

	Node* tmp = it.curr_; //node to be deleted
	Node* nxt = tmp->next_; //node following the node that is to be deleted

	//unlink tmp and then delete it
	tmp->prev_->next_ = nxt;
	nxt->prev_ = tmp->prev_;

	delete tmp;
	return iterator(nxt);

}
//removes all nodes from first to last
//including the node referred to by first but not the one referred to by last
template <typename T>
typename CacheList<T>::iterator CacheList<T>::erase(iterator first, iterator last) {

	if (!empty())
	{
		//erase all nodes unitl the last node is reached
		for (iterator it = first; it != last;) 
		{
			erase(it++);
		}
	}
	return last;

}
//check if list is empty and return result
template <typename T>
bool CacheList<T>::empty() const {

	if (front_->next_ == back_) {
		return true;
	}
	return false;

}
//check and return size of list
template <typename T>
int CacheList<T>::size() const {

	int size = 0;
	Node* nn = front_->next_;

	while (nn != back_) {
		size++;
		nn = nn->next_;
	}

	return size;
}
