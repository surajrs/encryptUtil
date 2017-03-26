#include "thread_manager.h"

thread_ctxt g_tlist[MAX_NUM_THREAD];
thread_ctxt g_writer_thread;
UINT32 g_num_thread;
msg_queue writer_q;
int encode_msg_id;
void *thread_cb(void *ptr);
void *write_thread_cb(void *ptr);

/*
 * This function creates all the required threads as requested by user.
 */
void create_worker(UINT32 num_thread) {
    UINT32 i;
    int status;
    int msgflg = IPC_CREAT | IPC_EXCL | 0666;
    g_num_thread = num_thread;
    key_t encode_msg_key;

    /* Create the queue keeping track of the data order */
    int_msg_queue(&writer_q);

    /* Initialize the message queue used by all threads to communicate */
    encode_msg_key = ftok("/prj/qca/cnss_fw/santaclara/dev01/surajs/encUtil", 'b');
    encode_msg_key = IPC_PRIVATE;
    
    if ((encode_msg_id = msgget(encode_msg_key, msgflg )) < 0) {
        perror("Reader thread message Message queue create failed:");
        exit(1);
    }

    /* Create the worker threads doing the encoding */
    for(i = 0; i < g_num_thread; i++) {
        g_tlist[i].msg_type = THREAD_INDEX_BASE + i;
        g_tlist[i].state = THREAD_READY;

        if(pthread_create( &g_tlist[i].id, NULL, thread_cb, &g_tlist[i]) != 0) {
           perror("Thread Create Failed\n");
            exit(1);
        }
        
    }

    /* Create the writer thread responsible for writing to the file */
    status = pthread_create( &g_writer_thread.id, NULL, write_thread_cb, &writer_q);
}

/* Deletes all threads and facilitate process exit */
void delete_worker() {
    UINT32 i;
    enc_data del_msg;
    del_msg.e_status = MSG_EXIT; 

    if (g_num_thread != 0) {
        for(i = 0; i < g_num_thread; i++) {
            /* Message all worker thread to exit */ 
            del_msg.type = g_tlist[i].msg_type;
            if (msgsnd(encode_msg_id , &del_msg, (sizeof(enc_data) - sizeof(long)), 0) < 0) {
                perror("Message Send to worker thread failed\n");
                exit(1);
            }
            pthread_join(g_tlist[i].id, NULL);
        }
    }

    /* Message the write queue at the end so that all the worker threads are done by then */ 
    del_msg.type = WRITER_THREAD_MSG_ID;
    if (msgsnd(encode_msg_id , &del_msg, (sizeof(enc_data) - sizeof(long)), 0) < 0) {
        perror("Message Send to writer thread failed\n");
        exit(1);
    }
    pthread_join( g_writer_thread.id, NULL);
}

/* Called by reader to identify a thread and assign work to the thread that is free */
void update_key_worker(UCHAR *frag, UINT32 frag_len, UCHAR *key) {
    enc_data enc;
    enc_data worker_msg;
    usr_data *ptr = NULL;
    int status;

    /* Wait for a worker thread to message that it is ready */
    worker_msg.type = READER_THREAD_MSG_ID;
    status = msgrcv(encode_msg_id, &worker_msg, (sizeof(enc_data) - sizeof(long)), READER_THREAD_MSG_ID, 0);
    if(status < 0) {
         perror("msg receive fail : ");
        exit(1);
    }

    /* Form data buffer to send */
    ptr = malloc(sizeof(usr_data));
    if(ptr == NULL) {
        perror("Malloc Failed : ");
        exit(1);
    }
    ptr->data = frag;
    ptr->len = frag_len;
    ptr->status = ENC_STATUS_PROCESSING;
    ptr->key = key;
    enc.buf = ptr;

    /* Retrive the index of the thread whose message was received */
    enc.type = worker_msg.id;
    enc.e_status = MSG_ENC; 
    msg_enqueue(&writer_q, ptr);

    /* Message the worker thread to process */
    status = msgsnd(encode_msg_id, &enc, (sizeof(enc_data) - sizeof(long)), 0);
    if(status < 0) {
        perror("update Key msg send failed : ");
        exit(1);
    }
}

