// #include "btree.h"
#include <algorithm>
#include<bitset>








/** Generates default traits for a B+ tree used as a map. It estimates leaf and
 * inner node sizes by assuming a cache line size of 256 bytes. */
template <typename _Key, typename _Data>
class btree_default_map_traits
{
public:
    /// If true, the tree will self verify it's invariants after each insert()
    /// or erase(). The header must have been compiled with BTREE_DEBUG defined.
    static const bool selfverify = false;

    /// If true, the tree will print out debug information and a tree dump
    /// during insert() or erase() operation. The header must have been
    /// compiled with BTREE_DEBUG defined and key_type must be std::ostream
    /// printable.
    static const bool debug = false;

    /// Number of slots in each leaf of the tree. Estimated so that each node
    /// has a size of about 256 bytes.
    static const int leafslots = BTREE_MAX(8, 256 / (sizeof(_Key) + sizeof(_Data)));

    /// Number of slots in each inner node of the tree. Estimated so that each node
    /// has a size of about 256 bytes.
    static const int innerslots = BTREE_MAX(8, 256 / (sizeof(_Key) + sizeof(void*)));

    /// As of stx-btree-0.9, the code does linear search in find_lower() and
    /// find_upper() instead of binary_search, unless the node size is larger
    /// than this threshold. See notes at
    /// http://panthema.net/2013/0504-STX-B+Tree-Binary-vs-Linear-Search
    static const size_t binsearch_threshold = 32;
};









template <typename _Key, typename _Data,
          typename _Value = std::pair<_Key, _Data>,
          typename _Compare = std::less<_Key>,
          typename _Traits = btree_default_map_traits<_Key, _Data>,
          bool _Duplicates = false,
          typename _Alloc = std::allocator<_Value>,
          bool _UsedAsSet = false>
class bltree
{
public:
    int btree_level = 0;
    typedef _Key key_type;
    typedef _Data data_type;
    typedef _Value value_type;
    typedef _Compare key_compare;
    typedef _Traits traits;
    static const bool allow_duplicates = _Duplicates;
    typedef _Alloc allocator_type;
    static const bool used_as_set = _UsedAsSet;

    typedef bltree<key_type, data_type, value_type, key_compare, traits, allow_duplicates,
        allocator_type, used_as_set> self_type;
    typedef size_t size_type;
    typedef std::pair<key_type, value_type> pair_type;
    static const unsigned short leafslotmax = traits::leafslots;
    static const unsigned short innerslotmax = traits::innerslots;
    static const unsigned short minleafslots = (leafslotmax/2);
    static const unsigned short mininnerslots = (innerslotmax/2);
    static const bool selfverify = traits::selfverify;
    static const bool debug = traits::debug;
    enum modelType  {LINE=0,BINOMIAL,GENERAL,X4,SINX001,X2,LINE_X,X3,X2R,X2D,LINE_EXP, GAPX6,GAPX2,GAPX3,LINE_INT,S1,S2,TU,AO,SX,LN};


private:

    struct node{
        unsigned short level;
        unsigned short slotuse;
        modelType model_type = modelType::GENERAL;
        double fk[3];
        double fb[3];
        std::bitset<leafslotmax+1> bs;

        node (const unsigned short l, const unsigned short s = 0)
            : level(l),slotuse(s) {
                std::fill(fk,fk+3,0);
                std::fill(fb,fb+3,0);
            }

        bool isleafnode() const {
            return level == 0;
        }
        
    };

    struct inner_node:public node{
        key_type  slotkey[innerslotmax];
        node * childid[innerslotmax+1];

        inner_node(const unsigned short l):node(l){}

        inner_node(const node& top) : node(top.level,top.slotuse){}

        bool isfull() const{
            return node::slotuse == innerslotmax;
        }

        bool isfew() const{
            return node::slotuse < mininnerslots;
        }

        bool isunderflow() const{
            return node::slotuse < mininnerslots;
        }

        key_type minkey()const {
            return slotkey[0];
        }

        key_type maxkey() const {
            return slotkey[node::slotuse-1];
        }
    };

