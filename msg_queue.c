#include "msg_queue.h"
void int_msg_queue(msg_queue *m_q) {
    m_q->frnt = NULL;
    m_q->back = NULL;
    m_q->num_entries = 0;
    pthread_mutex_init(&m_q->mx, NULL);
}

void delete_msg_queue(msg_queue *m_q) {
    m_q->frnt = NULL;
    m_q->back = NULL;
    pthread_mutex_destroy(&m_q->mx);
}



void msg_enqueue(msg_queue *m_q, void *data) {
    node *n = malloc(sizeof(node));
    //fprintf( stderr, "msg-> %x %d\n",  n, sizeof(node));
    if(n == NULL) {
        fprintf(stderr, "memory allocation failed \n");
        return;
    }
    m_q->num_entries++;
    pthread_mutex_lock(&m_q->mx);
    n->data = data;
    n->next = NULL;

    if( m_q->back != NULL)
        m_q->back->next = n;
    m_q->back = n;
    if(m_q->frnt == NULL)
        m_q->frnt = n;
    pthread_mutex_unlock(&m_q->mx);
}
node *msg_peak_front(msg_queue *m_q) {

    return m_q->frnt;
}

node *msg_dequeue(msg_queue *m_q) {

    node *n;
    if(m_q->frnt == NULL)
        return NULL;

    pthread_mutex_lock(&m_q->mx);
    m_q->num_entries--;
    n = m_q->frnt;
    m_q->frnt = m_q->frnt->next;

    if(m_q->frnt == NULL)
        m_q->back = NULL;
    
    pthread_mutex_unlock(&m_q->mx);
    return n;
}
