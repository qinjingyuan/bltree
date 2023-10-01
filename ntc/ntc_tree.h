#pragma one
#include<bits/stdc++.h>




namespace ntc{

#define NEWTREE ntc_tree



template<typename Key_t, typename Value_t, typename Compare>
class NEWTREE{
  struct node{
    /// Level in the b-tree, if level == 0 -> leaf node
    unsigned short level;

    /// Number of key slotuse use, so number of valid children or data
    /// pointers
    unsigned short slotuse;

    /// Delayed initialisation of constructed node
    inline node(const unsigned short l, const unsigned short s = 0)
        : level(l), slotuse(s)
    { }

    /// True if this is a leaf node
    inline bool    isleafnode() const
    {
        return (level == 0);
    }
  };

/// Extended structure of a inner node in-memory. Contains only keys and no
/// data items.
  struct inner_node : public node{
    /// Define an related allocator for the inner_node structs.
    typedef typename _Alloc::template rebind<inner_node>::other alloc_type;

    /// Keys of children or data pointers
    key_type slotkey[innerslotmax];

    /// Pointers to children
    node     * childid[innerslotmax + 1];

    /// Set variables to initial values
    inline inner_node(const unsigned short l)
        : node(l)
    { }

    /// Construction during restore from node top
    inline inner_node(const node& top)
        : node(top.level, top.slotuse)
    { }

    /// True if the node's slots are full
    inline bool isfull() const
    {
        return (node::slotuse == innerslotmax);
    }

    /// True if few used entries, less than half full
    inline bool isfew() const
    {
        return (node::slotuse <= mininnerslots);
    }

    /// True if node has too few entries
    inline bool isunderflow() const
    {
        return (node::slotuse < mininnerslots);
    }

    inline key_type minkey() const {
        return slotkey[0];
    }
    inline key_type maxkey() const {
        return slotkey[node::slotuse-1];
    }
  };

/// Extended structure of a leaf node in memory. Contains pairs of keys and
/// data items. Key and data slots are kept in separate arrays, because the
/// key array is traversed very often compared to accessing the data items.
  struct leaf_node : public node{
    /// Define an related allocator for the leaf_node structs.
    typedef typename _Alloc::template rebind<leaf_node>::other alloc_type;

    /// Double linked list pointers to traverse the leaves
    leaf_node * prevleaf;

    /// Double linked list pointers to traverse the leaves
    leaf_node * nextleaf;

    /// Keys of children or data pointers
    key_type  slotkey[leafslotmax];

    /// Array of data
    data_type slotdata[used_as_set ? 1 : leafslotmax];

    /// Set variables to initial values
    inline leaf_node()
        : node(0), prevleaf(NULL), nextleaf(NULL)
    { }

    /// Construction during restore from node top
    inline leaf_node(const node& top)
        : node(top.level, top.slotuse), prevleaf(NULL), nextleaf(NULL)
    { }

    /// True if the node's slots are full
    inline bool isfull() const
    {
        return (node::slotuse == leafslotmax);
    }

    /// True if few used entries, less than half full
    inline bool isfew() const
    {
        return (node::slotuse <= minleafslots);
    }

    /// True if node has too few entries
    inline bool isunderflow() const
    {
        return (node::slotuse < minleafslots);
    }

    /// Set the (key,data) pair in slot. Overloaded function used by
    /// bulk_load().
    inline void set_slot(unsigned short slot, const pair_type& value)
    {
        BTREE_ASSERT(used_as_set == false);
        BTREE_ASSERT(slot < node::slotuse);
        slotkey[slot] = value.first;
        slotdata[slot] = value.second;
    }

    /// Set the key pair in slot. Overloaded function used by
    /// bulk_load().
    inline void set_slot(unsigned short slot, const key_type& key)
    {
        BTREE_ASSERT(used_as_set == true);
        BTREE_ASSERT(slot < node::slotuse);
        slotkey[slot] = key;
    }