/* writer thread function : make sure that each chunk in the buffer 
are written back in the same order the corresponding buffers were read */
void *write_thread_cb(void *ptr) {

    enc_data rsp;
    usr_data *pdata;
    node *n;

    rsp.type = WRITER_THREAD_MSG_ID;

    while(1) {
        if(msgrcv(encode_msg_id , &rsp, (sizeof(enc_data) - sizeof(long)), WRITER_THREAD_MSG_ID, 0) < 0) {
            perror("writer message receive failed : ");
            exit(1);
        }

        if(rsp.e_status == MSG_EXIT) 
            break;

        /* Check if the front of the queue is encoded. If not wait 
        */
        n = msg_peak_front(&writer_q);
        if(n) {
            pdata = (usr_data*)n->data;

            /* The front of the queue is encoded, write it to file */
            if(pdata->status == ENC_STATUS_DONE) {

                n = msg_dequeue(&writer_q);
                fwrite(pdata->data, pdata->len, 1, stdout);
                free(pdata->data);
                free(pdata);
                free(n);
            }
        }
    }

    /* Exit message received. All workers are done by now. do a blind write */
    while((n = msg_dequeue(&writer_q)) != NULL) {
         pdata = (usr_data*)n->data;
         fwrite(pdata->data, pdata->len, 1, stdout);
         free(pdata->data);
         free(pdata);
         free(n);
    }
    return NULL;
}

/* Main worker thread function: This function process the actual packet */
void *thread_cb(void *ptr) {
    thread_ctxt *ctxt = (thread_ctxt*)ptr;
    enc_data enc;
    enc_data rsp;
    int msgflg = IPC_CREAT | 0666;
    int status;
    int i;
#ifdef DEBUG
    struct timespec time_to_sleep = {0, 100000};
#endif
   
    /* Send a Dummy message to tell the readr that the workder is up and running */ 
    rsp.id = ctxt->msg_type;
    rsp.type = READER_THREAD_MSG_ID;
    status = msgsnd(encode_msg_id, &rsp, (sizeof(enc_data) - sizeof(long)), 0);
    if(status < 0) {
         perror("Worker thread message queue send failed:");
         exit(1);
    }

    while(1) {

        /* Receive Buffer + key buffer from reader thread */
        enc.type = ctxt->msg_type;
        if ((status = msgrcv(encode_msg_id , &enc, (sizeof(enc_data) - sizeof(long)), ctxt->msg_type,0)) < 0) {
            perror("Message receive failed at the workdr thread:");
            exit(1);
        }

        /* Received Exit notification. reader is done with reading the whole file */
        if(enc.e_status == MSG_EXIT) 
            break;

        ctxt->state = THREAD_BUSY;
        /* Do XOR transform */
        xor_trans_buffer(enc.buf->data, enc.buf->len, enc.buf->key, enc.buf->len);
#ifdef DEBUG
        time_to_sleep.tv_nsec = rand() * 1000000;
        nanosleep(&time_to_sleep, NULL);
#endif

        /* Free the key buffer, no more needed as the transform is done*/
        free(enc.buf->key);

        rsp.type = READER_THREAD_MSG_ID;
        rsp.id = ctxt->msg_type;
        ctxt->state = THREAD_READY;
        enc.buf->status = ENC_STATUS_DONE;

        /* Message reader thread asking for more data to process */
        msgsnd(encode_msg_id, &rsp, (sizeof(enc_data) - sizeof(long)), 0);

        rsp.type = WRITER_THREAD_MSG_ID;
        /* Message writer thread to notify the buffer is ready to write */
        msgsnd(encode_msg_id, &rsp, (sizeof(enc_data) - sizeof(long)), 0);
    }
    ctxt->state = THREAD_EXIT;
    return NULL;
}
