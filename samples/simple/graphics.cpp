#include "graphics.h"

Graphics::Graphics()
{

}
Graphics::~Graphics()
{

}

///	初期化
void Graphics::init(void)
{
	LogTrace("\n");
	mGeom.init();
	mWnd.init();
	m_disp_status_enable=true;
	m_disp_performance_enable=true;
	m_draw_facet_normal_enable=false;
	m_disp_bounding_box_enable=false;
	m_menu_entries=0;
	LogTrace("\n");
	return;
}

///	終了処理
/// 全てのメモリ開放
void Graphics::term(void)
{
	LogTrace("start\n");
	mGeom.term();
	mWnd.term();
	LogTrace("end \n");
}

void Graphics::Draw(oflModel_t *model)
{
	uint32_t mode;

	float ambient[]={0.2f,0.2f,0.2f,1.0f};
	float diffuse[]={0.8f,0.8f,0.8f,1.0f};
	float specular[]={0.0f,0.0f,0.0f,1.0f};
	float shininess=65.0f;
	glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
	glMaterialf(GL_FRONT,GL_SHININESS,shininess);

	switch(mGeom.GetMaterialMode()){
		case 0:
			if(GetDrawFacetNormalEnable()){
				mode=OFL_FLAT;
			}else{
				mode=OFL_SMOOTH;
			}
			break;
		case 1:
			if(GetDrawFacetNormalEnable()){
				mode=OFL_FLAT|OFL_COLOR;
			}else{
				mode=OFL_SMOOTH|OFL_COLOR;
			}
			break;
		case 2:
			if(GetDrawFacetNormalEnable()){
				mode=OFL_FLAT|OFL_MATERIAL;
			}else{
				mode=OFL_SMOOTH|OFL_MATERIAL;
			}
			break;
		default:
			perror("nosuchmaterialmode.\n");
			assert(false);
	}
	oflDraw(model,mode);
}

void Graphics::oflDraw(oflModel_t *model,uint32_t mode)
{
	static uint32_t i;
	static oflGroup_t *group;
	static oflTriangle_t *triangle;
	static oflMaterial_t *material;
	assert(model);
	assert(model->vertices);
	if(mode & OFL_FLAT && !model->facetnorms){
		LogWarn("oflDraw() warning: flat render mode requested with no facet normals defined.\n");
		mode &= ~OFL_FLAT;
	}
	if(mode & OFL_SMOOTH && !model->normals){
		LogWarn("oflDraw() warning: smooth render mode requested with no normals defined.\n");
		mode &= ~OFL_SMOOTH;
	}
	if(mode & OFL_TEXTURE && !model->texcoords){
		LogWarn("oflDraw() warning: texture render mode requested with no texture coordinates defined.\n");
		mode &= ~OFL_TEXTURE;
	}
	if(mode & OFL_FLAT && mode & OFL_SMOOTH){
		LogWarn("oflDraw() warning: flat render mode requested and smooth render mode requested (using smooth).\n");
		mode &= ~OFL_FLAT;
	}
	if(mode & OFL_COLOR && !model->materials){
		LogWarn("oflDraw() warning: color render mode requested with no materials defined.\n");
		mode &= ~OFL_COLOR;
	}
	if(mode & OFL_MATERIAL && !model->materials){
		LogWarn("oflDraw() warning: material render mode requested with no materials defined.\n");
		mode &= ~OFL_MATERIAL;
	}
	if(mode & OFL_COLOR && mode & OFL_MATERIAL){
		LogWarn("oflDraw() warning: color and material render mode requested using only material mode.\n");
		mode &= ~OFL_COLOR;
	}
	if(mode & OFL_COLOR){
		glEnable(GL_COLOR_MATERIAL);
	}else if(mode & OFL_MATERIAL) {
		glDisable(GL_COLOR_MATERIAL);
	}
	group=model->groups;
	while(group){ //グループごとの描画
		if(mode&OFL_MATERIAL){
			material=&model->materials[group->material];
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,material->ambient);
			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,material->diffuse);
			glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,material->specular);
			glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,material->shininess);
		}
		if(mode&OFL_COLOR){
			glColor3fv(material->diffuse);
		}
		glBegin(GL_TRIANGLES);
		for(i=0;i<group->numtriangles;i++){
			triangle=&model->triangles[(group->triangles[i])];
			if(mode&OFL_FLAT){
				glNormal3fv(&model->facetnorms[3*triangle->findex]);
			}
			if(mode&OFL_SMOOTH){
				glNormal3fv(&model->normals[3*triangle->nindices[0]]);
			}
			if(mode&OFL_TEXTURE){
				glTexCoord2fv(&model->texcoords[2*triangle->tindices[0]]);
			}
			glVertex3fv(&model->vertices[3*triangle->vindices[0]]);
			
			if(mode&OFL_SMOOTH){
				glNormal3fv(&model->normals[3*triangle->nindices[1]]);
			}
			if(mode&OFL_TEXTURE){
				glTexCoord2fv(&model->texcoords[2*triangle->tindices[1]]);
			}
			glVertex3fv(&model->vertices[3*triangle->vindices[1]]);
			
			if(mode&OFL_SMOOTH){
				glNormal3fv(&model->normals[3*triangle->nindices[2]]);
			}
			if(mode&OFL_TEXTURE){
				glTexCoord2fv(&model->texcoords[2*triangle->tindices[2]]);
			}
			glVertex3fv(&model->vertices[3*triangle->vindices[2]]);
		}
		glEnd();
		group=group->next;
	}
}





bool	Graphics::GetDrawFacetNormalEnable( void ) {
	return m_draw_facet_normal_enable;
}
bool	Graphics::GetDispBoundingBoxEnable( void ) {
	return m_disp_bounding_box_enable;
}

bool	Graphics::GetDispStatusEnable( void ) {
	return m_disp_status_enable;
}

bool Graphics::GetDispPerformanceEnable( void ) {
	return m_disp_performance_enable;
}

//メニューエントリ設定
void Graphics::SetMenuEntries( uint32_t entries ) {
	m_menu_entries = entries;
}
//メニューエントリ取得
uint32_t	Graphics::GetMenuEntries( void ) {
	return m_menu_entries;
}
