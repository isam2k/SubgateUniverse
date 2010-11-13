/*
 *
 *	File:			suFileHandling.h
 *	Author:			Samuel Aeberhard
 *	Description:	This file contains all the functions necessary to load a model
 *					exported from blender as *.obj and *.mtl - files into the game
 *					(in fact, everything is loaded into a model_t - struct).
 *
 */

/*	***	FUNCTION PROTOTYPES	*/

model_t *fnGetModels(void);					// search the model directory for models and load them
model_t *fnGetModel(char *pName);			// load an .obj and the associated .mtl file
material_t *fnGetMaterials(char *pName);	// extract materials from .mtl file

model_t *fnAddModel(model_t *pMoList, model_t *pModel);						// dynamic list operation, adds a model to a dynamic list
vector_t *fnAddVector(vector_t *pVList, vector_t *pVector);					// dynamic list operation, adds a vector to a list of vectors (basically used to manage vertices)
material_t *fnAddMaterial(material_t *pMList, material_t *pMaterial);
face_t *fnAddFace(face_t *pFList, face_t *pFace);
vector_t *fnGetVector(vector_t *pVList, int iVectI);
material_t *fnGetMaterial(material_t *pMList, char *pMatName);

void fnMakeDisplayList(model_t *pModel, GLfloat fR, GLfloat fG, GLfloat fB, int iDispListId);
void fnMakeFaces(face_t *pFaces);
void fnBuildDefaultLists(model_t *pModels);
