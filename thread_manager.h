
#ifndef _THEREAD_MANAGER
#define _THEREAD_MANAGER

#include "encrypt_util.h"
#include "msg_queue.h"
#include "encrypt_util_api.h"

#define THREAD_INDEX_BASE 1100
#define READER_THREAD_MSG_ID (THREAD_INDEX_BASE - 1)
#define WRITER_THREAD_MSG_ID (READER_THREAD_MSG_ID - 1) 

#define ENC_STATUS_READY 0
#define ENC_STATUS_PROCESSING 1
#define ENC_STATUS_DONE 2

#define MSG_EXIT 2
#define MSG_ENC  4

#define THREAD_READY  1
#define THREAD_BUSY   2
#define THREAD_EXIT   3

typedef struct usr_data {
    UCHAR *data;
    UCHAR *key;
    UINT32 len;
    UINT32 status;
} usr_data;
typedef struct enc_data {
    long type;
    UINT32 id;
    usr_data *buf;
    UCHAR e_status;
} enc_data;

typedef struct thread_ctxt {
    pthread_t id;
    INT msg_type;
    UCHAR state;
}thread_ctxt;

void create_worker(UINT32 num_thread);
void delete_worker();
void update_key_worker(UCHAR *frag, UINT32 frag_len, UCHAR *key);

#endif //_THEREAD_MANAGER
