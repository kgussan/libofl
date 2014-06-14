//
//LICENCE
//
// Copyright (c) 2014 KOGUCHI Takahiro. kgussan@gmail.com
//
// This software is released under the MIT License.
// A part of this software is based on glm library by Nate Robins under the following license.
//
//Copyright 2010 Nate Robins. All rights reserved.
//Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//THIS SOFTWARE IS PROVIDED BY NATE ROBINS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NATE ROBINS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//The views and conclusions contained in the software and documentation are those of the authors and should not be interpreted as representing official policies, either expressed or implied, of Nate Robins.
//
#include "ofl.h"

#ifdef _MSC_VER //VISUAL STUDIO �̂Ƃ�����
#pragma warning(push)
#pragma warning(disable:4996)
#endif//_MSC_VER
#define OFL_DEBUG 0
#define OFL_TRACE 0
#if OFL_DEBUG
#	ifndef LogDebug
#	define LogDebug(...)	printf("Debug %s %d %s: ",__FILE__,__LINE__,__FUNCTION__);printf(__VA_ARGS__)
#	endif
#else
#	ifndef LogDebug
#	define LogDebug(...)
#	endif
#endif
#if OFL_TRACE
#	ifndef LogTrace
#	define LogTrace(...)	printf("Trace %s %d %s: ",__FILE__,__LINE__,__FUNCTION__);printf(__VA_ARGS__)
#	endif
#else
#	ifndef LogTrace
#	define LogTrace(...)
#	endif
#endif

#ifdef _WIN32 //64bit windows ����`����Ă���B
#define isnan _isnan
#else
#endif

#define SINGLE_STRING_GROUP_NAMES 0

namespace nsOfl{

///returns the maximum of two floats
float MaxFloat(float a,float b) 
{
	if(b>a)
		return b;
	return a;
}

//returns the absolute value of a float
float AbsFloat(float f)
{
	if(f<0){
		return -f;
	}
	return f;
}

//	compute the dot product of two vectors
//	-	u - array of 3 floats (float u[3])
//	-	v - array of 3 floats (float v[3])
float Vec3Dot(float* u,float* v)
{
	assert(u);
	assert(v);
	return u[0]*v[0]+u[1]*v[1]+u[2]*v[2];
}

//	compute the cross product of two vectors
// result : ����
//	-	u ����1
//	-	v ���͂Q
void Vec3Cross(float *result, const float *u, const float *v)
{
	assert(result);
	assert(u);
	assert(v);
	result[0] = u[1] * v[2] - u[2] * v[1];
	result[1] = u[2] * v[0] - u[0] * v[2];
	result[2] = u[0] * v[1] - u[1] * v[0];
}

//	normalize a vector
//	-	v - array of 3 floats (float v[3]) to be normalized
void Vec3Normalize(float* v)
{
	float l;

	assert(v);	///< �l���[���Ȃ�x�����Ă����B
	l=(float)sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);

	//�[���ɂȂ�ʂ悤�B�[���Ȃ炾��

	if(l!=0.0f){

		v[0]/=l;
		v[1]/=l;
		v[2]/=l;

	}else{

		v[0]=0.0f;
		v[1]=0.0f;
		v[2]=1.0f;

	}
}

//	compares two vectors and returns true if they are
//	equal (within a certain threshold) or false if not. An epsilon
//	that works fairly well is 0.000001.
//	-	u - array of 3 floats (float u[3])
//	-	v - array of 3 floats (float v[3]) 
bool Vec3Equal(float* u,float* v,float epsilon)
{
	if( AbsFloat(u[0]-v[0])<epsilon &&
		AbsFloat(u[1]-v[1])<epsilon &&
		AbsFloat(u[2]-v[2])<epsilon)
	{
		return true;
	}
	return false;
}

// ��������v���C�x�[�g�֐��Q�n�� {


static inline void Vector3Cross(float *r,const float *a,const float *b)
{
	r[0]=a[1]*b[2]-a[2]*b[1];
	r[1]=a[2]*b[0]-a[0]*b[2];
	r[2]=a[0]*b[1]-a[1]*b[0];
}


//tangent����
//	������̃^���W�F���g����
//	http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20051014
void calcTangentByNormalSphere(float tangent[3], const float n[3])
{
	float l = n[0] * n[0] + n[2] * n[2];//�@���̂��Ƃ��̂Q����v�Z
	float t[3];
	
	// �ڃx�N�g�����i�[���� attribute �ϐ��̃n���h�� 
	//extern GLint tangent;
  
	// �ڃx�N�g�� = (0, 1, 0) �~ n �@
	if (l > 0) {
		float a = sqrt(l);// sqrt(nx^2 + nz^2)
    
		t[0] = n[2] / a;
		t[1] = 0.0; //tangent ��Y�����͂O�ɂȂ��Ă���
		t[2] = -n[0] / a;
	}else {
		t[0] = t[1] = t[2] = 0.0;
	}  
	//glVertexAttrib3dv(tangent, t);
	tangent[0]=(float)t[0];
	tangent[1]=(float)t[1];
	tangent[2]=(float)t[2];
}

//tangent����
//	���ӁF���O��normal �͐��K������Ă��Ȃ���΂Ȃ�Ȃ��B
void calcTangentByNormal( float tangent[3], const float n[3] )
{
	float vecTanGen[] = {0.0f, 1.0f, 0.0f};
	float t[3];
	float length = n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
	// �ڃx�N�g�� = (0, 1, 0) �~ n �@
	if (length > 0) {
		Vector3Cross( t, vecTanGen, n );
		length = t[0] * t[0] + t[1] * t[1] + t[2] * t[2];
		float a = sqrt( length );
		t[0] /= a;
		t[1] /= a;
		t[2] /= a;
	}else {
		t[0] = 0.0f;
		t[1] = 0.0f;
		t[2] = 0.0f;
	}  
	tangent[0] = (float)t[0];
	tangent[1] = (float)t[1];
	tangent[2] = (float)t[2];
}

//tangent����
//	uv ���W���琶��
void calcTangentBinormal(	float tangent[3],
							float binormal[3],
							const float pos0[3], 
							const float pos1[3], 
							const float pos2[3],
							const float uv0[2], 
							const float uv1[2], 
							const float uv2[2],
							const float normal[3] )
{
	float x1 = pos1[0] - pos0[0];
	float x2 = pos2[0] - pos0[0];
	float y1 = pos1[1] - pos0[1];
	float y2 = pos2[1] - pos0[1];
	float z1 = pos1[2] - pos0[2];
	float z2 = pos2[2] - pos0[2];
	float s1 = uv1[0] - uv0[0];
	float s2 = uv2[0] - uv0[0];
	float t1 = uv1[1] - uv0[1];
	float t2 = uv2[1] - uv0[1];

	float r = 1.0f / (s1 * t2 - s2 * t1);
	float sdir[3];
	float tdir[3];
	sdir[0] = (         t2 * x1 - t1 * x2 ) * r;
	sdir[1] = (         t2 * y1 - t1 * y2 ) * r;
    sdir[2] = (         t2 * z1 - t1 * z2 ) * r;
    tdir[0] = ( -1.0f * s2 * x1 + s1 * x2 ) * r;
	tdir[1] = ( -1.0f * s2 * y1 + s1 * y2 ) * r;
    tdir[2] = ( -1.0f * s2 * z1 + s1 * z2 ) * r;

	float dotNT = sdir[0]*normal[0] + sdir[1]*normal[1] + sdir[2]*normal[2];
	tangent[0] = sdir[0] - normal[0] * dotNT;
	tangent[1] = sdir[1] - normal[1] * dotNT;
	tangent[2] = sdir[2] - normal[2] * dotNT;
	float mag = sqrtf( tangent[0]*tangent[0] + tangent[1]*tangent[1] + tangent[2]*tangent[2] );
	tangent[0] /= mag;
	tangent[1] /= mag;
	tangent[2] /= mag;

	float chirality=0.0f;
	float tmp[3];
	Vec3Cross( tmp, normal, tangent );
	if ( ( tmp[0]*tdir[0] + tmp[1]*tdir[1] + tmp[2]*tdir[2] ) < 0.0f ){
		chirality = -1.0f;
	} else {
		chirality = +1.0f;
	}

	Vec3Cross( binormal, normal, tangent );
	binormal[0] *= chirality;
	binormal[1] *= chirality;
	binormal[2] *= chirality;
	mag = sqrtf( binormal[0]*binormal[0] + binormal[1]*binormal[1] + binormal[2]*binormal[2] );
	binormal[0] /= mag;
	binormal[1] /= mag;
	binormal[2] /= mag;
	
}

///�E�F���h�x�N�^
///	���Ӓ��_�̍폜
///eliminate ( weld ) vectors that are within an epsilon of each other.
///����
///	vectors	 - array of float[ 3 ]'s to be welded
///	numvectors - number of float[ 3 ]'s in vectors
///	epsilon	 - maximum difference between vectors 
///�o��
///	���_���߂�l�ɁB
float *oflWeldVectors(float* vectors,uint32_t* numvectors,float epsilon)
{
	float* copies;
	uint32_t copied;
	uint32_t i,j;
#	if(APPLY_MEMALIGN)
	copies=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(*numvectors+1)+SAFETY_PADDING_A_BIT);
#	else
	copies=(float*)malloc(sizeof(float)*3*(*numvectors+1)+SAFETY_PADDING_A_BIT);
#	endif // APPLY_MEMALIGN
	memcpy(copies,vectors,(sizeof(float)*3*(*numvectors+1)));
	copied = 1;
	for(i=1;i<=*numvectors;i++){
		for(j=1;j<=copied;j++){
			if(Vec3Equal(&vectors[3*i],&copies[3*j],epsilon)){
				goto duplicate;
			}
		}
		// must not be any duplicates -- add to the copies array 
		copies[3*copied+0]=vectors[3*i+0];
		copies[3*copied+1]=vectors[3*i+1];
		copies[3*copied+2]=vectors[3*i+2];
		j=copied;//pass this a long for below
		copied++;
duplicate:
		//set the first component of this vector to point at the correct
		//index into the new copies array
		vectors[3*i+0]=(float)j;
	}
	*numvectors=copied-1;
	return copies;
}
//���f�����̃}�e���A������������
//����
//	���f���\����
//	�}�e���A�����̕�����
//�o��
//	�߂�l��
//����
//	�d�����_�͍폜���Ȃ��ƕ`��ɉe������\��������B
uint32_t oflFindMaterial(oflModel_t* model,char* name)
{
	uint32_t i;
	/// XXX doing a linear search on a string key'd list is pretty lame,
	///	but it works and is fast enough for now.
	for(i=0;i<model->nummaterials;i++){
		if(!strcmp(model->materials[i].name,name)){
			return i;
		}
	}
	///	didn't find the name, so print a warning and return the default material ( 0 ).
	LogDebug("oflFindMaterial( ): can't find material \"%s\".\n",name);
	LogTrace("\n");
	i=0;
	return i;
}

//oflFirstPass ��obj �t�@�C���̑S�Ẵ��f���̐ÓI�Ȓl��ǂݍ��ށB
//ReadObj( )����Ă΂��B
//���_���A�@�����Ȃ�
//-	first pass at a Wavefront OBJ file that gets all the statistics of the model
//	( such as #vertices, #normals, etc )
//����
//-	model - properly initialized oflModel_t structure
//-	file - ( fopen'd ) file descriptor 
//�o��
//-	model �֏o��
static void oflFirstPass(oflModel_t *model,FILE *file)
{
	uint32_t numvertices;	///<number of vertices in model
	uint32_t numnormals;	///<number of normals in model
	uint32_t numtexcoords;	///<number of texcoords in model
	uint32_t numtriangles;	///<number of triangles in model
	oflGroup_t *group;		///<current group 
	uint32_t v,n,t;
	char buf[128];
	// make a default group
	group=oflAddGroup(model,(char *)"default");
	numvertices=numnormals=numtexcoords=numtriangles=0;
	while(fscanf(file,"%s",buf)!=EOF){
		switch(buf[0]){
			case '#':// comment
				// eat up rest of line 
				fgets(buf,sizeof(buf),file);
				break;
			case 'v':// v, vn, vt
				switch(buf[1]){
					case '\0':	// vertex
						//eat up rest of line
						fgets(buf,sizeof(buf),file);
						numvertices++;
						break;
					case 'n':	// normal
						// eat up rest of line
						fgets(buf,sizeof(buf),file);
						numnormals++;
						break;
					case 't':	// texcoord
						// eat up rest of line
						fgets(buf,sizeof(buf),file);
						numtexcoords++;
						break;
					default:
						printf("oflFirstPass( ): Unknown token \"%s\".\n",buf);
						exit(EXIT_FAILURE);
						break;
				}
				break;
			case 'm':
				fgets(buf,sizeof(buf),file);
				sscanf(buf,"%s %s",buf,buf);
				model->mtllibname=strdup(buf);//���̖��O������Ƃ��ɂ�MTL��ǂ�
				oflReadMtl(model,buf);
				break;
			case 'u':
				// eat up rest of line
				fgets(buf,sizeof(buf),file);
				break;
			case 'g':	// group
				// eat up rest of line
				fgets(buf,sizeof(buf),file);
#				if SINGLE_STRING_GROUP_NAMES
				sscanf(buf,"%s",buf);
#				else
				buf[strlen(buf)-1]='\0';	//nuke'\n'
#				endif
				group=oflAddGroup(model,buf);
				break;
			case 'f':	//face
				v=n=t=0;
				fscanf(file,"%s",buf);
				// can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d
				if(strstr(buf,"//")){
					// v//n 
					sscanf(buf,"%d//%d",&v,&n);
					fscanf(file,"%d//%d",&v,&n);
					fscanf(file,"%d//%d",&v,&n);
					numtriangles++;
					group->numtriangles++;
					while(fscanf(file,"%d//%d",&v,&n)>0){
						numtriangles++;
						group->numtriangles++;
					}
				}else if(sscanf(buf,"%d/%d/%d",&v,&t,&n)==3){
					// v/t/n 
					fscanf(file,"%d/%d/%d",&v,&t,&n);
					fscanf(file,"%d/%d/%d",&v,&t,&n);
					numtriangles++;
					group->numtriangles++;
					while(fscanf(file,"%d/%d/%d",&v,&t,&n)>0){
						numtriangles++;
						group->numtriangles++;
					}
				}else if(sscanf(buf,"%d/%d",&v,&t)==2){
					// v/t
					fscanf(file,"%d/%d",&v,&t);
					fscanf(file,"%d/%d",&v,&t);
					numtriangles++;
					group->numtriangles++;
					while(fscanf(file,"%d/%d",&v,&t)>0){
						numtriangles++;
						group->numtriangles++;
					}
				}else{
					//v
					fscanf(file,"%d",&v);
					fscanf(file,"%d",&v);
					numtriangles++;
					group->numtriangles++;
					while(fscanf(file,"%d",&v)>0){
						numtriangles++;
						group->numtriangles++;
					}
				}
				break;
			default:
				///�c��̍s���g���ʂ����Beat up rest of line
				fgets(buf,sizeof(buf),file);
				break;
		}
	}
	// set the stats in the model structure
	model->numvertices=numvertices;
	model->numnormals=numnormals;
	model->numtexcoords=numtexcoords;
	model->numtriangles=numtriangles;
	//�O���[�v���̃g���C�A���O���̃������m�ۂ��s���Ballocate memory for the triangles in each group
	group=model->groups;
	while(group){
#		if APPLY_MEMALIGN
		group->triangles=(uint32_t*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(uint32_t)*group->numtriangles+SAFETY_PADDING_SIZE);
#		else
		group->triangles=(uint32_t*)malloc(sizeof(uint32_t)*group->numtriangles+SAFETY_PADDING_SIZE);
#		endif //APPLY_MEMALIGN
		group->numtriangles=0;
		group=group->next;
	}
}

