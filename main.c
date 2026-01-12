#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file; // Pointer
    char buffer[16]; // Buffer to hold file content(16 bytes)
    int raw_temp; 
    float final_temp; 


    // Open the temperature input file(AMD)
    // The usual path is /sys/class/thermal/thermal_zone0/temp
    file = fopen("/sys/class/hwmon/hwmon2/temp1_input", "r");
    
    if(file == NULL) { // Check if file exists
        printf("Error...");
        return 1; 
    }

    // Reads the temperature from file and store it in buffer
    if(fgets(buffer, sizeof(buffer), file) != NULL){ 
        raw_temp = atoi(buffer); // Converts temp string to int
        final_temp = raw_temp / 1000.0; // Divides temp by 1000 to get Celsius temp
        printf("Current Temperature: %.2f °C\n", final_temp);
    }

    // Release the file handle back to the OS 
    fclose(file); 

    return 0;
}