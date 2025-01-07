#include <bits/stdc++.h>

class LifeSpanTester;

class OutsideManager{

private:

    LifeSpanTester *lst_address;

public:

    void set(LifeSpanTester *address) {
        lst_address = address;
    }

    void fun() {
        std::cout << lst_address << std::endl;
    }

};


class LifeSpanTester{

public:

    LifeSpanTester(OutsideManager *OM)
    {
        OM->set(this);
        std::cout << "constructed" << std::endl;
    }

    ~LifeSpanTester(){
        std::cout << "deconstructed" << std::endl;
    }

    void say(LifeSpanTester *address)
    {
        std::cout << "LifeSpanTesterObject at " << address << std::endl;
    }


};

void storage(LifeSpanTester &&lst)
{

}

std::vector<LifeSpanTester*> lstv;

OutsideManager om;

void test()
{
    std::cout << "test() begins" << std::endl;

    //lstv.push_back(LifeSpanTester());
    LifeSpanTester lst(&om);

    om.fun();
    //storage(std::move(lst));

    std::cout << "test() ends" << std::endl;
}


int main(int argc, char **argv)
{

    std::cout << "main starts" << std::endl;
    test();
    std::cout << "main ends" << std::endl;

    return 0;
}