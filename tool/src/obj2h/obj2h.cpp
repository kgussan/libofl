//obj2h
//2010/11/17 11:51:45	static const やめた。変数名称変更。
//2010/02/04 14:37:28
//2009/11/30 18:35:21
// TODO:引数対応でCW,CCW
#include "obj2h.h"
//#include "ofl.h"
using namespace nsOfl;

enum{
	SIZE_DIRECT=0,
	SIZE_UNITIZE,
	SIZE_END,
};
enum{
	MATERIAL_NONE=0,
	MATERIAL_DIRECT,
	MATERIAL_COLOR_GEN,
	MATERIAL_END,
};

char g_version[256];
void Usage(void)
{
	printf("\n");
	printf("obj2h v%s\n",g_version);
	printf("convert a wavefront obj formated file to a C/C++ header file.\n");
	printf("\n");
	printf("usage : obj2h OBJ_FILE \n");
	printf("\n");
}

/// entry point
int main(int argc, char **argv)
{
	static const uint32_t triangle_order=TRIANGLE_ORDER_CCW;
	static const uint32_t size_control=SIZE_DIRECT;
	static const uint32_t material_control=MATERIAL_COLOR_GEN;
	FILE *fp;
	float fSmoothingAngle=90.0f;
	uint32_t u32MaterialMode=0;
	oflModel_t *oflModel;// ofl model data structure 
	if(0){
		if((fp=fopen("../../version.txt","r"))==NULL){
			sprintf( g_version,"UNKNOWN");
		}else{
			fgets(g_version,256,fp);
			g_version[strlen(g_version)-1]=' ';
			//printf("debug %s %d %f \n", g_version, g_version, g_version );
			//while( fgets( g_version, 256, fp )!=NULL ){
				//printf("%s",s);
				//sprintf(g_version,"%s",);
			//}
			fclose(fp);	//< (5)ファイルのクローズ
		}
	}else{
#		include "../../version.txt"
		sprintf(g_version,"%s",VERSION);
	}
	if(argc!=2){///<引数にファイル名指定がなければ実行しない。
		Usage();
		exit(EXIT_FAILURE);
	}
	printf("//obj2h converter v%s by KOGUCHI Takahiro %s %s\n",g_version,__DATE__,__TIME__);
	oflModel=oflReadObj(argv[1]);
	oflFacetNormals(oflModel);
	if(size_control==SIZE_UNITIZE){
		oflUnitize(oflModel);
	}
	if(triangle_order==TRIANGLE_ORDER_CCW){
		printf("//triangle order=CCW \n");
	}else{
		oflReverseWinding(oflModel);
		printf("//triangle order=CW \n");
	}
	oflFacetNormals(oflModel);
	//oflVertexNormals(oflModel,fSmoothingAngle);
	printf("//Memory: %lu bytes \n",oflModel->numtriangles*3*(3*sizeof(float)
									+3*sizeof(float)*(oflModel->numnormals?1:0)
									+2*sizeof(float)*(oflModel->numtexcoords ?1:0)));
	//oflModel->numvertices*3*sizeof(float)
	//+oflModel->numnormals*3*sizeof(float)*(oflModel->numnormals?1:0)
	//+oflModel->numtexcoords*2*sizeof(float)*(oflModel->numtexcoords?1:0));
	//+numtriangles*sizeof(OFLtriangle));
	if(oflModel->nummaterials>0){
		u32MaterialMode=2;
	}else{
		u32MaterialMode=0;
	}
	//oflExportCHeader(oflModel,false,false);
	oflExportCHeader(oflModel,true,true);

	if(0){//windows debug
		//dprintf(("end \n"));
		char *str="end of program\n";
		char tmp[1024];
		//sscanf(&tmp, "end of program\n");
		scanf(str,"%s",tmp);
	}
	return 0;
}

