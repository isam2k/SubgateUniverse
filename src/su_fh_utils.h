/*
 *					~~~ SUBGATE UNIVERSE ~~~
 *	Subgate Universe is a 3D multiplayer space shooter. The project embraces a server, as well
 *	as a client application.
 *
 *	Copyright (C) 2010 Aeberhard Samuel
 *
 *	This program is free software; you can redistribute it and/or modify it under the terms of
 *	the GNU General Public License as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *	
 *	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *	without even the implied warranty of MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *	See the GNU General Public License for more details.
 *
 *	You should have recieved a copy of the GNU General Public License along with this program;
 *	if not, see
 *	<http://www.gnu.org/licenses/>
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
