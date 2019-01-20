
#include <string>
#include <utility>
using namespace std;

template <class TYPE>
class Table {
public:
	Table() {}
	virtual bool update(const string& key, const TYPE& value) = 0;
	virtual bool remove(const string& key) = 0;
	virtual bool find(const string& key, TYPE& value) = 0;
	virtual int numRecords() const = 0;
	virtual bool isEmpty() const = 0;
	virtual ~Table() {}
};

template <class TYPE>
class SimpleTable :public Table<TYPE> {

	struct Record {
		TYPE data_;
		string key_;
		Record(const string& key, const TYPE& data) {
			key_ = key;
			data_ = data;
		}

	};

	Record** records_;   //the table
	int max_;           //capacity of the array
	int size_;          //current number of records held
	int search(const string& key);

public:
	SimpleTable(int capacity);
	SimpleTable(const SimpleTable& other);
	SimpleTable(SimpleTable&& other);
	virtual bool update(const string& key, const TYPE& value);
	virtual bool remove(const string& key);
	virtual bool find(const string& key, TYPE& value);
	virtual const SimpleTable& operator=(const SimpleTable& other);
	virtual const SimpleTable& operator=(SimpleTable&& other);
	virtual ~SimpleTable();
	virtual bool isEmpty() const { return size_ == 0; }
	virtual int numRecords() const { return size_; }
};


//returns index of where key is found.
template <class TYPE>
int SimpleTable<TYPE>::search(const string& key) {
	int rc = -1;
	for (int i = 0; i<size_; i++) {
		if (records_[i]->key_ == key) {
			rc = i;
		}
	}
	return rc;
}


template <class TYPE>
SimpleTable<TYPE>::SimpleTable(int capacity) : Table<TYPE>() {
	records_ = new Record*[capacity];
	max_ = capacity;
	size_ = 0;
}

template <class TYPE>
SimpleTable<TYPE>::SimpleTable(const SimpleTable<TYPE>& other) {
	records_ = new Record*[other.max_];
	max_ = other.max_;
	size_ = 0;
	for (int i = 0; i<other.size_; i++) {
		update(other.records_[i]->key_, other.records_[i]->data_);
	}
}
template <class TYPE>
SimpleTable<TYPE>::SimpleTable(SimpleTable<TYPE>&& other) {
	size_ = other.size_;
	max_ = other.max_;
	records_ = other.records_;
	other.records_ = nullptr;
	other.size_ = 0;
	other.max_ = 0;
}

template <class TYPE>
bool SimpleTable<TYPE>::update(const string& key, const TYPE& value) {
	int idx = search(key);
	if (idx == -1) {
		if (size_ < max_) {
			records_[size_++] = new Record(key, value);
			for (int i = 0; i<size_ - 1; i++) {
				for (int j = 0; j<size_ - 1 - i; j++) {
					if (records_[j] > records_[j + 1]) {
						TYPE tmp = records_[j];
						records_[j] = records_[j + 1];
						records_[j + 1] = tmp;
					}
				}
			}
		}
	}
	else {
		records_[idx]->data_ = value;
	}
	return true;
}

template <class TYPE>
bool SimpleTable<TYPE>::remove(const string& key) {
	int idx = search(key);
	if (idx != -1) {
		delete records_[idx];
		for (int i = idx; i<size_ - 1; i++) {
			records_[i] = records_[i + 1];
		}
		size_--;
		return true;
	}
	else {
		return false;
	}
}

template <class TYPE>
bool SimpleTable<TYPE>::find(const string& key, TYPE& value) {
	int idx = search(key);
	if (idx == -1)
		return false;
	else {
		value = records_[idx]->data_;
		return true;
	}
}

template <class TYPE>
const SimpleTable<TYPE>& SimpleTable<TYPE>::operator=(const SimpleTable<TYPE>& other) {
	if (this != &other) {
		if (records_) {
			int sz = size_;
			for (int i = 0; i<sz; i++) {
				remove(records_[0]->key_);
			}
			delete[] records_;
		}
		records_ = new Record*[other.max_];
		max_ = other.max_;
		size_ = 0;
		for (int i = 0; i<other.size_; i++) {
			update(other.records_[i]->key_, other.records_[i]->data_);
		}

	}
	return *this;
}
template <class TYPE>
const SimpleTable<TYPE>& SimpleTable<TYPE>::operator=(SimpleTable<TYPE>&& other) {
	swap(records_, other.records_);
	swap(size_, other.size_);
	swap(max_, other.max_);
	return *this;
}
template <class TYPE>
SimpleTable<TYPE>::~SimpleTable() {
	if (records_) {
		int sz = size_;
		for (int i = 0; i<sz; i++) {
			remove(records_[0]->key_);
		}
		delete[] records_;
	}
}

template <class TYPE>
class LPTable :public Table<TYPE>
{
	//Struct for records contained in the hash table
	struct Record
	{
		TYPE data_;
		string key_;
		Record(const string& key, const TYPE& data)
		{
			key_ = key;
			data_ = data;
		}
	};

	Record** records_;
	int max_;
	int size_;
	double lambda_;

public:
	LPTable(int capacity, double maxLoadFactor);
	LPTable(const LPTable& other);
	LPTable(LPTable&& other);
	virtual bool update(const string& key, const TYPE& value);
	virtual bool remove(const string& key);
	virtual bool find(const string& key, TYPE& value);
	virtual const LPTable& operator=(const LPTable& other);
	virtual const LPTable& operator=(LPTable&& other);
	virtual ~LPTable();
	virtual bool isEmpty() const {
		return size_ == 0 ? true : false;
	}

