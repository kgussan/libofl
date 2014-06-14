#include "ofl_ppm.h"
namespace nsOfl{
uint8_t* oflReadPPM(char* filename,int* width,int* height)
{
	FILE* fp;
	int i,w,h,d;
	unsigned char* image;
	char head[70];	// max line <= 70 in PPM (per spec). 
	fp=fopen(filename,"rb");
	if(!fp){
		perror(filename);
		return NULL;
	}
	// grab first two chars of the file and make sure that it has the
	// correct magic cookie for a raw PPM file.
	fgets(head,70,fp);
	if (strncmp(head,"P6",2)){
		fprintf(stderr,"%s: Not a raw PPM file\n",filename);
		return NULL;
	}
	//grab the three elements in the header (height, width, maxval). 
	i=0;
	while(i<3){
		fgets(head,70,fp);
		if(head[0]=='#')//skip comments.
			continue;
		if(i==0)
			i+=sscanf(head,"%d %d %d",&h,&w,&d);
		else if(i==1)
			i+=sscanf(head,"%d %d",&w,&d);
		else if(i==2)
			i+=sscanf(head,"%d",&d);
	}
	// grab all the image data in one fell swoop.
#if APPLY_MEMALIGN
	image=(unsigned char*)memalign(ofl_alignment,sizeof(unsigned char)*w*h*3);
#else
	image=(unsigned char*)malloc(sizeof(unsigned char)*w*h*3);
#endif // APPLY_MEMALIGN
	fread(image,sizeof(unsigned char),w*h*3,fp);
	fclose(fp);
	*width=w;
	*height=h;
	return image;
}

}
