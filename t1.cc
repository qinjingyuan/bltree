
// template <typename _Key, typename _Data>
class btree
{
    struct node1
    {
        public:
        /// Level in the b-tree, if level == 0 -> leaf node
        unsigned short level;

        /// Number of key slotuse use, so number of valid children or data
        /// pointers
        unsigned short slotuse;

        // Polynomial variable
        uint16_t fa(0);
        uint16_t fb(0);
        uint16_t fc(0);
        uint16_t gap_ratio(0);
        uint16_t insert_count(0);
        uint16_t delete_count(0);

        // /// Delayed initialisation of constructed node
        // inline node(const unsigned short l, const unsigned short s = 0)
        //     : level(l), slotuse(s)
        // { }

        // /// True if this is a leaf node
        // inline bool    isleafnode() const
        // {
        //     return (level == 0);
        // }
    };


    /// Extended structure of a inner node in-memory. Contains only keys and no
    /// data items.
    struct inner_node : public node1
    {

    };

    /// Extended structure of a leaf node in memory. Contains pairs of keys and
    /// data items. Key and data slots are kept in separate arrays, because the
    /// key array is traversed very often compared to accessing the data items.
    struct leaf_node : public node1
    {
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


    void t1(){
        leaf_node* n;
        n->
    }
};



class C{

    struct A {
        int a;
    };

    struct B:public A{
        int b;
    };

    C(){
        B *b;   
        b-

    }
    



};
