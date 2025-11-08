#To compile and run code

#To compile all files
gcc -c -O2 -std=c99 *.c
gcc *.o -o image_compressor -lm

#To run the code for 'k' values and input.jpg to output.jpg
./image_compressor input.jpg output.jpg k