//obj �t�@�C����2��ڂ̓ǂݍ��݁B
//���̓ǂݍ��݂Ŏ��f�[�^���擾����B
// ReadObj( )����Ă΂��B
//����
//	-#	model - properly initialized oflModel_t structure
//	-#	file - ( fopen'd ) file descriptor 
//�o��
//	-#	second pass at a Wavefront OBJ file that gets all the data.
static void oflSecondPass(oflModel_t *model,FILE *file)
{
	uint32_t numvertices;//<number of vertices in model
	uint32_t numnormals;//<number of normals in model 
	uint32_t numtexcoords;//<number of texcoords in model
	uint32_t numtriangles;//<number of triangles in model
	float *vertices;//<array of vertices 
	float *normals;//<array of normals
	float *texcoords;//<array of texture coordinates
	oflGroup_t *group;//<current group pointer
	uint32_t material;//<current material
	int32_t v,n,t;
	char buf[128];
	bool OFL_READMODE_PRIMITIVE_TRIANGLEFAN_ENABLE=true;//<���͂�FAN,�o�͂�TRIANGLES�ŏ����BTRUE
	// set the pointer shortcuts
	vertices=model->vertices;
	normals=model->normals;
	texcoords=model->texcoords;
	group=model->groups;
	/// on the second pass through the file, read all the data into the allocated arrays
	///�f�[�^�̓ǂݍ��݂ƒl�̑��
	numvertices=1;
	vertices[3*0+0]=0.0f;//���߂̒��_�ɂ̓[�������Ă����B
	vertices[3*0+1]=0.0f;//Debug�r���h��Release�r���h�ł̐U�镑���𓯂��ɁB
	vertices[3*0+2]=0.0f;//Release �r���h�ł͏�����������Ȃ��B
	numnormals=1;
	numtexcoords=1;
	numtriangles=0;
	material=0;
	while(fscanf(file,"%s",buf)!=EOF){
		switch(buf[0]){
			case '#': //< comment
				///�c��̍s���g���ʂ����Beat up rest of line
				fgets(buf,sizeof(buf),file);
				break;
			case 'v': //< v, vn, vt
				switch(buf[1]){
					case '\0'://<vertex
						fscanf(file,"%f %f %f",&vertices[3*numvertices+0],
											   &vertices[3*numvertices+1],
											   &vertices[3*numvertices+2]);
						numvertices++;
						break;
					case 'n'://<normal
						fscanf(file,"%f %f %f",&normals[3*numnormals+0],&normals[3*numnormals+1],&normals[3*numnormals+2]);
						numnormals++;
						break;
					case 't'://<texcoord
						fscanf(file,"%f %f",&texcoords[2*numtexcoords+0],&texcoords[2*numtexcoords+1]);
						numtexcoords++;
						break;
				}
				break;
			case 'u':
				///�ގ�����T���B
				fgets(buf,sizeof(buf),file);
				sscanf(buf,"%s %s",buf,buf);
				group->material=material=oflFindMaterial(model,buf);
				break;
			case 'g':	//< group
				///�c��̍s���g���ʂ����Beat up rest of line
				fgets(buf,sizeof(buf),file);
#				if SINGLE_STRING_GROUP_NAMES
				sscanf(buf,"%s",buf);
#				else
				buf[strlen(buf)-1]='\0';//<nuke'\n'
#				endif
				group=oflFindGroup(model,buf);
				group->material=material;
				break;
			case 'f'://<face�O�p�`����̏��
				v=0;
				n=0;
				t=0;
				fscanf(file,"%s",buf);
				//can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d 
				//�t�F�[�X�f�[�^�̃p�^�[���͌��܂��Ă���
				//���̂͂��߂����ʃV�[�P���X�Ȃ̂́A�f�[�^�\����2��ڈȍ~�A�����Ă��邩��B
				//��: f 4//4 3//3 2//2 1//1 
				if(OFL_READMODE_PRIMITIVE_TRIANGLEFAN_ENABLE){
					LogDebug("triangles gen OFL_READMODE_PRIMITIVE_TRIANGLEFAN_ENABLE\n");
					if(strstr(buf,"//")){
						//v//n,�n�߂̈��
						sscanf(buf,"%d//%d",&v,&n);
						model->triangles[numtriangles].vindices[0]=v;
						model->triangles[numtriangles].nindices[0]=n;
						fscanf(file,"%d//%d",&v,&n);
						model->triangles[numtriangles].vindices[1]=v;
						model->triangles[numtriangles].nindices[1]=n;
						fscanf(file,"%d//%d",&v,&n);
						model->triangles[numtriangles].vindices[2]=v;
						model->triangles[numtriangles].nindices[2]=n;
						group->triangles[group->numtriangles++]=numtriangles;
						numtriangles++;
						//�I���܂Ńf�[�^�擾
						while(fscanf(file,"%d//%d",&v,&n)>0){
							///���̓g���C�A���O���t�@���A�o�͓͂Ɨ��O�p�`
							model->triangles[numtriangles].vindices[0]=model->triangles[numtriangles-1].vindices[0];
							model->triangles[numtriangles].nindices[0]=model->triangles[numtriangles-1].nindices[0];
							model->triangles[numtriangles].vindices[1]=model->triangles[numtriangles-1].vindices[2];
							model->triangles[numtriangles].nindices[1]=model->triangles[numtriangles-1].nindices[2];
							model->triangles[numtriangles].vindices[2]=v;
							model->triangles[numtriangles].nindices[2]=n;
							group->triangles[group->numtriangles++]=numtriangles;
							numtriangles++;
						}
					}else if(sscanf(buf,"%d/%d/%d",&v,&t,&n)==3){
						//v/t/n,�n�߂̈��
						model->triangles[numtriangles].vindices[0]=v;
						model->triangles[numtriangles].tindices[0]=t;
						model->triangles[numtriangles].nindices[0]=n;
						fscanf(file,"%d/%d/%d",&v,&t,&n);
						model->triangles[numtriangles].vindices[1]=v;
						model->triangles[numtriangles].tindices[1]=t;
						model->triangles[numtriangles].nindices[1]=n;
						fscanf(file,"%d/%d/%d",&v,&t,&n);
						model->triangles[numtriangles].vindices[2]=v;
						model->triangles[numtriangles].tindices[2]=t;
						model->triangles[numtriangles].nindices[2]=n;
						group->triangles[group->numtriangles++]=numtriangles;
						numtriangles++;
						//�I���܂Ńf�[�^�擾
						while(fscanf(file,"%d/%d/%d",&v,&t,&n)>0){
							model->triangles[numtriangles].vindices[0]=model->triangles[numtriangles-1].vindices[0];
							model->triangles[numtriangles].tindices[0]=model->triangles[numtriangles-1].tindices[0];
							model->triangles[numtriangles].nindices[0]=model->triangles[numtriangles-1].nindices[0];
							model->triangles[numtriangles].vindices[1]=model->triangles[numtriangles-1].vindices[2];
							model->triangles[numtriangles].tindices[1]=model->triangles[numtriangles-1].tindices[2];
							model->triangles[numtriangles].nindices[1]=model->triangles[numtriangles-1].nindices[2];
							model->triangles[numtriangles].vindices[2]=v;
							model->triangles[numtriangles].tindices[2]=t;
							model->triangles[numtriangles].nindices[2]=n;
							group->triangles[group->numtriangles++]=numtriangles;
							numtriangles++;
						}
					}else if(sscanf(buf,"%d/%d",&v,&t)==2){
						//v/t,�n�߂̈��
						model->triangles[numtriangles].vindices[0]=v;
						model->triangles[numtriangles].tindices[0]=t;
						fscanf(file,"%d/%d",&v,&t);
						model->triangles[numtriangles].vindices[1]=v;
						model->triangles[numtriangles].tindices[1]=t;
						fscanf(file,"%d/%d",&v,&t);
						model->triangles[numtriangles].vindices[2]=v;
						model->triangles[numtriangles].tindices[2]=t;
						group->triangles[group->numtriangles++]=numtriangles;
						numtriangles++;
						//�I���܂Ńf�[�^�擾
						while(fscanf(file,"%d/%d",&v,&t)>0){
							model->triangles[numtriangles].vindices[0]=model->triangles[numtriangles-1].vindices[0];
							model->triangles[numtriangles].tindices[0]=model->triangles[numtriangles-1].tindices[0];
							model->triangles[numtriangles].vindices[1]=model->triangles[numtriangles-1].vindices[2];
							model->triangles[numtriangles].tindices[1]=model->triangles[numtriangles-1].tindices[2];
							model->triangles[numtriangles].vindices[2]=v;
							model->triangles[numtriangles].tindices[2]=t;
							group->triangles[group->numtriangles++]=numtriangles;
							numtriangles++;
						}
					}else{
						//v�����̍\��,�n�߂̈��
						sscanf(buf,"%d",&v);
						model->triangles[numtriangles].vindices[0]=v;
						fscanf(file,"%d",&v);
						model->triangles[numtriangles].vindices[1]=v;
						fscanf(file,"%d",&v);
						model->triangles[numtriangles].vindices[2]=v;
						group->triangles[group->numtriangles++]=numtriangles;
						numtriangles++;
						//�I���܂Ńf�[�^�擾
						while(fscanf(file,"%d",&v)>0){
							model->triangles[numtriangles].vindices[0]=model->triangles[numtriangles-1].vindices[0];
							model->triangles[numtriangles].vindices[1]=model->triangles[numtriangles-1].vindices[2];
							model->triangles[numtriangles].vindices[2]=v;
							group->triangles[group->numtriangles++]=numtriangles;
							numtriangles++;
						}
					}
				}else{
					///TRIANGLES
					LogDebug("triangles gen\n");
					if(strstr(buf,"//")){
						//v//n,�n�߂̈��
						while(fscanf(file,"%d//%d",&v,&n)>0){
							sscanf(buf,"%d//%d",&v,&n);
							model->triangles[numtriangles].vindices[0]=v;
							model->triangles[numtriangles].nindices[0]=n;
							fscanf(file,"%d//%d",&v,&n);
							model->triangles[numtriangles].vindices[1]=v;
							model->triangles[numtriangles].nindices[1]=n;
							fscanf(file,"%d//%d",&v,&n);
							model->triangles[numtriangles].vindices[2]=v;
							model->triangles[numtriangles].nindices[2]=n;
							group->triangles[group->numtriangles++]=numtriangles;
							numtriangles++;
						}
					}else if(sscanf(buf,"%d/%d/%d",&v,&t,&n)==3){
						//v/t/n,�n�߂̈��
						//�I���܂Ńf�[�^�擾
						while(fscanf(file,"%d/%d/%d",&v,&t,&n)>0){
							model->triangles[numtriangles].vindices[0]=v;
							model->triangles[numtriangles].tindices[0]=t;
							model->triangles[numtriangles].nindices[0]=n;
							fscanf(file,"%d/%d/%d",&v,&t,&n);
							model->triangles[numtriangles].vindices[1]=v;
							model->triangles[numtriangles].tindices[1]=t;
							model->triangles[numtriangles].nindices[1]=n;
							fscanf(file,"%d/%d/%d",&v,&t,&n);
							model->triangles[numtriangles].vindices[2]=v;
							model->triangles[numtriangles].tindices[2]=t;
							model->triangles[numtriangles].nindices[2]=n;
							group->triangles[group->numtriangles++]=numtriangles;
							numtriangles++;
						}
					}else if(sscanf(buf,"%d/%d",&v,&t)==2){
						//v/t,�n�߂̈��B�I���܂Ńf�[�^�擾�B
						while(fscanf(file,"%d/%d",&v,&t)>0){
							model->triangles[numtriangles].vindices[0]=v;
							model->triangles[numtriangles].tindices[0]=t;
							fscanf(file,"%d/%d",&v,&t);
							model->triangles[numtriangles].vindices[1]=v;
							model->triangles[numtriangles].tindices[1]=t;
							fscanf(file,"%d/%d",&v,&t);
							model->triangles[numtriangles].vindices[2]=v;
							model->triangles[numtriangles].tindices[2]=t;
							group->triangles[group->numtriangles++]=numtriangles;
							numtriangles++;
						}
					}else{
						//v�����̍\��,�n�߂̈��B�I���܂Ńf�[�^�擾�B
						while(fscanf(file,"%d",&v)>0){
							sscanf(buf,"%d",&v);
							model->triangles[numtriangles].vindices[0]=v;
							fscanf(file,"%d",&v);
							model->triangles[numtriangles].vindices[1]=v;
							fscanf(file,"%d",&v);
							model->triangles[numtriangles].vindices[2]=v;
							group->triangles[group->numtriangles++]=numtriangles;
							numtriangles++;
						}
					}
				}
				break;
			default:
				///�c��̍s���g���ʂ����Beat up rest of line
				fgets(buf,sizeof(buf),file);
				break;
		}
	}
	///announce the memory requirements
	LogDebug("//Memory: %lu bytes (Not actual byte because of memory alignment.) \n",
			 numvertices*3*sizeof(float)
			 +numnormals*3*sizeof(float)*(numnormals?1:0)
			 +numtexcoords*3*sizeof(float)*(numtexcoords?1:0)
			 +numtriangles*sizeof(oflTriangle_t));
	LogDebug("numvertices=%d\n",numvertices);
	LogDebug("numnormals=%d\n",numnormals);
	LogDebug("numtexcoords=%d\n",numtexcoords);
	LogDebug("numtriangles=%d\n",numtriangles);
}
//} //�����܂Ńv���C�x�[�g�֐��Q

// ��������p�u���b�N�֐��Q�n�� {

//���ӁF���_�̐���1-vertex num�܂łł���B0�͓����Ă��Ȃ��B
float oflUnitize(oflModel_t *model)
{
	uint32_t i;
	float maxx,minx,maxy,miny,maxz,minz;
	float cx,cy,cz,w,h,d;
	float scale;
	assert(model);
	assert(model->vertices);
	// get the max/mins 
	maxx=model->vertices[3+0];
	maxy=model->vertices[3+1];
	maxz=model->vertices[3+2];
	minx=model->vertices[3+0];
	miny=model->vertices[3+1];
	minz=model->vertices[3+2];
	LogDebug("maxx=%f,minx=%f \n",maxx,minx);
	LogDebug("maxy=%f,miny=%f \n",maxy,miny);
	LogDebug("maxz=%f,minz=%f \n",maxz,minz);
	
	if(0){
		for(i=0;i<model->numvertices;i++){
			LogDebug("%d %f %f %f \n",
					i,
					model->vertices[3*i+0],
					model->vertices[3*i+1],
					model->vertices[3*i+2] );
		}
	}

	for(i=1;i<=model->numvertices;i++){
		if(maxx<model->vertices[3*i+0]){
			maxx=model->vertices[3*i+0];
			//printf("here max!\n");
		}
		if(minx>model->vertices[3*i+0]){
			minx=model->vertices[3*i+0];
			//printf("here min! i=%d minx=%f \n", i, minx);
		}
		if(maxy<model->vertices[3*i+1]){
			maxy=model->vertices[3*i+1];
		}
		if(miny>model->vertices[3*i+1]){
			miny=model->vertices[3*i+1];
		}
		if(maxz<model->vertices[3*i+2]){
			maxz=model->vertices[3*i+2];
		}
		if(minz>model->vertices[3*i+2]){
			minz=model->vertices[3*i+2];
		}
	}
	LogDebug("maxx=%f,minx=%f \n",maxx,minx);
	LogDebug("maxy=%f,miny=%f \n",maxy,miny);
	LogDebug("maxz=%f,minz=%f \n",maxz,minz);
	//calculatemodelwidth,height,anddepth
	w=AbsFloat(maxx)+AbsFloat(minx);
	h=AbsFloat(maxy)+AbsFloat(miny);
	d=AbsFloat(maxz)+AbsFloat(minz);
	//calculatecenterofthemodel
	cx=(float)(maxx+minx)/2.0f;
	cy=(float)(maxy+miny)/2.0f;
	cz=(float)(maxz+minz)/2.0f;
	//calculateunitizingscalefactor
	scale=2.0f/MaxFloat(MaxFloat(w,h),d);
	//translate around center then scale
	for(i=0;i<=model->numvertices;i++){//������1����͂��߂Ă�OK�B0�͎g���Ă��Ȃ�
		model->vertices[3*i+0]-=cx;
		model->vertices[3*i+1]-=cy;
		model->vertices[3*i+2]-=cz;
		model->vertices[3*i+0]*=scale;
		model->vertices[3*i+1]*=scale;
		model->vertices[3*i+2]*=scale;
	}
	return scale;
}

void oflDimensions(oflModel_t *model,float *dimensions)
{
	uint32_t i;
	float maxx,minx,maxy,miny,maxz,minz;
	assert(model);
	assert(model->vertices);
	assert(dimensions);
	//get the max/mins 
	maxx=minx=model->vertices[3+0];
	maxy=miny=model->vertices[3+1];
	maxz=minz=model->vertices[3+2];
	for(i=1;i<=model->numvertices;i++){
		if(maxx<model->vertices[3*i+0]) maxx=model->vertices[3*i+0];
		if(minx>model->vertices[3*i+0]) minx=model->vertices[3*i+0];
		if(maxy<model->vertices[3*i+1]) maxy=model->vertices[3*i+1];
		if(miny>model->vertices[3*i+1]) miny=model->vertices[3*i+1];
		if(maxz<model->vertices[3*i+2]) maxz=model->vertices[3*i+2];
		if(minz>model->vertices[3*i+2]) minz=model->vertices[3*i+2];
	}
	// calculate model width, height, and depth 
	dimensions[0]=AbsFloat(maxx)+AbsFloat(minx);
	dimensions[1]=AbsFloat(maxy)+AbsFloat(miny);
	dimensions[2]=AbsFloat(maxz)+AbsFloat(minz);
}

void oflScale(oflModel_t *model,float scale)
{
	uint32_t i;
	for(i=1;i<=model->numvertices;i++){
		model->vertices[3*i+0]*=scale;
		model->vertices[3*i+1]*=scale;
		model->vertices[3*i+2]*=scale;
	}
}

void oflReverseWinding(oflModel_t *model)
{
	uint32_t i;
	uint32_t u32Swap;
	assert(model);
	for(i=0;i<model->numtriangles;i++){
		u32Swap=model->triangles[i].vindices[0];
		model->triangles[i].vindices[0]=model->triangles[i].vindices[2];
		model->triangles[i].vindices[2]=u32Swap;
		if(model->numnormals){
			u32Swap=model->triangles[i].nindices[0];
			model->triangles[i].nindices[0]=model->triangles[i].nindices[2];
			model->triangles[i].nindices[2]=u32Swap;
		}
		if(model->numtexcoords){
			u32Swap=model->triangles[i].tindices[0];
			model->triangles[i].tindices[0]=model->triangles[i].tindices[2];
			model->triangles[i].tindices[2]=u32Swap;
		}
	}
	// reverse facet normals 
	for(i=1;i<=model->numfacetnorms;i++){
		model->facetnorms[3*i+0]=-model->facetnorms[3*i+0];
		model->facetnorms[3*i+1]=-model->facetnorms[3*i+1];
		model->facetnorms[3*i+2]=-model->facetnorms[3*i+2];
	}
	// reverse vertex normals 
	for(i=1;i<=model->numnormals;i++){
		model->normals[3*i+0]=-model->normals[3*i+0];
		model->normals[3*i+1]=-model->normals[3*i+1];
		model->normals[3*i+2]=-model->normals[3*i+2];
	}
}

