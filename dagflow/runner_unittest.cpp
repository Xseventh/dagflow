//
// Created by seventh on 2020/12/13.
//
#include <iostream>
#include "runner.h"
#include "node.h"

class Test {
  public:
    Test() { std::cout << "constructor" << std::endl; }
    ~Test() { std::cout << "destructor" << std::endl; }
};
template<typename ... Args>
class TestRunner : public dagflow::IRunner<Args...> {
  private:
    void mSubmit(std::shared_ptr<Args>... inputs) override {
        std::cout << "mSubmit Succ" << std::endl;
    }
};

int main() {
    std::unique_ptr<Test> a = std::make_unique<Test>();
    std::shared_ptr<Test> b = std::make_shared<Test>();
    Test *c = new Test();
    TestRunner<Test, Test, Test> test_runner;
    std::cout << "Submit begin" << std::endl;
    test_runner.Submit(std::move(a), b, c);
    std::cout << "Submit end" << std::endl;
    delete c;
    std::cout << "main end" << std::endl;
}
