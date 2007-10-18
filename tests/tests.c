#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <sys/stat.h>

#include "CUnit/Basic.h"
#include "openpgpsdk/readerwriter.h"
// \todo remove the need for this
#include "../src/advanced/parse_local.h"

#include "tests.h"

int main()
    {

    mem_literal_data=ops_memory_new();
    setup_test_keys();

    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    if (NULL == suite_crypto())
        {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (NULL == suite_packet_types())
        {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (NULL == suite_rsa_encrypt()) 
        {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (NULL == suite_rsa_decrypt()) 
        {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (NULL == suite_rsa_signature()) 
        {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (NULL == suite_rsa_verify()) 
        {
        CU_cleanup_registry();
        return CU_get_error();
        }

#ifdef TODO
    if (NULL == suite_rsa_create_key())
        {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (NULL == suite_rsa_sign_key())
        {
        CU_cleanup_registry();
        return CU_get_error();
        }

    if (NULL == suite_rsa_verify_key())
        {
        CU_cleanup_registry();
        return CU_get_error();
        }
#endif

    // Run tests
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    cleanup();

    return CU_get_error();
    }

// EOF
