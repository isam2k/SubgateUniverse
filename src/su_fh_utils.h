/*
 *
 *	File:		su_fh_utils.h
 *	Author:		Samuel Aeberhard
 *	Description:	This file contains all the functions necessary to load a model
 *			exported from blender as *.obj and *.mtl - files into the game
 *			(in fact, everything is loaded into a model_t - struct).
 *
 */

/*	***	FUNCTION PROTOTYPES	*/

model_t *fnGetModels(void);	// search the model directory for models and load them
model_t *fnGetModel(char *pName);	// load an .obj and the associated .mtl file
int fnGetMaterials(model_t *pModel, char *pName);	// extract materials from .mtl file

model_t *fnAddModel(model_t *pMoList, model_t *pModel);	// dynamic list operation, adds a model to a dynamic list
int fnAddVertice(model_t *pModel, vector_t *pVector);
int fnAddNormal(model_t *pModel, vector_t *pVector);
int fnAddMaterial(model_t *pModel, material_t *pMaterial);
face_t *fnAddFace(face_t *pFList, face_t *pFace);
vector_t *fnGetVector(vector_t *pVList, int iVectI);
material_t *fnGetMaterial(material_t *pMList, char *pMatName);
vector_t *fnStrToVector(char *str);

void fnMakeDisplayList(model_t *pModel, GLfloat fR, GLfloat fG, GLfloat fB, int iDispListId);
void fnMakeFaces(face_t *pFaces);
void fnBuildDefaultLists(model_t *pModels);
