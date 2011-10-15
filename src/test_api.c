#include "amv.h"

int main() {
    struct mirosot_vision_config config;
    
    init_config(&config);
    
    free_config(&config);
    
    return 0;
}
