#if 1
#include <stdio.h>
int main() {
    printf("test skipped\n");
    return 0;
}
#else
#include "amv.h"
int main() {
    struct amv_config config;
    
    amv_config_init(&config);
    
    return 0;
}
#endif
