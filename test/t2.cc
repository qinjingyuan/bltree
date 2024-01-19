#include<iostream>
#include<memory>



struct base{
    int insert_count;
    int select_count;
    explicit base(int x = 0,int y = 0) : insert_count(x),select_count(y) {}
    // virtual void p() = 0;
    void p(){
        std::cout << insert_count << std::endl;
        std::cout << select_count << std::endl;
    }
    explicit base(std::initializer_list<int> l){
        std::cout << "inittalizer_list\n";
        insert_count = *l.begin();
        select_count = *(l.begin()+1);
    }
};

// struct t2 : public base
// {
//     int b;
//     void p(){
//         std::cout << insert_count << std::endl;
//     }
//     t2(int x, int y) : base(x),b(y)  {std::cout << 2 << std::endl;}
//     t2(std::initializer_list<int> l):base(1){
//         for(auto e : l){
//             std::cout << e << std::endl;

//         }
//     }
// };

void test(base b){
    b.p();
}


int main(){
    // std::shared_ptr<t2> t = std::make_shared<t2>(4);
    // t->p();

    // t2 a = {3,4,123,4};
    // a.p();

    base a (std::initializer_list<int>{3,4,5});
    a.p();
    test(a);
}