void oflFacetNormals(oflModel_t* model)
{
	uint32_t i;
	float u[3];
	float v[3];
	assert(model);
	assert(model->vertices);
	// clobber any old facetnormals
	if(model->facetnorms){
		free(model->facetnorms);
	}
	// allocate memory for the new facet normals
	model->numfacetnorms=model->numtriangles;
#	if APPLY_MEMALIGN
	model->facetnorms=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numfacetnorms+1)+SAFETY_PADDING_SIZE);
#	else
	model->facetnorms=(float*)malloc(sizeof(float)*3*(model->numfacetnorms+1)+SAFETY_PADDING_SIZE);
#	endif // APPLY_MEMALIGN
	for(i=0;i<model->numtriangles;i++){
		model->triangles[i].findex=i+1;
		u[0]=model->vertices[3*model->triangles[i].vindices[1]+0]-model->vertices[3*model->triangles[i].vindices[0]+0];
		u[1]=model->vertices[3*model->triangles[i].vindices[1]+1]-model->vertices[3*model->triangles[i].vindices[0]+1];
		u[2]=model->vertices[3*model->triangles[i].vindices[1]+2]-model->vertices[3*model->triangles[i].vindices[0]+2];
		v[0]=model->vertices[3*model->triangles[i].vindices[2]+0]-model->vertices[3*model->triangles[i].vindices[0]+0];
		v[1]=model->vertices[3*model->triangles[i].vindices[2]+1]-model->vertices[3*model->triangles[i].vindices[0]+1];
		v[2]=model->vertices[3*model->triangles[i].vindices[2]+2]-model->vertices[3*model->triangles[i].vindices[0]+2];
		//Vec3Cross(u,v,&model->facetnorms[3*(i+1)]);
		Vec3Cross( &model->facetnorms[3*(i+1)], u, v );
		Vec3Normalize(&model->facetnorms[3*(i+1)]);
	}
}

void oflVertexNormals(oflModel_t *model,float angle)
{
	oflNode_t* node;
	oflNode_t* tail;
	oflNode_t** members;
	float* normals;
	uint32_t numnormals;
	float average[3];
	float dot;
	float cos_angle;
	uint32_t avg;
	assert(model);
	assert(model->facetnorms);
	// �R�T�C���̌v�Z calculate the cosine of the angle ( in degrees ) 
	cos_angle=(float)cos(angle*M_PI/180.0);
	// �O�̖@���̊J�� nuke any previous normals 
	if(model->normals){
		free(model->normals);
	}
	// �V�����@���̗̈���m�� (allocate space for new normals)
	model->numnormals=model->numtriangles*3; // �O�p�`����3 �@��
#	if APPLY_MEMALIGN
	model->normals=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numnormals+1)+SAFETY_PADDING_SIZE);
#	else
	model->normals=(float*)malloc(sizeof(float)*3*(model->numnormals+1)+SAFETY_PADDING_SIZE);
#	endif //APPLY_MEMALIGN

	// ���ꂼ��̒��_�̎O�p�`�̃C���f�b�N�X�̃����N�h���X�g��ێ�����\���̂�ێ�����B
	// ( allocate a structure that will hold a linked list of triangle indices for each vertex )
#	if APPLY_MEMALIGN
	members=(oflNode_t**)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(oflNode_t*)*(model->numvertices+1)+SAFETY_PADDING_SIZE);
#	else
	members=(oflNode_t**)malloc(sizeof(oflNode_t*)*(model->numvertices+1)+SAFETY_PADDING_SIZE);
#	endif // APPLY_MEMALIGN

	for(uint32_t i=1;i<=model->numvertices;i++ ){
		members[i]=NULL;
	}

	// for every triangle, create a node for each vertex in it
	for(uint32_t i=0;i<model->numtriangles;i++){
#		if APPLY_MEMALIGN
		node=(oflNode_t*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(oflNode_t));
#		else
		node=(oflNode_t*)malloc(sizeof(oflNode_t)+SAFETY_PADDING_A_BIT);
#		endif // APPLY_MEMALIGN
		node->index=i;
		node->next=members[model->triangles[i].vindices[0]];
		members[model->triangles[i].vindices[0]]=node;
#		if APPLY_MEMALIGN
		node=(oflNode_t*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(oflNode_t));
#		else
		node=(oflNode_t*)malloc(sizeof(oflNode_t)+SAFETY_PADDING_A_BIT);
#		endif // APPLY_MEMALIGN
		node->index=i;
		node->next=members[model->triangles[i].vindices[1]];
		members[model->triangles[i].vindices[1]]=node;

#		if APPLY_MEMALIGN
		node=(oflNode_t*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(oflNode_t));
#		else
		node=(oflNode_t*)malloc(sizeof(oflNode_t)+SAFETY_PADDING_A_BIT);
#		endif // APPLY_MEMALIGN
		node->index=i;
		node->next=members[model->triangles[i].vindices[2]];
		members[model->triangles[i].vindices[2]]=node;
	}
	// calculate the average normal for each vertex
	numnormals=1;
	for(uint32_t i=1;i<=model->numvertices;i++){
		// �S�Ă̎O�p�`�A���_�𕽋ς���facet �@���ɂ���āA���_�̕��ς����@�����v�Z����
		//	calculate an average normal for this vertex by averaging the
		//	facet normal of every triangle this vertex is in
		node=members[i];
		if(!node){
			fprintf(stderr,"oflVertexNormals(): vertex w/o a triangle\n");
		}
		average[0]=0.0f;
		average[1]=0.0f;
		average[2]=0.0f;
		avg = 0;
		while(node){
			//	only average if the dot product of the angle between the two
			//	facet normals is greater than the cosine of the threshold
			//	angle -- or, said another way, the angle between the two
			//	facet normals is less than ( or equal to ) the threshold angle
			dot=Vec3Dot(
					&model->facetnorms[3*model->triangles[node->index].findex],
					&model->facetnorms[3*model->triangles[members[i]->index].findex]);
			if(dot>cos_angle){
				node->averaged=true;
				average[0]+=model->facetnorms[3*model->triangles[node->index].findex+0];
				average[1]+=model->facetnorms[3*model->triangles[node->index].findex+1];
				average[2]+=model->facetnorms[3*model->triangles[node->index].findex+2];
				avg=1;	//< we averaged at least one normal!
			}else{
				node->averaged=false;
			}
			node=node->next;
		}
		if(avg){
			// normalize the averaged normal
			Vec3Normalize(average);
			// add the normal to the vertex normals list 
			model->normals[3*numnormals+0]=average[0];
			model->normals[3*numnormals+1]=average[1];
			model->normals[3*numnormals+2]=average[2];
			avg=numnormals;
			numnormals++;
		}
		
		// set the normal of this vertex in each triangle it is in 
		node=members[i];
		while(node){
			if(node->averaged){
				//ifthisnodewasaveraged,usetheaveragenormal
				if(model->triangles[node->index].vindices[0]==i){
					model->triangles[node->index].nindices[0]=avg;
				}else if(model->triangles[node->index].vindices[1]==i){
					model->triangles[node->index].nindices[1]=avg;
				}else if(model->triangles[node->index].vindices[2]==i){
					model->triangles[node->index].nindices[2]=avg;
				}
			}else{
				/// if this node wasn't averaged, use the facet normal 
				model->normals[3*numnormals+0]=model->facetnorms[3*model->triangles[node->index].findex+0];
				model->normals[3*numnormals+1]=model->facetnorms[3*model->triangles[node->index].findex+1];
				model->normals[3*numnormals+2]=model->facetnorms[3*model->triangles[node->index].findex+2];
				if(model->triangles[node->index].vindices[0]==i){
					model->triangles[node->index].nindices[0]=numnormals;
				}else if(model->triangles[node->index].vindices[1]==i){
					model->triangles[node->index].nindices[1]=numnormals;
				}else if(model->triangles[node->index].vindices[2]==i){
					model->triangles[node->index].nindices[2]=numnormals;
				}
				numnormals++;
			}
			node=node->next;
		}
	}
	model->numnormals=numnormals-1;
	// free the member information
	for(uint32_t i=1;i<=(model->numvertices);i++){
		node=members[i];
		while(node){
			tail=node;
			node=node->next;
			free(tail);
		}
	}
	free(members);
	// pack the normals array ( we previously allocated the maximum number of normals
	//	that could possibly be created ( numtriangles*3 ),
	//	so get rid of some of them ( usually alot unless none of the facet normals were averaged ) )
	normals=model->normals;
#	if APPLY_MEMALIGN
	model->normals=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numnormals+1)+SAFETY_PADDING_SIZE);
#	else
	model->normals=(float*)malloc(sizeof(float)*3*(model->numnormals+1)+SAFETY_PADDING_SIZE);
#	endif // APPLY_MEMALIGN
	for(uint32_t i=1;i<=model->numnormals;i++){
		model->normals[3*i+0]=normals[3*i+0];
		model->normals[3*i+1]=normals[3*i+1];
		model->normals[3*i+2]=normals[3*i+2];
	}
	free(normals);
}

void oflLinearTexture(oflModel_t *model)
{
	oflGroup_t *group;
	float dimensions[3];
	float x,y,scalefactor;
	uint32_t i;
	assert(model);
	if(model->texcoords){
		free(model->texcoords);
	}
	model->numtexcoords=model->numvertices;
#	if APPLY_MEMALIGN
	model->texcoords=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*2*(model->numtexcoords+1+SAFETY_PADDING_SIZE));
#	else
	model->texcoords=(float*)malloc(sizeof(float)*2*(model->numtexcoords+1+SAFETY_PADDING_SIZE));
#	endif//APPLY_MEMALIGN

	oflDimensions(model,dimensions);
	// ��ԑ傫�ȗv�f�̐�Βl�̋t��x2 �ŃX�P�[�����O����
	scalefactor=2.0f/AbsFloat(MaxFloat(MaxFloat(dimensions[0],dimensions[1]),dimensions[2]));
	// do the calculations 
	for(i=1;i<=model->numvertices;i++){
		x=model->vertices[3*i+0]*scalefactor;
		y=model->vertices[3*i+2]*scalefactor;
		model->texcoords[2*i+0]=(x+1.0f)/2.0f;
		model->texcoords[2*i+1]=(y+1.0f)/2.0f;
	}
	//�S�Ă̎O�p�`�̃e�N�X�`�����W�C���f�b�N�X���o��
	//go through and put texture coordinate indices in all the triangles 
	group=model->groups;
	while(group){
		for(i=0;i<group->numtriangles;i++){
			model->triangles[(group->triangles[i])].tindices[0]=model->triangles[(group->triangles[i])].vindices[0];
			model->triangles[(group->triangles[i])].tindices[1]=model->triangles[(group->triangles[i])].vindices[1];
			model->triangles[(group->triangles[i])].tindices[2]=model->triangles[(group->triangles[i])].vindices[2];
		}	
		group=group->next;
	}
	LogDebug("oflLinearTexture( ): generated %d linear texture coordinates\n",
				(unsigned int)model->numtexcoords);
}

void oflSpheremapTexture(oflModel_t *model)
{
	oflGroup_t* group;
	float theta,phi,rho,x,y,z,r;
	uint32_t i;
	const float cfPi=3.14159265f;
	assert(model);
	assert(model->normals);
	if(model->texcoords){
		free(model->texcoords);
	}
	model->numtexcoords=model->numnormals;
#	if APPLY_MEMALIGN
	model->texcoords=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*2*(model->numtexcoords+1+SAFETY_PADDING_SIZE));
#	else
	model->texcoords=(float*)malloc(sizeof(float)*2*(model->numtexcoords+1+SAFETY_PADDING_SIZE));
#	endif // APPLY_MEMALIGN

	// SPHERE MAP ��NORMAL �̐���TEXCOORD ���w��
	for(i=1;i<=model->numnormals;i++){
		// re-arrange for pole distortion 
		z=model->normals[3*i+0];
		y=model->normals[3*i+1];
		x=model->normals[3*i+2];
		r=sqrt((x*x)+(y*y));
		rho=sqrt((r*r)+(z*z));
		if(r==0.0f){
			theta=0.0f;
			phi=0.0f;
		}else{
			if(z==0.0f){
				phi=(float)cfPi/2.0f;
			}else{
				phi=acos(z/rho);
			}
			if(y==0.0f){
				theta=(float)cfPi/2.0f;
			}else{
				theta=(float)asin(y/r)+(cfPi/2.0f);
			}
		}
		model->texcoords[2*i+0]=(float)theta/cfPi;
		model->texcoords[2*i+1]=(float)phi/cfPi;
	}
	// go through and put texcoord indices in all the triangles 
	group=model->groups;
	while(group){
		for(i=0;i<group->numtriangles;i++){
			model->triangles[group->triangles[i]].tindices[0]=model->triangles[group->triangles[i]].nindices[0];
			model->triangles[group->triangles[i]].tindices[1]=model->triangles[group->triangles[i]].nindices[1];
			model->triangles[group->triangles[i]].tindices[2]=model->triangles[group->triangles[i]].nindices[2];
		}
		group=group->next;
	}
}

void oflDelete(oflModel_t *model)
{
	oflGroup_t* group;
	uint32_t i;
	assert(model);//<���f�����Ȃ�������x���B
	if(model->pathname)
		free(model->pathname);
	if(model->mtllibname)
		free(model->mtllibname);
	if(model->vertices)
		free(model->vertices);
	if(model->normals)
		free(model->normals);
	if(model->texcoords)
		free(model->texcoords);
	if(model->facetnorms)
		free(model->facetnorms);
	if(model->triangles)
		free(model->triangles);
	if(model->materials){
		for(i=0;i<model->nummaterials;i++){
			free(model->materials[i].name);
		}
	}
	free(model->materials);
	while(model->groups){
		group=model->groups;
		model->groups=model->groups->next;
		free(group->name);
		free(group->triangles);
		free(group);
	}
	free(model);
}

oflModel_t* oflReadObj(const char *filename)
{
	oflModel_t *model;
	FILE *file;
	//�t�@�C���I�[�v��
	file=fopen(filename,"r");
	if(!file){
		fprintf(stderr, "oflReadObj() failed: can't open data file \"%s\".\n",filename);
		exit(EXIT_FAILURE);
	}
	// �V�������f���̃A���P�[�g
#	if APPLY_MEMALIGN
	model=(oflModel_t*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(oflModel_t));
#	else
	model=(oflModel_t*)malloc(sizeof(oflModel_t));
#	endif //APPLY_MEMALIGN
	model->pathname		=strdup(filename);
	model->mtllibname	=NULL;
	model->numvertices	=0;
	model->vertices		=NULL;
	model->numnormals	=0;
	model->normals		=NULL;
	model->numtexcoords	=0;
	model->texcoords	=NULL;
	model->numfacetnorms=0;
	model->facetnorms	=NULL;
	model->numtriangles	=0;
	model->triangles	=NULL;
	model->nummaterials	=0;
	model->materials	=NULL;
	model->numgroups	=0;
	model->groups		=NULL;
	model->position[0]	=0.0;
	model->position[1]	=0.0;
	model->position[2]	=0.0;
	// �n�߂̃p�X�Ńt�@�C����ǂݗ����A���_���A�@���̐��A�e�N�X�`���ƎO�p�`�̐����擾
	// -	make a first pass through the file to get a count of \n
	//		the number of vertices, normals, texcoords & triangles
	//printf( "debug: 1 numnormals=%d \n", model->numnormals );
	oflFirstPass(model,file);
	//printf( "debug: 2 numnormals=%d \n", model->numnormals );
	/// allocate memory 
	
	/// vertices
#	if APPLY_MEMALIGN
	model->vertices=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
#	else
	model->vertices=(float*)malloc(sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
#	endif//APPLY_MEMALIGN

	//triangles
#	if APPLY_MEMALIGN
	model->triangles=(oflTriangle_t *)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(oflTriangle_t)*model->numtriangles+SAFETY_PADDING_A_BIT);
#	else
	model->triangles=(oflTriangle_t *)malloc(sizeof(oflTriangle_t)*model->numtriangles+SAFETY_PADDING_A_BIT);
#	endif//APPLY_MEMALIGN

	//normals
	if(model->numnormals){
#		if APPLY_MEMALIGN
		model->normals=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numnormals+1)+SAFETY_PADDING_A_BIT);
#		else
		model->normals=(float*)malloc(sizeof(float)*3*(model->numnormals+1)+SAFETY_PADDING_A_BIT);
#		endif//APPLY_MEMALIGN
	}
	//printf( "debug: 3 numnormals=%d \n", model->numnormals );

	//texcoords
	if(model->numtexcoords){
#		if APPLY_MEMALIGN
		model->texcoords=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*2*(model->numtexcoords+1)+SAFETY_PADDING_A_BIT);
#		else
		model->texcoords=(float*)malloc(sizeof(float)*2*(model->numtexcoords+1)+SAFETY_PADDING_A_BIT);
#		endif//APPLY_MEMALIGN
	}
	rewind(file); //�n�߂̃p�X�͏I���B�Ăуt�@�C���|�C���^��擪�܂Ŋ����߂��B
	oflSecondPass(model,file); // 2��ڂ̓ǂݍ��݃p�X�J�n�B
	fclose(file);
	return model;
}

