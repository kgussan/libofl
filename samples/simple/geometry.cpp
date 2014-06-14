#include "geometry.h"
using namespace nsOfl;

//geometry
//コンストラクタ
Geometry::Geometry()
{
	sprintf(mGeomModelName[0],"kuma.obj");
	sprintf(mGeomModelName[1],"kuma.obj");
	sprintf(mGeomModelName[2],"box.obj");
}

///デストラクタ
Geometry::~Geometry()
{

}

///初期化
void Geometry::init(void)
{
	LogTrace("\n");
	m_model_index = 0;
	oflModel=(oflModel_t **)malloc(GEOM_MODEL_MAX*sizeof(oflModel_t));
	// ファイル名称指定 
	mModelFilename=(char **)malloc(GEOM_MODEL_MAX*(sizeof(char *)));
	for(size_t i=0;i<GEOM_MODEL_MAX;i++){
		mModelFilename[i]=(char *)malloc(sizeof(char)*1024);
	}
	LogTrace("\n");
	// はじめに読み込むファイルを指定。
	for(size_t i=0;i<GEOM_MODEL_MAX;i++){
		sprintf(mModelFilename[i],mGeomModelName[i]);
	}
	m_material_mode=0;
	m_smoothing_angle=90.0f;
	m_weld_distance=0.00001f;
}

///後処理
void Geometry::term(void)
{
	LogTrace("\n");
	//free(oflModel);
	for(size_t i=0;i<GEOM_MODEL_MAX;i++){
		nsOfl::oflDelete(oflModel[i]);
	}
	LogTrace("\n");
}
oflModel_t *Geometry::GetModel(uint32_t val)
{
	return oflModel[val];
}
oflModel_t *Geometry::GetModel(void)
{
	return GetModel(m_model_index);
}
void Geometry::SetModel(uint32_t index,const char *filename)
{
	oflModel[index]=nsOfl::oflReadObj(filename);
}

//モデル名称取得
const char* Geometry::GetModelName(uint32_t index)
{
	return mModelFilename[index];
}
//モデル名称取得
const char* Geometry::GetModelName(void)
{
	return mModelFilename[m_model_index];
}
//モデル名称設定
void Geometry::SetModelName(uint32_t index,const char *name)
{
	sprintf(mModelFilename[index],"%s",name);
}

void Geometry::IncreMaterialMode( void )
{
	m_material_mode++;
	if(m_material_mode>2){
		m_material_mode=0;
	}
}
void Geometry::DecreMaterialMode(void)
{
	if(m_material_mode==0){
		m_material_mode=2;
	}else{
		m_material_mode--;
	}
}

void	Geometry::SetMaterialMode( uint32_t val ) {
	m_material_mode = val;
}
uint32_t Geometry::GetMaterialMode(void){
	return m_material_mode;
}

void Geometry::SetModelIndex(uint32_t val){
	m_model_index = val;
}
uint32_t	Geometry::GetModelIndex( void ) {
	return m_model_index;
}

void	Geometry::SetScale( double val ) {
	m_scale = val;
}
double	Geometry::GetScale( void ) {
	return m_scale;
}

void	Geometry::SetSmoothingAngle( double val ) {
	m_smoothing_angle = val;
}
double	Geometry::GetSmoothingAngle( void ) {
	return m_smoothing_angle;
}

void	Geometry::SetWeldDistance( double val ) {
	m_weld_distance = val;
}
double	Geometry::GetWeldDistance( void ) {
	return m_weld_distance;
}
