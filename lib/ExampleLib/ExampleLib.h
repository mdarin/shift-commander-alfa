#ifndef EXAMPLE_LIB_H
#define EXAMPLE_LIB_H

#ifdef __cplusplus
extern "C"
{
#endif
    /* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

    /* Exported constants --------------------------------------------------------*/

    /* Exported macro ------------------------------------------------------------*/

    /* Exported types ------------------------------------------------------------*/
    typedef struct example_lib_t
    {
        int a;
        bool b;
    } example_lib_t;

    /* Exported constants --------------------------------------------------------*/

    /* Exported macro ------------------------------------------------------------*/

    /* Exported functions prototypes ---------------------------------------------*/
    void exaple_func(void);

    /* Exported variables -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_LIB_H */