    inline key_type minkey() const {
        return slotkey[0];
    }
    inline key_type maxkey() const {
        return slotkey[node::slotuse-1];
    }
  };

  base_node<Key_t,Value_t> * root_;
  leaf_node<Key_t,Value_t> * m_headleaf;
  leaf_node<Key_t,Value_t> * m_tailleaf;
  Compare key_less;


public:
  NEWTREE():root_(NULL),m_headleaf(NULL),m_tailleaf(NULL) {

  }

  int find_lower(node * n, Key_t key){
    if ( 0 && sizeof(n->slotkey) > 64 )
    {
        if (n->slotuse == 0) return 0;

        int lo = 0, hi = n->slotuse;

        while (lo < hi)
        {
            int mid = (lo + hi) >> 1;

            if (key_lessequal(key, n->slotkey[mid])) {
                hi = mid; // key <= mid
            }
            else {
                lo = mid + 1; // key > mid
            }
        }

        BTREE_PRINT("btree::find_lower: on " << n << " key " << key << " -> " << lo << " / " << hi);

        // verify result using simple linear search
        if (selfverify)
        {
            int i = 0;
            while (i < n->slotuse && key_less(n->slotkey[i],key)) ++i;

            BTREE_PRINT("btree::find_lower: testfind: " << i);
            BTREE_ASSERT(i == lo);
        }

        return lo;
    }
    else // for nodes <= binsearch_threshold do linear search.
    {
        int lo = 0;
        while (lo < n->slotuse && key_less(n->slotkey[lo],key)) ++lo;
        return lo;
    }

  }


    void split_leaf_node(leaf_node* leaf, key_type* _newkey, node** _newleaf)
    {
        BTREE_ASSERT(leaf->isfull());

        unsigned int mid = (leaf->slotuse >> 1);

        BTREE_PRINT("btree::split_leaf_node on " << leaf);

        leaf_node* newleaf = allocate_leaf();

        newleaf->slotuse = leaf->slotuse - mid;

        newleaf->nextleaf = leaf->nextleaf;
        if (newleaf->nextleaf == NULL) {
            BTREE_ASSERT(leaf == m_tailleaf);
            m_tailleaf = newleaf;
        }
        else {
            newleaf->nextleaf->prevleaf = newleaf;
        }

        std::copy(leaf->slotkey + mid, leaf->slotkey + leaf->slotuse,
                  newleaf->slotkey);
        data_copy(leaf->slotdata + mid, leaf->slotdata + leaf->slotuse,
                  newleaf->slotdata);

        leaf->slotuse = mid;
        leaf->nextleaf = newleaf;
        newleaf->prevleaf = leaf;

        *_newkey = leaf->slotkey[leaf->slotuse - 1];
        *_newleaf = newleaf;
    }



  auto split_inner_node(inner_node* inner, Key_t& newkey, node* &, uint16_t addslot) -> bool{
    uint16_t mid = (inner->slotuse >> 1);
    if (addslot <= mid && mid > inner->slotuse - (mid + 1))
        mid--;
    inner_node *newinner = new inner_node(inner->level);

    newinner->slotuse = inner->slotuse - (mid + 1);

    std::copy(inner->keys + mid+1, inner->keys + inner->slotuse, newinner->keys);
    std::copy(inner->values + mid+1, inner->values + inner->slotuse+1, newinner->values);

    inner->slotuse = mid;
    // get min key in node.
    *_newkey = inner->slotkey[mid];
    *_newinner = newinner;

  }

