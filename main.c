#include "encrypt_util_api.h"

void help() {

    fprintf(stderr, "encryptUtil [-n #] [-k keyfile]\n-n # Number of threads to create\n-k keyfile Path to file containing key\n");
}
int main(int argc, char **argv) {
    INT8 c;
    UCHAR key[MAX_KEY_SIZE];
    UINT32 key_len = 0;
    UINT32 num_thread = 0;
    FILE *fp = NULL;

    if(argc < 3) {
        help();
        return 1;
    }
    /* Parse command line paramters */
    while ((c = getopt (argc, argv, "n:k:")) != -1) {
        switch(c) {
            case 'n':
                num_thread = strtol(optarg,NULL,0);
                fprintf( stderr,"Num of threads %d \n", num_thread);
                if (num_thread > MAX_NUM_THREAD) {
                    fprintf( stderr,"Num of threads %d exceeds the maximum number of thread allowed\n", num_thread);
                    return 1;
                }
            break;
            case 'k':
                fprintf( stderr,"File Name \"%s\"\n", optarg);
                fp = fopen(optarg, "r");
                if(fp == NULL) {
                    fprintf( stderr,"File Name \"%s\" open failed\n", optarg);
                    help();
                    return 1;
                }
                /* Limitation: the maximum key file size is limited */
                key_len  = fread(key, 1, MAX_KEY_SIZE, fp);
                if (key_len == MAX_KEY_SIZE) {
                    fprintf( stderr, "Key Length too big: Provide a key file of size less than %d:\n", MAX_KEY_SIZE);
                    fclose(fp);
                    return 1;
                }
               
                if(key_len == 0) {
                    fprintf( stderr,"Empty key File\n");
                    fclose(fp);
                    return 1;
                }
                fclose(fp);
            break;
            case '?':
                help();
                return 1;
            break;
            default:
                help();
                abort();
        }
    }

    /* Check parameter validity */
    if(num_thread == 0 || key_len == 0) {
        help();
        return 1;
    }

    init_encrypt_util(num_thread);
    process_encrypt_util(key, key_len);
    cleanup_encrypt_util();
}
