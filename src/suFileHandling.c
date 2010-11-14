#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <GL/glew.h>
#include "suTypes.h"

/*	*** FUNCTION DECLARATIONS */

void fnMakeFaces(face_t *pFaces)
{
	if (pFaces != NULL)
	{
		glBegin(GL_TRIANGLES);
			glMaterialfv(GL_FRONT, GL_AMBIENT, pFaces->pMaterial->lAmbient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, pFaces->pMaterial->lDiffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, pFaces->pMaterial->lSpecular);
			glMaterialf(GL_FRONT, GL_SHININESS, pFaces->pMaterial->lShininess);
			glNormal3fv(pFaces->pNormal->fVectC);
			glVertex3fv(pFaces->pVertice1->fVectC);
			glVertex3fv(pFaces->pVertice2->fVectC);
			glVertex3fv(pFaces->pVertice3->fVectC);
		glEnd();
		fnMakeFaces(pFaces->pNext);
	}
} // fnMakeFaces

model_t *fnAddModel(model_t *pMoList, model_t *pModel)
{
	if (pMoList == NULL)
	{
		pModel->pNext = NULL;
		return pModel;
	}
	else
	{
		if (pMoList->pNext == NULL)
		{
			pMoList->pNext = pModel;
			pModel->pNext = NULL;
		}
		else
		{
			fnAddModel(pMoList->pNext, pModel);
		}
	}
	return pMoList;
} // fnAddModel

vector_t *fnAddVector(vector_t *pVList, vector_t *pVector)
{
	if (pVList == NULL)
	{
		pVector->iVectI = 1;
		pVector->pNext = NULL;
		return pVector;
	}
	else
	{
		if (pVList->pNext == NULL)
		{
			pVList->pNext = pVector;
			pVector->iVectI = pVList->iVectI + 1;
			pVector->pNext = NULL;
		}
		else
		{
			fnAddVector(pVList->pNext, pVector);
		}
	}
	return pVList;
} // fnAddVector

material_t *fnAddMaterial(material_t *pMList, material_t *pMaterial)
{
	if (pMList == NULL)
	{
		pMaterial->pNext = NULL;
		return pMaterial;
	}
	else
	{
		if (pMList->pNext == NULL)
		{
			pMList->pNext = pMaterial;
			pMaterial->pNext = NULL;
		}
		else
		{
			fnAddMaterial(pMList->pNext, pMaterial);
		}
	}
	return pMList;
} // fnAddMaterial

face_t *fnAddFace(face_t *pFList, face_t *pFace)
{
	if (pFList == NULL)
	{
		pFace->pNext = NULL;
		return pFace;
	}
	else
	{
		if (pFList->pNext == NULL)
		{
			pFList->pNext = pFace;
			pFace->pNext = NULL;
		}
		else
		{
			fnAddFace(pFList->pNext, pFace);
		}
	}
	return pFList;
} // fnAddFace

vector_t *fnGetVector(vector_t *pVList, int iVectI)
{
	if (pVList != NULL)
	{
		if (pVList->iVectI == iVectI)
			return pVList;
		else
			return fnGetVector(pVList->pNext, iVectI);
	}
	else
		return NULL;
} // fnGetVector

material_t *fnGetMaterial(material_t *pMList, char *pMatName)
{
	if (pMList != NULL)
	{
		if (strcmp(pMatName, pMList->pName) == 0)
			return pMList;
		else
			return fnGetMaterial(pMList->pNext, pMatName);
	}
	else
		return NULL;
} // fnGetMaterial

material_t *fnGetMaterials(char *pName)
{
	FILE		*fp;
	char		*ln, *tmp, buffer[50];
	size_t		size;
	material_t	*pMaterial, *pMaterials;
	
	pMaterial = NULL;
	pMaterials = NULL;
	
	strcpy(buffer, "res/models/");
	strcat(buffer, pName);
	
	if ((fp = fopen(buffer, "r")) == NULL)
		return NULL;
	
	if ((ln = malloc(sizeof(char))) == NULL)
		return NULL;
	
	size = 1;
	while (!feof(fp))
	{
		ln[size - 1] = (char)fgetc(fp);
		while (ln[size - 1] != '\n' && !feof(fp))
		{
			tmp = ln;
			
			size++;
			if ((ln = realloc(ln, sizeof(char) * size)) == NULL)
			{
				free(tmp);
				return NULL;
			}
			ln[size - 1] = (char)fgetc(fp);
		} // while
		ln[size - 1] = '\0';
		
		// treat line data
		if (strstr(ln, "newmtl ") != NULL)
		{
			if (pMaterial != NULL)
			{
				pMaterials = fnAddMaterial(pMaterials, pMaterial);
			}
			
			if ((pMaterial = malloc(sizeof(material_t))) == NULL)
				return NULL;
			sscanf(ln, "newmtl %s", pMaterial->pName);
		}
		else if (strstr(ln, "Ns ") != NULL && pMaterial != NULL)
		{
			sscanf(ln, "Ns %f", &pMaterial->lShininess);
		}
		else if (strstr(ln, "Ka ") != NULL && pMaterial != NULL)
		{
			sscanf(ln, "Ka %f %f %f", &pMaterial->lAmbient[0], &pMaterial->lAmbient[1], &pMaterial->lAmbient[2]);
		}
		else if (strstr(ln, "Kd ") != NULL && pMaterial != NULL)
		{
			sscanf(ln, "Kd %f %f %f", &pMaterial->lDiffuse[0], &pMaterial->lDiffuse[1], &pMaterial->lDiffuse[2]);
			pMaterial->lDiffuse[3] = 1.0f;
		}
		else if (strstr(ln, "Ks ") != NULL && pMaterial != NULL)
		{
			sscanf(ln, "Ks %f %f %f", &pMaterial->lSpecular[0], &pMaterial->lSpecular[1], &pMaterial->lSpecular[2]);
		}
		
		size = 1;
		free(ln);
		
		if ((ln = malloc(size)) == NULL)
		{
			return NULL;
		}
	} // while
	
	if (pMaterial != NULL) pMaterials = fnAddMaterial(pMaterials, pMaterial);
	return pMaterials;
} // fnGetMaterials

