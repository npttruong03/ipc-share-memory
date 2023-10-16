#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 1024
#define SHM_KEY 0x1234

struct shmseg {
    int cnt;
    int complete;
    char buf[BUF_SIZE];
};

int main(int argc, char *argv[]) {
    int shmid;
    struct shmseg *shmp;

    // Kết nối đến bộ nhớ chia sẻ
    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644);
    if (shmid == -1) {
        perror("Shared memory");
        exit(1);
    }

    // Kết nối đến bộ nhớ chia sẻ
    shmp = (struct shmseg *)shmat(shmid, NULL, 0);
    if (shmp == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    while (1) {
        printf("Interact Process Menu:\n");
        printf("1. Add Data\n");
        printf("2. Display Data\n");
        printf("3. Modify Data\n");
        printf("4. Delete Data\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        int choice;
        scanf("%d", &choice);
        getchar(); // Consume the newline character

        switch (choice) {
            case 1: // Add Data
                printf("Enter data to add: ");
                char input[BUF_SIZE];
                fgets(input, sizeof(input), stdin);
                input[strlen(input) - 1] = '\0';  // Remove the newline character

                if (strlen(input) + shmp->cnt < BUF_SIZE) {
                    strcat(shmp->buf, input);
                    shmp->cnt += strlen(input);
                    printf("Interact Process: Added data to shared memory.\n");
                } else {
                    printf("Interact Process: Shared Memory is full. Cannot add more data.\n");
                }
                break;

            case 2: // Display Data
                if (shmp->cnt > 0) {
                    printf("Shared Memory Data: %s\n", shmp->buf);
                } else {
                    printf("Interact Process: Shared Memory is empty.\n");
                }
                break;

            case 3: // Modify Data
                printf("Enter new data: ");
                fgets(shmp->buf, sizeof(shmp->buf), stdin);
                shmp->cnt = strlen(shmp->buf);
                printf("Interact Process: Modified shared memory data.\n");
                break;

            case 4: // Delete Data
                shmp->buf[0] = '\0';
                shmp->cnt = 0;
                printf("Interact Process: Deleted shared memory data.\n");
                break;

            case 5: // Exit
                shmdt(shmp);
                exit(0);

            default:
                printf("Interact Process: Invalid choice. Please try again.\n");
                break;
        }
    }

    return 0;
}
