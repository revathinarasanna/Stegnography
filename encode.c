#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"
#include<unistd.h>
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
uint get_file_size(FILE *fptr)
{
	//seek to end of the file
        fseek(fptr,0,SEEK_END) ;
        
        //return size of the file
       int val=ftell(fptr) ;

return val;
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    // No failure return e_success
    return e_success;
}

OperationType check_operation_type( char *argv[])
{
	//condition for checking it is encoding                                                  
        if( strcmp( argv[1] , "-e") == 0)               
        {
             return e_encode ;                                     
        }
        else if( strcmp( argv[1] , "-d") == 0 )    //condition for checking decode
        {
             return e_decode ;                     
        }
        else
        {
             return e_unsupported ;
        }
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	if( strstr( argv[2] , ".bmp") != NULL)                  //checking the source file is .bmp file or not
        {
                encInfo->src_image_fname  = argv[2] ;           //storing into the src_image_fname pointer
        	if((strstr( argv[3] ,".txt" ) != NULL) || ( strstr( argv[3] , ".c") != NULL) || ( strstr( argv[3] ,".sh") != NULL)) //checking secret file is .txt or .c or .sh
                {
                	encInfo->secret_fname = argv[3] ;        //storing into the secret_fname pointer
		strcpy(encInfo->extn_secret_file,strstr(argv[3],"."));
                        if( argv[4] == NULL)                     //checking the destination file is present or not
                        {
                        	encInfo->stego_image_fname = "stego.bmp" ;     //Default stego file
                                return e_success ;
                        }
                        else
                        {
                                if( strstr( argv[4] , ".bmp") != NULL)       //if present checking it is the .bmp file or not
                                {
                                	encInfo->stego_image_fname = argv[4] ;       //storing into the stego_image_name pointer
                                        return e_success ;
                                }
                        }
                 }
         }
         return e_failure ;
 }