model_t *fnGetModel(char *pName)
{
	FILE		*fp;
	char		*ln, *tmp, buffer[50], mat[50];
	size_t		size;
	model_t		*pModel;
	vector_t 	*pVect, *pVertices, *pNormals;
	face_t		*pFace;
	int			iV1, iV2, iV3, iN;
	
	pVect = NULL;
	pVertices = NULL;
	pNormals = NULL;
	pFace = NULL;
	
	
	strcpy(buffer, "res/models/");
	strcat(buffer, pName);
	
	if ((fp = fopen(buffer, "r")) == NULL)
		return NULL;
	
	if ((ln = malloc(sizeof(char))) == NULL)
		return NULL;
	
	if ((pModel = malloc(sizeof(model_t))) == NULL)
		return NULL;
	
	pModel->pFaces = NULL;
	pModel->pMaterials = NULL;
	
	size = 1;
	while (!feof(fp))
	{
		ln[size - 1] = (char)fgetc(fp);
		while (ln[size - 1] != '\n' && !feof(fp))
		{
			tmp = ln;
			
			size++;
			if ((ln = realloc(ln, sizeof(char) * size)) == NULL)
			{
				free(tmp);
				return NULL;
			}
			ln[size - 1] = (char)fgetc(fp);
		} // while
		ln[size - 1] = '\0';
		
		// treat line data
		if (strstr(ln, "mtllib ") != NULL)
		{
			sscanf(ln, "mtllib %s", buffer);
			printf("Associated MTL file found: %s. Loading...\n", buffer);
			pModel->pMaterials = fnGetMaterials(buffer);
		} // get materials
		else if (strstr(ln, "v ") != NULL)
		{
			if ((pVect = malloc(sizeof(vector_t))) == NULL)
				return NULL;
			sscanf(ln, "v %f %f %f", &pVect->fVectC[0], &pVect->fVectC[1], &pVect->fVectC[2]);
			pVertices = fnAddVector(pVertices, pVect);
		} // get vertices
		else if (strstr(ln, "vn ") != NULL)
		{
			if ((pVect = malloc(sizeof(vector_t))) == NULL)
				return NULL;
			sscanf(ln, "vn %f %f %f", &pVect->fVectC[0], &pVect->fVectC[1], &pVect->fVectC[2]);
			pNormals = fnAddVector(pNormals, pVect);
		} // get normals
		else if (strstr(ln, "usemtl ") != NULL)
		{
			sscanf(ln, "usemtl %s", mat);
		} // set material for faces
		else if (strstr(ln, "f ") != NULL)
		{
			if ((pFace = malloc(sizeof(face_t))) == NULL)
				return NULL;
			sscanf(ln, "f %d//%d %d//%d %d//%d", &iV1, &iN, &iV2, &iN, &iV3, &iN);
			pFace->pVertice1 = fnGetVector(pVertices, iV1);
			pFace->pVertice2 = fnGetVector(pVertices, iV2);
			pFace->pVertice3 = fnGetVector(pVertices, iV3);
			pFace->pNormal = fnGetVector(pNormals, iN);
			pFace->pMaterial = fnGetMaterial(pModel->pMaterials, mat);
			pModel->pFaces = fnAddFace(pModel->pFaces, pFace);
		} // get faces
		
		size = 1;
		free(ln);
		
		if ((ln = malloc(size)) == NULL)
		{
			return NULL;
		}
	} // while
	
	fclose(fp);
	return pModel;
} // fnGetModel

model_t *fnGetModels(void)
{
	DIR				*dp;
	struct dirent	*ep;
	model_t			*pModel, *pModels;
	
	pModel = NULL;
	pModels = NULL;
	
	if ((dp = opendir("res/models")) == NULL)
		return NULL;
	
	while ((ep = readdir(dp)) != NULL)
	{
		if (strstr(ep->d_name, ".obj") != NULL)			// treat .obj file
		{
			printf("Loading Model from file: %s...\n", ep->d_name);
			pModel = fnGetModel(ep->d_name);
			sscanf(ep->d_name, "i%dt%d", &pModel->iDispListId, &pModel->iModelType);
			pModels = fnAddModel(pModels, pModel);
		}
	} // while
	
	closedir(dp);
	return pModels;
} // fnGetModels

void fnMakeDisplayList(model_t *pModel, GLfloat fR, GLfloat fG, GLfloat fB, int iDispListId)
{
	material_t *pFractMat;
	
	pFractMat = fnGetMaterial(pModel->pMaterials, "fraction");
	if (pFractMat != NULL)
	{
		pFractMat->lDiffuse[0] = fR;
		pFractMat->lDiffuse[1] = fG;
		pFractMat->lDiffuse[2] = fB;
		pFractMat->lAmbient[0] = fR;
		pFractMat->lAmbient[1] = fG;
		pFractMat->lAmbient[2] = fB;
	}
	
	glNewList(iDispListId, GL_COMPILE);
		fnMakeFaces(pModel->pFaces);
	glEndList();
} // fnMakeDisplayList

void fnBuildDefaultLists(model_t *pModels)
{
	if (pModels != NULL)
	{
		fnMakeDisplayList(pModels, 0.0f, 0.0f, 0.0f, pModels->iDispListId);
		fnBuildDefaultLists(pModels->pNext);
	}
} // fnBuildDefaultLists
