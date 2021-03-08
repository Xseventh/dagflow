//
// Created by seventh on 2020/12/15.
//
#include <iostream>
#include <cassert>
#include "data_manager.h"

int main() {
    dagflow::DataManager manager;

    size_t id1;
    std::shared_ptr<std::shared_ptr<int>> pp_int1 = manager.NewDataPtr<int>(id1);
    *pp_int1 = std::make_shared<int>(10);

    std::shared_ptr<std::shared_ptr<int>> pp_int2;
    pp_int2 = manager.Get<int>(id1);
    *pp_int2 = std::make_shared<int>(20);

    assert(&*pp_int1 == &*pp_int2);
    return 0;
}