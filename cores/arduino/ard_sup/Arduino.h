#ifndef Arduino_h
#define Arduino_h


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    // Include Apollo headers
    #include "am_mcu_apollo.h"
    #include "am_util.h"
    #include "am_bsp.h"

    /* system functions */
    int main(void);
    // void init(void);

    /* sketch */
    void setup(void);
    void loop(void);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // Arduino_h
