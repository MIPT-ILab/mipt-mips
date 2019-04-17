/////////////////////////////////////////////////////////////////////////////
//
// You may use this source code free of charge in any environment, pending
// you e-mail Justin (justin@nodeka.com) so he is aware of how the tree
// container is being used and send updates as they are made.
//
// (c) 1999-2005 Justin Gottschlich
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
// Bug fix assistance by:
//
// Stefan Kluehspies (missing typenames, constructor member initialization order)
// Wayne Vucenic (zero initialization of data_ for native types)
//
/////////////////////////////////////////////////////////////////////////////

#ifndef tree_header_file
#define tree_header_file

#ifndef NULL
#define NULL 0
#endif

#if WIN32
#pragma warning( push )
// Disable warning for multiple operator= defines
#pragma warning( disable : 4522 )
#pragma warning( disable : 4786 )
#endif // WIN32

namespace core {

/////////////////////////////////////////////////////////////////////////////
// tree_iterator forward declaration
/////////////////////////////////////////////////////////////////////////////
template <typename T> class tree_iterator;

/////////////////////////////////////////////////////////////////////////////
// tree pair object definition
/////////////////////////////////////////////////////////////////////////////
template <typename T> class tree
{
public:

	typedef tree_iterator<T> iterator;
	typedef const tree_iterator<T> const_iterator;

private:

	// Class data
	mutable T data_;

	// Nobody gets any access to this
	mutable tree *next_;
	mutable tree *prev_;
	mutable tree *in_;
	mutable tree *out_;

	// What level are we on?
	mutable size_t level_;
	mutable size_t size_;

	//////////////////////////////////////////////////////////////////////////
	// Removes a link to a node ... doesn't destroy the CTree, just rips it
	// out of it's current location. This is used so it can be placed elsewhere
	// without trouble.
	//////////////////////////////////////////////////////////////////////////
	void disconnect_()
	{
		// unlink this from the master node
		if (this->out_ != NULL) {

			tree *out = this->out_;

			// Decrement the size of the outter level
			--(out->size_);

			if (out->in_ == this) {
				if (NULL == this->next_) {
					// If this is the last node of this level, zap the hidden node
					delete this->prev_;
					out->in_ = NULL;
				}
				else {
					// Otherwise, just reattatch the head node to the next node
					this->prev_->next_ = this->next_;
					this->next_->prev_ = this->prev_;
					out->in_ = this->next_;
				}
			}
			else {
				// We should be able to do this absolutely.
				this->prev_->next_ = this->next_;
				if (NULL != this->next_) this->next_->prev_ = this->prev_;
			}
		}
		// Point to nothing
		this->next_ = this->prev_ = NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// End of the tree list, private only
	//////////////////////////////////////////////////////////////////////////
	const tree* end_() const { return (NULL); }

	//////////////////////////////////////////////////////////////////////////
	// Does the actual insert into the tree
	//////////////////////////////////////////////////////////////////////////
	tree& i_insert(tree *inTree, tree *level, bool (*pObj)(const T&, const T&))
	{
		// Do NOT move this line beyond this point. The reason is because we must
		// check to see if the node exists here because we may be removing the ONLY
		// node in the tree. If it is then NULL == level->in_. DO NOT REMOVE THIS
		//if (false == level->mDuplicates)

		// never allow duplicate keys
		level->remove(inTree->data());

		// if there's no inner tree, make it
		if (NULL == level->in_) {
			// Dummy node, create it -- if good memory do stuff, if NULL throw
			if (tree *temp = new tree) {
				temp->next_ = inTree;
				inTree->prev_ = temp;
				level->in_ = inTree;
			}
			else throw "allocation failed";
		}
		else {

			tree *temp = level->in_->prev_;

			while (true) {
				if (NULL == temp->next_) {
					temp->next_ = inTree;
					inTree->prev_ = temp;
					break;
				}
				else if ( pObj(inTree->data(), temp->next_->data()) ) {

					tree *hold = temp->next_;

					// temp -> inTree -> hold
					temp->next_ = inTree;
					inTree->next_ = hold;

					// temp <- inTree <- hold
					hold->prev_ = inTree;
					inTree->prev_ = temp;

					// If we just inserted on the first node, we need to make sure
					// the in pointer goes to inTree
					if (hold == level->in_) level->in_ = inTree;
					break;
				}
				temp = temp->next_;
			}
		}

		inTree->out_ = level;
		++(level->size_);

		inTree->level_ = level->level() + 1;
		return (*inTree);
	}

	//////////////////////////////////////////////////////////////////////////
	iterator push_back_no_remove(const T &inT)
	{
		tree *createdTree = new tree(inT);
		if (NULL == createdTree) throw "allocation failed";
		return iterator(i_push_back_no_remove(createdTree, this));
	}

	//////////////////////////////////////////////////////////////////////////
	tree& i_push_back_no_remove(tree *inTree, tree *level)
	{
		// if there's no inner tree, make it
		if (NULL == level->in_)
      {
			// Dummy node, create it -- if good memory do stuff, if NULL throw
			if (tree *temp = new tree) {
				temp->next_ = inTree;
				inTree->prev_ = temp;
				level->in_ = inTree;
			}
			else throw "allocation failed";
		}
		else
      {
			tree *temp = level->in_->prev_;

			while (true) {
				if (NULL == temp->next_) {
					temp->next_ = inTree;
					inTree->prev_ = temp;
					break;
				}
				temp = temp->next_;
			}
		}

		inTree->out_ = level;
		++(level->size_);

		inTree->level_ = level->level() + 1;
		return *inTree;
	}

	//////////////////////////////////////////////////////////////////////////
	tree& i_push_back(tree *inTree, tree *level)
	{
		level->remove(inTree->data());

		// if there's no inner tree, make it
		if (NULL == level->in_)
      {
			// Dummy node, create it -- if good memory do stuff, if NULL throw
			if (tree *temp = new tree) {
				temp->next_ = inTree;
				inTree->prev_ = temp;
				level->in_ = inTree;
			}
			else throw "allocation failed";
		}
		else
      {
			tree *temp = level->in_->prev_;

			while (true) {
				if (NULL == temp->next_) {
					temp->next_ = inTree;
					inTree->prev_ = temp;
					break;
				}
				temp = temp->next_;
			}
		}

		inTree->out_ = level;
		++(level->size_);

		inTree->level_ = level->level() + 1;
		return *inTree;
	}

	//////////////////////////////////////////////////////////////////////////
	tree& i_push_front(tree *inTree, tree *level)
	{
		level->remove(inTree->data());

		// if there's no inner tree, make it
		if (NULL == level->in_)
      {
			// Dummy node, create it -- if good memory do stuff, if NULL throw
			if (tree *temp = new tree) {
				temp->next_ = inTree;
				inTree->prev_ = temp;
				level->in_ = inTree;
			}
			else throw "allocation failed";
		}
		else
      {
			tree *temp = level->in_->prev_;

			tree *hold = temp->next_;
			// temp -> inTree -> hold
			temp->next_ = inTree;
			inTree->next_ = hold;
			// temp <- inTree <- hold
			hold->prev_ = inTree;
			inTree->prev_ = temp;
			// If we just inserted on the first node, we need to make sure
			// the in pointer goes to inTree
			if (hold == level->in_) level->in_ = inTree;
		}

		inTree->out_ = level;
		++(level->size_);

		inTree->level_ = level->level() + 1;
		return *inTree;
	}

	//////////////////////////////////////////////////////////////////////////
	// No function object
	//////////////////////////////////////////////////////////////////////////
	tree& i_insert(tree *inTree, tree *level)
	{
		// Do NOT move this line beyond this point. The reason is because we must
		// check to see if the node exists here because we may be removing the ONLY
		// node in the tree. If it is then NULL == level->in_. DO NOT REMOVE THIS
		//if (false == level->mDuplicates)
		level->remove(inTree->data());

		// if there's no inner tree, make it
		if (NULL == level->in_) {
			// Dummy node, create it -- if good memory do stuff, if NULL throw
			if (tree *temp = new tree) {
				temp->next_ = inTree;
				inTree->prev_ = temp;
				level->in_ = inTree;
			}
			else throw "allocation failed";
		}
		else {

			tree *temp = level->in_->prev_;

			while (true) {
				if (NULL == temp->next_) {
					temp->next_ = inTree;
					inTree->prev_ = temp;
					break;
				}
				else if ( inTree->data() < temp->next_->data() ) {
					tree *hold = temp->next_;
					// temp -> inTree -> hold
					temp->next_ = inTree;
					inTree->next_ = hold;
					// temp <- inTree <- hold
					hold->prev_ = inTree;
					inTree->prev_ = temp;
					// If we just inserted on the first node, we need to make sure
					// the in pointer goes to inTree
					if (hold == level->in_) level->in_ = inTree;
					break;
				}
				temp = temp->next_;
			}
		}

		inTree->out_ = level;
		++(level->size_);

		inTree->level_ = level->level() + 1;
		return (*inTree);
	}

	//////////////////////////////////////////////////////////////////////////
   size_t i_tree_size() const
   {
      size_t totalSize = this->size(); // + 1 to include root node

      for (iterator i = this->begin(); i != this->end(); ++i)
      {
         totalSize += i.tree_ref().i_tree_size();
      }

      return totalSize;
   }

protected:

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	size_t size(const tree& in) const { return in.size(); }
	size_t level(const tree& in) const { return in.level(); }

	//////////////////////////////////////////////////////////////////////////
	// Points to the beginning of the list and sets the current
	//////////////////////////////////////////////////////////////////////////
	iterator begin(const tree& in) const { return iterator( *(in.in_) ); }

	//////////////////////////////////////////////////////////////////////////
	// Notice that we're returning a const tree* here and not an iterator.
	// This is because the iterator itself has a member to a pointer. Doing
	// an iterator constructor here would be less efficient than just
	// returning a tree* which can be assigned internally inside the iterator
	// operator--(). Also because no one can call prev from a tree itself
	// (this is protected), we don't have to worry about safety issues except
	// for iterator safety.
	//////////////////////////////////////////////////////////////////////////
	const tree* prev(const tree& in) const { return (in.prev_); }

	//////////////////////////////////////////////////////////////////////////
	// Notice that we're returning a const tree* here and not an iterator.
	// This is because the iterator itself has a member to a pointer. Doing
	// an iterator constructor here would be less efficient than just
	// returning a tree* which can be assigned internally inside the iterator
	// operator++(). Also because no one can call prev from a tree itself
	// (this is protected), we don't have to worry about safety issues except
	// for iterator safety.
	//////////////////////////////////////////////////////////////////////////
	const tree* next(const tree& in) const { return (in.next_); }

	//////////////////////////////////////////////////////////////////////////
	iterator in(const tree& in) const { return iterator( *(in.in_) ); }

	//////////////////////////////////////////////////////////////////////////
	iterator out(const tree& in) const { return iterator( *(in.out_) ); }

public:

	//////////////////////////////////////////////////////////////////////////
	// Default constructor
	//////////////////////////////////////////////////////////////////////////
	tree() : data_(), next_(0), prev_(0), in_(0), out_(0), level_(0), size_(0) {}

	//////////////////////////////////////////////////////////////////////////
	// Paired <T> constructor
	//////////////////////////////////////////////////////////////////////////
	tree(const T &inT) : data_(inT), next_(0), prev_(0), in_(0), out_(0), level_(0), size_(0) {}

	//////////////////////////////////////////////////////////////////////////
	// operator==, expects operator== has been written for both t and u
	//////////////////////////////////////////////////////////////////////////
	const bool operator==(const tree &inTree) const
	{
		if (this->data_ == inTree.data_) return true;
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// The operator= which is a real copy, hidden and undefined
	//////////////////////////////////////////////////////////////////////////
	const tree& operator=(const tree& in)
	{
		this->clear();

		this->data_ = in.data_;
		this->copy_tree(in);

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	iterator get_tree_iterator() const { return iterator( *(this) ); }
	iterator get_tree_iterator() { return iterator( *(this) ); }

	//////////////////////////////////////////////////////////////////////////
	// copy constructor - now visible
	//////////////////////////////////////////////////////////////////////////
	tree(const tree &in) : data_(in.data_), next_(0), prev_(0), in_(0), out_(0),
		level_(0), size_(0) { *this = in; }

	//////////////////////////////////////////////////////////////////////////
	// destructor -- cleans out all branches, destroyed entire tree
	//////////////////////////////////////////////////////////////////////////
	virtual ~tree()
	{
		// Disconnect ourselves -- very important for decrementing the
		// size of our parent
		this->disconnect_();

		// Now get rid of our children -- but be smart about it,
		// right before we destroy it set it's out_ to NULL
		// that way Disconnect fails immediately -- much faster

		if (this->size() > 0) {
            tree *cur = this->in_, *prev = this->in_->prev_;

			// Delete the head node
			prev->out_ = NULL;
			delete prev;

			for (; this->size_ > 0; --this->size_) {

				prev = cur;
				cur = cur->next_;

				prev->out_ = NULL;
				delete prev;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void copy_tree(const tree& in)
	{
		// for each branch iterate through all nodes and copy them
		for (iterator i = in.begin(); in.end() != i; ++i) {
			iterator inserted = this->push_back_no_remove(i.data());

			// for each node, see if there are inners - if so, copy those too
			if (i.size() != 0) inserted.tree_ptr()->copy_tree(*i.tree_ptr());
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the first element of our tree
	//////////////////////////////////////////////////////////////////////////
	iterator begin() const { return iterator( *(this->in_) ); }
	iterator begin() { return iterator( *(this->in_) ); }

	//////////////////////////////////////////////////////////////////////////
	// Returns end_of_iterator
	//////////////////////////////////////////////////////////////////////////
	const iterator& end() const { return tree::iterator::end_iterator(); }

	//////////////////////////////////////////////////////////////////////////
	// Returns the first element of our tree
	//////////////////////////////////////////////////////////////////////////
	iterator in() const { return iterator( *(this->in_) ); }
	iterator in() { return iterator( *(this->in_) ); }

	//////////////////////////////////////////////////////////////////////////
	// Returns an iterator which steps out one level
	//////////////////////////////////////////////////////////////////////////
	iterator out() const { return iterator( *(this->out_) ); }
	iterator out() { return iterator( *(this->out_) ); }

	//////////////////////////////////////////////////////////////////////////
	// much like destructor with the exception that empty can be called from
	// an iterator, calling delete on an iterator will only delete the iterator
	// calling empty from an iterator will delete the tree it's iterating.
	//////////////////////////////////////////////////////////////////////////
	void clear()
	{
		// Now get rid of our children -- but be smart about it,
		// right before we destroy it set it's out_ to NULL
		// that way disconnect_ fails immediately, much faster
		if (this->size() > 0) {
            tree *cur = this->in_, *prev = this->in_->prev_;

			// Delete the head node
			prev->out_ = NULL;
			delete prev;

			for (; this->size_ > 0; --this->size_) {

				prev = cur;
				cur = cur->next_;

				prev->out_ = NULL;
				delete prev;
			}

			// Set our inner pointer and our size to 0
			this->in_ = NULL;
			this->size_ = 0;
		}
	}

	T& operator*() { return this->data_; }
	const T& operator*() const { return this->data_; }
	T& data() { return this->data_; }
	const T& data() const { return this->data_; }

	const T& data(const T &inData) { return (this->data_ = inData); }

	//////////////////////////////////////////////////////////////////////////
	size_t level() const { return (this->level_); }

	//////////////////////////////////////////////////////////////////////////
	size_t size() const { return this->size_; }

	//////////////////////////////////////////////////////////////////////////
	size_t tree_size() const
   {
      return i_tree_size() + 1;
   }

	//////////////////////////////////////////////////////////////////////////
	iterator push_front(const T &inT)
	{
		tree *createdTree = new tree(inT);
		if (NULL == createdTree) throw "allocation failed";
		return iterator(i_push_front(createdTree, this));
	}

	//////////////////////////////////////////////////////////////////////////
	iterator push_back(const T &inT)
	{
		tree *createdTree = new tree(inT);
		if (NULL == createdTree) throw "allocation failed";
		return iterator(i_push_back(createdTree, this));
	}

   //////////////////////////////////////////////////////////////////////////
	// This creates a new tree node from parameters and then inserts it
	// Also takes a function object which can be used for sorting on inserts
	//////////////////////////////////////////////////////////////////////////
	iterator insert(const T &inT, bool (*pObj)(const T&, const T&))
	{
		tree *createdTree = new tree(inT);
		if (NULL == createdTree) throw "allocation failed";
		return iterator(i_insert(createdTree, this, pObj));
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	iterator insert(const iterator &i)
	{
		tree *createdTree = new tree(i.data());
		if (NULL == createdTree) throw "allocation failed";

		return iterator(i_insert(createdTree, this));
	}

	//////////////////////////////////////////////////////////////////////////
	// Insert with no function object
	//////////////////////////////////////////////////////////////////////////
	iterator insert(const T &inT)
	{
		tree *createdTree = new tree(inT);
		if (NULL == createdTree) throw "allocation failed";
		return iterator(i_insert(createdTree, this));
	}

	//////////////////////////////////////////////////////////////////////////
	// This takes an existing node, disconnects it from wherever it is, and then
	// inserts it into a different tree. This does not create a new node from the
	// passed in data.
	//////////////////////////////////////////////////////////////////////////
	iterator reinsert(tree *in, bool (*pObj)(const T&, const T&))
	{
		in->disconnect_();
		return iterator(i_insert(in, this, pObj));
	}

	//////////////////////////////////////////////////////////////////////////
	// Reinsert with no function object
	//////////////////////////////////////////////////////////////////////////
	iterator reinsert(tree *in)
	{
		in->disconnect_();
		return iterator(i_insert(in, this));
	}

	//////////////////////////////////////////////////////////////////////////
	// removes first matching t, returns true if found, otherwise false
	//////////////////////////////////////////////////////////////////////////
	bool remove(const T &inData)
	{
		if (tree *temp = this->in_) {
			do {
				if (inData == temp->data_) {
					delete temp;
					return true;
				}
			} while (NULL != (temp = temp->next_) );
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool erase(const iterator& i)
	{
		if (tree *temp = this->in_) {
			do {
				if (temp == i.tree_ptr()) {
					delete temp;
					return true;
				}
			} while (NULL != (temp = temp->next_) );
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	iterator operator[](size_t loc) const
	{
      tree *temp;
		for (temp = this->in_; loc > 0; --loc) temp = temp->next_;
		return iterator(*temp);
	}

	//////////////////////////////////////////////////////////////////////////
	iterator operator[](size_t loc)
	{
      tree *temp;
		for (temp = this->in_; loc > 0; --loc) temp = temp->next_;
		return iterator(*temp);
	}

	//////////////////////////////////////////////////////////////////////////
	iterator find(const T &inT) const
   { return find(inT, iterator(*this->in_)); }

	//////////////////////////////////////////////////////////////////////////
	iterator find(const T &inT, bool (*obj)(const T&, const T&)) const
   { return find(inT, iterator(*this->in_), obj); }

	//////////////////////////////////////////////////////////////////////////
   iterator tree_find_depth(const T &inT) const
   { return tree_find_depth(inT, iterator(*this->in_)); }

	//////////////////////////////////////////////////////////////////////////
	iterator tree_find_depth(const T &inT, bool (*obj)(const T&, const T&)) const
   { return tree_find_depth(inT, iterator(*this->in_), obj); }

	//////////////////////////////////////////////////////////////////////////
   iterator tree_find_breadth(const T &inT) const
   { return tree_find_breadth(inT, iterator(*this->in_)); }

	//////////////////////////////////////////////////////////////////////////
   iterator tree_find_breadth(const T &inT, bool (*obj)(const T&, const T&)) const
   { return tree_find_breadth(inT, iterator(*this->in_), obj); }

	//////////////////////////////////////////////////////////////////////////
	// internal_only interface, can't be called even with derived objects due
	// to its direct reference to tree's private members
	//////////////////////////////////////////////////////////////////////////
	iterator find(const T &inT, const iterator &iter) const
	{
		if (tree *temp = iter.tree_ptr()) {
			do {
				if (inT == temp->data_) return iterator(*temp);
			} while (NULL != (temp = temp->next_) );
		}
		return tree::iterator::end_iterator();
	}

	//////////////////////////////////////////////////////////////////////////
	iterator find(const T &inT, const iterator &iter, bool (*obj)(const T&, const T&)) const
	{
		if (tree *temp = iter.tree_ptr()) {
			do {
				if ( obj(inT, temp->data_) ) return ( iterator(*temp) );
			} while (NULL != (temp = temp->next_) );
		}
		return tree::iterator::end_iterator();
	}

	//////////////////////////////////////////////////////////////////////////
	iterator tree_find_depth(const T &inT, const iterator &iter) const
	{
		if (tree *temp = iter.tree_ptr()) {
			do {
				if (inT == temp->data_) return iterator(*temp);
            // do a depth search, search it for inT
            iterator i = temp->tree_find_depth(inT);
            if (i != tree::iterator::end_iterator()) return i;
			} while (NULL != (temp = temp->next_) );
		}
		return tree::iterator::end_iterator();
	}

	//////////////////////////////////////////////////////////////////////////
	iterator tree_find_depth(const T &inT, const iterator &iter, bool (*obj)(const T&, const T&)) const
	{
		if (tree *temp = iter.tree_ptr()) {
			do {
				if ( obj(inT, temp->data_) ) return ( iterator(*temp) );
            // do a depth search, search it for inT
            iterator i = temp->tree_find_depth(inT, obj);
            if (i != tree::iterator::end_iterator()) return i;
			} while (NULL != (temp = temp->next_) );
		}
		return tree::iterator::end_iterator();
	}

	//////////////////////////////////////////////////////////////////////////
   iterator tree_find_breadth(const T &inT, const iterator &iter) const
   {
      // search the entire level for a find first
		if (tree *temp = iter.tree_ptr()) {
			do {
				if (inT == temp->data_) return iterator(*temp);
			} while (NULL != (temp = temp->next_) );

         // now search each branch for the find within it
         temp = iter.tree_ptr();
			do {
            iterator i = temp->tree_find_breadth(inT);
            if (i != tree::iterator::end_iterator()) return i;
			} while (NULL != (temp = temp->next_) );
		}
		return tree::iterator::end_iterator();
   }

	//////////////////////////////////////////////////////////////////////////
   iterator tree_find_breadth(const T &inT, const iterator &iter, bool (*obj)(const T&, const T&)) const
   {
      // search the entire level for a find first
		if (tree *temp = iter.tree_ptr()) {
			do {
				if ( obj(inT, temp->data_) ) return iterator(*temp);
			} while (NULL != (temp = temp->next_) );

         // now search each branch for the find within it
         temp = iter.tree_ptr();
			do {
            iterator i = temp->tree_find_breadth(inT, obj);
            if (i != tree::iterator::end_iterator()) return i;
			} while (NULL != (temp = temp->next_) );
		}
		return tree::iterator::end_iterator();
   }
};

/////////////////////////////////////////////////////////////////////////////
// Iterator for the tree
//
// Derived from tree<> only so iterator can access tree's protected
// methods directly and implement them in the way they make sense for the
// iterator
//
// The actual tree base members are never used (nor could they be since they
// are private to even iterator). When a tree object is created an "iterator"
// object is automatically created of the specific type. Thus forming the
// perfect relationship between the tree and the iterator, also keeping the
// template types defined on the fly for the iterator based specifically on
// the tree types which are being created.
/////////////////////////////////////////////////////////////////////////////
template <typename T>
class tree_iterator : private tree<T>
{
private:
	typedef tree<T> TreeType;

	mutable TreeType *current_;

	static tree_iterator end_of_iterator;

	//////////////////////////////////////////////////////////////////////////
	// unaccessible from the outside world
	//////////////////////////////////////////////////////////////////////////
	TreeType* operator&();
	const TreeType* operator&() const;

public:

	TreeType* tree_ptr() const { return current_; }
	TreeType& tree_ref() const { return *current_; }

	//////////////////////////////////////////////////////////////////////////
	// Returns the end_of_iterator for this <T,U,V> layout, this really speeds
	// up things like if (iter != tree.end() ), for (;iter != tree.end(); )
	//////////////////////////////////////////////////////////////////////////
	static const typename tree<T>::iterator& end_iterator() { return end_of_iterator; }

	//////////////////////////////////////////////////////////////////////////
	// Default constructor
	//////////////////////////////////////////////////////////////////////////
	tree_iterator() : current_(NULL) {}

	//////////////////////////////////////////////////////////////////////////
	// Copy constructors for iterators
	//////////////////////////////////////////////////////////////////////////
	tree_iterator(const tree_iterator& i) : current_(i.current_) {}

	//////////////////////////////////////////////////////////////////////////
	// Copy constructor for trees
	//////////////////////////////////////////////////////////////////////////
	tree_iterator(TreeType &tree_ref) : current_(&tree_ref) {}

	//////////////////////////////////////////////////////////////////////////
	// Operator= for iterators
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator& operator=(const tree_iterator& iter)
	{
		this->current_ = iter.current_;
		return (*this);
	}

	//////////////////////////////////////////////////////////////////////////
	// Operator= for iterators
	//////////////////////////////////////////////////////////////////////////
	const typename tree<T>::iterator& operator=(const tree_iterator& iter) const
	{
		this->current_ = iter.current_;
		return (*this);
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator operator[](size_t loc) const
	{ return (*this->current_)[loc]; }

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator operator[](size_t loc)
	{ return (*this->current_)[loc]; }

	//////////////////////////////////////////////////////////////////////////
	// Operator= for trees
	//////////////////////////////////////////////////////////////////////////
	const typename tree<T>::iterator& operator=(const TreeType& rhs) const
	{
		this->current_ = &(const_cast< TreeType& >(rhs) );
		return (*this);
	}

	//////////////////////////////////////////////////////////////////////////
	// Destructor
	//////////////////////////////////////////////////////////////////////////
	~tree_iterator() {};

	//////////////////////////////////////////////////////////////////////////
	// Operator equals
	//////////////////////////////////////////////////////////////////////////
	bool operator==(const tree_iterator& rhs) const
	{
		if (this->current_ == rhs.current_) return true;
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// Operator not equals
	//////////////////////////////////////////////////////////////////////////
	bool operator!=(const tree_iterator& rhs) const
	{ return !(*this == rhs); }

	//////////////////////////////////////////////////////////////////////////
	// operator++, prefix
	//////////////////////////////////////////////////////////////////////////
	const typename tree<T>::iterator& operator++() const
	{
		this->current_ = ( const_cast< TreeType* >
			( this->TreeType::next( *current_ ) ) );
		return (*this);
	}

	//////////////////////////////////////////////////////////////////////////
	// operator++, postfix
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator operator++(int) const
	{
		typename tree<T>::iterator iTemp = *this;
		++(*this);
		return (iTemp);
	}

	//////////////////////////////////////////////////////////////////////////
	// operator--
	//////////////////////////////////////////////////////////////////////////
	const typename tree<T>::iterator& operator--() const
	{
		this->current_ = ( const_cast< TreeType* >
			( this->TreeType::prev( *current_ ) ) );
		return (*this);
	}

	//////////////////////////////////////////////////////////////////////////
	// Begin iteration through the tree
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator begin() const { return this->TreeType::begin( *current_ ); }
	typename tree<T>::iterator begin() { return this->TreeType::begin( *current_ ); }

	//////////////////////////////////////////////////////////////////////////
	// Return the in iterator of this tree
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator in() const { return this->TreeType::in( *current_ ); }
	typename tree<T>::iterator in() { return this->TreeType::in( *current_ ); }

	//////////////////////////////////////////////////////////////////////////
	// Return the out iterator of this tree
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator out() const { return this->TreeType::out( *current_ ); }
	typename tree<T>::iterator out() { return this->TreeType::out( *current_ ); }

	//////////////////////////////////////////////////////////////////////////
	// Are we at the end?
	//////////////////////////////////////////////////////////////////////////
	const typename tree<T>::iterator& end() const { return this->TreeType::end(); }

	//////////////////////////////////////////////////////////////////////////
	// Return the next guy
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator next() const
	{ return typename tree<T>::iterator (* const_cast< TreeType* >( this->TreeType::next( *current_ ) ) ); }

	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator push_back(const T& t)
	{ return this->current_->TreeType::push_back(t); }

	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator push_front(const T& t)
	{ return this->current_->TreeType::push_front(t); }

	//////////////////////////////////////////////////////////////////////////
	// Insert into the iterator's mTree
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator insert(const T& t)
	{ return this->current_->TreeType::insert(t); }

	//////////////////////////////////////////////////////////////////////////
	// Insert into the iterator's mTree (with a function object)
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator insert(const T& t, bool (*obj)(const T&, const T&))
	{ return this->current_->TreeType::insert(t, obj); }

	//////////////////////////////////////////////////////////////////////////
	// This takes an existing node, disconnects it from wherever it is, and then
	// inserts it into a different tree. This does not create a new node from the
	// passed in data.
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator reinsert(const typename tree<T>::iterator &in, bool (*obj)(const T&, const T&))
	{ return this->current_->TreeType::reinsert(in.current_, obj); }

	//////////////////////////////////////////////////////////////////////////
	// Reinsert with no function object
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator reinsert(const typename tree<T>::iterator &in)
	{ return this->current_->TreeType::reinsert(in.current_); }

	//////////////////////////////////////////////////////////////////////////
	// get the data of the iter
	//////////////////////////////////////////////////////////////////////////
	T& operator*() { return this->current_->data(); }
	const T& operator*() const { return this->current_->data(); }
	T& data() { return this->current_->data(); }
	const T& data() const { return this->current_->data(); }

	//////////////////////////////////////////////////////////////////////////
	// sets and retrieves the t and u members of the pair
	//////////////////////////////////////////////////////////////////////////
	const T& data(const T &inData) const { return this->current_->data(inData); }

	//////////////////////////////////////////////////////////////////////////
	// Get the size of the current tree_iter
	//////////////////////////////////////////////////////////////////////////
	size_t size() const { return this->TreeType::size( *current_ ); }

	//////////////////////////////////////////////////////////////////////////
	size_t level() const { return this->TreeType::level( *current_ ); }

	//////////////////////////////////////////////////////////////////////////
	// Removes the first instance of T in the tree
	//////////////////////////////////////////////////////////////////////////
	bool remove(const T &inT) { return current_->remove(inT); }

	//////////////////////////////////////////////////////////////////////////
	// Finds the first instance of T in the tree
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator find(const T &inT) const { return current_->find(inT); }

	typename tree<T>::iterator find(const T &inT, bool (*obj)(const T&, const T&)) const
	{ return current_->find(inT, obj); }

	typename tree<T>::iterator tree_find_depth(const T &inT) const { return current_->tree_find_depth(inT); }

	typename tree<T>::iterator tree_find_depth(const T &inT, bool (*obj)(const T&, const T&)) const
	{ return current_->tree_find_depth(inT, obj); }

	typename tree<T>::iterator tree_find_breadth(const T &inT) const { return current_->tree_find_breadth(inT); }

	typename tree<T>::iterator tree_find_breadth(const T &inT, bool (*obj)(const T&, const T&)) const
	{ return current_->tree_find_breadth(inT, obj); }

	//////////////////////////////////////////////////////////////////////////
	// Finds the next instance of T based on the iterator passed in
	//////////////////////////////////////////////////////////////////////////
	typename tree<T>::iterator find(const T &inT, const typename tree<T>::iterator &iter) const
	{ return current_->find(inT, iter); }

	typename tree<T>::iterator find(const T &inT, const typename tree<T>::iterator &iter,
		bool (*obj)(const T&, const T&)) const
	{ return current_->find( inT, iter, obj ); }

	typename tree<T>::iterator tree_find_depth(const T &inT, const typename tree<T>::iterator &iter) const
	{ return current_->tree_find_depth(inT, iter); }

	typename tree<T>::iterator tree_find_depth(const T &inT, const typename tree<T>::iterator &iter,
		bool (*obj)(const T&, const T&)) const
	{ return current_->tree_find_depth( inT, iter, obj ); }

	typename tree<T>::iterator tree_find_breadth(const T &inT, const typename tree<T>::iterator &iter) const
	{ return current_->tree_find_breadth(inT, iter); }

	typename tree<T>::iterator tree_find_breadth(const T &inT, const typename tree<T>::iterator &iter,
		bool (*obj)(const T&, const T&)) const
	{ return current_->tree_find_breadth( inT, iter, obj ); }

	//////////////////////////////////////////////////////////////////////////
	// Empty this entire tree
	//////////////////////////////////////////////////////////////////////////
	void clear_tree() { delete this->current_; this->current_ = NULL; }

	//////////////////////////////////////////////////////////////////////////
	// Empty this tree's children
	//////////////////////////////////////////////////////////////////////////
	void clear_children() { this->current_->clear(); }
};

//////////////////////////////////////////////////////////////////////////
// Static iterator initialization
//////////////////////////////////////////////////////////////////////////
template <typename T>
tree_iterator<T> tree_iterator<T>::end_of_iterator;

};

#if WIN32
#pragma warning( pop )
#endif // WIN32

#endif // tree_header_file
