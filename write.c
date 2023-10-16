#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1024
#define SHM_KEY 0x1234
#define MAX_DATA_ITEMS 10 // Số lượng mẫu dữ liệu tối đa

struct data_item {
    int cnt;
    char buf[BUF_SIZE];
};

struct shared_data {
    int num_items;
    struct data_item items[MAX_DATA_ITEMS];
};

int main(int argc, char *argv[]) {
    int shmid;
    struct shared_data *shared_data_ptr;
    
    shmid = shmget(SHM_KEY, sizeof(struct shared_data), 0644 | IPC_CREAT);
    if (shmid == -1) {
        perror("Shared memory");
        return 1;
    }

    // Attach to the segment to get a pointer to it.
    shared_data_ptr = (struct shared_data *)shmat(shmid, NULL, 0);
    if ((void *)shared_data_ptr == (void *) -1) {
        perror("Shared memory attach");
        return 1;
    }

    printf("Enter text to write to shared memory (type 'exit' to finish):\n");

    while (1) {
        char input[BUF_SIZE];
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = '\0';  // Remove the newline character

        if (strcmp(input, "exit") == 0) {
            break;  // Exit the loop if the user types 'exit'
        }

        if (shared_data_ptr->num_items < MAX_DATA_ITEMS) {
            struct data_item *item = &(shared_data_ptr->items[shared_data_ptr->num_items]);
            int input_len = strlen(input);

            strncpy(item->buf, input, input_len);
            item->buf[input_len] = '\0';
            item->cnt = input_len;
            shared_data_ptr->num_items++;
            printf("Writing Process: Shared Memory Write: Wrote %d bytes\n", item->cnt);
        } else {
            printf("Writing Process: Shared Memory is full. Please wait for it to be read.\n");
        }
    }

    printf("Writing Process: Complete\n");

    if (shmdt(shared_data_ptr) == -1) {
        perror("shmdt");
        return 1;
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        return 1;
    }

    return 0;
}
