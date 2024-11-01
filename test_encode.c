/*                  Name : N.Revathi
                    Description : Project on LSB Image stegnography
                    Date : 08-10-2024
                    sample input : ./a.out beautiful.bmp secret.txt
                    sample output:1.Successfully opened files.....
                                  width = 1024
                                  height = 768
                                  2.Successfully checked the capacity of the source file.....
                                  3.Successfully copied bmp header to the output file.....
                                  4.Successfully encoded magic string.....
                                  5.Successfully encoded extension size.....
                                  6.Successfully secret file encoded extension size.....
                                  7.Successfully encoded secret file size.....
                                  Function encode_secret_file_data called
                                  8.Successfully encoded secret file data.....
                                  9.Successfully encoded remaining data to the destination file.....
                                  Encoding Success
                                  ./a.out stego.bmp ttt.txt
                                  
                                  Decoding started.....
                                  Opening required files.....
                                  1.Successfully files opened
                                  2.Successfully decoded magic string....
                                  3.Successfully decoded file extension size....
                                  4.Successfully decoded secret file extension.....
                                  5.Successfully opened output file......
                                  6.Successfully decoded file size......
                                  INFO: Secret file decoded successfully
                                  7.Successfully decoded secret file data.....
                                  Decoding Success
     
                                  cat ttt.txt --->My secret password is :)
*/
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
int main( int argc , char *argv[])
{
    if( argc >= 3 )
    {
    	if( check_operation_type( argv) == e_encode)
        {
        	if( argc >= 4 && argc <= 5)
                {
                	EncodeInfo encInfo ;
                	if( read_and_validate_encode_args( argv , &encInfo) == e_success)
                	{
                		if(do_encoding(&encInfo) == e_success) 
                        	{
                        		printf("Encoding Success\n") ;
                        	}
                        	else
                        	{
                                	printf("Encoding Failed\n") ;
                        	}	 
               	 	}
                	else
                	{
                		printf("Error :Read and validate is failed\n");
                        }
                 }
                 else
                 {
			printf("Pass correct number of arguments\n");
	         }
        }
        else if ( check_operation_type(argv) == e_decode )
        {     
                  DecodeInfo decInfo ;
                  if( argc >= 3 && argc <= 4)
                  {
                  	if( read_and_validate_decode_args( argv , &decInfo) == e_success )
                  	{
                          	if( do_decoding(&decInfo,argv) == e_success)
                          	{
                                  	printf("Decoding Success\n") ;
                          	}
                          	else
                          	{
                                  	printf("Decoding Failed\n");
                          	}
                	}
                	else
                	{
                		printf("Error : Read and validate is failed\n");
                	}
                }	    
           	else
                {
           		printf("Pass correct number of arguments\n");
                } 
        }    
    else
    {
            printf("Invalid input\n");
    }
  }
  else
  {
	printf("Invalid input\n");
  }

}
