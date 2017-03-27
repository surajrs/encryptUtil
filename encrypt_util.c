#include "encrypt_util.h"
#include "thread_manager.h"
#include "encrypt_util.h"
#include "encrypt_util_api.h"

#define MAX_KEY 1023

UINT32 generate_key_blob(UCHAR *buf, UINT32 buf_len, UCHAR *key, UINT32 key_len);

/* Initialize the encrypt utility */
void init_encrypt_util(UINT32 num_thread) {
    create_worker(num_thread);
}
/* Cleanupe the encrypt utility */
void cleanup_encrypt_util(void) {
    delete_worker();
}



/* Return the biggest multiple of key_len < MAX_KEY
   This make sure that we process a buffer that is mutiple of the key length
*/
UINT32 suggested_buf_size(UINT32 key_len) {

#ifdef PROCESS_SINGLE_KEY
    return key_len;
#else
    if(key_len > MAX_KEY)
        return key_len;

    return (MAX_KEY/key_len) * key_len;
#endif
}

/* Reads data from stdin and process it */
void process_encrypt_util(UCHAR *key, UINT32 key_len) {
    UCHAR *ptr;
    UCHAR *key_blob;
    UINT32 len;
    UINT32 str_len, frag_len;
    UCHAR * buffer;
    UINT32 read_size = suggested_buf_size(key_len);

    do {
           buffer = malloc(read_size);
           if(buffer == NULL) {
               fprintf( stderr, "buffer allocation failure\n");
               return;
           } 

           len = fread(buffer, 1, read_size, stdin);
           if(len) {
               key_blob = malloc(len);
               if(key_blob == NULL) {
                   fprintf( stderr, "key buffer allocation failure\n");
                   return;
               } 
               generate_key_blob(key_blob, len, key, key_len); 
               update_key_worker(buffer, len,key_blob);
           }else {
               free(buffer);
           } 
    } while(len);
    fprintf( stderr, "File Proces complete\n");

}

/* This function does a right shift of the key stream */
void shift_stream(UCHAR * str, UINT32 size) {

        UCHAR ovf = 0x00;
        UCHAR tmp = 0x00;
        UINT32 i;

        for(i = 0; i < size;i ++) {
                tmp = (str[i] & 0x80) >> 7;
                str[i] = (str[i] << 1)| ovf;
                ovf = tmp;
        }

        str[0] |= ovf;
}

/* This function creates a key blob from the key by continouosly doing a left shift and copy
   Until it reach buf_len.
   The size of buf_len will be a multiple of key_len only except for the last buffer in the input file.
   THis help in doing a bulk processing of the data 
*/
UINT32 generate_key_blob(UCHAR *buf, UINT32 buf_len, UCHAR *key, UINT32 key_len) {
    UCHAR *ptr = buf;
    UINT32 cnt = 0;
    UINT32 inc = 0;
    UINT32 i;

    while(cnt < buf_len) {

       if(buf_len - cnt < key_len)
           inc = buf_len - cnt;
       else
           inc = key_len;
       memcpy(ptr, key, inc);
       cnt += inc;
       ptr += inc;
       shift_stream(key, key_len);
    }
    return buf_len;
}

/* Do an XOR transform of the buf using key */
void xor_trans_buffer(UCHAR *buf, UINT32 buff_len, UCHAR *key, UINT32 key_len) {
    UINT32 i = 0,j = 0;

    while(i < key_len && j < buff_len) {
        buf[j++] ^= key[i++];
    }
}
