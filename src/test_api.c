#include "amv.h"

int main() {
    struct amv_config config;
    
    amv_init_config(&config);
    amv_free_config(&config);
    
    return 0;
}