    struct leaf_node : public node{
        leaf_node * prevleaf;
        leaf_node * nextleaf;

        key_type slotkey[leafslotmax];
        data_type slotdata[used_as_set ? 1 : leafslotmax];
        leaf_node() : node(0) , prevleaf(nullptr), nextleaf(nullptr) {}
        leaf_node(const node& top) : node(top.level, top.slotuse), prevleaf(nullptr), nextleaf(nullptr);

        bool isfull() const {
            return node::slotuse == leafslotmax;
        }

        bool isfew()const{
            return node::slotuse <= minleafslots;
        }

        bool isunderflow()const{
            return node::slotuse < minleafslots;
        }


        void set_slot(unsigned short slot, const pair_type& value){
            BTREE_ASSERT(used_as_set == false);
            BTREE_ASSERT(slot < node::slotuse);
            slotkey[slot] = value.first;
            slotdata[slot] = value.second;
        }

        key_type minkey() const {
            return slotkey[0];
        }

        key_type maxkey() const {
            return slotkey[node::slotuse-1];
        }

    };

public:
    struct tree_stats{
        size_type itemcount;
        size_type leaves;
        size_type innernodes;
        static const unsigned short leafslots = self_type::leafslotmax;
        static const unsigned short innerslots = self_type::innerslotmax;

        tree_stats() : itemcount(0), leaves(0), innernodes(0){}
        size_type nodes() const {
            return innernodes + leaves;
        }

        double avgfill_leaves() const{
            return static_cast<double>(itemcount) / (leaves * leafslots);
        }
    };


    


private:
    node* m_root;
    leaf_node* m_headleaf;
    leaf_node* m_tailleaf;
    tree_stats m_stats;
    key_compare m_key_less;
    
public:
    template <class InputIterator>
    inline btree(InputIterator first, InputIterator last,
                 const allocator_type& alloc = allocator_type())
        : m_root(NULL), m_headleaf(NULL), m_tailleaf(NULL), m_allocator(alloc)
    {
        insert(first, last);
    }

    inline ~btree()
    {
        clear();
    }


private:
    // *** Convenient Key Comparison Functions Generated From key_less

    /// True if a < b ? "constructed" from m_key_less()
    inline bool key_less(const key_type& a, const key_type& b) const
    {
        return m_key_less(a, b);
    }

    /// True if a <= b ? constructed from key_less()
    inline bool key_lessequal(const key_type& a, const key_type& b) const
    {
        return !m_key_less(b, a);
    }

    /// True if a > b ? constructed from key_less()
    inline bool key_greater(const key_type& a, const key_type& b) const
    {
        return m_key_less(b, a);
    }

    /// True if a >= b ? constructed from key_less()
    inline bool key_greaterequal(const key_type& a, const key_type& b) const
    {
        return !m_key_less(a, b);
    }

    /// True if a == b ? constructed from key_less(). This requires the <
    /// relation to be a total order, otherwise the B+ tree cannot be sorted.
    inline bool key_equal(const key_type& a, const key_type& b) const
    {
        return !m_key_less(a, b) && !m_key_less(b, a);
    }

public:
    // *** Allocators

    /// Return the base node allocator provided during construction.
    allocator_type get_allocator() const
    {
        return m_allocator;
    }


private:
    // *** Node Object Allocation and Deallocation Functions

    /// Return an allocator for leaf_node objects
    typename leaf_node::alloc_type leaf_node_allocator()
    {
        return typename leaf_node::alloc_type(m_allocator);
    }

    /// Return an allocator for inner_node objects
    typename inner_node::alloc_type inner_node_allocator()
    {
        return typename inner_node::alloc_type(m_allocator);
    }

    /// Allocate and initialize a leaf node
    inline leaf_node * allocate_leaf()
    {
        leaf_node* n = new (leaf_node_allocator().allocate(1))
                       leaf_node();
        m_stats.leaves++;
        return n;
    }

    /// Allocate and initialize an inner node
    inline inner_node * allocate_inner(unsigned short level)
    {
        inner_node* n = new (inner_node_allocator().allocate(1))
                        inner_node(level);
        m_stats.innernodes++;
        return n;
    }

