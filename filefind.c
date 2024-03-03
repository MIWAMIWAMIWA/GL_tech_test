#define _DEFAULT_SOURCE
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "filefind.h"

#define MAX_PATH_LENGTH 512
#define MAX_THREADS 8

int is_Found = 0;

pthread_mutex_t lock;
pthread_cond_t signal;

char pattern[100];
char data_stack[10000][MAX_PATH_LENGTH];

int data_count = 0;


void submit_task(char *path){
    pthread_mutex_lock(&lock);
    strcpy(data_stack[data_count],path);
    data_count++;
    pthread_mutex_unlock(&lock);
    pthread_cond_signal(&signal);
}

void* start_thread(void *args){
    while(is_Found==0){
        pthread_mutex_lock(&lock);
        while (data_count == 0){
          pthread_cond_wait(&signal,&lock);
        }
        char to_check[MAX_PATH_LENGTH];
        strcpy(to_check,data_stack[0]);
        for (int i = 0 ; i < (data_count-1);i++){
            strcpy(data_stack[i],data_stack[i+1]);
        }
        data_count--;
        pthread_mutex_unlock(&lock);
        find_file(to_check);
    }
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&signal);
}


void init_find_file(char *pattern_to_find){
    pthread_t threadpool[MAX_THREADS];
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&signal, NULL);
    struct dirent *dr;
    char *path_to_dir = "/";
    strcpy(pattern,pattern_to_find);
    for ( int a =0; a < MAX_THREADS;a++){
        if(pthread_create(&threadpool[a],NULL,&start_thread,NULL)!=0){
           perror("thread was not created");
        }
    }
    DIR *de = opendir(path_to_dir);
    if (de == NULL) {
        perror("Unable to open directory");
        return;
    }
    while ((dr = readdir(de)) != NULL){
        if (strcmp(dr->d_name, ".") == 0 || strcmp(dr->d_name, "..") == 0) {
            continue; 
        }
        if (dr->d_type == DT_DIR) {
        char new_path[MAX_PATH_LENGTH];
        snprintf(new_path, sizeof(new_path), "%s/%s", path_to_dir, dr->d_name);
        submit_task(new_path);
      }
    }
    closedir(de);
    for (int i =0;i<MAX_THREADS;i++){
        pthread_join(threadpool[i], NULL);
    }

}


void find_file(char *path_to_d) { 
    const char *path_to_dir = path_to_d;
    struct dirent *de;
    DIR *dr = opendir(path_to_dir);
    if (dr == NULL) {
        perror("Unable to open directory");
        return;
    }
    while ((de = readdir(dr)) != NULL && is_Found == 0) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            continue; 
        }
        char new_path[MAX_PATH_LENGTH];
        snprintf(new_path, sizeof(new_path), "%s/%s", path_to_dir, de->d_name);

        if (de->d_type == DT_DIR) {
            find_file(new_path); 
        } else {
            if (de->d_type == DT_REG && strstr(de->d_name, pattern) != NULL) {
            is_Found = 1;
            printf("%s\n", new_path);
            break;
        }
      }
    }
    closedir(dr);
}
