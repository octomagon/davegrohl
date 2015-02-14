#include "DGTime.h"

void DGTimer::start(){
    gettimeofday(&tstart, NULL);
}

float DGTimer::stop(){
    gettimeofday(&tstop, NULL);
    int secs = (tstop.tv_sec - tstart.tv_sec);
    float ms = (tstop.tv_usec - tstart.tv_usec) * 0.000001;
    float theTime = secs + ms;
    return theTime;
}

char * DGTimer::elapsed(){
    gettimeofday(&tnow, NULL);
    int secs = (tnow.tv_sec - tstart.tv_sec);
    int mins = 0, hours = 0, days= 0;
    
    if (secs > 3600){
        hours = secs / 3600;
        secs = secs - (hours * 3600);
    }
    
    if (secs > 60){
        mins = secs / 60;
        secs = secs - (mins * 60);
    }
    
    sprintf(prettyTime, "%04d:%02d:%02d", hours, mins, secs);
    return prettyTime;
}

void secsToTimeUnits(DGTimeUnits *tu, long double total){
    long double rem;
    char buffer[2048] = {};
    
    if (total >= 31536000){
        tu->years = total / 31536000;
        rem = total - (tu->years * 31536000);
        total = rem;
    }
    
    if (total >= 86400){
        tu->days = total / 86400;
        rem = total - (tu->days * 86400);
        total = rem;
    }
    
    if (total >= 3600){
        tu->hours = total / 3600;
        rem = total - (tu->hours * 3600);
        total = rem;
    }
    
    if (total >= 60) {
        tu->minutes = total / 60;
        rem = total - (tu->minutes * 60);
        total = rem;
    }
    
    tu->seconds = total;
    
    // sprintf(tu->prettyTime, "%llu Years %llu Days %llu Hours %llu Minutes %llu Seconds", tu->years, tu->days, tu->hours, tu->minutes, tu->seconds);
    tu->prettyTime[0] = '\0';
    
    if (tu->years >= 13750000000) {
        strcat(tu->prettyTime, "More than 13.75 billion years");
        return;
    }
    
    if (tu->years > 1) {
        sprintf(buffer, "%llu Years ", tu->years);
        strcat(tu->prettyTime, buffer);
    } else if (tu->years == 1) {
        sprintf(buffer, "%llu Year ", tu->years);
        strcat(tu->prettyTime, buffer);
    }
    
    if (tu->days > 1) {
        sprintf(buffer, "%llu Days ", tu->days);
        strcat(tu->prettyTime, buffer);
    } else if (tu->days == 1) {
        sprintf(buffer, "%llu Day ", tu->days);
        strcat(tu->prettyTime, buffer);
    }
    
    if (tu->hours > 1 && tu->years == 0) {
        sprintf(buffer, "%llu Hours ", tu->hours);
        strcat(tu->prettyTime, buffer);
    } else if (tu->hours == 1 && tu->years == 0) {
        sprintf(buffer, "%llu Hour ", tu->hours);
        strcat(tu->prettyTime, buffer);
    }
    
    if (tu->minutes > 1 && tu->years == 0 && tu->days == 0) {
        sprintf(buffer, "%llu Minutes ", tu->minutes);
        strcat(tu->prettyTime, buffer);
    } else if (tu->minutes == 1 && tu->years == 0 && tu->days == 0) {
        sprintf(buffer, "%llu Minute ", tu->minutes);
        strcat(tu->prettyTime, buffer);
    }
    
    if (tu->seconds > 1 && tu->years == 0 && tu->days == 0) {
        sprintf(buffer, "%llu Seconds ", tu->seconds);
        strcat(tu->prettyTime, buffer);
    } else if (tu->seconds == 1 && tu->years == 0 && tu->days == 0) {
        sprintf(buffer, "%llu Second ", tu->seconds);
        strcat(tu->prettyTime, buffer);
    }
    
    if (tu->prettyTime[strlen(tu->prettyTime)-1] == ' ') {
        tu->prettyTime[strlen(tu->prettyTime)-1] = '\0';
    }
    
}

int intwithcommas(char theString[STR_BUFF_LEN], uint64_t theInt){
    int setsofthree, x = 0, y, z, len, rem;
    char buff[STR_BUFF_LEN],buff2[STR_BUFF_LEN];
    
    snprintf(buff, STR_BUFF_LEN, "%lld", theInt);
    
    len = strlen(buff);
    setsofthree = (len / 3);
    rem = fmod(len, 3);
    
    z = rem;
    while (z > 0) {
        buff2[x] = buff[x];
        z--;
        x++;
    }
    
    y = x;
    
    if (setsofthree > 0 && rem != 0){
        buff2[x] = ',';
        y = x+1;
    }
    
    
    while (setsofthree > 0) {
        for (z = 0; z < 3; z++) {
            buff2[y] = buff[x];
            x++;
            y++;
        }
        setsofthree--;
        if(setsofthree != 0){
            buff2[y] = ',';
            y++;
        }
    }
    buff2[y] = '\0';
    
    strcpy(theString, buff2);
    
    return 0;
}