    /// Correctly free either inner or leaf node, destructs all contained key
    /// and value objects
    inline void free_node(node* n)
    {
        if (n->isleafnode()) {
            leaf_node* ln = static_cast<leaf_node*>(n);
            typename leaf_node::alloc_type a(leaf_node_allocator());
            a.destroy(ln);
            a.deallocate(ln, 1);
            m_stats.leaves--;
        }
        else {
            inner_node* in = static_cast<inner_node*>(n);
            typename inner_node::alloc_type a(inner_node_allocator());
            a.destroy(in);
            a.deallocate(in, 1);
            m_stats.innernodes--;
        }
    }

    /// Convenient template function for conditional copying of slotdata. This
    /// should be used instead of std::copy for all slotdata manipulations.
    template <class InputIterator, class OutputIterator>
    static OutputIterator data_copy(InputIterator first, InputIterator last,
                                    OutputIterator result)
    {
        if (used_as_set) return result; // no operation
        else return std::copy(first, last, result);
    }

    /// Convenient template function for conditional copying of slotdata. This
    /// should be used instead of std::copy for all slotdata manipulations.
    template <class InputIterator, class OutputIterator>
    static OutputIterator data_copy_backward(InputIterator first, InputIterator last,
                                             OutputIterator result)
    {
        if (used_as_set) return result; // no operation
        else return std::copy_backward(first, last, result);
    }



public:
    // *** Fast Destruction of the B+ Tree

    /// Frees all key/data pairs and all nodes of the tree
    void clear()
    {
        if (m_root)
        {
            clear_recursive(m_root);
            free_node(m_root);

            m_root = NULL;
            m_headleaf = m_tailleaf = NULL;

            m_stats = tree_stats();
        }

        BTREE_ASSERT(m_stats.itemcount == 0);
    }

private:
    /// Recursively free up nodes
    void clear_recursive(node* n)
    {
        if (n->isleafnode())
        {
            leaf_node* leafnode = static_cast<leaf_node*>(n);

            for (unsigned int slot = 0; slot < leafnode->slotuse; ++slot)
            {
                // data objects are deleted by leaf_node's destructor
            }
        }
        else
        {
            inner_node* innernode = static_cast<inner_node*>(n);

            for (unsigned short slot = 0; slot < innernode->slotuse + 1; ++slot)
            {
                clear_recursive(innernode->childid[slot]);
                free_node(innernode->childid[slot]);
            }
        }
    }
public:




        if(n->model_type == modelType::GENERAL){
            return find_lower(n, key);
        }

        if (n->slotuse == 0) return 0;
        int lo = 0, hi = n->slotuse;
        int pre_target,point;
        int pt0,pt1,pt2;

#ifdef MUL_TIME
        auto currentTime1 = std::chrono::high_resolution_clock::now();
#endif

        // std::cout << n->fk[0] << " " << n->fk[1] << " " << n->fk[2] << " \n";
        // std::cout << n->fb[0] << " " << n->fb[1] << " " << n->fb[2] << " \n";
        // std::cout << pt0 << " " << pt1 << " " << pt2 << " \n";

        pt0 = static_cast<int>(n->fk[0] * static_cast<double>(key) + n->fb[0]);

        switch (n->model_type)
        {
        case modelType::LINE:
            pre_target = pt0;
            break;
        
        case modelType::S1:
            // small big
            pt1 = static_cast<int>(n->fk[1] * static_cast<double>(key) + n->fb[1]);
            pt2 = static_cast<int>(n->fk[2] * static_cast<double>(key) + n->fb[2]);
            pre_target = std::max(std::min(pt0,pt1),pt2);
            // pre_target = getSecond(pt0,pt1,pt2);
            break;
        
        case modelType::S2:
            // small big
            pt1 = static_cast<int>(n->fk[1] * static_cast<double>(key) + n->fb[1]);
            pt2 = static_cast<int>(n->fk[2] * static_cast<double>(key) + n->fb[2]);
            pre_target = std::min(std::max(pt0,pt1),pt2);
            // pre_target = getSecond(pt0,pt1,pt2);
            break;
        
        case modelType::AO:
            pt1 = static_cast<int>(n->fk[1] * static_cast<double>(key) + n->fb[1]);
            pre_target = std::max(pt0,pt1);
            break;
        
        case modelType::TU:
            pt1 = static_cast<int>(n->fk[1] * static_cast<double>(key) + n->fb[1]);
            pre_target = std::min(pt0,pt1);
            break;
        
        default:
            pre_target = pt0;
            break;
        }

