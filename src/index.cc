#include "interface.h"

NAN_MODULE_INIT(InitModule) {
    DataBase::Init(target);
}

NODE_MODULE(search,InitModule)