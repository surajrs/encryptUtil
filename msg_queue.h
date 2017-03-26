
#ifndef _MSG_QUEUE
#define _MSG_QUEUE
#include "encrypt_util.h"

typedef struct node{
    UCHAR * str;
    UINT32 str_len;
    void *data;
    UINT32 index;
    struct node *next;
}node ;

typedef struct msg_queue {
    node *frnt;
    node *back;
    pthread_mutex_t mx;
    UINT32 num_entries;
}msg_queue;

void int_msg_queue(msg_queue *m_q);
void msg_enqueue(msg_queue *m_q, void *data);
node * msg_dequeue(msg_queue *m_q);
node *msg_peak_front(msg_queue *m_q);

#endif //_MSG_QUEUE