        // pre_target = pt0;

        point = pre_target = std::min(std::max(pre_target,lo),hi-1);
#ifdef MUL_TIME
        auto currentTime2 = std::chrono::high_resolution_clock::now();
        auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
        auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
        mul_times[btree_level] +=  (nanoseconds2 - nanoseconds1-21);
        mul_counts[btree_level]++;
#endif


#ifdef LOAD_TIME
        auto currentTime3 = std::chrono::high_resolution_clock::now();
#endif

        if(n->slotkey[point] < key){

            while (point < hi && key_less(n->slotkey[point], key)) {
                point++;
            }
        }else{
            while (lo <= point && key_greaterequal(n->slotkey[point], key)) {
                point--;
            }
            point++;
        }
        // std::cout << pre_target << " "<<point<< "\n";

#ifdef LOAD_TIME
        auto currentTime4 = std::chrono::high_resolution_clock::now();
        auto nanoseconds3 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime3.time_since_epoch()).count();
        auto nanoseconds4 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime4.time_since_epoch()).count();
        load_times[btree_level] +=  (nanoseconds4 - nanoseconds3-21);
        load_counts[btree_level]++;
#endif


#ifdef COUNT_GAP
        int gap = abs(pre_target-point) ;
        gaps[btree_level] += gap;
        gaps_count[btree_level]++;
