#include <stdio.h>
#include <stdlib.h>
#include <time.h> // Timestamping
#include <unistd.h> // Sleep()
#include <stdbool.h> 

int main() {
    while(1) { // Infinite loop
        FILE *fptr, *lptr; // File pointer(fptr) and Log pointer(lptr)
        char buffer[16]; // Buffer to hold file content(16 bytes)
        int raw_temp; 
        float final_temp; 
        bool alert_sent = false; // Flag to track high temp alert status - Prevent notifications spam

        system("echo \"80000\" | sudo tee /tmp/fake_temp > /dev/null"); // TESTING ALERT - Create a fake temp file with 80C value

        // --- READ LOGIC ---
        
        // fptr = fopen("/sys/class/hwmon/hwmon2/temp1_input", "r"); COMMENTED TO TEST ALERT | Open the temperature input file(AMD), the usual path is /sys/class/thermal/thermal_zone0/temp
        fptr = fopen("/tmp/fake_temp", "r"); // TESTING ALERT - Using a fake temp file 
        if(fptr == NULL) return 1; // Exit if file not found
        fgets(buffer, 16, fptr); // Read the content into buffer
        fclose(fptr); // Close the file
        final_temp = atof(buffer) / 1000.0; // Convert to Celsius  
        // printf("Current Temperature: %.2f°C\n", final_temp);  Print to console - Commented for gui preparation 

        // --- LOGGING LOGIC ---

        time_t now = time(NULL); // Get current time
        struct tm *t = localtime(&now); // Convert to local time structure
        char time_str[64] = {0}; // Buffer for formatted time
        strftime(time_str, sizeof(time_str), "%d-%m-%Y %H:%M:%S", t); // Format time

        lptr = fopen("cpu_log.csv", "a"); // Open log file in append mode(csv because I wanna do some data analysis later)
        if(lptr != NULL){
            fprintf(lptr, "%s, %.2f\n", time_str, final_temp); // Write timestamp and temperature to log file
            fclose(lptr); 
        }

        // --- ALERT LOGIC ---

        if (final_temp > 70.0 && !alert_sent) {
            char command[128];
            snprintf(command, sizeof(command), "notify-send -u critical 'High CPU Temperature' 'Temperature: %.2f°C'", final_temp);
            system(command);
            alert_sent = true;
        }
        else if (final_temp < 65.0){ // 65 for hysteresis purpose - prevent multiple alerts when temp hovers around threshold
            alert_sent = false;
        }
        
        sleep(5); // Sleep for 5 seconds before next temp check 
    }
    
    return 0;
}