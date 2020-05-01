#include "imagen.h"
#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TRAILER "..FINAL.."
#define TAM_TRAILER strlen(TRAILER)

unsigned char * reservarMemoria(uint32_t w,uint32_t h);
unsigned char * lsb_encode(unsigned char *img_in,unsigned char *img_out,
uint32_t w,uint32_t h,char *payload);
int main(int argc, char *argv[])
{
	bmpInfoHeader info;
	unsigned char *imagenDest,*imagenRGB;
	printf( "Abriendo imagen ...\n" );
	imagenRGB = abrirBMP( argv[1],&info );
	displayInfo( &info );
	//convertir a niveles de gris
	imagenDest = reservarMemoria( info.width, info.height );

	lsb_encode(imagenRGB,imagenDest,info.width,info.height,argv[3]);

	guardarBMP( argv[2], &info, imagenDest );

	free( imagenRGB );
	free( imagenDest );
    return 0;
}
unsigned char * lsb_encode(
	unsigned char *img_in,unsigned char *img_out,
	uint32_t w,uint32_t h,char *payload){
	int fd = open(payload,O_RDONLY), n_bytes,tam = w*h*3;
	char buff;
    if( fd == -1 ){errx(EXIT_FAILURE,"Error en lsb_encode(), abriendo el archivo.");}
	int cont_trailer = 0;
	n_bytes = read(fd,&buff,sizeof(buff));
	for (size_t i = 0; i < tam; i+=8){
		// Se escriben en los 8b LSB 
		for (size_t j = 0; j < 8; j++){
			img_out[i+j] = img_in[i+j];
			if( n_bytes > 0){
				img_out[i+j] |=  ( buff & (1 << j) ) >> j ;
			}else if( cont_trailer < TAM_TRAILER ){
				cont_trailer++;
			}
		}
		if( n_bytes > 0){
			n_bytes = read(fd,&buff,sizeof(buff));
		}else if( cont_trailer < TAM_TRAILER ){
			cont_trailer++;
		}
	}
	close(fd);
	return img_out;
}
unsigned char * lsb_decode(
	unsigned char *img_in,unsigned char *img_out,
	uint32_t w,uint32_t h,char *carrier){
	int fd = open(payload,O_RDONLY), n_bytes,tam = w*h*3;
	char buff;
    if( fd == -1 ){errx(EXIT_FAILURE,"Error en lsb_encode(), abriendo el archivo.");}
	n_bytes = read(fd,&buff,sizeof(buff));
	for (size_t i = 0; i < tam; i+=8)
		for (size_t j = 0; j < 8; j++){
			img_out[i+j] = img_in[i+j];
			if( n_bytes > 0)
				img_out[i+j] |=  ( buff & (1 << j) ) >> j ;
		}
	close(fd);
	return img_out;
}


unsigned char * reservarMemoria(uint32_t w,uint32_t h){
	unsigned char *imagen;
	imagen = (unsigned char *) malloc( 3 * w * h * sizeof(unsigned char) );
	if (imagen == NULL)
	{
		perror("Error en reservarMemoria() conversion.c");
		exit(EXIT_FAILURE);
	}
	return imagen;
}


