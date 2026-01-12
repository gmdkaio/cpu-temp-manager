#include <stdio.h>
#include <stdlib.h>
#include <time.h> // Timestamping
#include <unistd.h> // Sleep()
#include <stdbool.h> 

int main() {
    bool alert_sent = false; 

    // Start zenity 
    FILE *gui_pipe = popen("zenity --progress --title='CPU Temperature Monitor' --percentage=0 --no-cancel", "w");

    if(gui_pipe == NULL){
        perror("Could not open zenity pipe");
        return 1;
    }

    while(1){
        FILE *fptr, *lptr;
        char buffer[16];
        float final_temp;

        // --- READ LOGIC ---
        
        // fptr = fopen("/sys/class/hwmon/hwmon2/temp1_input", "r"); COMMENTED TO TEST ALERT | Open the temperature input file(AMD), the usual path is /sys/class/thermal/thermal_zone0/temp
        fptr = fopen("/tmp/fake_temp", "r"); // TESTING ALERT - Using a fake temp file 
        if(fptr == NULL) break;; // Exit if file not found
        fgets(buffer, 16, fptr); // Read the content into buffer
        fclose(fptr); // Close the file
        final_temp = atof(buffer) / 1000.0; // Convert to Celsius  

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


        // ---GUI LOGIC ---

        // 1. Bar percentage
        fprintf(gui_pipe, "%d\n", (int)(final_temp)); // Send the temperature as percentage to zenity

        // 2. Label based on temp
        if(final_temp > 70.0){
            fprintf(gui_pipe, "# WARNING - HIGH TEMPERATURE!(%.2f °C)\n", final_temp);
        } else {
            fprintf(gui_pipe, "# Status - Normal(%.2f °C)\n", final_temp);
        }

        // Push data to zenity - If fails, probably the user closed the GUI
        if (fflush(gui_pipe) != 0) {
            fprintf(stderr, "\nGUI closed by user. Exiting...\n");
            break;
        }

        // --- ALERT LOGIC ---
        if(final_temp > 70.0 && !alert_sent){
            system("zenity --warning --text='High Temp Detected' &");
            alert_sent = true;
        }
        else if (final_temp < 65.0){ // 65 for hysteresis purpose - prevent multiple alerts when temp hovers around threshold
            alert_sent = false; 
        }

        sleep(1);
    }

    pclose(gui_pipe); // Close the zenity pipe
    return 0;
}
