#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DEBUGFS_VM_PATH "/sys/kernel/debug/lab2_os/top"

struct request {
    int error;
    int process_limit;
};

struct cpu_load {
    long avg_load;      
    long total_ram;	    
    long free_ram;	    
    long shared_ram;	
    long total_swap;	
    long free_swap;	    
    long proc_count;	
};

struct process {
    long pid;
    unsigned int uid;
    int prio;
    int nice;
    unsigned long virt_mem;
    unsigned long rss;
    char state;
    unsigned long cpu_usage;
    long utime;
    long stime;
    char command[256];
};

struct process_info {
    int error;
    struct cpu_load load;
    struct process procs[10];
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <process_count>\n", argv[0]);
        return 1;
    }
    int fd = open(DEBUGFS_VM_PATH, O_RDWR);
    if (fd == -1) {
        perror("Error opening debugfs file");
        return 1;
    }
    int process_count = atoi(argv[1]);
    struct request req = {
        .process_limit = process_count
    };
    ssize_t err = write(fd, &req, sizeof(struct request));
    if (err < 0) {
        fprintf(stderr, "Error while writing to file");
        close(fd);
        return 1;
    }
    struct process_info info;
    ssize_t bytesRead = read(fd, &info, sizeof(struct process_info));
    if (bytesRead == 0) {
        printf("Cpu load: %ld\nTotal Ram: %ld\tFree Ram: %ld\n\
Total swap: %ld\tFree swap: %ld\nShared mem: %ld\n\
Current processes: %ld\n",
         info.load.avg_load, info.load.total_ram, info.load.free_ram,
         info.load.total_swap, info.load.free_swap, info.load.shared_ram,
         info.load.proc_count);
        printf("PID\tUID\tPR\tNI\tVIRT\tRSS\tState\t%%CPU\t\tUtime\t\tStime\t\t%%MEM\tCOMMAND\n");
        for (int i = 0; i < 10; i++) {
            printf("%ld\t%-5d\t%-3d\t%-3d\t%-3ld\t%-5ld\t%-3c\t%-9lu\t%-9ld\t%-9ld\t%-6.2f\t%s\n",
            info.procs[i].pid, info.procs[i].uid, info.procs[i].prio, info.procs[i].nice,
            info.procs[i].virt_mem, info.procs[i].rss, info.procs[i].state,
            info.procs[i].cpu_usage, info.procs[i].utime, info.procs[i].stime, 
            (float) info.procs[i].virt_mem / (float) info.load.total_ram, info.procs[i].command);
        }
    } else {
        perror("Error reading from debugfs file");
        close(fd);
        return 1;
    }
 
    close(fd);
    return 0;
}