oflModel_t* oflReadGb(const char *filename)
{
	printf("%s begin.\n",__FUNCTION__);
	oflModel_t *model;
	oflBinHeader_t header;
	//�t�@�C���I�[�v��
	FILE *fp;
	if((fp = fopen(filename, "rb")) == NULL ) {
		printf("file open error.\n");
		exit(EXIT_FAILURE);
	}
	fread(&header,sizeof(oflBinHeader_t),1,fp);
	if(header.version!=2){
		assert(1);
	}

	// �V�������f���̃A���P�[�g
#	if APPLY_MEMALIGN
	model=(oflModel_t*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(oflModel_t));
#	else
	model=(oflModel_t*)malloc(sizeof(oflModel_t));
#	endif //APPLY_MEMALIGN

	model->pathname		=strdup(filename);
	model->mtllibname	=NULL;

	model->numvertices	=header.numVerticies;
	model->vertices		=NULL;
	
	if(header.elemNumNorm>0){
		model->numnormals=header.numVerticies;
	}else{
		model->numnormals=0;
	}
	model->normals		=NULL;

	if(header.elemNumTangent>0){
		model->numtangents=header.numVerticies;
	}else{
		model->numtangents=0;
	}
	model->tangents=NULL;

	if(header.elemNumBinormal>0){
		model->numbinormals=header.numVerticies;
	}else{
		model->numbinormals=0;
	}
	model->binormals=NULL;

	if(header.elemNumTex>0){
		model->numtexcoords	=header.numVerticies;
	}else{
		model->numtexcoords	=0;
	}
	model->texcoords	=NULL;
	model->numfacetnorms=0;
	model->facetnorms	=NULL;
	model->numtriangles	=header.numTriangles;
	model->triangles	=NULL;
	model->nummaterials	=0;
	model->materials	=NULL;
	model->numgroups	=0;
	model->groups		=NULL;
	model->position[0]	=0.0;
	model->position[1]	=0.0;
	model->position[2]	=0.0;

	model->groups=oflAddGroup(model,"default");
	model->groups[0].numtriangles=model->numtriangles;//�O���[�v�͈��
	printf("signature=%s\n",		header.signature);
	printf("version=%d\n",			header.version);
	printf("numTriangles=%d\n",		header.numTriangles);
	printf("numVerticies=%d\n",		header.numVerticies);
	printf("elemNumPos =%d\n",		header.elemNumPos);
	printf("elemNumNorm=%d\n",		header.elemNumNorm);
	printf("elemNumTangent=%d\n",	header.elemNumTangent);
	printf("elemNumBinormal=%d\n",	header.elemNumBinormal);
	printf("elemNumTex =%d\n",		header.elemNumTex);
	printf("sizePos =%d\n",			header.sizePos);
	printf("sizeNorm=%d\n",			header.sizeNorm);
	printf("sizeTangent=%d\n",		header.sizeTangent);
	printf("sizeBinormal=%d\n",		header.sizeBinormal);
	printf("sizeTex =%d\n",			header.sizeTex);
	printf("offsetToData=%d\n",		header.offsetToData);
	printf("offsetToPos =%d\n",		header.offsetToPos );
	printf("offsetToNorm=%d\n",		header.offsetToNorm);
	printf("offsetToTangent=%d\n",	header.offsetToTangent);
	printf("offsetToBinormal=%d\n", header.offsetToBinormal);
	printf("offsetToTex =%d\n",		header.offsetToTex );
	//printf("debug %d\n",__LINE__);
	/// vertices
#	if APPLY_MEMALIGN
	model->vertices=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
#	else
	model->vertices=(float*)malloc(sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
#	endif//APPLY_MEMALIGN

	//triangles
#	if APPLY_MEMALIGN
	model->triangles=(oflTriangle_t *)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(oflTriangle_t)*header.numTriangles+SAFETY_PADDING_A_BIT);
#	else
	model->triangles=(oflTriangle_t *)malloc(sizeof(oflTriangle_t)*header.numTriangles+SAFETY_PADDING_A_BIT);
#	endif//APPLY_MEMALIGN

	//normals
	if(model->numnormals){
#		if APPLY_MEMALIGN
		model->normals=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
#		else
		model->normals=(float*)malloc(sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
#		endif//APPLY_MEMALIGN
	}

	//tangents
	if(model->numtangents){
#		if APPLY_MEMALIGN
		model->tangents=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
#		else
		model->tangents=(float*)malloc(sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
#		endif//APPLY_MEMALIGN
	}

	//binormals
	if(model->numbinormals){
#		if APPLY_MEMALIGN
		model->binormals=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
#		else
		model->binormals=(float*)malloc(sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
#		endif//APPLY_MEMALIGN
	}

	//texcoords
	if(model->numtexcoords){
#		if APPLY_MEMALIGN
		model->texcoords=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
#		else
		model->texcoords=(float*)malloc(sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
#		endif//APPLY_MEMALIGN
	}
#	if APPLY_MEMALIGN
	model->groups[0].triangles=(uint32_t*)memalign(nsOfl::SAFETY_ALIGNMENT_SIZE,
												sizeof(uint32_t)*(model->groups->numtriangles+1)+nsOfl::SAFETY_PADDING_A_BIT);
#	else
	//model->groups[0].numtriangles=model->numtriangles;
	model->groups[0].triangles=(uint32_t*)malloc(sizeof(uint32_t)*(model->groups[0].numtriangles+1)+nsOfl::SAFETY_PADDING_A_BIT);
#	endif //APPLY_MEMALIGN
	//pos =(float*)malloc(header.sizePos);
	//norm=(float*)malloc(header.sizeNorm);
	//tex =(float*)malloc(header.sizeTex);
	//printf("debug %d\n",__LINE__);
	//printf("pos\n");
	fseek(fp,header.offsetToPos,SEEK_SET);
	//printf("debug %d\n",__LINE__);
	fread(model->vertices,header.sizePos,1,fp);
	//printf("debug %d\n",__LINE__);
	//for(int i=0;i<header.numVerticies;i++){
	//	printf("%f ",model->vertices[i*header.elemNumPos+0]);
	//	printf("%f ",model->vertices[i*header.elemNumPos+1]);
	//	printf("%f ",model->vertices[i*header.elemNumPos+2]);
	//	printf("\n");
	//}
	//glVertex3fv(&model->vertices[3*triangle->vindices[2]]);

	//printf("norm\n");
	if(model->numnormals){
		fseek(fp,header.offsetToNorm,SEEK_SET);
		fread(model->normals,header.sizeNorm,1,fp);
		//for(int i=0;i<header.numVerticies;i++){
		//	printf("%f ",model->normals[i*header.elemNumNorm+0]);
		//	printf("%f ",model->normals[i*header.elemNumNorm+1]);
		//	printf("%f ",model->normals[i*header.elemNumNorm+2]);
		//	printf("\n");
		//}
	}

	if(model->numtangents){
		fseek(fp,header.offsetToTangent,SEEK_SET);
		fread(model->tangents,header.sizeTangent,1,fp);
	}

	if(model->numbinormals){
		fseek(fp,header.offsetToBinormal,SEEK_SET);
		fread(model->binormals,header.sizeBinormal,1,fp);
	}


	//printf("tex\n");
	if(model->numtexcoords){
		fseek(fp,header.offsetToTex,SEEK_SET);
		fread(model->texcoords,header.sizeTex,1,fp);
		//for(int i=0;i<header.numVerticies;i++){
		//	printf("%f ",model->texcoords[i*header.elemNumTex+0]);
		//	printf("%f ",model->texcoords[i*header.elemNumTex+1]);
		//	printf("\n");
		//}
	}
	fclose(fp);
	for(unsigned int i=0;i<header.numTriangles;i++){
		model->triangles[i].vindices[0]=i*3+0;
		model->triangles[i].vindices[1]=i*3+1;
		model->triangles[i].vindices[2]=i*3+2;
		if(model->numnormals){
			model->triangles[i].nindices[0]=i*3+0;
			model->triangles[i].nindices[1]=i*3+1;
			model->triangles[i].nindices[2]=i*3+2;
		}
		//tangent ��binormal��normal�Ɠ���index�ł������낤

		if(model->numtexcoords){
			model->triangles[i].tindices[0]=i*3+0;
			model->triangles[i].tindices[1]=i*3+1;
			model->triangles[i].tindices[2]=i*3+2;
		}
	}
	for(uint32_t i=0;i<model->numtriangles;i++){
		model->groups[0].triangles[i]=i;//DCC�d�l�ɍ��킹��2�i�K�ɃC���f�b�N�X�����邩�������x�w�肵�Ă���B
	}
	printf("%s end.\n",__FUNCTION__);
	return model;
}

oflModel_t* oflCube(void)
{
	LogTrace("start\n");
	oflModel_t *model;
#	if APPLY_MEMALIGN
	model=(oflModel_t*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(oflModel_t));//���f���̃������m��
#	else
	model=(oflModel_t*)malloc(sizeof(oflModel_t));
#	endif // APPLY_MEMALIGN
	LogTrace("\n");
	//model->pathname		=strdup(filename);
	//sprintf(model->pathname,"oflCube");
	model->mtllibname	=NULL;
	model->numvertices	=8;
	model->vertices		=NULL;
	model->numnormals	=8;
	model->normals		=NULL;
	model->numtexcoords	=8;
	model->texcoords	=NULL;
	model->numfacetnorms=0;
	model->facetnorms	=NULL;
	model->numtriangles	=12;
	model->triangles	=NULL;
	model->nummaterials	=0;
	model->materials	=NULL;
	model->numgroups	=0;
	model->groups		=NULL;
	model->position[0]	=0.0f;
	model->position[1]	=0.0f;
	model->position[2]	=0.0f;
	LogTrace("\n");
	model->groups=oflAddGroup(model,"default");
#	if APPLY_MEMALIGN
	model->vertices=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
	model->triangles=(oflTriangle_t*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(oflTriangle_t)*model->numtriangles+SAFETY_PADDING_A_BIT);
#	else
	model->vertices=(float*)malloc(sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_A_BIT);
	model->triangles=(oflTriangle_t*)malloc(sizeof(oflTriangle_t)*model->numtriangles+SAFETY_PADDING_A_BIT);
#	endif // APPLY_MEMALIGN
	LogTrace("\n");
	if(model->numnormals){
#		if APPLY_MEMALIGN
		model->normals=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numnormals+1)+SAFETY_PADDING_A_BIT);
#		else
		model->normals=(float*)malloc(sizeof(float)*3*(model->numnormals+1)+SAFETY_PADDING_A_BIT);
#		endif //APPLY_MEMALIGN
	}
	if(model->numtexcoords){
#		if APPLY_MEMALIGN
		model->texcoords=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*2*(model->numtexcoords+1)+SAFETY_PADDING_A_BIT);
#		else
		model->texcoords=(float*)malloc(sizeof(float)*2*(model->numtexcoords+1)+SAFETY_PADDING_A_BIT);
#		endif //APPLY_MEMALIGN
	}
	model->groups->numtriangles=model->numtriangles;

#	if APPLY_MEMALIGN
	model->groups->triangles=(uint32_t*)memalign(nsOfl::SAFETY_ALIGNMENT_SIZE,
					sizeof(uint32_t)*(model->groups->numtriangles+1)+nsOfl::SAFETY_PADDING_A_BIT);
#	else
	model->groups->triangles=(uint32_t*)malloc(sizeof(uint32_t)*(model->groups->numtriangles+1)+nsOfl::SAFETY_PADDING_A_BIT);
#	endif //APPLY_MEMALIGN

	LogTrace("\n");
	float S=-0.5f;
	float L=+0.5f;
	model->vertices[0*3+0+3]=S;//
	model->vertices[0*3+1+3]=S;
	model->vertices[0*3+2+3]=S;
	model->vertices[1*3+0+3]=S;//
	model->vertices[1*3+1+3]=L;
	model->vertices[1*3+2+3]=S;
	model->vertices[2*3+0+3]=L;//
	model->vertices[2*3+1+3]=L;
	model->vertices[2*3+2+3]=S;
	model->vertices[3*3+0+3]=L;//
	model->vertices[3*3+1+3]=S;
	model->vertices[3*3+2+3]=S;
	model->vertices[4*3+0+3]=S;//
	model->vertices[4*3+1+3]=S;
	model->vertices[4*3+2+3]=L;
	model->vertices[5*3+0+3]=S;//
	model->vertices[5*3+1+3]=L;
	model->vertices[5*3+2+3]=L;
	model->vertices[6*3+0+3]=L;//
	model->vertices[6*3+1+3]=L;
	model->vertices[6*3+2+3]=L;
	model->vertices[7*3+0+3]=L;//
	model->vertices[7*3+1+3]=S;
	model->vertices[7*3+2+3]=L;
	//����index��Wavefront Obj�t�@�C���d�l��+1�ɂȂ��Ă��邽�ߒ��ӂ��邱�ƁB
	model->triangles[0].vindices[0]=3;
	model->triangles[0].vindices[1]=1;
	model->triangles[0].vindices[2]=2;
	model->triangles[1].vindices[0]=1;
	model->triangles[1].vindices[1]=3;
	model->triangles[1].vindices[2]=4;
	model->triangles[2].vindices[0]=4;
	model->triangles[2].vindices[1]=3;
	model->triangles[2].vindices[2]=7;
	model->triangles[3].vindices[0]=4;
	model->triangles[3].vindices[1]=7;
	model->triangles[3].vindices[2]=8;
	model->triangles[4].vindices[0]=5;
	model->triangles[4].vindices[1]=8;
	model->triangles[4].vindices[2]=7;
	model->triangles[5].vindices[0]=5;
	model->triangles[5].vindices[1]=7;
	model->triangles[5].vindices[2]=6;
	model->triangles[6].vindices[0]=5;
	model->triangles[6].vindices[1]=6;
	model->triangles[6].vindices[2]=2;
	model->triangles[7].vindices[0]=5;
	model->triangles[7].vindices[1]=2;
	model->triangles[7].vindices[2]=1;
	model->triangles[8].vindices[0]=3;
	model->triangles[8].vindices[1]=2;
	model->triangles[8].vindices[2]=6;
	model->triangles[9].vindices[0]=3;
	model->triangles[9].vindices[1]=6;
	model->triangles[9].vindices[2]=7;
	model->triangles[10].vindices[0]=1;
	model->triangles[10].vindices[1]=4;
	model->triangles[10].vindices[2]=5;
	model->triangles[11].vindices[0]=4;
	model->triangles[11].vindices[1]=8;
	model->triangles[11].vindices[2]=5;
	for(uint32_t i=0;i<model->numtriangles;i++){
		model->groups->triangles[i]=i;//DCC�d�l�ɍ��킹��2�i�K�ɃC���f�b�N�X�����邩�������x�w�肵�Ă���B
	}
	LogTrace("end\n");
	return model;
}

void oflWriteObj(oflModel_t *model,char *filename,uint32_t mode)
{
	LogTrace("start\n");
	uint32_t i;
	FILE *file;
	oflGroup_t *group;
	assert(model);//< ���f���|�C���^�̃`�F�b�N
	// �x��
	if(mode & OFL_FLAT && !model->facetnorms){
		printf("oflWriteObj() warning: flat normal output requested with no facet normals defined.\n" );
		mode &= ~OFL_FLAT;
	}
	if(mode & OFL_SMOOTH && !model->normals){
		printf("oflWriteObj() warning: smooth normal output requested with no normals defined.\n" );
		mode &= ~OFL_SMOOTH;
	}
	if(mode & OFL_TEXTURE && !model->texcoords){
		printf("oflWriteObj() warning: texture coordinate output requested "
			"with no texture coordinates defined.\n" );
		mode &= ~OFL_TEXTURE;
	}
	if(mode & OFL_FLAT && mode & OFL_SMOOTH){
		printf( "oflWriteObj() warning: flat normal output requested "
			"and smooth normal output requested ( using smooth ).\n" );
		mode &= ~OFL_FLAT;
	}
	if(mode & OFL_COLOR && !model->materials){
		printf( "oflWriteObj( ) warning: color output requested "
			"with no colors ( materials ) defined.\n" );
		mode &= ~OFL_COLOR;
	}
	if(mode & OFL_MATERIAL && !model->materials){
		printf( "oflWriteObj( ) warning: material output requested "
			"with no materials defined.\n" );
		mode &= ~OFL_MATERIAL;
	}
	if(mode & OFL_COLOR && mode & OFL_MATERIAL){
		printf( "oflWriteObj( ) warning: color and material output requested "
			"outputting only materials.\n" );
		mode &= ~OFL_COLOR;
	}

	// �t�@�C���I�[�v��
	file=fopen(filename,"w");
	if(!file){
		fprintf(stderr,"oflWriteObj() failed: can't open file \"%s\" to write.\n",filename);
		exit(EXIT_FAILURE);
	}
	// �w�b�_�[���o�� spit out a header 
	fprintf(file,"# Wavefront OBJ file. by libofl, KOGUCHI Takahiro\n");
	if(mode&OFL_MATERIAL && model->mtllibname){
		fprintf(file,"\nmtllib %s\n\n",model->mtllibname);
		oflWriteMtl(model,filename,model->mtllibname);
	}
	// ���_���o��
	// spit out the vertices 
	fprintf(file,"\n" );
	fprintf(file,"# %d vertices\n", model->numvertices );
	for(i=1;i<=(model->numvertices);i++){
		fprintf(file,"v %f %f %f\n",
			model->vertices[3*i+0],model->vertices[3*i+1],model->vertices[3*i+2]);
	}
	// �X���[�Y/�t���b�g�Ȗ@�����o�� spit out the smooth/flat normals
	if(mode & OFL_SMOOTH){
		fprintf(file,"\n");
		fprintf(file,"# %d normals\n",model->numnormals);
		for(i=1;i<=model->numnormals;i++){
			fprintf(file,"vn %f %f %f\n",
					model->normals[3*i+0],model->normals[3*i+1],model->normals[3*i+2]);
		}
	}else if(mode & OFL_FLAT){
		fprintf(file,"\n");
		fprintf(file,"# %d normals\n", model->numfacetnorms );
		for(i=1;i<=model->numnormals;i++){
			fprintf(file,"vn %f %f %f\n", 
				model->facetnorms[3*i+0],model->facetnorms[3*i+1],model->facetnorms[3*i+2]);
		}
	}
	// �e�N�X�`�����W�𕪒f�Bspit out the texture coordinates 
	if(mode & OFL_TEXTURE){
		fprintf(file,"\n");
		fprintf(file,"# %d texcoords\n",model->numtexcoords);
		for(i=1;i<=model->numtexcoords;i++){
			fprintf(file,"vt %f %f\n",model->texcoords[2*i+0],model->texcoords[2*i+1]);
		}
	}
	fprintf(file,"\n");
	fprintf(file,"# %d groups\n",model->numgroups);
	fprintf(file,"# %d faces (triangles)\n",model->numtriangles);
	fprintf(file,"\n");

	LogTrace("group\n");
	group=model->groups;
	while(group){
		fprintf(file,"g %s\n",group->name);
		if(mode & OFL_MATERIAL){
			fprintf(file,"usemtl %s\n",model->materials[group->material].name);
		}
		for(i=0;i<group->numtriangles;i++){
			if(mode & OFL_SMOOTH && mode & OFL_TEXTURE){
				fprintf(file,"f %d/%d/%d %d/%d/%d %d/%d/%d\n",
					model->triangles[(group->triangles[i])].vindices[0], 
					model->triangles[(group->triangles[i])].nindices[0], 
					model->triangles[(group->triangles[i])].tindices[0],
					model->triangles[(group->triangles[i])].vindices[1],
					model->triangles[(group->triangles[i])].nindices[1],
					model->triangles[(group->triangles[i])].tindices[1],
					model->triangles[(group->triangles[i])].vindices[2],
					model->triangles[(group->triangles[i])].nindices[2],
					model->triangles[(group->triangles[i])].tindices[2] );
			}else if(mode & OFL_FLAT && mode & OFL_TEXTURE){
				fprintf(file,"f %d/%d %d/%d %d/%d\n",
					model->triangles[(group->triangles[i])].vindices[0],
					model->triangles[(group->triangles[i])].findex,
					model->triangles[(group->triangles[i])].vindices[1],
					model->triangles[(group->triangles[i])].findex,
					model->triangles[(group->triangles[i])].vindices[2],
					model->triangles[(group->triangles[i])].findex);
			}else if (mode & OFL_TEXTURE){
				fprintf(file,"f %d/%d %d/%d %d/%d\n",
					model->triangles[(group->triangles[i])].vindices[0],
					model->triangles[(group->triangles[i])].tindices[0],
					model->triangles[(group->triangles[i])].vindices[1],
					model->triangles[(group->triangles[i])].tindices[1],
					model->triangles[(group->triangles[i])].vindices[2],
					model->triangles[(group->triangles[i])].tindices[2]);
			}else if(mode & OFL_SMOOTH){
				fprintf(file,"f %d//%d %d//%d %d//%d\n",
					model->triangles[(group->triangles[i])].vindices[0],
					model->triangles[(group->triangles[i])].nindices[0],
					model->triangles[(group->triangles[i])].vindices[1],
					model->triangles[(group->triangles[i])].nindices[1],
					model->triangles[(group->triangles[i])].vindices[2],
					model->triangles[(group->triangles[i])].nindices[2]);
			}else if(mode & OFL_FLAT){
				fprintf(file, "f %d//%d %d//%d %d//%d\n",
					model->triangles[(group->triangles[i])].vindices[0],
					model->triangles[(group->triangles[i])].findex,
					model->triangles[(group->triangles[i])].vindices[1],
					model->triangles[(group->triangles[i])].findex,
					model->triangles[(group->triangles[i])].vindices[2],
					model->triangles[(group->triangles[i])].findex);
			}else{
				fprintf(file,"f %d %d %d\n",
					model->triangles[(group->triangles[i])].vindices[0],
					model->triangles[(group->triangles[i])].vindices[1],
					model->triangles[(group->triangles[i])].vindices[2]);
			}
		}
		fprintf(file,"\n");
		group=group->next;
	}
	fclose(file);
	LogTrace("end\n");
}

void oflWeld(oflModel_t *model,float epsilon)
{
	float *vectors;
	float *copies;
	uint32_t numvectors;
	uint32_t i;
	// vertices
	numvectors=model->numvertices;
	vectors=model->vertices;
	copies=oflWeldVectors(vectors,&numvectors,epsilon);
#if 1
	printf("oflWeld(): %u redundant vertices.\n",(unsigned int)model->numvertices-numvectors-1);
#endif
	for(i=0;i<model->numtriangles;i++){
		model->triangles[(i)].vindices[0]=(uint32_t)vectors[3*model->triangles[(i)].vindices[0]+0];
		model->triangles[(i)].vindices[1]=(uint32_t)vectors[3*model->triangles[(i)].vindices[1]+0];
		model->triangles[(i)].vindices[2]=(uint32_t)vectors[3*model->triangles[(i)].vindices[2]+0];
	}
	//free space for old vertices
	free(vectors);
	// allocate space for the new vertices
	model->numvertices = numvectors;
#if APPLY_MEMALIGN
	model->vertices=(float*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_SIZE);
#else
	model->vertices=(float*)malloc(sizeof(float)*3*(model->numvertices+1)+SAFETY_PADDING_SIZE);
#endif // APPLY_MEMALIGN
	// copy the optimized vertices into the actual vertex list
	for(i=1;i<=model->numvertices;i++){
		model->vertices[3*i+0]=copies[3*i+0];
		model->vertices[3*i+1]=copies[3*i+1];
		model->vertices[3*i+2]=copies[3*i+2];
	}
	free(copies);
}

void oflPrintAndAdjustNewLine( uint32_t index, uint32_t change_line_elem=3 )
{
	//static const uint32_t change_line_elem = 3;
	if(index%change_line_elem==(change_line_elem-1)){
		printf("\n\t");
	}

}

//obj�����o�C�i���Ƃ��ďo�͂���B
//printf�ŕW���o�͂ɏo�Ă��邱�Ƃɒ��ӁB
void oflExportBin(oflModel_t *model,char *fn, bool exportTangentEnable, bool exportBinormalEnable)
{
	enum{
		OUTPUT_INDEX_DIRECT=0,		//�C���f�b�N�X���A�g���r���[�g���Ƃɕ�����
		OUTPUT_INDEX_SUMMARIZED,	//�C���f�b�N�X����ɂ���B
		OUTPUT_INDEX_END,
	};
	enum{
		OUTPUT_MATERIAL_NONE=0,
		OUTPUT_MATERIAL_DIRECT,
		OUTPUT_MATERIAL_END,
	};
	enum{
		OUTPUT_GROUP_NONE=0,
		OUTPUT_GROUP_DIRECT,
		OUTPUT_GROUP_END,
	};
	const uint32_t output_index_control=OUTPUT_INDEX_SUMMARIZED;
	const uint32_t output_material_control=OUTPUT_MATERIAL_NONE;
	const uint32_t output_group_control=OUTPUT_GROUP_DIRECT;
	const bool output_normal_generation_enable=false;
	const bool NORMALIZE_NORMAL_ENABLE=false;
	const uint32_t elemnum=10; ///<triangle elem �̗v�f�� ���ʎg��Ȃ��悤�ɂ����B

	//�f�[�^�S�̂̏o�́B�O���[�v�͉������Ȃ��B�K���o��
	{
		FILE *fp;
		//----------------------------------------------------------------
		//�w�b�_���o��
		//�o�C�i��
		//----------------------------------------------------------------
		// �����ŁA�t�@�C���|�C���^���擾����
		if ((fp = fopen(fn, "wb")) == NULL) {
			printf("file open error!!\n");
			exit(EXIT_FAILURE);
		}
		oflBinHeader_t binHeader;
		sprintf(binHeader.signature,"ofl bin export");
		//sprintf(binHeader.pathname,"%s",model->pathname);
		binHeader.version=2;
		binHeader.numTriangles=model->numtriangles;
		binHeader.numVerticies=model->numtriangles*3;
		binHeader.elemNumPos	=3;
		if((int32_t)model->numnormals!=0){
			binHeader.elemNumNorm	=3;
			if(exportTangentEnable){
				binHeader.elemNumTangent=3;
			}else{
				binHeader.elemNumTangent=0;
			}
			if(exportBinormalEnable){
				binHeader.elemNumBinormal=3;
			}else{
				binHeader.elemNumBinormal=0;
			}
		}else{
			binHeader.elemNumNorm		=0;
			binHeader.elemNumTangent	=0;
			binHeader.elemNumBinormal	=0;
		}
		if((int32_t)model->numtexcoords){
			binHeader.elemNumTex	=2;
		}else{
			binHeader.elemNumTex	=0;
		}
		binHeader.sizePos		=binHeader.elemNumPos *binHeader.numVerticies*sizeof(float);
		binHeader.sizeNorm		=binHeader.elemNumNorm*binHeader.numVerticies*sizeof(float);
		binHeader.sizeTangent	=binHeader.elemNumTangent*binHeader.numVerticies*sizeof(float);
		binHeader.sizeBinormal	=binHeader.elemNumBinormal*binHeader.numVerticies*sizeof(float);
		binHeader.sizeTex		=binHeader.elemNumTex *binHeader.numVerticies*sizeof(float);
		//binHeader.offsetToHeader=0;
		binHeader.offsetToData		=sizeof(oflBinHeader_t); // header byte
		binHeader.offsetToPos		=binHeader.offsetToData;
		binHeader.offsetToNorm		=binHeader.offsetToPos		+ binHeader.sizePos;
		binHeader.offsetToTangent	=binHeader.offsetToNorm		+ binHeader.sizeNorm;
		binHeader.offsetToBinormal	=binHeader.offsetToTangent	+ binHeader.sizeTangent;
		binHeader.offsetToTex		=binHeader.offsetToBinormal + binHeader.sizeBinormal;
		//fprintf(fp,"char pathname[]=\"%s\";\n",model->pathname);
		//fprintf("char mtlname[]=\"%s\"; \n",model->mtllibname);
		//fprintf(fp,"int numTriangles=%d;\n",(int)(model->numtriangles));
		//fprintf(fp,"int numVerticies=%d;\n",(int)(model->numtriangles*3));
		//printf("int triangle_size=%d; \n", (uint32_t)sizeof(oflTriangle_t));
		//printf("int numvertices=%d;\n",(int32_t)model->numvertices );
		//fclose(fp);
		fwrite(&binHeader,sizeof(oflBinHeader_t),1,fp);

		//----------------------------------------------------------------
		//�{�f�B�i�f�[�^�j���o��
		//�o�C�i��
		//----------------------------------------------------------------

		//	obj �t�@�C���̌X�̃A�g���r���[�g�̃C���f�b�N�X�����g�킸�A
		//	���ׂĕR�����ďo�́B���GPU�ł͕����C���f�b�N�X�͈����Ȃ��B
		//position�o��
		float loadedf;
		const float zeroData=0.0f;
		for(uint32_t i=0;i<model->numtriangles;i++){	// triangle index 
			for(uint32_t j=0;j<3;j++){					// vertex index
				for(uint32_t k=0;k<3;k++){				// x y z
					loadedf=model->vertices[3*(model->triangles[i].vindices[j])+k];
					if(isnan(loadedf)==0){
						fwrite(&loadedf,sizeof(float),1,fp);
					}else{
						fwrite(&zeroData,sizeof(float),1,fp);
					}
				}
			}
		}

		//�@���o��
		float normal[3];
		float tangent[3];
		float binormal[3];
		if((int32_t)model->numnormals!=0){
			if(NORMALIZE_NORMAL_ENABLE){
				//printf("// NORMALIZE_NORMAL_ENABLE \n");
			}else{
				//printf("// NORMALIZE_NORMAL_DISABLE \n");
			}
			if(output_normal_generation_enable){
				oflVertexNormals(model,0.0f);
			}
			for(uint32_t i=0;i<model->numtriangles;i++){ // triangle
				for(uint32_t j=0;j<3;j++){ // vertex 
					//normalize normal
					normal[0]=model->normals[3*(model->triangles[i].nindices[j])+0];
					normal[1]=model->normals[3*(model->triangles[i].nindices[j])+1];
					normal[2]=model->normals[3*(model->triangles[i].nindices[j])+2];
					if(NORMALIZE_NORMAL_ENABLE){
						Vec3Normalize(normal);
					}
					for(uint32_t k=0;k<3;k++){ //< nx ny nz
						if(isnan(normal[k])==0){
							fwrite(&normal[k],sizeof(float),1,fp);
						}else{
							fwrite(&zeroData,sizeof(float),1,fp);
						}
					}
				}
			}

			if(exportTangentEnable){
				for(uint32_t i=0;i<model->numtriangles;i++){ // triangle
					float pos0[3];
					float pos1[3];
					float pos2[3];
					float uv0[2];
					float uv1[2];
					float uv2[2];
					for(uint32_t k=0;k<3;k++){
						pos0[k] = model->vertices[3*(model->triangles[i].vindices[0])+k];
						if ( isnan( pos0[k] )!=0 ){
							pos0[k]=0.0f;
						}
					}
					for(uint32_t k=0;k<3;k++){
						pos1[k] = model->vertices[3*(model->triangles[i].vindices[1])+k];
						if ( isnan( pos1[k] )!=0 ){
							pos1[k]=0.0f;
						}
					}
					for(uint32_t k=0;k<3;k++){
						pos2[k] = model->vertices[3*(model->triangles[i].vindices[2])+k];
						if ( isnan( pos2[k] )!=0 ){
							pos2[k]=0.0f;
						}
					}

					for(uint32_t k=0;k<2;k++){
						uv0[k] = model->texcoords[2*(model->triangles[i].tindices[0])+k];
						if ( isnan( uv0[k] )!=0 ){
							uv0[k]=0.0f;
						}
					}
					for(uint32_t k=0;k<2;k++){
						uv1[k] = model->texcoords[2*(model->triangles[i].tindices[1])+k];
						if ( isnan( uv1[k] )!=0 ){
							uv1[k]=0.0f;
						}
					}
					for(uint32_t k=0;k<2;k++){
						uv2[k] = model->texcoords[2*(model->triangles[i].tindices[2])+k];
						if ( isnan( uv2[k] )!=0 ){
							uv2[k]=0.0f;
						}
					}
					/*
					float ab[3], bc[3];
					ab[0] = pos1[0] - pos0[0];
					ab[1] = pos1[1] - pos0[1];
					ab[2] = pos1[2] - pos0[2];
					bc[0] = pos2[0] - pos1[0];
					bc[1] = pos2[1] - pos1[1];
					bc[2] = pos2[2] - pos1[2];
					Vec3Cross(normal, ab, bc);
					*/
					normal[0]=model->normals[3*(model->triangles[i].nindices[0])+0];
					normal[1]=model->normals[3*(model->triangles[i].nindices[0])+1];
					normal[2]=model->normals[3*(model->triangles[i].nindices[0])+2];
					if(NORMALIZE_NORMAL_ENABLE){
						Vec3Normalize(normal);
					}
					Vec3Normalize(normal);
					calcTangentBinormal( tangent, binormal, pos0, pos1, pos2, uv0, uv1, uv2, normal );
					for(uint32_t j=0;j<3;j++){ // vertex 
						for(uint32_t k=0;k<3;k++){ //< nx ny nz
							if(isnan(tangent[k])==0){
								fwrite(&tangent[k],sizeof(float),1,fp);
							}else{
								fwrite(&zeroData,sizeof(float),1,fp);
							}
						}
					}
				}
			}
			if( exportBinormalEnable){
				for(uint32_t i=0;i<model->numtriangles;i++){ // triangle
					float pos0[3];
					float pos1[3];
					float pos2[3];
					float uv0[2];
					float uv1[2];
					float uv2[2];
					for(uint32_t k=0;k<3;k++){
						pos0[k] = model->vertices[3*(model->triangles[i].vindices[0])+k];
						if ( isnan( pos0[k] )!=0 ){
							pos0[k]=0.0f;
						}
					}
					for(uint32_t k=0;k<3;k++){
						pos1[k] = model->vertices[3*(model->triangles[i].vindices[1])+k];
						if ( isnan( pos1[k] )!=0 ){
							pos1[k]=0.0f;
						}
					}
					for(uint32_t k=0;k<3;k++){
						pos2[k] = model->vertices[3*(model->triangles[i].vindices[2])+k];
						if ( isnan( pos2[k] )!=0 ){
							pos2[k]=0.0f;
						}
					}

					for(uint32_t k=0;k<2;k++){
						uv0[k] = model->texcoords[2*(model->triangles[i].tindices[0])+k];
						if ( isnan( uv0[k] )!=0 ){
							uv0[k]=0.0f;
						}
					}
					for(uint32_t k=0;k<2;k++){
						uv1[k] = model->texcoords[2*(model->triangles[i].tindices[1])+k];
						if ( isnan( uv1[k] )!=0 ){
							uv1[k]=0.0f;
						}
					}
					for(uint32_t k=0;k<2;k++){
						uv2[k] = model->texcoords[2*(model->triangles[i].tindices[2])+k];
						if ( isnan( uv2[k] )!=0 ){
							uv2[k]=0.0f;
						}
					}
					normal[0]=model->normals[3*(model->triangles[i].nindices[0])+0];
					normal[1]=model->normals[3*(model->triangles[i].nindices[0])+1];
					normal[2]=model->normals[3*(model->triangles[i].nindices[0])+2];
					if(NORMALIZE_NORMAL_ENABLE){
						Vec3Normalize(normal);
					}
					Vec3Normalize(normal);
					calcTangentBinormal( tangent, binormal, pos0, pos1, pos2, uv0, uv1, uv2, normal );

					for(uint32_t j=0;j<3;j++){ // vertex 
						for(uint32_t k=0;k<3;k++){ // nx ny nz
							if(isnan(binormal[k])==0){
								fwrite(&binormal[k],sizeof(float),1,fp);
							}else{
								fwrite(&zeroData,sizeof(float),1,fp);
							}
						}
					}
				}
			}
		}
		//�e�N�X�`�����W�o��
		if((int32_t)model->numtexcoords){
			for(uint32_t i=0;i<model->numtriangles;i++){ // triangles
				printf("\t");
				for(uint32_t j=0;j<3;j++){ // vertex
					for(uint32_t k=0;k<2;k++){ // u v
						float loadedf=model->texcoords[2*(model->triangles[i].tindices[j])+k];
						if(isnan(loadedf)==0){
							fwrite(&loadedf,sizeof(float),1,fp);
						}else{
							fwrite(&zeroData,sizeof(float),1,fp);
						}
					}
				}
			}
		}
		if(0){//	face �̏��B
			if((int32_t)model->numfacetnorms){
				printf("float face[]={\n\t");
				for(uint32_t i=0;i<model->numtriangles;i++){
					printf("%ff,",*(model->facetnorms+(int32_t)(*(&model->triangles->findex-1+i*elemnum))-1));
					oflPrintAndAdjustNewLine(i);
				}
				printf("}; \n");
			}
		}
		fclose(fp);	// �t�@�C���̃N���[�Y
	}
	//----------------------------------------------------------------
	//	�}�e���A�����(������)
	//----------------------------------------------------------------
	//	�V�F�[�_��e�N�X�`���ȂǂŊǗ�����B
	if(output_material_control==OUTPUT_MATERIAL_DIRECT){
		printf("// OUTPUT_MATERIAL_DIRECT \n");
		printf("int nummaterials=%d; \n",(int32_t)model->nummaterials);
		printf("char materials_name[][256]={ \n");
		for(uint32_t i=0;i<model->nummaterials;i++){
			printf("\t\"%s\",\n",model->materials[i].name);
		}
		printf("};\n");
		printf("float materials_diffuse[]={ \n");
		for(uint32_t i=0;i<model->nummaterials;i++){
			for(uint32_t j=0;j<3;j++){
				printf( "\t%ff,",*(model->materials[i].diffuse+j) );
			}
			printf("\n");
		}
		printf("};\n");
		printf("float materials_ambient[]={ \n");
		for(uint32_t i=0;i<model->nummaterials;i++){
			for(uint32_t j=0;j<3;j++){
				printf( "\t%ff,",*(model->materials[i].ambient+j) );
			}
			printf("\n");
		}
		printf("};\n");
		printf("float materials_specular[]={ \n");
		for(uint32_t i=0;i<model->nummaterials;i++){
			for(uint32_t j=0;j<3;j++){
				printf( "\t%ff,",*(model->materials[i].specular+j) );
			}
			printf("\n");
		}
		printf("};\n");
		printf("float materials_emmissive[]={ \n");
		for(uint32_t i=0;i<model->nummaterials;i++){
			for(uint32_t j=0;j<3;j++){
				printf( "\t%ff,",*(model->materials[i].emmissive+j) );
			}
			printf("\n");
		}
		printf("};\n");
		printf("float materials_shininess[]={ \n");
		for(uint32_t i=0;i<model->nummaterials;i++){
			printf( "\t%ff,",model->materials[i].shininess );
			printf("\n");
		}
		printf("};\n");
	}

	//----------------------------------------------------------------
	//	�O���[�v
	//----------------------------------------------------------------
	{
		FILE *fpGb;
		FILE *fpGroupText;
		oflGroup_t *group;
		//float loadedf;

		if(output_group_control==OUTPUT_GROUP_DIRECT){
			uint32_t maxTriangleNum=0;
			uint32_t groupIndex=0;
			printf("int numgroups=%d;\n",(int32_t)model->numgroups);
			group=model->groups;
			printf("char group_name[%d][256]={\n",model->numgroups);

			//�O���[�v���t�@�C������
			char outGbTextFileName[256];
			char outGbTextFileNameBuf[256];
			uint32_t outGbTextFileLength;
			outGbTextFileLength=strlen(fn);
			strncpy(outGbTextFileNameBuf,fn,outGbTextFileLength-3);
			outGbTextFileNameBuf[outGbTextFileLength-3]='\0';
			sprintf(outGbTextFileName,"%s-groupname.txt",outGbTextFileNameBuf);
			if((fpGroupText = fopen(outGbTextFileName, "w")) == NULL) {
				printf("file open error!!\n");
				exit(EXIT_FAILURE);
			}
			while(group){
				printf("%d %s \n",groupIndex,group->name);
				fprintf(fpGroupText,"%d %s \n",groupIndex,group->name);
				group=group->next;
				groupIndex++;
			}
			fclose(fpGroupText);	// �t�@�C���̃N���[�Y


			printf("};\n");
			group=model->groups;
			groupIndex=0;
			//printf("int group_numtriangles[%d]={\n",(int32_t)model->numgroups);
			while(group){
				char outGbFileNameBuf[256];
				char outGbFileName[256];
				uint32_t outFileLength;
				outFileLength=strlen(fn);
				if(exportTangentEnable==1 || exportBinormalEnable==1 ){
					strncpy(outGbFileNameBuf,fn,outFileLength-4);
					outGbFileNameBuf[outFileLength-4]='\0';
					sprintf(outGbFileName,"%s-g%04d.gb3",outGbFileNameBuf,groupIndex);
				}else{
					strncpy(outGbFileNameBuf,fn,outFileLength-3);
					outGbFileNameBuf[outFileLength-3]='\0';
					sprintf(outGbFileName,"%s-g%04d.gb",outGbFileNameBuf,groupIndex);
					//sprintf(outGbFileName,"%s-g-%s.gb",outGbFileNameBuf,group->name);
				}
				//printf("outGbFileName=%s \n",outGbFileName);
				//----------------------------------------------------------------
				//�w�b�_���o��
				//�A�X�L�[
				//----------------------------------------------------------------
				// �����ŁA�t�@�C���|�C���^���擾����
				if ((fpGb = fopen(outGbFileName, "wb")) == NULL) {
					printf("file open error!!\n");
					exit(EXIT_FAILURE);
				}
				oflBinHeader_t binHeader;
				sprintf(binHeader.signature,"ofl bin export");
				//sprintf(binHeader.pathname,"%s",model->pathname);
				binHeader.version=1;
				binHeader.numTriangles=group->numtriangles;
				binHeader.numVerticies=group->numtriangles*3;
				binHeader.elemNumPos	=3;
				if((int32_t)model->numnormals!=0){
					binHeader.elemNumNorm	=3;
					if(exportTangentEnable){
						binHeader.elemNumTangent=3;
					}else{
						binHeader.elemNumTangent=0;
					}
					if(exportBinormalEnable){
						binHeader.elemNumBinormal=3;
					}else{
						binHeader.elemNumBinormal=0;
					}
				}else{
					binHeader.elemNumNorm		=0;
					binHeader.elemNumTangent	=0;
					binHeader.elemNumBinormal	=0;
				}
				if((int32_t)model->numtexcoords){
					binHeader.elemNumTex	=2;
				}else{
					binHeader.elemNumTex	=0;
				}
				binHeader.sizePos		=binHeader.elemNumPos *binHeader.numVerticies*sizeof(float);
				binHeader.sizeNorm		=binHeader.elemNumNorm*binHeader.numVerticies*sizeof(float);
				binHeader.sizeTangent	=binHeader.elemNumTangent*binHeader.numVerticies*sizeof(float);
				binHeader.sizeBinormal	=binHeader.elemNumBinormal*binHeader.numVerticies*sizeof(float);
				binHeader.sizeTex		=binHeader.elemNumTex *binHeader.numVerticies*sizeof(float);
				//binHeader.offsetToHeader=0;
				binHeader.offsetToData		=sizeof(oflBinHeader_t); // header byte
				binHeader.offsetToPos		=binHeader.offsetToData;
				binHeader.offsetToNorm		=binHeader.offsetToPos		+ binHeader.sizePos;
				binHeader.offsetToTangent	=binHeader.offsetToNorm		+ binHeader.sizeNorm;
				binHeader.offsetToBinormal	=binHeader.offsetToTangent	+ binHeader.sizeTangent;
				binHeader.offsetToTex		=binHeader.offsetToBinormal + binHeader.sizeBinormal;
				//binHeader.offsetToTex	=binHeader.offsetToNorm + binHeader.sizeNorm;
				//fprintf(fp,"char pathname[]=\"%s\";\n",model->pathname);
				//fprintf("char mtlname[]=\"%s\"; \n",model->mtllibname);
				//fprintf(fp,"int numTriangles=%d;\n",(int)(model->numtriangles));
				//fprintf(fp,"int numVerticies=%d;\n",(int)(model->numtriangles*3));
				//printf("int triangle_size=%d; \n", (uint32_t)sizeof(oflTriangle_t));
				//printf("int numvertices=%d;\n",(int32_t)model->numvertices );
				//fclose(fp);
				fwrite(&binHeader,sizeof(oflBinHeader_t),1,fpGb);

				//----------------------------------------------------------------
				//�{�f�B�i�f�[�^�j���o��
				//�o�C�i��
				//----------------------------------------------------------------

				//	obj �t�@�C���̌X�̃A�g���r���[�g�̃C���f�b�N�X�����g�킸�A
				//	���ׂĕR�����ďo�́B���GPU�ł͕����C���f�b�N�X�͈����Ȃ��B
				//position�o��
				float loadedf;
				const float zeroData=0.0f;
				static oflTriangle_t *triangle;

				for(uint32_t i=0;i<group->numtriangles;i++){//triangle index 
					triangle=&model->triangles[(group->triangles[i])];
					for(uint32_t j=0;j<3;j++){//vertex index
						for(uint32_t k=0;k<3;k++){// x y z
							loadedf=model->vertices[3*(triangle->vindices[j])+k];
							if(isnan(loadedf)==0){
								fwrite(&loadedf,sizeof(float),1,fpGb);
							}else{
								fwrite(&zeroData,sizeof(float),1,fpGb);
							}
						}
					}
				}
				//�@���o��
				float normal[3];
				float tangent[3];
				float binormal[3];
				if((int32_t)model->numnormals!=0){
					if(NORMALIZE_NORMAL_ENABLE){
						//printf("//NORMALIZE_NORMAL_ENABLE\n");
					}else{
						//printf("//NORMALIZE_NORMAL_DISABLE\n");
					}
					if(output_normal_generation_enable){
						oflVertexNormals(model,0.0f);
					}
					for(uint32_t i=0;i<group->numtriangles;i++){ // triangle
						triangle=&model->triangles[(group->triangles[i])];
						for(uint32_t j=0;j<3;j++){ // vertex 
							//normalize normal
							normal[0]=model->normals[3*(triangle->nindices[j])+0];
							normal[1]=model->normals[3*(triangle->nindices[j])+1];
							normal[2]=model->normals[3*(triangle->nindices[j])+2];
							if(NORMALIZE_NORMAL_ENABLE){
								Vec3Normalize(normal);
							}
							for(uint32_t k=0;k<3;k++){ //< nx ny nz
								if(isnan(normal[k])==0){
									fwrite(&normal[k],sizeof(float),1,fpGb);
								}else{
									fwrite(&zeroData,sizeof(float),1,fpGb);
								}
							}
						}
					}
					
					if(exportTangentEnable){
						for(uint32_t i=0;i<group->numtriangles;i++){ // triangle
							triangle=&model->triangles[(group->triangles[i])];

							float pos0[3];
							float pos1[3];
							float pos2[3];
							float uv0[2];
							float uv1[2];
							float uv2[2];
							for(uint32_t k=0;k<3;k++){
								pos0[k] = model->vertices[3*(triangle->vindices[0])+k];
								if ( isnan( pos0[k] )!=0 ){
									pos0[k]=0.0f;
								}
							}
							for(uint32_t k=0;k<3;k++){
								pos1[k] = model->vertices[3*(triangle->vindices[1])+k];
								if ( isnan( pos1[k] )!=0 ){
									pos1[k]=0.0f;
								}
							}
							for(uint32_t k=0;k<3;k++){
								pos2[k] = model->vertices[3*(triangle->vindices[2])+k];
								if ( isnan( pos2[k] )!=0 ){
									pos2[k]=0.0f;
								}
							}

							for(uint32_t k=0;k<2;k++){
								uv0[k] = model->texcoords[2*(triangle->tindices[0])+k];
								if ( isnan( uv0[k] )!=0 ){
									uv0[k]=0.0f;
								}
							}
							for(uint32_t k=0;k<2;k++){
								uv1[k] = model->texcoords[2*(triangle->tindices[1])+k];
								if ( isnan( uv1[k] )!=0 ){
									uv1[k]=0.0f;
								}
							}
							for(uint32_t k=0;k<2;k++){
								uv2[k] = model->texcoords[2*(triangle->tindices[2])+k];
								if ( isnan( uv2[k] )!=0 ){
									uv2[k]=0.0f;
								}
							}
							/*
							float ab[3], bc[3];
							ab[0] = pos1[0] - pos0[0];
							ab[1] = pos1[1] - pos0[1];
							ab[2] = pos1[2] - pos0[2];
							bc[0] = pos2[0] - pos1[0];
							bc[1] = pos2[1] - pos1[1];
							bc[2] = pos2[2] - pos1[2];
							Vec3Cross(normal, ab, bc);
							*/
							normal[0]=model->normals[3*(triangle->nindices[0])+0];
							normal[1]=model->normals[3*(triangle->nindices[0])+1];
							normal[2]=model->normals[3*(triangle->nindices[0])+2];
							if(NORMALIZE_NORMAL_ENABLE){
								Vec3Normalize(normal);
							}
							Vec3Normalize(normal);
							calcTangentBinormal( tangent, binormal, pos0, pos1, pos2, uv0, uv1, uv2, normal );
							/*
							for(uint32_t j=0;j<3;j++){ // vertex 
								calcTangentByNormal(tangent,(const float *)normal);
								for(uint32_t k=0;k<3;k++){ //< nx ny nz
									if(isnan(tangent[k])==0){
										fwrite(&tangent[k],sizeof(float),1,fpGb);
									}else{
										fwrite(&zeroData,sizeof(float),1,fpGb);
									}
								}
							}
							*/

							for(uint32_t j=0;j<3;j++){ // vertex 
								for(uint32_t k=0;k<3;k++){ //< nx ny nz
									if(isnan(tangent[k])==0){
										fwrite(&tangent[k],sizeof(float),1,fpGb);
									}else{
										fwrite(&zeroData,sizeof(float),1,fpGb);
									}
								}
							}
						}
					}
					if( exportBinormalEnable){
						for(uint32_t i=0;i<group->numtriangles;i++){ // triangle
							triangle=&model->triangles[(group->triangles[i])];
							//for(uint32_t j=0;j<3;j++){ // vertex 
						//for(uint32_t i=0;i<model->numtriangles;i++){ // triangle
							float pos0[3];
							float pos1[3];
							float pos2[3];
							float uv0[2];
							float uv1[2];
							float uv2[2];
							for(uint32_t k=0;k<3;k++){
								pos0[k] = model->vertices[3*(triangle->vindices[0])+k];
								if ( isnan( pos0[k] )!=0 ){
									pos0[k]=0.0f;
								}
							}
							for(uint32_t k=0;k<3;k++){
								pos1[k] = model->vertices[3*(triangle->vindices[1])+k];
								if ( isnan( pos1[k] )!=0 ){
									pos1[k]=0.0f;
								}
							}
							for(uint32_t k=0;k<3;k++){
								pos2[k] = model->vertices[3*(triangle->vindices[2])+k];
								if ( isnan( pos2[k] )!=0 ){
									pos2[k]=0.0f;
								}
							}

							for(uint32_t k=0;k<2;k++){
								uv0[k] = model->texcoords[2*(triangle->tindices[0])+k];
								if ( isnan( uv0[k] )!=0 ){
									uv0[k]=0.0f;
								}
							}
							for(uint32_t k=0;k<2;k++){
								uv1[k] = model->texcoords[2*(triangle->tindices[1])+k];
								if ( isnan( uv1[k] )!=0 ){
									uv1[k]=0.0f;
								}
							}
							for(uint32_t k=0;k<2;k++){
								uv2[k] = model->texcoords[2*(triangle->tindices[2])+k];
								if ( isnan( uv2[k] )!=0 ){
									uv2[k]=0.0f;
								}
							}
							normal[0]=model->normals[3*(triangle->nindices[0])+0];
							normal[1]=model->normals[3*(triangle->nindices[0])+1];
							normal[2]=model->normals[3*(triangle->nindices[0])+2];
							if(NORMALIZE_NORMAL_ENABLE){
								Vec3Normalize(normal);
							}
							Vec3Normalize(normal);
							calcTangentBinormal( tangent, binormal, pos0, pos1, pos2, uv0, uv1, uv2, normal );

							for(uint32_t j=0;j<3;j++){ // vertex 
								for(uint32_t k=0;k<3;k++){ // nx ny nz
									if(isnan(binormal[k])==0){
										fwrite(&binormal[k],sizeof(float),1,fpGb);
									}else{
										fwrite(&zeroData,sizeof(float),1,fpGb);
									}
								}
							}
						}
					}

					/*
					if(exportTangentEnable){
						for(uint32_t i=0;i<group->numtriangles;i++){ // triangle
							triangle=&model->triangles[(group->triangles[i])];
							for(uint32_t j=0;j<3;j++){ // vertex 
								//normalize normal
								normal[0]=model->normals[3*(model->triangles[i].nindices[j])+0];
								normal[1]=model->normals[3*(model->triangles[i].nindices[j])+1];
								normal[2]=model->normals[3*(model->triangles[i].nindices[j])+2];
								if(NORMALIZE_NORMAL_ENABLE){
									Vec3Normalize(normal);
								}
								calcTangentByNormal(tangent,(const float *)normal);
								for(uint32_t k=0;k<3;k++){ //< nx ny nz
									if(isnan(tangent[k])==0){
										fwrite(&tangent[k],sizeof(float),1,fpGb);
									}else{
										fwrite(&zeroData,sizeof(float),1,fpGb);
									}
								}
							}
						}
					}
					if(exportBinormalEnable){
						for(uint32_t i=0;i<group->numtriangles;i++){ // triangle
							triangle=&model->triangles[(group->triangles[i])];
							for(uint32_t j=0;j<3;j++){ // vertex 
								//normalize normal
								normal[0]=model->normals[3*(model->triangles[i].nindices[j])+0];
								normal[1]=model->normals[3*(model->triangles[i].nindices[j])+1];
								normal[2]=model->normals[3*(model->triangles[i].nindices[j])+2];
								if(NORMALIZE_NORMAL_ENABLE){
									Vec3Normalize(normal);
								}
								calcTangentByNormal(tangent,(const float *)normal);
								Vector3Cross(binormal,normal,tangent);

								for(uint32_t k=0;k<3;k++){ //< nx ny nz
									if(isnan(binormal[k])==0){
										fwrite(&binormal[k],sizeof(float),1,fpGb);
									}else{
										fwrite(&zeroData,sizeof(float),1,fpGb);
									}
								}
							}
						}
					}
					*/

				}
				//�e�N�X�`�����W�o��
				if((int32_t)model->numtexcoords){
					for(uint32_t i=0;i<group->numtriangles;i++){ // triangles
						triangle=&model->triangles[(group->triangles[i])];
						printf("\t");
						for(uint32_t j=0;j<3;j++){ // vertex
							for(uint32_t k=0;k<2;k++){ // u v
								loadedf=model->texcoords[2*(triangle->tindices[j])+k];
								if(isnan(loadedf)==0){
									fwrite(&loadedf,sizeof(float),1,fpGb);
								}else{
									fwrite(&zeroData,sizeof(float),1,fpGb);
								}
							}
						}
					}
				}
				if(0){// face �̏��B
					if((int32_t)model->numfacetnorms){
						printf("float face[]={\n\t");
						for(uint32_t i=0;i<group->numtriangles;i++){
							printf("%ff,",*(model->facetnorms+(int32_t)(*(&model->triangles->findex-1+i*elemnum))-1));
							oflPrintAndAdjustNewLine(i);
						}
						printf("}; \n");
					}
				}
				fclose(fpGb); // �t�@�C���̃N���[�Y
				printf("\t%d,//%s\n",(int32_t)(group->numtriangles),group->name);
				if(maxTriangleNum < group->numtriangles){
					maxTriangleNum=group->numtriangles;
				}
				group=group->next;
				groupIndex++;
			}
		}
	}
}

//obj�����w�b�_�Ƃ��ďo�͂���Bprintf�ŕW���o�͂ɏo�Ă��邱�Ƃɒ��ӁB
void oflExportCHeader(oflModel_t *model, bool exportTangentEnable, bool exportBinormalEnable)
{
	enum{
		OUTPUT_INDEX_DIRECT=0,	//wavefront obj�̌��̌`��
		//OUTPUT_INDEX_SUMMARIZED,//�C���f�b�N�X��g�ݑւ�������
		OUTPUT_INDEX_SUM_TANGENT_BINORMAL,
		OUTPUT_INDEX_END,
	};
	enum{
		OUTPUT_MATERIAL_NONE=0,
		OUTPUT_MATERIAL_DIRECT,
		OUTPUT_MATERIAL_END,
	};
	enum{
		OUTPUT_GROUP_NONE=0,
		OUTPUT_GROUP_DIRECT,
		OUTPUT_GROUP_END,
	};
	
	//const uint32_t output_index_control=OUTPUT_INDEX_SUMMARIZED;
	const uint32_t output_index_control=OUTPUT_INDEX_SUM_TANGENT_BINORMAL;
	const uint32_t output_material_control=OUTPUT_MATERIAL_DIRECT;
	const uint32_t output_group_control=OUTPUT_GROUP_DIRECT;
	const bool output_normal_generation_enable=false;

	const bool NORMALIZE_NORMAL_ENABLE=false;
	const uint32_t elemnum=10; ///<triangle elem �̗v�f�� ���ʎg��Ȃ��悤�ɂ����B
	const uint32_t adjustmentDirect=0;

	float loadedf;		//�e���|�����̒l�B�ǂݍ��񂾃t���[�g�l�B��肪�Ȃ���΂����Ɏg���B
	float normal[3];	//�e���|�����̒l�B�ǂݍ��񂾃t���[�g�l�B��肪�Ȃ���΂����Ɏg���B
	float tangent[3];	//�e���|�����̒l�B�ǂݍ��񂾃t���[�g�l�B��肪�Ȃ���΂����Ɏg���B
	float binormal[3];	//�e���|�����̒l�B�ǂݍ��񂾃t���[�g�l�B��肪�Ȃ���΂����Ɏg���B

	switch(output_index_control){
		case OUTPUT_INDEX_SUM_TANGENT_BINORMAL:
		{
			printf("char pathname[]=\"%s\";\n",model->pathname);
			//printf("char mtlname[]=\"%s\"; \n",model->mtllibname);
			printf("int numtriangles=%d;\n",(int)(model->numtriangles));
			printf("int numverticies=%d;\n",(int)(model->numtriangles*3));
			//printf("int triangle_size=%d; \n", (uint32_t)sizeof(oflTriangle_t));
			//printf("int numvertices=%d;\n",(int32_t)model->numvertices );
			//for(uint32_t i=0;i<strlen(model->pathname)-4;i++){
			//	printf("%c",model->pathname[i]);
			//}
			//printf("\n");
			//position�͕K���o�͂���B
			printf("float position[]={\n \t");
			for(uint32_t i=0;i<model->numtriangles;i++){///<triangle index 
				for(uint32_t j=0;j<3;j++){///<vertex index
					for(uint32_t k=0;k<3;k++){///< x y z
						loadedf=model->vertices[3*(model->triangles[i].vindices[j])+k];
						if(isnan(loadedf)==0){
							printf("%ff,",loadedf);
							//printf("\n debug:index=%d,index~%d i=%d, j=%d\n",
							//		3*model->triangles[i].vindices[j]-1,
							//		(uint32_t)((int32_t)(3*model->triangles[i].vindices[j])+0),
							//		i,j);
						}else{
							printf("0.0f,");
						}
					}
					oflPrintAndAdjustNewLine(i*3+j);
				}
			}
			printf("};\n");

			if((int32_t)model->numnormals!=0){

				if(NORMALIZE_NORMAL_ENABLE){
					printf("//NORMALIZE_NORMAL_ENABLE\n");
				}else{
					printf("//NORMALIZE_NORMAL_DISABLE\n");
				}
				if(output_normal_generation_enable){
					oflVertexNormals(model,0.0f);
				}
				printf("float normal[]={\n");
				for(uint32_t i=0;i<model->numtriangles;i++){ //< triangle
					printf("\t");
					for(uint32_t j=0;j<3;j++){ //< vertex 
						normal[0]=model->normals[3*(model->triangles[i].nindices[j])+0];
						normal[1]=model->normals[3*(model->triangles[i].nindices[j])+1];
						normal[2]=model->normals[3*(model->triangles[i].nindices[j])+2];
						if(NORMALIZE_NORMAL_ENABLE){
							Vec3Normalize(normal);
						}
						for(uint32_t k=0;k<3;k++){ //< nx ny nz
							if(isnan(normal[k])==0){
								printf("%ff,",normal[k]);
							}else{
								printf("0.0f,");
							}
						}
						oflPrintAndAdjustNewLine(i*3+j);
					}
				}
				printf("};\n");
				if(exportTangentEnable){
					printf("float tangent[]={\n");
					for(uint32_t i=0;i<model->numtriangles;i++){ //< triangle
						printf("\t");
						for(uint32_t j=0;j<3;j++){ //< vertex 
							normal[0]=model->normals[3*(model->triangles[i].nindices[j])+0];
							normal[1]=model->normals[3*(model->triangles[i].nindices[j])+1];
							normal[2]=model->normals[3*(model->triangles[i].nindices[j])+2];
							if(NORMALIZE_NORMAL_ENABLE){
								Vec3Normalize(normal);
							}
							calcTangentByNormal(tangent,(const float *)normal);
							for(uint32_t k=0;k<3;k++){ //< nx ny nz
								if(isnan(tangent[k])==0){
									printf("%ff,",tangent[k]);
								}else{
									printf("0.0f,");
								}
							}
							oflPrintAndAdjustNewLine(i*3+j);
						}
					}
					printf("};\n");
				}
				if(exportBinormalEnable){
					printf("float binormal[]={\n");
					for(uint32_t i=0;i<model->numtriangles;i++){ //< triangle
						printf("\t");
						for(uint32_t j=0;j<3;j++){ //< vertex 
							normal[0]=model->normals[3*(model->triangles[i].nindices[j])+0];
							normal[1]=model->normals[3*(model->triangles[i].nindices[j])+1];
							normal[2]=model->normals[3*(model->triangles[i].nindices[j])+2];
							if(NORMALIZE_NORMAL_ENABLE){
								Vec3Normalize(normal);
							}
							calcTangentByNormal(tangent,(const float *)normal);
							Vector3Cross(binormal,normal,tangent);
							for(uint32_t k=0;k<3;k++){ //< nx ny nz
								if(isnan(tangent[k])==0){
									printf("%ff,",binormal[k]);
								}else{
									printf("0.0f,");
								}
							}
							oflPrintAndAdjustNewLine(i*3+j);
						}
					}
					printf("};\n");
				}
			}

			if((int32_t)model->numtexcoords){
				printf("float texcoord[]={ \n");
				for(uint32_t i=0;i<model->numtriangles;i++){ ///<triangles
					printf("\t");
					for(uint32_t j=0;j<3;j++){ ///<vertex
						for(uint32_t k=0;k<2;k++){ ///< u v
							loadedf=model->texcoords[2*(model->triangles[i].tindices[j])+k];
							if(isnan(loadedf)==0){
								printf("%ff,",loadedf);
							}else{
								printf("0.0f,");
							}
						}
						oflPrintAndAdjustNewLine(i*2+j,2);
					}
				}
				printf("};\n");
			}
			//	face �̏��B
			if(0){
				if((int32_t)model->numfacetnorms){
					printf("float face[]={\n\t");
					for(uint32_t i=0;i<model->numtriangles;i++){
						printf("%ff,",*(model->facetnorms+(int32_t)(*(&model->triangles->findex-1+i*elemnum))-1));
						oflPrintAndAdjustNewLine(i);
					}
					printf("}; \n");
				}
			}
			
			//	obj �t�@�C�������̌X�̃A�g���r���[�g���Ƃ̃C���f�b�N�X�Ή�
			//	��x�R�����Ă���̂ŏo�͕s�v
			if(0){
				printf("int numvertices=%d;\n",(int32_t)model->numvertices);
				printf("float org_vertices[]={");
				for(uint32_t i=1;i<model->numvertices;i++){
					if(isnan((float)*(model->vertices+i))==0){
						printf("%ff, ",(float)*(model->vertices+i));
					}else{
						printf("0.0f, ");
					}
					oflPrintAndAdjustNewLine(i);
				}
				printf("}; \n");
				printf("int numnormals=%d; \n",(int32_t)model->numnormals);
				printf("float org_normals[]={");
				for(uint32_t i=1;i<model->numnormals;i++){
					printf("%ff, ",*(model->normals+i));
					oflPrintAndAdjustNewLine(i);
				}
				printf("}; \n");
				printf("int numtexcoords=%d; \n",(int32_t)model->numtexcoords);
				printf("float org_texcoords[]={");
				for(uint32_t i=1;i<model->numtexcoords;i++){
					printf("%ff, ",*(model->texcoords+i));
					oflPrintAndAdjustNewLine(i);
				}
				printf("}; \n");
				printf("int numfacetnorms=%d;\n",(int32_t)model->numfacetnorms);
				printf("float org_facetnorms[]={");
				for(uint32_t i=1;i<model->numfacetnorms;i++){
					printf("%ff, ",*(model->facetnorms+i));
					oflPrintAndAdjustNewLine(i);
				}
				printf("};\n");
			}
			//	�C���f�b�N�X�Q�̕\���B
			//	��x�C���f�b�N�X���Ƃ��č�蒼���Ă���̂ŕs�v
			if(0){
				printf("//triangles indecies (x,y,z,nx,ny,nz,u,v,p,face)=\n");
				for(uint32_t i=0;i<model->numtriangles;i++){
					printf("//");
					for(uint32_t j=0;j<3;j++){
						printf("%d,",(int32_t)(*(model->triangles->vindices+j+i*elemnum)));
					}
					for(uint32_t j=0;j<3;j++){
						printf("%d,",(int32_t)*((model->triangles->nindices+j+i*elemnum)));
					}
					for(uint32_t j=0;j<3;j++){
						printf("%d,",(int32_t)(*(model->triangles->tindices+j+i*elemnum)));
					}
					printf("%d,",(int32_t)(*(&model->triangles->findex+i*elemnum)));
					printf("\n");
				}
				printf("\n");
			}

			//	�}�e���A�����
			//	�V�F�[�_��e�N�X�`���ȂǂŊǗ�����B
			if(output_material_control==OUTPUT_MATERIAL_DIRECT){
				printf("// OUTPUT_MATERIAL_DIRECT \n");
				printf("int nummaterials=%d; \n",(int32_t)model->nummaterials);
				printf("char materials_name[][256]={ \n");
				for(uint32_t i=0;i<model->nummaterials;i++){
					printf("\t\"%s\",\n",model->materials[i].name);
				}
				printf("};\n");
				printf("float materials_diffuse[]={ \n");
				for(uint32_t i=0;i<model->nummaterials;i++){
					for(uint32_t j=0;j<3;j++){
						printf( "\t%ff,",*(model->materials[i].diffuse+j) );
					}
					printf("\n");
				}
				printf("};\n");
				printf("float materials_ambient[]={ \n");
				for(uint32_t i=0;i<model->nummaterials;i++){
					for(uint32_t j=0;j<3;j++){
						printf( "\t%ff,",*(model->materials[i].ambient+j) );
					}
					printf("\n");
				}
				printf("};\n");
				printf("float materials_specular[]={ \n");
				for(uint32_t i=0;i<model->nummaterials;i++){
					for(uint32_t j=0;j<3;j++){
						printf( "\t%ff,",*(model->materials[i].specular+j) );
					}
					printf("\n");
				}
				printf("};\n");
				printf("float materials_emmissive[]={ \n");
				for(uint32_t i=0;i<model->nummaterials;i++){
					for(uint32_t j=0;j<3;j++){
						printf( "\t%ff,",*(model->materials[i].emmissive+j) );
					}
					printf("\n");
				}
				printf("};\n");
				printf("float materials_shininess[]={ \n");
				for(uint32_t i=0;i<model->nummaterials;i++){
					printf( "\t%ff,",model->materials[i].shininess );
					printf("\n");
				}
				printf("};\n");
			}

			//�O���[�v���Ƃ̏��
			oflGroup_t *group;
			if(output_group_control==OUTPUT_GROUP_DIRECT){
				uint32_t maxTriangleNum=0;
				uint32_t groupIndex=0;

				printf("int numgroups=%d;\n",(int32_t)model->numgroups);

				group=model->groups;
				printf("char group_name[%d][256]={\n",model->numgroups);
				while(group){
					printf("\t\"%s\",\n",group->name);
					group=group->next;
				}
				printf("};\n");

				group=model->groups;
				printf("int group_numtriangles[%d]={\n",(int32_t)model->numgroups);
				while(group){
					printf("\t%d,//%s\n",(int32_t)(group->numtriangles),group->name);
					if(maxTriangleNum < group->numtriangles){
						maxTriangleNum=group->numtriangles;
					}
					group=group->next;
				}
				printf("};\n");
				
				group=model->groups;
				groupIndex=0;
				while(group){
					printf("float group_v%d[]={\n",groupIndex);
					printf("\t");
					float lf=0.0f;
					for(uint32_t i=0;i<group->numtriangles;i++){
						for(uint32_t j=0;j<3;j++){// vertex index
							for(uint32_t k=0;k<3;k++){ // x y z
								loadedf=model->vertices[ 3*( model->triangles[ group->triangles[i] ].vindices[j] )+k];
								if(isnan(loadedf)==0){
									printf("%ff,",loadedf);
								}else{
									printf("0.0f,");
								}
							}
						}
						printf("\n");
					}
					printf("};//%s,%d\n",group->name,group->numtriangles);

					if((int32_t)model->numnormals!=0){
						if(NORMALIZE_NORMAL_ENABLE){
							printf("//NORMALIZE_NORMAL_ENABLE\n");
						}else{
							printf("//NORMALIZE_NORMAL_DISABLE\n");
						}
						printf("float group_n%d[]={\n",groupIndex);
						printf("\t");
						float lf=0.0f;
						for(uint32_t i=0;i<group->numtriangles;i++){
							for(uint32_t j=0;j<3;j++){// vertex index
								normal[0]=model->normals[ 3*( model->triangles[ group->triangles[i] ].vindices[j] )+0];
								normal[1]=model->normals[ 3*( model->triangles[ group->triangles[i] ].vindices[j] )+1];
								normal[2]=model->normals[ 3*( model->triangles[ group->triangles[i] ].vindices[j] )+2];
								if(NORMALIZE_NORMAL_ENABLE){
									Vec3Normalize(normal);
								}
								for(uint32_t k=0;k<3;k++){ //< nx ny nz
									if(isnan(normal[k])==0){
										printf("%ff,",normal[k]);
									}else{
										printf("0.0f,");
									}
								}
								oflPrintAndAdjustNewLine(i*3+j);
							}
						}
						printf("};//%s,%d\n",group->name,group->numtriangles);

						if(exportTangentEnable){
							printf("float group_tangent%d[]={\n",groupIndex);
							for(uint32_t i=0;i<group->numtriangles;i++){ //< triangle
								printf("\t");
								for(uint32_t j=0;j<3;j++){ //< vertex 
									normal[0]=model->normals[ 3*( model->triangles[ group->triangles[i] ].vindices[j] )+0];
									normal[1]=model->normals[ 3*( model->triangles[ group->triangles[i] ].vindices[j] )+1];
									normal[2]=model->normals[ 3*( model->triangles[ group->triangles[i] ].vindices[j] )+2];
									if(NORMALIZE_NORMAL_ENABLE){
										Vec3Normalize(normal);
									}
									calcTangentByNormal(tangent,(const float *)normal);
									for(uint32_t k=0;k<3;k++){ //< nx ny nz
										if(isnan(tangent[k])==0){
											printf("%ff,",tangent[k]);
										}else{
											printf("0.0f,");
										}
									}
									oflPrintAndAdjustNewLine(i*3+j);
								}
							}
							printf("};\n");
						}
						if(exportBinormalEnable){
							printf("float group_binormal%d[]={\n",groupIndex);
							for(uint32_t i=0;i<group->numtriangles;i++){ //< triangle
								printf("\t");
								for(uint32_t j=0;j<3;j++){ //< vertex 
									normal[0]=model->normals[ 3*( model->triangles[ group->triangles[i] ].vindices[j] )+0];
									normal[1]=model->normals[ 3*( model->triangles[ group->triangles[i] ].vindices[j] )+1];
									normal[2]=model->normals[ 3*( model->triangles[ group->triangles[i] ].vindices[j] )+2];
									if(NORMALIZE_NORMAL_ENABLE){
										Vec3Normalize(normal);
									}
									calcTangentByNormal(tangent,(const float *)normal);

									Vector3Cross(binormal,normal,tangent);

									for(uint32_t k=0;k<3;k++){ //< nx ny nz
										if(isnan(tangent[k])==0){
											printf("%ff,",binormal[k]);
										}else{
											printf("0.0f,");
										}
									}
									oflPrintAndAdjustNewLine(i*3+j);
								}
							}
							printf("};\n");
						}
					}
					if((int32_t)model->numtexcoords!=0){
						printf("float group_t%d[]={\n",groupIndex);
						printf("\t");
						float lf=0.0f;
						for(uint32_t i=0;i<group->numtriangles;i++){
							for(uint32_t j=0;j<3;j++){// vertex index
								for(uint32_t k=0;k<2;k++){ // u v
									loadedf=model->normals[ 2*( model->triangles[ group->triangles[i] ].vindices[j] )+k];
									if(isnan(loadedf)==0){
										printf("%ff,",loadedf);
									}else{
										printf("0.0f,");
									}
								}
							}
							printf("\n");
						}
						printf("};//%s,%d\n",group->name,group->numtriangles);
					}
					group=group->next;
					groupIndex++;
				}
				group=model->groups;
				printf("int group_material[%d]={\n",model->numgroups);
				while(group){
					printf("\t%d,//\"%s\"\n",(int32_t)(group->material),group->name);
					group=group->next;
				}
				printf("};\n");
				printf("\n");
			};
			break;
		}

		//	obj �`���̂܂܂̕����̃C���f�b�N�X�����o��
		//	���ʂ�GPU�ł͎g���Ȃ��̂Ń����e�i���X�����A���u���Ă���B
		//	��L��Case�ƈꏏ�ɂ���K�v����B
		case OUTPUT_INDEX_DIRECT:
			printf("char *pathname[]=\"%s\"; \n",model->pathname);
			printf("char *mtlname[]=\"%s\"; \n",model->mtllibname);
			printf("int nump=%d; \n",(int32_t)model->numvertices);
			printf("float p[]={ ");
			for(uint32_t i=0; i<model->numvertices*3; i++){
				printf("%f,",*(model->vertices+i+adjustmentDirect));
				oflPrintAndAdjustNewLine(i);
			}
			printf("}; \n");
			printf("int numn=%d; \n",(int32_t)model->numnormals);
			printf("float n[]={");
			for(uint32_t i=0;i<model->numnormals*3;i++){
				printf( "%f,", model->normals[i] );
				oflPrintAndAdjustNewLine(i);
			}
			printf("}; \n");
			printf( "int numt=%d; \n",(int32_t)model->numtexcoords );
			printf( "float t[]={" );
			for(uint32_t i=0;i<model->numtexcoords*3;i++){
				printf("%f, ",*( model->texcoords+i));
				oflPrintAndAdjustNewLine(i);
			}
			printf("}; \n");
			printf("int numfacetnorms=%d; \n",(int32_t)model->numfacetnorms );
			printf("float facetnorms[]={");
			for(uint32_t i=0;i<(model->numfacetnorms);i++){
				printf("%f, ",*( model->facetnorms+i));
				oflPrintAndAdjustNewLine(i);
			}
			printf("};\n");
			printf("int numtriangles=%d; \n",(int32_t)model->numtriangles );
			printf("int triangle_size = %d; \n",(uint32_t)sizeof(oflTriangle_t));
			printf("// triangles indecies ( x, y, z, nx, ny, nz, u, v, p, face ) = \n" );
			for(uint32_t i=0;i<model->numtriangles;i++){
				printf("//");
				for(uint32_t j=0;j<3;j++){
					printf("%d, ",(int32_t)(*(model->triangles->vindices+j+i*10)));
				}
				for(uint32_t j=0;j<3;j++){
					printf("%d, ",(int32_t)*((model->triangles->nindices+j+i*10)));
				}
				for(uint32_t j=0;j<3;j++){
					printf("%d, ",(int32_t)(*(model->triangles->tindices+j+i*10)));
				}
				printf("%d, ",(int32_t)(*(&model->triangles->findex+i*10)));
				printf("\n");
			}
			printf("\n");
			/* �����͕s���S�B��L�̂悤�ɂ���K�v����B
			printf("int nummaterials=%d;\n",(int32_t)model->nummaterials);
			printf("float materials[]={\n");
			for(uint32_t i=0;i<model->nummaterials;i++){
				//printf( "%s \n", *( &model->materials->name + i*sizeof( oflMaterial_t ) ) );
				for(uint32_t j=0;j<3;j++){
					printf("\t%f, ",*(model->materials->diffuse+j+i*sizeof(oflMaterial_t)));
				}
				printf("//< diffuse \n");
				for(uint32_t j=0;j<3;j++){
					printf("\t%f, ",*(model->materials->ambient+j+i*sizeof(oflMaterial_t)));
				}
				printf("//< ambient \n");
				for(uint32_t j=0;j<3;j++){
					printf("\t%f, ",*(model->materials->specular+j+i*sizeof(oflMaterial_t)));
				}
				printf("//< specular \n");
				for(uint32_t j=0;j<3;j++){
					printf("\t%f, ",*(model->materials->emmissive+j+i*sizeof(oflMaterial_t)));
				}
				printf("//< emmissive \n");
				printf("\t%f, ",*(&model->materials->shininess+i*sizeof(oflMaterial_t)));
				printf("//< shininess \n");
			}
			printf("};\n");
			printf("int numgroups=%d;\n",(int32_t)model->numgroups);
			printf( "char groups[]={ \n" );
			for(uint32_t i=0;i<model->numgroups;i++){
				//char *name; ///< name of this group
				//uint32_t numtriangles; ///< number of triangles in this group
				//uint32_t *triangles; ///< array of triangle indices
				//uint32_t material; ///< index to material for group
				//struct oflGroup_t *next; ///< pointer to next group in model
				printf("\t\"%s\",",model->groups->name);
				printf("\t//%d,",(int32_t)(*(&model->groups->numtriangles)));
				printf("\t//%d,",(int32_t)(*(model->groups->triangles)));
				printf("\t//%d,",(int32_t)(*(&model->groups->material)));
				//printf("%lf",*(model->groups+i));
				printf("\n");
			}
			printf("};\n");
			*/
			printf("\n");
			break;

		default:
			printf("%s %d error. no such mode \n",__FILE__,__LINE__);
			exit(1);
	}
}

char* oflDirName(char *path)
{
	char *dir;
	char *s;
	dir=strdup(path);
	//�t������̃X���b�V�������̌���
	s=strrchr(dir,'/');
	// NULL �����̒ǉ�
	if(s){
		s[1]='\0';
	}else{
		dir[0]='\0';
	}
	return dir;
}

int32_t oflReadMtl(oflModel_t *model,char *name)
{
	LogTrace("\n");
	int32_t ret=0;
	FILE *file;
	char *dir;
	char *filename;
	char buf[128];
	uint32_t nummaterials,i;
	dir=oflDirName(model->pathname);
#	if APPLY_MEMALIGN
	filename=(char*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(char)*(strlen(dir)+strlen(name)+1));
#	else
	filename=(char*)malloc(sizeof(char)*(strlen(dir)+strlen(name)+1)+SAFETY_PADDING_A_BIT);
#	endif //APPLY_MEMALIGN
	strcpy(filename,dir);
	strcat(filename,name);
	free(dir);
	file=fopen(filename,"r");
	if(!file){
		fprintf(stdout,"oflReadMtl() failed: can't open material file \"%s\".\n",filename);
		//fprintf(stderr,"oflReadMtl() failed: can't open material file \"%s\".\n",filename);
		//exit(1);
		ret=-1;
		return ret;
	}
	free(filename);
	/// count the number of materials in the file 
	nummaterials = 1;
	while(fscanf(file,"%s",buf) != EOF){
		switch(buf[0]){
			case '#'://comment
				// eat up rest of line
				fgets(buf, sizeof(buf),file);
				break;
			case 'n'://newmtl
				fgets(buf, sizeof(buf),file);
				nummaterials++;
				sscanf(buf,"%s %s",buf,buf);
				break;
			default:
				// eat up rest of line 
				fgets(buf,sizeof(buf),file);
				break;
		}
	}
	rewind(file);
#	if APPLY_MEMALIGN
	model->materials=(oflMaterial_t*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(oflMaterial_t)*nummaterials);
#	else
	model->materials=(oflMaterial_t*)malloc(sizeof(oflMaterial_t)*nummaterials);
#	endif //APPLY_MEMALIGN
	model->nummaterials=nummaterials;
	/// set the default material
	for(i=0;i<nummaterials;i++){
		model->materials[i].name=NULL;
		model->materials[i].shininess=65.0f;
		model->materials[i].diffuse[0]=0.8f;
		model->materials[i].diffuse[1]=0.8f;
		model->materials[i].diffuse[2]=0.8f;
		model->materials[i].diffuse[3]=1.0f;
		model->materials[i].ambient[0]=0.2f;
		model->materials[i].ambient[1]=0.2f;
		model->materials[i].ambient[2]=0.2f;
		model->materials[i].ambient[3]=1.0f;
		model->materials[i].specular[0]=0.0f;
		model->materials[i].specular[1]=0.0f;
		model->materials[i].specular[2]=0.0f;
		model->materials[i].specular[3]=1.0f;
	}
	model->materials[0].name=strdup("default");
	//now, read in the data 
	nummaterials=0;
	while(fscanf(file,"%s",buf)!=EOF){
		switch(buf[0]){
			case '#'://comment
				// eat up rest of line
				fgets(buf,sizeof(buf),file);
				break;
			case 'n'://newmtl
				fgets(buf,sizeof(buf), file);
				sscanf(buf,"%s %s",buf,buf);
				nummaterials++;
				model->materials[nummaterials].name=strdup(buf);
				break;
			case 'N':
				fscanf(file,"%f",&model->materials[nummaterials].shininess);
				///wavefront shininess is from [0, 1000], so scale for OpenGL
				model->materials[nummaterials].shininess/=1000.0;
				model->materials[nummaterials].shininess*=128.0;
				break;
			case 'K':
				switch(buf[1]){
					case 'd':
						fscanf(file,"%f %f %f",
							&model->materials[nummaterials].diffuse[0],
							&model->materials[nummaterials].diffuse[1],
							&model->materials[nummaterials].diffuse[2]);
						break;
					case 's':
						fscanf(file,"%f %f %f",
							&model->materials[nummaterials].specular[0],
							&model->materials[nummaterials].specular[1],
							&model->materials[nummaterials].specular[2]);
						break;
					case 'a':
						fscanf(file,"%f %f %f",
							&model->materials[nummaterials].ambient[0],
							&model->materials[nummaterials].ambient[1],
							&model->materials[nummaterials].ambient[2]);
						break;
					default:
						// eat up rest of line 
						fgets(buf, sizeof(buf),file);
						break;
				}
				break;
			default:
				// eat up rest of line
				fgets(buf, sizeof(buf),file);
				break;
		}
	}
	fclose(file);
	return ret;
}

int32_t oflWriteMtl(oflModel_t *model,char *modelpath,char *mtllibname)
{
	int32_t ret=0;
	FILE* file;
	char* dir;
	char* filename;
	oflMaterial_t* material;
	uint32_t i;
	dir=oflDirName(modelpath);
#	if APPLY_MEMALIGN
	filename=(char*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(char)*(strlen(dir)+strlen(mtllibname)));
#	else
	filename=(char*)malloc(sizeof(char)*(strlen(dir)+strlen(mtllibname))+SAFETY_PADDING_A_BIT);
#	endif //APPLY_MEMALIGN
	strcpy(filename,dir);
	strcat(filename,mtllibname);
	free(dir);
	//open the file
	file=fopen(filename,"w");
	if(!file){
		fprintf(stderr,"oflWriteMtl() failed: can't open file \"%s\".\n",filename);
		//exit(1);
		ret=-1;
		return ret;
	}
	free(filename);
	// spit out a header
	fprintf(file,"# \n");
	fprintf(file,"# Wavefront MTL generated by OFL library. \n");
	fprintf(file,"# \n\n");
	for(i=0;i<model->nummaterials;i++){
		material=&model->materials[i];
		fprintf(file,"newmtl %s\n",material->name);
		fprintf(file,"Ka %f %f %f\n",material->ambient[0],material->ambient[1],material->ambient[2]);
		fprintf(file,"Kd %f %f %f\n",material->diffuse[0],material->diffuse[1],material->diffuse[2]);
		fprintf(file,"Ks %f %f %f\n",material->specular[0],material->specular[1],material->specular[2]);
		fprintf(file,"Ns %f\n",material->shininess/128.0f*1000.0f);
		fprintf(file,"\n");
	}
	return ret;
}

oflGroup_t* oflFindGroup(oflModel_t *model,char *name)
{
	oflGroup_t *group;
	assert(model);
	// �O���[�v���擾
	group=model->groups;
	// �T��
	while(group){
		if(!strcmp(name,group->name)){
			break;
		}
		group=group->next;
	}
	return group;
}

oflGroup_t* oflAddGroup(oflModel_t *model,char *name)
{
	oflGroup_t *group;
	//�O���[�v����
	group=oflFindGroup(model,name);
	//�����O���[�v���Ȃ��ꍇ
	if(!group){
#		if(APPLY_MEMALIGN)
		group=(oflGroup_t*)memalign(SAFETY_ALIGNMENT_SIZE,sizeof(oflGroup_t));
#		else // X86
		group=(oflGroup_t*)malloc(sizeof(oflGroup_t)+SAFETY_PADDING_A_BIT);
#		endif // APPLY_MEMALIGN
		group->name=strdup(name); //���O�̃������擾
		//sprintf(group->name,"default name when this initialized.");
		LogDebug("group->name=%s \n",group->name);
		group->material=0;
		group->numtriangles=0;
		group->triangles=NULL;
		group->next=model->groups;
		model->groups=group;
		model->numgroups++;
	}
	return group;
}

//}//�����܂Ńp�u���b�N�֐��Q
}//namespace nsOfl

