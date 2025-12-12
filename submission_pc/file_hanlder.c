#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <direct.h>
#include <dirent.h>
#include <synchapi.h>
#define BUFFER_SIZE 256

void wait_ms(int ms){
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}


void replay_files(){
    printf("\n\t[-- REPLAYS --]\n");//display titles
    int list_entry = 0;
    char folder[] = "Replays";
    char list_file_names[200][200];//array for all replay files
    char replay_buffer[BUFFER_SIZE];

    //open folder
    DIR *dir = opendir(folder);
    if (dir == NULL) {
        perror("opendir");  // Print error if directory can't be opened
        return;
    }
    //get all file names and amount of files
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." entries
        if (entry->d_name[0] == '.') {
            continue;
        }

        strcpy(list_file_names[list_entry], entry->d_name);//copies the file names into the list array
        list_entry++;
        printf("%d: %s\n", list_entry, entry->d_name);//displays list array
    }
    closedir(dir);

    int input;
    while(1){
        printf("%d: Back \nPlease choose a replay file: ", list_entry+1);
        scanf("%d", &input);
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);//fixes false input bug after file is read (flushes)
        if(input == list_entry+1) return;
        // Validate input
        if (input > 0 && input <= list_entry) {
            //get file path as a string
            char file_directory[150];
            snprintf(file_directory, sizeof(file_directory), "%s/%s", folder, list_file_names[input-1]);

            // Open the file with the full path
            FILE *file = fopen(file_directory, "r");
            if (file) {
                while (fgets(replay_buffer, sizeof(replay_buffer),file)) {
                    wait_ms(1000);  // Delay for reading each line
                    printf("%s", replay_buffer);
                }
                fclose(file);//close file
            } else {
                printf("Failed to open file: %s\n", file_directory);//failed
            }
            return;
        } else {
            printf("Wrong Input\n");
        }
    }
}

void save_replay(char* pc_cache){
    //Variables for time
    time_t current_time = time(NULL);
    struct tm *tm_info = localtime(&current_time);

    char string_time[26];
    //gets current time in specifed format
    strftime(string_time, sizeof(string_time), "%Y-%m-%d-%H-%M-%S", tm_info);
    //format for replay file name
    char directory[] = "Replays";
    char file_extension[] = ".txt";
    char file_directory[150];
    //inserts current time into requested format
    snprintf(file_directory, sizeof(file_directory), "%s/%s%s", directory, string_time, file_extension);
    //open file
    FILE *file = fopen(file_directory,"w");

    if (file) {
        strcat(pc_cache,"\n\t  [-- SAVED --]");
        strcat(pc_cache,"\n\t[-- GAME END --]\n");

        fputs(pc_cache,file); //save replay
        fclose(file);
        printf("\n\t [-- SAVED! --]");
    } else {
        printf("\nFailed to save"); //failed
        wait_ms(2000);
    }
}

