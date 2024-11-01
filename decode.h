#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;   //store the source file nane
    FILE *fptr_src_image;    //store the source file pointer
    uint image_capacity;    //size of image
    uint bits_per_pixel;     
    char image_data[MAX_IMAGE_BUF_SIZE];  //to store image data

    /* Secret File Info */
    char *secret_fname;               //to store secret file name
    FILE *fptr_secret;                //to store secret file pointer
    char extn_secret_file[MAX_FILE_SUFFIX];    //to store extension of secret file
    char secret_data[MAX_SECRET_BUF_SIZE];    //to store data of secret file
    long size_secret_file;                   //to store secret file size
    int extn_secret_file_size ;             //to store the extension of secret file
    
    /* Stego Image Info */
    char *stego_image_fname;        //store the output file name
    FILE *fptr_stego_image;         //file pointer for output file

} DecodeInfo;


/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo , char  *argv[]);

/* Get File pointers for i/p and o/p files */
Status open_files_for_decoding(DecodeInfo *decInfo) ;

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo) ;

/* Decode secret file extenstion */
 Status decode_secret_file_extn( DecodeInfo *decInfo) ;

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo) ;


/*secret file extension size */ 
Status decode_secret_file_extn_size(DecodeInfo *decInfo) ;

/* Decoding size to lsb */
Status decode_size_to_lsb(int *size, char *image_buffer) ;

/* Decode secret file data*/ 
 Status decode_secret_file_data(DecodeInfo *decInfo) ;

/* Decode function, which does the real decoding */
 Status decode_image_to_data(DecodeInfo *decInfo , char *data , int size) ;

/* Decode a byte from LSB of image data array */
Status decode_byte_to_lsb(char *data, char *image_buffer) ;

/* Decode to open output file */
Status open_output_file(DecodeInfo *decInfo,char *argv[]);
#endif