  auto insert_descend(node* n, Key_t key, Value_t value, Key_t& splitkey, base_node* & splitnode) -> bool {
    if(!n->isleafnode){
      Key_t newkey;
      inner_node * newvalue = NULL;
      inner_node * inner = static_cast<inner_node*>(root_);
      int slot = find_lower(inner, key);

      insert_descend(inner->values[slot], key, value, newkey, newvalue);

      // if lower layer split, return the new node.  
      if(nv){
        // if upper layer node is full, split it.
        if(inner->isfull()){
          split_inner_node(inner, splitkey, splitnode, slot);
          if (slot == inner->slotuse + 1 && inner->slotuse < (*splitnode)->slotuse) {
              inner_node* splitinner = static_cast<inner_node*>(*splitnode);
              inner->keys[inner->slotuse] = *splitkey;
              inner->values[inner->slotuse + 1] = splitinner->childid[0];
              inner->slotuse++;
              splitinner->childid[0] = newchild;
              *splitkey = newkey;
              return r;
          }
          else if (slot >= inner->slotuse + 1)
          {
              slot -= inner->slotuse + 1;
              inner = static_cast<inner_node*>(*splitnode);
              BTREE_PRINT("btree::insert_descend switching to splitted node " << inner << " slot " << slot);
          }
        }

      }


    }else{
      leaf_node* leaf = static_cast<leaf_node*>(n);

      int slot = find_lower(leaf, key);

      if (!allow_duplicates && slot < leaf->slotuse && key_equal(key, leaf->slotkey[slot])) {
          return std::pair<iterator, bool>(iterator(leaf, slot), false);
      }

      if (leaf->isfull())
      {
          split_leaf_node(leaf, splitkey, splitnode);

          // check if insert slot is in the split sibling node
          if (slot >= leaf->slotuse)
          {
              slot -= leaf->slotuse;
              leaf = static_cast<leaf_node*>(*splitnode);
          }
      }

      // move items and put data item into correct data slot
      BTREE_ASSERT(slot >= 0 && slot <= leaf->slotuse);

      std::copy_backward(leaf->slotkey + slot, leaf->slotkey + leaf->slotuse,
                          leaf->slotkey + leaf->slotuse + 1);
      data_copy_backward(leaf->slotdata + slot, leaf->slotdata + leaf->slotuse,
                          leaf->slotdata + leaf->slotuse + 1);

      leaf->slotkey[slot] = key;
      if (!used_as_set) leaf->slotdata[slot] = value;
      leaf->slotuse++;

      if (splitnode && leaf != *splitnode && slot == leaf->slotuse - 1)
      {
          // special case: the node was split, and the insert is at the
          // last slot of the old node. then the splitkey must be
          // updated.
          *splitkey = key;
      }

      return std::pair<iterator, bool>(iterator(leaf, slot), true);
    }
  }

  auto insert(Key_t key,Value_t value) -> bool{
    node* newchild = NULL;
    Key_t newkey;

    if (m_root == NULL) {
        m_root = m_headleaf = m_tailleaf = allocate_leaf();
    }
    assert(insert_descend(m_root, key, value, &newkey, &newchild));
    
    if (newchild)
    {
        inner_node* newroot = allocate_inner(m_root->level + 1);
        newroot->slotkey[0] = newkey;

        newroot->childid[0] = m_root;
        newroot->childid[1] = newchild;

        newroot->slotuse = 1;

        m_root = newroot;
    }

    return true;
  };

  auto find(Key_t key, std::pair<Key_t,Value_t>& res ) -> bool {
    base_node* n = root_;
    if(n == NULL) return false;

    while(n->level != 0){
      inner_node *inner = static_cast<inner_node*>(n);
      int slot = find_lower(inner, key);
      n = inner->values[slot];
    }
    leaf_node* leaf = static_cast<leaf_node*>(n);
    int slot = find_lower(leaf, key);



    if(leaf->keys[slot] != key){
      return false;
    }
    res.first = key;
    res.second = leaf->values[slot];
    return true;
  }

  size_t erase_one(){

  }


  size_t erase(){


  }


  uint16_t slot get_site(uint16_t fa, uint16_t fb, uint16_t fc, Key_t key){
    Key_t tmp = key - fc;
    tmp = tmp / a;
    tmp = tmp; // 开方
    tmp = tmp - fb;

    return static_cast<uint16_t>(tmp);
  }






};







};