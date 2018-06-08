#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//STRUCTS
typedef struct _DATE{
	int year;
	int month;
	int day;
	int hour;
	int min;
} DATE;

typedef struct _SESSION{
    char* id;
    DATE *start;
    DATE *end;
    struct _SESSION *next;
} SESSION;

//CONSTANTS
const int AM_OFFSET = 3;
const int MIN_OFFSET = 6;
const int HOUR_OFFSET = 9;
const int DAY_OFFSET = 12;
const int MONTH_OFFSET = 15;
const int YEAR_OFFSET = 20;

//GLOBALS
SESSION* session_list;

//METHOD SIGNATURES
void read_file(char* file);
void add_session(char* id, DATE *start, DATE *end);
char* read_session_id(const char* line);
DATE* read_date(const char* line, int length);
void print_session_summary();
int date_difference(DATE *start, DATE *end);
void cleanup();

int main(int argc, char *argv[]){
    session_list = NULL;
    
    if(argc == 0){
        printf("no file argument given\n");
        exit(0);
    }
    
	read_file(argv[1]);
    print_session_summary();
    cleanup();
}

void read_file(char* file){
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_count = 0;
    DATE *start = NULL, *end = NULL;
    char* id = "";

    fp = fopen(file, "r");
    if(fp == NULL) exit(EXIT_FAILURE);

    while((read = getline(&line, &len, fp)) != -1){
        if(line_count % 3 == 1){
            id = read_session_id(line);
            start = read_date(line, (int)read);
        }else if(line_count % 3 == 2){
            end = read_date(line, (int)read);
            add_session(id, start, end);
        }

        line_count++;
    }
}

void add_session(char* id, DATE *start, DATE *end){
    SESSION* new_session = malloc((sizeof(SESSION)));
    new_session->id = id;
    new_session->start = start;
    new_session->end = end;
    new_session->next = session_list;
    session_list = new_session;
}

char* read_session_id(const char* line){
    char* temp = calloc(50, sizeof(char));
    char* result;
    int i = 0;

    while(line[i] != ' '){
        temp[i] = line[i];
        i++;
    }
    temp[i] = '\0';

    result = malloc(sizeof(char) * strlen(temp));
    strcpy(result, temp);
    free(temp);

    return result;
}

DATE* read_date(const char* line, int length){
    DATE *result = malloc(sizeof(DATE));
    int year, month, day, hour, min;

    //offsets assume \n  at end of line
    if(line[length-1] != '\n') length++;

    //get year
    year = (line[length-YEAR_OFFSET] - '0') * 1000;
    year += (line[length-YEAR_OFFSET + 1] - '0') * 100;
    year += (line[length-YEAR_OFFSET + 2] - '0') * 10;
    year += (line[length-YEAR_OFFSET + 3] - '0');

    //get month
    month = (line[length-MONTH_OFFSET] - '0') * 10;
    month += (line[length-MONTH_OFFSET + 1] - '0');

    //get day
    day = (line[length-DAY_OFFSET] - '0') * 10;
    day += (line[length-DAY_OFFSET + 1] - '0');

    //get hour
    hour = (line[length-HOUR_OFFSET] - '0') * 10;
    hour += (line[length-HOUR_OFFSET + 1] - '0');
    if(hour == 12) hour = 0; //adjustment for AM/PM time

    //get min
    min = (line[length-MIN_OFFSET] - '0') * 10;
    min += (line[length-MIN_OFFSET + 1] - '0');

    //AM/PM hour adjustment
    if(line[length - AM_OFFSET] == 'P') hour += 12;

    //prepare result
    result->year = year;
    result->month = month;
    result->day = day;
    result->hour = hour;
    result->min = min;

    return result;
}

void print_session_summary(){
    if(session_list == NULL) return;

    SESSION *current_session = session_list;
    int total_sessions = 0;
    int total_time = 0;
    int average_time = 0;

    while(current_session != NULL){
        DATE *start = current_session->start;
        DATE *end = current_session->end;
        int session_length = date_difference(start, end);

        //print session ID
        printf("%s - ", current_session->id);

        //print start
        printf("%d/%02d/%02d - ", start->year, start->month, start->day);

        //print elapsed time
        printf("%02d:%02d\n", session_length/60, session_length % 60);

        //increment
        current_session = current_session->next;
        total_sessions++;
        total_time += session_length;
    }
    average_time = total_time / total_sessions;

    printf("\nTotal Sessions: %d\n", total_sessions);
    printf("Total time: %d:%d\n", total_time / 60, total_time % 60);
    printf("Average session length: %d:%d\n", average_time / 60, average_time % 60);
}

int date_difference(DATE *start, DATE *end){
    //make local copy of data
    int start_hour = start->hour, end_hour = end->hour;
    int start_min = start->min, end_min = end->min;

    if(start_hour > end_hour) end_hour += 24;
    start_min += start_hour * 60;
    end_min += end_hour * 60;

    return end_min - start_min;
}

void cleanup(){
    while(session_list != NULL){
        SESSION* temp = session_list;
        session_list = session_list->next;

        free(temp->id);
        free(temp->start);
        free(temp->end);
        free(temp);
    }
}

