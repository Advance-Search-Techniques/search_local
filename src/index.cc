#include "interface.h"

NAN_MODULE_INIT(InitModule) {
    Vector::Init(target);
}

NODE_MODULE(search,InitModule)