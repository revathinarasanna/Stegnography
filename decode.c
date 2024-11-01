#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"
#include <unistd.h>
#include<stdlib.h>
#define MAGIC_STRING_LENGTH 2 
     
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo) // Function to validate decoding arguments
{
    if (strstr(argv[2], ".bmp") != NULL)       // Check the source image file has a .bmp extension
    {
        decInfo->src_image_fname = argv[2];    // Store the source image file name
        if (argv[3] == NULL)                   // Check the secret file name is provided or not
        {
            decInfo->secret_fname = "output";  // Default file name
            return e_success;
        }
        else
        {
            decInfo->secret_fname = argv[3];   // Store the provided secret file name
            return e_success;
        }
    }
    return e_failure;                          // if any step fails
}
Status open_files_for_decoding(DecodeInfo *decInfo)
{
    printf("Opening required files.....\n");
    sleep(1);
     // Opening Source  bmp Image file
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "rb"); 
   
    // if src image not opened then print error
    if (decInfo->fptr_src_image == NULL)    
    {
        perror("fopen");                    // Print system error message
    	fprintf(stderr, "INFO: ERROR: Unable to open file %s\n", decInfo->src_image_fname);
        return e_failure;
    }
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)       // decode the magic string into the data
{                                                        
   // Allocate memory to store the decoded magic string
    char *decoded_magic_string = (char *)malloc((strlen(MAGIC_STRING )+ 1) * sizeof(char));
    if (decoded_magic_string == NULL)                 // Check for allocation success
    {
        printf("INFO: ERROR: Unable to allocate memory for decoded magic string\n");
        return e_failure;
    }

    fseek(decInfo->fptr_src_image, 54, SEEK_SET);     // To get position after BMP header 
    
    if (decode_image_to_data(decInfo,decoded_magic_string,strlen(MAGIC_STRING)) == e_success)
    {
        decoded_magic_string[strlen(MAGIC_STRING)] = '\0'; // Null-terminate the string
        
         if (strcmp(decoded_magic_string,MAGIC_STRING) == 0)   // Check against expected magic string
        {
            free(decoded_magic_string);               // Free memory and return success
            return e_success;
        }
        else
        {
            printf("INFO: ERROR: Magic string not decoded correctly\n");
            free(decoded_magic_string);				// Free memory if not decoded crt
            return e_failure;
        }
    }
    free(decoded_magic_string);						// Free memory if decoding failure
    return e_failure;
}

Status decode_image_to_data(DecodeInfo *decInfo, char *data, int size)
{
    char image_buffer[8];                                   // Buffer to store 8 bits
    for (int i = 0; i < size; i++)
    {
        fread(image_buffer,8,1,decInfo->fptr_src_image);    // Read 8 bits from image file
        decode_byte_to_lsb(&data[i], image_buffer);        // Decode the byte from lsb
    }

    return e_success;
}

Status decode_byte_to_lsb(char *data, char *image_buffer)   //decode a byte from the LSB
{
    *data = 0;                                              //initialize data is 0
    for (int i = 0; i < 8; i++)
    {

        *data |= (image_buffer[i] & 1) << (7 - i);          // Collect LSB and shifting to correct po
    }
    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)    //to decode the size of the extension into the data
{
    int size;
    char size_buffer[32];                                   // Buffer to store the size bits

    fread(size_buffer,32,1,decInfo->fptr_src_image);        // Read 32 bits from image file
    decode_size_to_lsb(&size,size_buffer);                  // Decode size from lsb
    decInfo->extn_secret_file_size = size;                  // Store the decoded size
    return e_success;
}

Status decode_size_to_lsb(int *size, char *image_buffer)    //decode the size from lsb
{
    *size = 0;                                              //initialize size as 0
    for (int i = 0; i < 32; i++)
    {
        *size |= (image_buffer[i] & 1) << (31 - i);         // Collect LSB and shifting to correct pos
    }
    return e_success;
}
Status decode_secret_file_extn(DecodeInfo *decInfo)         //to decode the extn into data
{
    if (decode_image_to_data(decInfo,decInfo->extn_secret_file,decInfo->extn_secret_file_size) == e_success) // Decode the secret file extension based on the decoded size
    {	    
        decInfo->extn_secret_file[decInfo->extn_secret_file_size] = '\0'; // Null-terminate the extension
       return e_success;
 
    }
    return e_failure;
}


