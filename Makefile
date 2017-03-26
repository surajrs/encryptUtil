HEADERS = encrypt_util.h thread_manager.h msg_queue.h encrypt_util_api.h
OBJECTS = encrypt_util.o thread_manager.o msg_queue.o main.o

default: encryptUtil

%.o: %.c $(HEADERS)
	gcc -lpthread -c -g $< -o $@

encryptUtil: $(OBJECTS)
	gcc -pthread $(OBJECTS) -g -o $@

clean:
	rm -f $(OBJECTS)
	rm -f encryptUtil