	virtual int numRecords() const
	{
		return size_;
	}

	/*This function takes in index for empty spot in array,index for current spot and
	hash value of current spot.This function checks if the empty spot is between
	value of hash key and current spot. If it is in between, function returns true else false. */
	bool is_between(size_t index, size_t emptySpot, size_t hash)
	{
		while (index == emptySpot || records_[index] != nullptr && index != hash)
		{
			if (index == emptySpot)
				return true;
			index--;
		}
		if (index < 0) {
			index = max_ - 1;
		}
		return false;
	}

};

/*capacity is the capacity of the table. maxLoadFactor is a number that
represents a percentage as a decimal number.*/
template <class TYPE>
LPTable<TYPE>::LPTable(int capcity, double maxLoadFactor) : Table<TYPE>()
{
	lambda_ = maxLoadFactor;
	max_ = capcity;
	size_ = 0;
	records_ = new Record*[max_];

	for (int i = 0; i < max_; i++)
	{
		records_[i] = nullptr;
	}
}

template <class TYPE>
LPTable<TYPE>::LPTable(const LPTable<TYPE>& other)
{
	operator=(other);
}


template <class TYPE>
LPTable<TYPE>::LPTable(LPTable<TYPE>&& other)
{
	Record** temp = records_;
	records_ = other.records_;
	other.records_ = temp;
	max_ = other.max_;
	size_ = other.size_;
	lambda_ = other.lambda_;
}

/*This function is passed a key-value pair. If the table already has a record with a matching key,
the record's value is replaced by the value passed to this function. If no record exists, a record with
key-value pair is added to the table as long as it is possible to do so (some implementations restrict
the amount of data it can hold). If the record was successfully added or updated, function returns true.
Otherwise false is returned.*/
template <class TYPE>
bool LPTable<TYPE>::update(const string& key, const TYPE& value)
{
	hash<string> hashFunction;
	size_t hash = hashFunction(key);
	size_t index = hash % max_;

	while (records_[index] != nullptr)
	{
		if (records_[index]->key_ == key)
		{
			records_[index]->data_ = value;
			return true;
		}

		index++;

		if (index >= max_)
		{
			index = 0;
		}
	}

	if (size_ < max_ * lambda_)
	{
		records_[index] = new Record(key, value);
		size_++;
		return true;
	}
	return false;
}

/*This function is passed a key. If the table contains a record with a matching key,
the record (both the key and the value) is removed from the table*/
template <class TYPE>
bool LPTable<TYPE>::remove(const string& key)
{
	//create
	hash<string> hashFunction;
	size_t hash = hashFunction(key);
	size_t index = hash % max_;

	while (records_[index] != nullptr && records_[index]->key_ != key)
	{
		index++;
		if (index >= max_)
		{
			index = 0;
		}
	}

	if (records_[index] == nullptr)
	{
		return false;
	}

	delete records_[index];
	records_[index] = nullptr;
	size_t emptySpot = index;
	index++;
	size_t max = max_;
	size_t i;

	while (records_[index] != nullptr)
	{
		hash = hashFunction(records_[index]->key_);
		i = hash % max_;
		if (emptySpot == i)
		{
			records_[emptySpot] = records_[index];
			records_[index] = nullptr;
			emptySpot = index;
		}
		else
		{	//using isBetween function
			if (is_between(index, emptySpot, i))
			{
				records_[emptySpot] = records_[index];
				records_[index] = nullptr;
				emptySpot = index;
			}
		}
		index++;

		if (index >= max)
		{
			index = 0;
		}
	}

	size_--;
	return true;
}

/*This function is passed a key and a reference for passing back a found value. If the table contains
a record with a matching key, the function returns true, and passes back the value from the record. If
it does not find a record with a matching key, function returns false.*/
template <class TYPE>
bool LPTable<TYPE>::find(const string& key, TYPE& value)
{
	//create hash
	hash<string> hashFunction;
	size_t hash = hashFunction(key);
	size_t index = hash % max_;
	while (records_[index] != nullptr && records_[index]->key_ != key) {
		index++;
		if (index >= max_) {
			index = 0;
		}
	}
	if (records_[index] != nullptr) {
		value = records_[index]->data_;
		return true;
	}
	return false;
}

//copy assignment operator, return address of current table
template <class TYPE>
const LPTable<TYPE>& LPTable<TYPE>::operator=(const LPTable<TYPE>& other)
{
	//copy data members of a received table into the current table
	max_ = other.max_;
	size_ = other.size_;
	lambda_ = other.lambda_;
	records_ = new Record*[max_];
	//iterate through current table and copy records
	for (int i = 0; i < max_; i++) {
		if (other.records_[i] != nullptr)
			records_[i] = new Record(other.records_[i]->key_, other.records_[i]->data_);
		else
			records_[i] = nullptr;
	}
	return *this;
}

//move assignment operator, returns adress of current table
template <class TYPE>
const LPTable<TYPE>& LPTable<TYPE>::operator=(LPTable<TYPE>&& other)
{
	Record** temp = records_;
	records_ = other.records_;
	other.records_ = temp;
	max_ = other.max_;
	size_ = other.size_;
	lambda_ = other.lambda_;
	return *this;
}

//destructor, iterate through table and delete all records
template <class TYPE>
LPTable<TYPE>::~LPTable()
{
	for (int i = 0; i < max_; i++)
	{
		delete records_[i];
	}
	delete[] records_;
}