Status open_output_file(DecodeInfo *decInfo, char *argv[]) // Open output file after decoding the extension
{
    char full_output_fname[50];  // Buffer to store the full output file name
    char secret_fname_copy[50];  // Create a copy of secret_fname to avoid modifying the original string
    
    // Make a copy of decInfo->secret_fname to safely use strtok
    strcpy(secret_fname_copy, decInfo->secret_fname);

    // Use strtok to get the name before "."
    char *token = strtok(secret_fname_copy, ".");
    if (token != NULL)
    {
        // Copy the base name to full_output_fname
        strcpy(full_output_fname, token);
    }
    else
    {
        // In case there's no extension in secret_fname, use the entire name
        strcpy(full_output_fname, decInfo->secret_fname);
    }

    // Concatenate the decoded file extension (e.g., ".txt", ".c")
    strcat(full_output_fname, decInfo->extn_secret_file);

    // Open the output file for writing
    decInfo->fptr_secret = fopen(full_output_fname, "w");
    if (decInfo->fptr_secret == NULL)  // If the output file fails to open, print an error
    {
        printf("INFO: ERROR: Unable to open file %s\n", full_output_fname);
        return e_failure;
    }

    return e_success;
}
Status decode_secret_file_size(DecodeInfo *decInfo)                   //to decode secret file size
{
    // Decode image to data
    if (decode_secret_file_extn_size(decInfo) == e_success)          
    {
         return e_success;
    }
    return e_failure;                               
}

Status decode_secret_file_data(DecodeInfo *decInfo)                    //to decode secret file data
{
    // Decode image to data
    char *data = (char *)malloc(decInfo->extn_secret_file_size);       
    if (data == NULL)    
    {
        printf("INFO: ERROR: Unable to allocate memory for data\n");   //if no data print error
        return e_failure;
    }
    // Decode image to data
    if (decode_image_to_data(decInfo,data,decInfo->extn_secret_file_size) == e_success) 
    {
        printf("INFO: Secret file decoded successfully\n");
       
        
         // Write the decoded data to the output file
        fwrite(data,decInfo->extn_secret_file_size, 1,decInfo->fptr_secret);            
       
         // Free the allocated memory
        // free(data);                                                   
        return e_success;
    }
// Free allocated memory in case of failure
    free(data);        
    return e_failure;
}




Status do_decoding(DecodeInfo *decInfo,char *argv[])
{
    printf("Decoding started.....\n");
    sleep(1);
    if(open_files_for_decoding(decInfo) == e_success)                                       
    {
         //Open the necessary files for decoding
        printf("1.Successfully files opened\n");
        sleep(1);
        if(decode_magic_string(decInfo) == e_success)                          
        {
            // decode the magic string into the data
            printf("2.Successfully decoded magic string....\n");
            sleep(1);
            if(decode_secret_file_extn_size(decInfo) == e_success)             				
            {
                //to decode the size of the extension into the data
                printf("3.Successfully decoded file extension size....\n"); 
                sleep(1);
                if(decode_secret_file_extn(decInfo) == e_success)  
                {
                    //to decode the extn into data
                    printf("4.Successfully decoded secret file extension.....\n");
                    sleep(1);
		    if(open_output_file(decInfo,argv) == e_success)            
                    {
                        //open output file after decoding extn
                        printf("5.Successfully opened output file......\n");
                        sleep(1);
                        if(decode_secret_file_size(decInfo) == e_success)      
                        {
                             //to decode secret file size
                            printf("6.Successfully decoded file size......\n");
                            sleep(1);
                            if(decode_secret_file_data(decInfo) == e_success)  
                            {
                                //to decode secret file data
                                printf("7.Successfully decoded secret file data.....\n");
                                sleep(1);
				
                                return e_success;
                            }
                        }
                    }
                }
            }
        }
    }
	fclose(decInfo->fptr_src_image);     //close the src image
	fclose(decInfo->fptr_secret);        //close the output image
    return e_failure;             //Return failure status if any step fails
}




























