#include "../include/scheduler.h"

void execute_file_copy(Subtask* s) {
    char src_chunk[256], dst_chunk[256];
    sprintf(src_chunk, "data/source_%d.bin", s->subtask_id);
    sprintf(dst_chunk, "data/dest_%d.bin", s->subtask_id);

    int src_fd = open(src_chunk, O_RDONLY);
    if (src_fd < 0) {
        printf("   [Error] Could not open %s\n", src_chunk);
        return;
    }

    int dst_fd = open(dst_chunk, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
         printf("   [Error] Could not create %s\n", dst_chunk);
         close(src_fd);
         return;
    }

    char buf[BUFFER_SIZE];
    ssize_t bytes;
    int bytes_read_total = 0;

    // Simulate reading just the requested range if we had one large file, 
    // but here we just read the whole chunk file we generated for testing!
    while ((bytes = read(src_fd, buf, BUFFER_SIZE)) > 0) {
        write(dst_fd, buf, bytes);
        bytes_read_total += bytes;
    }

    close(src_fd);
    close(dst_fd);
}

void execute_file_search(Subtask* s) {
    char folder_path[256];
    for (int f = s->range_start; f < s->range_end; f++) {
        sprintf(folder_path, "data/folder_%d", f);
        DIR* dir = opendir(folder_path);
        if (dir) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != NULL) {
                // Mock search logic
                if (strstr(entry->d_name, "target") != NULL) {
                    // Match found
                }
            }
            closedir(dir);
        }
    }
}

// Fallback logic for Windows MinGW execution (Fake /proc hierarchy in /data/)
void execute_process_monitor(Subtask* s) {
    DIR* proc = opendir("data/proc");
    if (!proc) return;

    struct dirent* entry;
    while ((entry = readdir(proc)) != NULL) {
        if (isdigit(entry->d_name[0])) { // PID folders
            char path[256];
            sprintf(path, "data/proc/%s/status", entry->d_name);
            
            // Only process assigned resources based on range
            // (CPU, RAM, Disk split as subtasks in our mock)
            // Just simulate reading the process descriptor
            int fd = open(path, O_RDONLY);
            if (fd >= 0) {
                char buf[1024];
                read(fd, buf, sizeof(buf));
                close(fd);
            }
        }
    }
    closedir(proc);
}
