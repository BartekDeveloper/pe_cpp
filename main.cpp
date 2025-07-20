#include "main.hpp"
#include "src/engine/engine.hpp"

int main() {
    
    try {
        Global::Init();
    } catch(const std::exception& e) {
        ERROR(e.what());
    }
    
    return 0;
}