Status check_capacity(EncodeInfo *encInfo)
{
        //to check the capacity of the source file
	int val = (54 +((strlen(MAGIC_STRING)+4+strlen(strstr(encInfo->secret_fname,"."))+4+get_file_size(encInfo->fptr_secret))*8)) ;
        if( val < get_image_size_for_bmp(encInfo->fptr_src_image)) 
        {

        	return e_success ;
        }
        else
        {
         	printf("Error not enough space in source image\n") ;
                return e_failure ;
        }
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
        rewind(fptr_src_image) ;
	
        char buffer[54] ;                 //buffer array for storing the header file bits
        
        fread(buffer,54,1,fptr_src_image) ;      //read into the buffer array from sorce image
         
        fwrite(buffer,54,1,fptr_dest_image);     //writing from buffer array to destination
        
        return e_success ; 
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
        //encoding data to image 
	return encode_data_to_image( magic_string , strlen(magic_string) , encInfo->fptr_src_image, encInfo->fptr_stego_image);

}
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char image_buffer[8] ;      //Extra buffer to store
        int i ;
        for( i = 0 ; i < size ; i++ ) 
        {
        	fread(image_buffer,8,1,fptr_src_image) ;    //read the data from source file and storing into the image_buffer
                encode_byte_to_lsb(data[i] , image_buffer) ; //function call to byte to lsb
                fwrite(image_buffer,8,1,fptr_stego_image) ;  //write the data from image buffer to stego image
        }
         return e_success ;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	int i ;
        for( i = 0 ; i < 8 ; i++ )
        {
        	image_buffer[i] = ( image_buffer[i] & ~ 1 | ((data & ( 1 << 7 - i ))) >> 7 - i ) ; //Doing bitwise operations
        }
        return e_success ;
}
Status encode_size_to_lsb( int size , char *image_buffer)
{
	
         int i ;
         unsigned int data = (unsigned int) size ;
         for( i = 0 ; i < 32 ; i++ )
         {
                 image_buffer[i] = ( image_buffer[i] & ~ 1 | ((data & ( 1 << 31 - i ))) >> 31 - i ) ;   //storing into the image buffer array 

         }
         return e_success ;

}
Status encode_extn_size( int size , EncodeInfo *encInfo) 
{

         char image_buffer[32] ;                  //Taking an extra buffer
         int i ;
         fread(image_buffer,32,1,encInfo->fptr_src_image) ;  //Reading data from source file image and storing into the image buffer 
         encode_size_to_lsb(size , image_buffer) ;
         fwrite(image_buffer,32,1,encInfo->fptr_stego_image) ;  //wriring data from stego image to image buffer
         return e_success ;
}
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
   
    // Ensure encInfo and secret_data are valid
    if (encInfo == NULL || encInfo->secret_data == NULL)
    {
        return e_failure; // Handle null pointer
    }
    // Call that calculates and encodes the file extension
     encode_data_to_image(encInfo->extn_secret_file, strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image);
      

    return e_success ;
}
Status encode_secret_file_size( long file_size , EncodeInfo *encInfo)
{
   char image_buffer[32] ;                      //Taking an extra array
   int i ;
   fread(image_buffer,32,1,encInfo->fptr_src_image) ;          //Reading data from src image and storing into the image buffer
   encode_size_to_lsb(file_size , image_buffer) ;
   fwrite(image_buffer,32,1,encInfo->fptr_stego_image) ;       //Writing data from image buffer and storing into the stego image
   return e_success ;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	
    rewind(encInfo->fptr_secret);                       
    printf("Function encode_secret_file_data called\n");
    char data[encInfo->size_secret_file];                         //Array for storing the secret data
    fread(data,encInfo->size_secret_file,1,encInfo->fptr_secret); //Read the data from secret file and storing into the data array

    encode_data_to_image(data,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image) ;
    return e_success; // Return success

}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer[4096]; // Buffer to hold data
    int  bytes_read;

    // Check if the file pointers are valid
    if (fptr_src == NULL || fptr_dest == NULL)
    {
        printf("Error: Invalid file pointer(s) in copy_remaining_img_data.\n");
        return e_failure;
    }
   
    // Assuming that the encoded data ends at a specific point
     long offset = ftell(fptr_src);  // Get the current position after encoding
     fseek(fptr_src, offset, SEEK_SET);  // Set the file pointer to where remaining data starts


    // Copy remaining data from source to destination
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0)
    {
        // Write to the destination file
        fwrite(buffer, 1, bytes_read, fptr_dest);
      
    }

    // Check for read errors
    if (ferror(fptr_src))
    {
        printf("Error: Failed to read from the source file.\n");
        return e_failure;
    }

    return e_success; // Return success if all data was copied successfully
}
Status do_encoding(EncodeInfo *encInfo)
{
	if(open_files(encInfo) == e_success)
        {
                //Used opening the files
                printf("1.Successfully opened files.....\n");
                sleep(1) ;
        	if(check_capacity(encInfo) == e_success)
                {     
                        //check the capacity of the source file
                        printf("2.Successfully checked the capacity of the source file.....\n");
                        sleep(1) ;
                	if(copy_bmp_header( encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success )
                	{
                                //copy the bmp header file to the output file
                                printf("3.Successfully copied bmp header to the output file.....\n");
                                sleep(1) ;
                        	if( encode_magic_string(MAGIC_STRING,encInfo) == e_success)
                                {  
                                        //Encoding the magic string
                                        printf("4.Successfully encoded magic string.....\n");
                                        sleep(1) ;
                                        //Encodeing the extension size
                                        if (encode_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
                                        {
				                
                                                printf("5.Successfully encoded extension size.....\n");
                                                sleep(1) ;
                                                //Encoding the secret file extension
                                        	if( encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success) 
                                                {
                                                        printf("6.Successfully secret file encoded extension size.....\n");
                                                        sleep(1);
                                                        encInfo->size_secret_file = get_file_size(encInfo->fptr_secret); // Get the size of the secret file
                                                         //Encode secret file size
                                                         if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                                                         { 
                                                                      printf("7.Successfully encoded secret file size.....\n");
                                                                      sleep(1) ;
                                                                      //Encode secret file data
                                                                      if (encode_secret_file_data(encInfo) == e_success) // Encode the secret file data
                                                                      {
                                                                           printf("8.Successfully encoded secret file data.....\n");
                                                                           sleep(1) ;
                                                                           //Copying the remaining data
                                                                           if (copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)                                                            
                                                                            {   sleep(1) ;
                                                                                 printf("9.Successfully encoded remaining data to the destination file.....\n");
                                                                                    return e_success ;
                                                                              }  
                                                                        }
                                                                }
                                                          } 

                                             }
                                 } 
                                
                        } 

                }
        }
         return e_failure ;    //return if it not valid
}
         
     
                   


