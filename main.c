#include <stdio.h>
#include <stdlib.h>
#include <time.h> // Timestamping
#include <unistd.h> // Sleep()
int main() {
    while(1) { // Infinite loop
        FILE *fptr, *lptr; // File pointer(fptr) and Log pointer(lptr)
        char buffer[16]; // Buffer to hold file content(16 bytes)
        int raw_temp; 
        float final_temp; 

        // READ LOGIC
        
        // Open the temperature input file(AMD), the usual path is /sys/class/thermal/thermal_zone0/temp
        fptr = fopen("/sys/class/hwmon/hwmon2/temp1_input", "r");
        if(fptr == NULL) return 1; // Exit if file not found
        fgets(buffer, 16, fptr); // Read the content into buffer
        fclose(fptr); // Close the file
        final_temp = atof(buffer) / 1000.0; // Convert to Celsius   
    }
}