#endif
        return point;

    }


    size_t* find_x(const key_type& key)
    {
        btree_level = 0;
        node* n = m_root;
        const inner_node* r = static_cast<const inner_node*>(n);
        if (!n) return nullptr;

        while (!n->isleafnode())
        {
            const inner_node* inner = static_cast<const inner_node*>(n);
            int slot = find_lower_liner_x(inner, key);

            n = inner->childid[slot];
            btree_level++;
        }
        leaf_node* leaf = static_cast<leaf_node*>(n);
        int slot = find_lower_liner_x(leaf, key);

        return (slot < leaf->slotuse && key_equal(key, leaf->slotkey[slot]))
               ? &(leaf->slotdata[slot]) : nullptr;
    }


    // generate duoxiangshi model
    template<typename T>
    inline bool generate_func_model(T* n){
        return node_training(n);
    }



    // generate duoxiangshi model
    template<typename T>
    bool node_training(T* n){
        // sure s1 != 0, define 5 site for compute k.

        int hi,lo,ele_gap,ele_num,base_site,r;
        double up_sum=0,down_sum=0,k=0,b=0,errs=0,fa=0,fb=0;
        hi = n->slotuse-1;
        lo = 0;
        ele_gap = 32;
        ele_num = hi / ele_gap + 1;
        base_site = (hi % ele_gap) / 2;
        /* defines target sites, keys, k and b */
        std::vector<double> keys;
        std::vector<double> sites;
        std::vector<double> ka(ele_num-1);
        std::vector<double> kb(ele_num-1);

        if(hi < 64) {
            n->model_type = modelType::LINE;
            n->fk[0] = static_cast<double>(hi - lo) / static_cast<double>(n->slotkey[hi] - n->slotkey[lo]);
            n->fb[0] = lo - n->fk[0] * n->slotkey[lo];
            return true;
        }else if(hi < 128){
            int x0,x1,x2,x3,x4;
            x2 = hi >> 1;
            x1 = hi >> 2;
            x0 = 0;
            x4 = hi;
            x3 = (x2+x4) >> 1;
            sites = {
                static_cast<double>(x0), 
                static_cast<double>(x1), 
                static_cast<double>(x2), 
                static_cast<double>(x3), 
                static_cast<double>(x4)};
            for(auto &e : sites){
                keys.push_back(static_cast<double>(n->slotkey[static_cast<int>(e)]));
            }

        }else{

            for(int i=base_site;i<=hi;i+=ele_gap){
                keys.push_back(static_cast<double>(n->slotkey[i]));
                sites.push_back(static_cast<double>(i));
            }

            for(int i=0;i<ele_num-1;i++){
                ka[i] = (keys[i+1] - keys[i]) / (sites[i+1] - sites[i]);
                kb[i] = keys[i] - ka[i] * sites[i];
            }

        }

        assert(keys.size() == sites.size());
        ele_num = keys.size();
        r = ele_num-1;

        k = (keys[r] - keys[0]) / (sites[r] - sites[0]);
        b = keys[0] - k * sites[0];

        /* computing distance area */
        for(int i=0;i<r;i++){
            double up=0,down=0;
            compute_integrate(k, ka[i], b, kb[i], sites[i], sites[i+1], up, down);
            up_sum+=up; down_sum+=down;
        }

        // array sites
        int x0,x1,x2,x3,x4;
        x4 = sites.size()-1;
        x2 = x4 >> 1;
        x1 = x4 >> 2;
        x3 = (x2 + x4) >> 1;
        x0 = 0;

        double k0,k1,k2,k3,k4;
        k0 = static_cast<double>(keys[x1] - keys[x0]) / (sites[x1] - sites[x0]);
        k1 = static_cast<double>(keys[x2] - keys[x1]) / (sites[x2] - sites[x1]);
        k2 = static_cast<double>(keys[x3] - keys[x2]) / (sites[x3] - sites[x2]);
        k3 = static_cast<double>(keys[x4] - keys[x3]) / (sites[x4] - sites[x3]);

        fa = (sites[r] - sites[0]) / (keys[r] - keys[0]);
        fb = sites[0] - fa * keys[0];
        // std::cout << fa << " " << fb << "\n";
        errs = (up_sum + down_sum) / (keys[r] - keys[0]);

        if(errs < 32){
            /* liner type */
            n->model_type = modelType::LINE;
            n->fk[0] = fa;
            n->fb[0] = fb;
            // node_type_counts[0]++;
        }

        else if(errs < 96){
            if(k > k0 && k < k3){
                // 小凸 small convex
                n->model_type = modelType::LINE;
                n->fk[0] = fa;
                n->fb[0] = fb + 0.8*errs;

            }else if(k < k0 && k > k3){
                // 小凹 small concave
                n->model_type = modelType::LINE;
                n->fk[0] = fa;
                n->fb[0] = fb - 0.8*errs;
            }else{
                // small S type
                n->model_type = modelType::LINE;
                n->fk[0] = fa;
                n->fb[0] = fb;
            }
        }

        else{
            std::vector<std::vector<double>> dp(ele_num,std::vector<double>(ele_num,0));
            // dp
            for(int i=0;i<ele_num;i++){
                for(int j=i;j<ele_num;j++){
                    dp[i][j] = meaning_distance(ka, kb, keys, sites, i, j);
                }
            }
            // int keys_size = keys.size();
            // for(int i=0;i<keys_size;i++){
            //     std::cout << keys[i] << " ";
            // }
            // std::cout << "\n";


            double final_k[3] = {0};
            double final_b[3] = {0};

            if(k > k0 && k < k3){
                n->model_type = modelType::TU;
                double min = DBL_MAX;
                int sp_site[4] = {0};
                for(int i=1;i<ele_num-1;i++){
                    double sum = dp[0][i] + dp[i][ele_num-1];
                    if(sum < min){
                        sp_site[0] = 0;
                        sp_site[1] = i;
                        sp_site[2] = ele_num-1;
                        min = sum;
                    }
                    // std::cout << sum << "\n";
                }
                
                // cout_nodeinfo(n);
                for(int i=0;i<2;i++){
                    final_k[i] = (sites[sp_site[i+1]] - sites[sp_site[i]]) / (keys[sp_site[i+1]] - keys[sp_site[i]])  ;
                    // std::cout << keys[sp_site[i+1]] << "\n";
                    final_b[i] = sites[sp_site[i]] - final_k[i] * keys[sp_site[i]];

                }
                // std::cout << final_k[0] << " " << final_k[1] << " " << final_k[2] << "\n";

            }else if(k < k0 && k > k3){
                n->model_type = modelType::AO;
                double min = DBL_MAX;
                int sp_site[4] = {0};
                for(int i=1;i<ele_num-1;i++){
                    double sum = dp[0][i] + dp[i][ele_num-1];
                    if(sum < min){
                        sp_site[0] = 0;
                        sp_site[1] = i;
                        sp_site[2] = ele_num-1;
                        min = sum;
                    }
                    // std::cout << sum << "\n";
                }
                
                // cout_nodeinfo(n);
                for(int i=0;i<2;i++){
                    final_k[i] = (sites[sp_site[i+1]] - sites[sp_site[i]]) / (keys[sp_site[i+1]] - keys[sp_site[i]])  ;
                    // std::cout << keys[sp_site[i+1]] << "\n";
                    final_b[i] = sites[sp_site[i]] - final_k[i] * keys[sp_site[i]];

                }
                // std::cout << final_k[0] << " " << final_k[1] << " " << final_k[2] << "\n";

            }else if(k > k0 && k > k3){
                n->model_type = modelType::S1;
                double min = DBL_MAX;
                int sp_site[4] = {0};
                for(int i=1;i<ele_num-2;i++){
                    for(int j=i+1;j<ele_num-1;j++){
                        double sum = dp[0][i] + dp[i][j] + dp[j][ele_num-1];
                        if(sum < min){
                            sp_site[0] = 0;
                            sp_site[1] = i;
                            sp_site[2] = j;
                            sp_site[3] = ele_num-1;
                            min = sum;
                        }
                        // std::cout << sum << "\n";
                    }
                }
                
                // cout_nodeinfo(n);
                for(int i=0;i<3;i++){
                    final_k[i] = (sites[sp_site[i+1]] - sites[sp_site[i]]) / (keys[sp_site[i+1]] - keys[sp_site[i]])  ;
                    // std::cout << keys[sp_site[i+1]] << "\n";
                    final_b[i] = sites[sp_site[i]] - final_k[i] * keys[sp_site[i]];

                }
                // std::cout << final_k[0] << " " << final_k[1] << " " << final_k[2] << "\n";

            }else{
                n->model_type = modelType::S2;
                double min = DBL_MAX;
                int sp_site[4] = {0};
                for(int i=1;i<ele_num-2;i++){
                    for(int j=i+1;j<ele_num-1;j++){
                        double sum = dp[0][i] + dp[i][j] + dp[j][ele_num-1];
                        if(sum < min){
                            sp_site[0] = 0;
                            sp_site[1] = i;
                            sp_site[2] = j;
                            sp_site[3] = ele_num-1;
                            min = sum;
                        }
                        // std::cout << sum << "\n";
                    }
                }
                
                // cout_nodeinfo(n);
                for(int i=0;i<3;i++){
                    final_k[i] = (sites[sp_site[i+1]] - sites[sp_site[i]]) / (keys[sp_site[i+1]] - keys[sp_site[i]])  ;
                    // std::cout << keys[sp_site[i+1]] << "\n";
                    final_b[i] = sites[sp_site[i]] - final_k[i] * keys[sp_site[i]];

                }
                // std::cout << final_k[0] << " " << final_k[1] << " " << final_k[2] << "\n";

            }

            // std::cout << final_k[0] << " " << final_k[1] << " " << final_k[2] << "\n";
            memcpy(n->fk,final_k,sizeof(final_k));
            memcpy(n->fb,final_b,sizeof(final_b));
            // std::cout << n->fk[0] << " " << n->fk[1] << " " << n->fk[2] << "\n";
        }

        n->insert_count = 0;
        n->delete_count = 0;
        return true;
    }


    /// Return the number of key/data pairs in the B+ tree
    inline size_type size() const
    {
        return m_stats.itemcount;
    }
    




    /* data */
public:
    bltree(/* args */);
    ~bltree();
};

bltree::bltree(/* args */)
{

}

bltree::~bltree()
{
}

