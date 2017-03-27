#ifndef _ENCRYPT_UTIL_API
#define _ENCRYPT_UTIL_API
#include "encrypt_util.h"

/* Maximum allowed size of the keyfile */
#define MAX_KEY_SIZE 1024

/* Maximum number of threads allowed */
#define MAX_NUM_THREAD 100

/* Initialize the encryption utility */
void init_encrypt_util(UINT32 num_thread);

/* Do the complete processing based on key including reading data from stdin */
void process_encrypt_util(UCHAR *key, UINT32 key_len);

/* XOR encode the buffer using a key */
void xor_trans_buffer(UCHAR *buf, UINT32 buff_len, UCHAR *key, UINT32 key_len);

/* Cleanup */
void cleanup_encrypt_util(void);

#endif //_ENCRYPT_UTIL_API
