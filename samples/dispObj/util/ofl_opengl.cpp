#include "ofl_opengl.h"

namespace nsOfl{

void oflDraw(oflModel_t *model,uint32_t mode)
{
	tprintf(("begin\n"));
	static uint32_t i;
	static oflGroup_t *group;
	static oflTriangle_t *triangle;
	static oflMaterial_t *material;
	assert(model);
	assert(model->vertices);
	if(mode & OFL_FLAT && !model->facetnorms){
		printf("oflDraw() warning: flat render mode requested "
			"with no facet normals defined.\n");
		mode &= ~OFL_FLAT;
	}
	if(mode & OFL_SMOOTH && !model->normals){
		printf("oflDraw() warning: smooth render mode requested "
			"with no normals defined.\n");
		mode &= ~OFL_SMOOTH;
	}
	if(mode & OFL_TEXTURE && !model->texcoords){
		printf("oflDraw() warning: texture render mode requested "
			"with no texture coordinates defined.\n");
		mode &= ~OFL_TEXTURE;
	}
	if(mode & OFL_FLAT && mode & OFL_SMOOTH){
		printf("oflDraw() warning: flat render mode requested "
			"and smooth render mode requested (using smooth).\n");
		mode &= ~OFL_FLAT;
	}
	if(mode & OFL_COLOR && !model->materials){
		printf("oflDraw() warning: color render mode requested "
			"with no materials defined.\n");
		mode &= ~OFL_COLOR;
	}
	if(mode & OFL_MATERIAL && !model->materials){
		printf("oflDraw() warning: material render mode requested "
			"with no materials defined.\n");
		mode &= ~OFL_MATERIAL;
	}
	if(mode & OFL_COLOR && mode & OFL_MATERIAL){
		printf("oflDraw() warning: color and material render mode requested "
			"using only material mode.\n");
		mode &= ~OFL_COLOR;
	}
	if(mode & OFL_COLOR){
		glEnable(GL_COLOR_MATERIAL);
	}else if(mode & OFL_MATERIAL){
		glDisable(GL_COLOR_MATERIAL);
	}
	group=model->groups;
	while(group){
		dprintf(("group=0x%x numtriangles=%d \n",group,group->numtriangles));
		if(mode & OFL_MATERIAL){
			material=&model->materials[group->material];
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,material->ambient);
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,material->diffuse);
			glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,material->specular);
			glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,material->shininess);
		}
		if(mode & OFL_COLOR){
			glColor3fv(material->diffuse);
		}
		glBegin(GL_TRIANGLES);
			for(i=0;i<group->numtriangles;i++){
				triangle=&model->triangles[(group->triangles[i])];
				if(mode & OFL_FLAT){
					glNormal3fv(&model->facetnorms[3*triangle->findex]);
				}
				if(mode & OFL_SMOOTH){
					glNormal3fv(&model->normals[3*triangle->nindices[0]]);
				}
				if(mode & OFL_TEXTURE){
					glTexCoord2fv(&model->texcoords[2*triangle->tindices[0]]);
				}
				glVertex3fv(&model->vertices[3*triangle->vindices[0]]);//vertex 0
				//debug 
				//printf("pos0 = %f %f %f \n",model->vertices[3*triangle->vindices[0]+0],
				//							model->vertices[3*triangle->vindices[0]+1],
				//							model->vertices[3*triangle->vindices[0]+2]);

				if(mode & OFL_SMOOTH){
					glNormal3fv(&model->normals[3*triangle->nindices[1]]);
				}
				if(mode & OFL_TEXTURE){
					glTexCoord2fv(&model->texcoords[2*triangle->tindices[1]]);
				}
				glVertex3fv(&model->vertices[3*triangle->vindices[1]]);//vertex 1
				//debug 
				//printf("pos1 = %f %f %f \n",model->vertices[3*triangle->vindices[0]+0],
				//							model->vertices[3*triangle->vindices[0]+1],
				//							model->vertices[3*triangle->vindices[0]+2]);

				if(mode & OFL_SMOOTH){
					glNormal3fv(&model->normals[3*triangle->nindices[2]]);
				}
				if(mode & OFL_TEXTURE){
					glTexCoord2fv(&model->texcoords[2*triangle->tindices[2]]);
				}
				glVertex3fv(&model->vertices[3*triangle->vindices[2]]);//vertex 2

				//debug 
				//printf("pos2 = %f %f %f \n",model->vertices[3*triangle->vindices[0]+0],
				//							model->vertices[3*triangle->vindices[0]+1],
				//							model->vertices[3*triangle->vindices[0]+2]);
			}
		glEnd();
		group=group->next;
	}
	tprintf(("end\n"));
}

uint32_t oflList(oflModel_t *model,uint32_t listnum,uint32_t mode)
{
	tprintf(("begin\n"));
	dprintf(("mode=%d(0x%x)\n",mode,mode));
	uint32_t list;
	list=glGenLists(listnum);
	glNewList(list,GL_COMPILE);
	oflDraw(model,mode);
	glEndList();
	tprintf(("end\n"));
	return list;
}

}// ns