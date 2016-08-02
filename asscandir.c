//
// AS-Scandir - scandir(3) replacement
// Includes size and file modification/creation/access time.
// Allows sorting by name, size, time and in reverse.
// Uses custom struct for data, reduced unnecessary *** to ** for namelist.
// Removed silly array size estimation bullshit.
// Removed select/filter argument.
//
// Copyright (c) 1999-2016 by Antoni Sawicki <as@tenoware.com>
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

typedef struct ASDIR_ {
    char name[NAME_MAX];
    mode_t type;
    off_t size;
    time_t atime, mtime, rtime;
} ASDIR;

static int namesort(const void *d1, const void *d2) {
    return(strcmp( ((ASDIR*)d1)->name, ((ASDIR*)d2)->name));
}

static int rnamesort(const void *d1, const void *d2) {
    return(strcmp( ((ASDIR*)d2)->name, ((ASDIR*)d1)->name));
}

static int sizesort(const void *d1, const void *d2) {
         if(((ASDIR*)d1)->size < ((ASDIR*)d2)->size) return -1;
    else if(((ASDIR*)d1)->size > ((ASDIR*)d2)->size) return 1;
    else return 0;
}

static int rsizesort(const void *d1, const void *d2) {
         if(((ASDIR*)d1)->size > ((ASDIR*)d2)->size) return -1;
    else if(((ASDIR*)d1)->size < ((ASDIR*)d2)->size) return 1;
    else return 0;
}

static int timesort(const void *d1, const void *d2) {
         if(((ASDIR*)d1)->mtime < ((ASDIR*)d2)->mtime) return -1;
    else if(((ASDIR*)d1)->mtime > ((ASDIR*)d2)->mtime) return 1;
    else return 0;
}

static int rtimesort(const void *d1, const void *d2) {
         if(((ASDIR*)d1)->mtime > ((ASDIR*)d2)->mtime) return -1;
    else if(((ASDIR*)d1)->mtime < ((ASDIR*)d2)->mtime) return 1;
    else return 0;
}

int asscandir(const char *dir, ASDIR **namelist, int (*compar)(const void *, const void *)) {
    DIR *dirh;
    ASDIR *names;
    struct dirent *entry;
    struct stat fileinfo;
    char filename[PATH_MAX];
    int entries=0;

    dirh=opendir(dir);
    if(dirh==NULL)
        return -1;
        
    names=(ASDIR*)malloc(sizeof(ASDIR));
    if(names==NULL)
        return -1;

    entry=readdir(dirh);
    while(entry!=NULL) {
        snprintf(filename, sizeof(filename), "%s/%s", dir, entry->d_name);
        if(stat(filename, &fileinfo)!=0)
            return -1;

        memset(&names[entries], 0, sizeof(ASDIR));
        strcpy(names[entries].name, entry->d_name);
        names[entries].type=fileinfo.st_mode;
        names[entries].size=fileinfo.st_size;
        names[entries].atime=fileinfo.st_atime;
        names[entries].mtime=fileinfo.st_mtime;
        names[entries].rtime=fileinfo.st_ctime;
        
        names=(ASDIR*)realloc((ASDIR*)names, sizeof(ASDIR)*(entries+2));
        if(names==NULL)
            return -1;
        entries++;
        entry=readdir(dirh);
    }
    closedir(dirh);

    if(entries)
        qsort(&names[0], entries, sizeof(ASDIR), compar);

    *namelist=names;
    return entries;
}


// usage demo
int main(int argc, char **argv) {
    ASDIR *testdir;
    int e, nent;

    nent=asscandir(".", &testdir,  namesort);
//    nent=asscandir(".", &testdir, rnamesort);
//    nent=asscandir(".", &testdir,  sizesort);
//    nent=asscandir(".", &testdir, rsizesort);
//    nent=asscandir(".", &testdir,  timesort);
//    nent=asscandir(".", &testdir, rtimesort);
        
    for(e=0; e<nent; e++) 
        printf("%c %s %.1fM \n", S_ISDIR(testdir[e].type) ? 'd' : 'f', testdir[e].name, (float)testdir[e].size/1024/1024);

    return 0;
}