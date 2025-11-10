#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void splitFile(const char *filename, int parts) {
    FILE *source = fopen(filename, "rb");
    if (!source) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    fseek(source, 0, SEEK_END);
    long fileSize = ftell(source);
    rewind(source);

    long partSize = fileSize / parts;
    long extra = fileSize % parts;

    char partName[200];
    for (int i = 0; i < parts; i++) {
        sprintf(partName, "%s.part%d", filename, i + 1);
        FILE *part = fopen(partName, "wb");
        if (!part) {
            printf("Error: Cannot create part file %s\n", partName);
            fclose(source);
            return;
        }

        long currentPartSize = partSize + (i == parts - 1 ? extra : 0);
        char buffer[BUFFER_SIZE];
        long bytesWritten = 0;

        while (bytesWritten < currentPartSize) {
            size_t toRead = BUFFER_SIZE;
            if (currentPartSize - bytesWritten < BUFFER_SIZE)
                toRead = currentPartSize - bytesWritten;

            size_t bytesRead = fread(buffer, 1, toRead, source);
            fwrite(buffer, 1, bytesRead, part);
            bytesWritten += bytesRead;
        }

        fclose(part);
        printf("Created: %s (%ld bytes)\n", partName, currentPartSize);
    }

    fclose(source);
    printf("File splitting completed.\n");
}

void mergeFiles(const char *output, int parts, const char *baseName) {
    FILE *dest = fopen(output, "wb");
    if (!dest) {
        printf("Error: Cannot create output file %s\n", output);
        return;
    }

    char partName[200];
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < parts; i++) {
        sprintf(partName, "%s.part%d", baseName, i + 1);
        FILE *part = fopen(partName, "rb");
        if (!part) {
            printf("Error: Cannot open part file %s\n", partName);
            fclose(dest);
            return;
        }

        size_t bytesRead;
        while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, part)) > 0) {
            fwrite(buffer, 1, bytesRead, dest);
        }

        fclose(part);
        printf("Merged: %s\n", partName);
    }

    fclose(dest);
    printf("File merging completed. Output: %s\n", output);
}

int main() {
    int choice;
    char filename[200], output[200];
    int parts;

    while (1) {
        printf("\n=== File Splitter and Merger ===\n");
        printf("1. Split File\n");
        printf("2. Merge Files\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            printf("Enter filename to split: ");
            getchar(); // clear leftover newline
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = 0; // remove newline

            printf("Enter number of parts: ");
            scanf("%d", &parts);

            splitFile(filename, parts);

        } else if (choice == 2) {
            printf("Enter base filename (without .partX): ");
            getchar(); 
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = 0;

            printf("Enter number of parts: ");
            scanf("%d", &parts);

            printf("Enter output filename: ");
            getchar();
            fgets(output, sizeof(output), stdin);
            output[strcspn(output, "\n")] = 0;

            mergeFiles(output, parts, filename);

        } else if (choice == 3) {
            printf("Exiting...\n");
            break;
        } else {
            printf("Invalid choice!\n");
        }
    }

    return 0;
}
