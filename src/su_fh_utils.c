#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <GL/glew.h>
#include "su_types.h"

/*	*** FUNCTION DECLARATIONS */

void fnMakeFaces(face_t *pFaces)
{
	face_t *pFace;
	for (pFace = pFaces; pFace != NULL; pFace = pFace->pNext)
	{
		glBegin(GL_TRIANGLES);
			glMaterialfv(GL_FRONT, GL_AMBIENT, pFace->pMaterial->lAmbient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, pFace->pMaterial->lDiffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, pFace->pMaterial->lSpecular);
			glMaterialf(GL_FRONT, GL_SHININESS, pFace->pMaterial->lShininess);
			glNormal3fv(pFace->pNormal->fVectC);
			glVertex3fv(pFace->pVertice1->fVectC);
			glVertex3fv(pFace->pVertice2->fVectC);
			glVertex3fv(pFace->pVertice3->fVectC);
		glEnd();
	} // for
} // fnMakeFaces

model_t *fnAddModel(model_t *pMoList, model_t *pModel)
{
	if (pModel == NULL)
	{
		return pMoList;
	} // if
	else
	{
		if (pMoList == NULL)
		{
			pModel->pNext = NULL;
			return pModel;
		} // else
		else
		{
			pModel->pNext = pMoList;
			return pModel;
		} // else
	} // else
} // fnAddModel

int fnAddVertice(model_t *pModel, vector_t *pVector)
{
	if (pVector == NULL || pModel == NULL)
		return 1;
	if (pModel->pVertices == NULL)
	{
		pVector->iVectI = 1;
	} // if
	else
	{
		pVector->iVectI = pModel->pVertices->iVectI + 1;
	} // else
	pVector->pNext = pModel->pVertices;
	pModel->pVertices = pVector;
	return 0;
} // fnAddVertice

int fnAddNormal(model_t *pModel, vector_t *pVector)
{
	if (pVector == NULL || pModel == NULL)
		return 1;
	if (pModel->pNormals == NULL)
	{
		pVector->iVectI = 1;
		pVector->pNext = NULL;
		pModel->pNormals = pVector;
	} // if
	else
	{
		pVector->iVectI = pModel->pNormals->iVectI + 1;
		pVector->pNext = pModel->pNormals;
		pModel->pNormals = pVector;
	} // else
	return 0;
} // fnAddNormal

int fnAddMaterial(model_t *pModel, material_t *pMaterial)
{
	if (pMaterial == NULL || pModel == NULL)
		return 1;
	pMaterial->pNext = pModel->pMaterials;
	pModel->pMaterials = pMaterial;
	return 0;
} // fnAddMaterial

int fnAddFace(model_t *pModel, face_t *pFace)
{
	if (pFace == NULL || pModel == NULL)
		return 1;
	pFace->pNext = pModel->pFaces;
	pModel->pFaces = pFace;
	return 0;
} // fnAddFace

vector_t *fnGetVector(vector_t *pVList, int iVectI)
{
	vector_t *pVector;
	for (pVector = pVList; pVector != NULL; pVector = pVector->pNext)
	{
		if (pVector->iVectI == iVectI)
			return pVector;
	} // for
	return NULL;
} // fnGetVector

material_t *fnGetMaterial(material_t *pMList, char *pMatName)
{
	material_t *pMaterial;
	for (pMaterial = pMList; pMaterial != NULL; pMaterial = pMaterial->pNext)
	{
		if (strcmp(pMatName, pMaterial->pName) == 0)
			return pMaterial;
	} // for
	return NULL;
} // fnGetMaterial

int fnGetMaterials(model_t *pModel, char *pName)
{
	FILE		*fp;
	char		*ln, *tmp, buffer[50];
	size_t		size;
	material_t	*pMaterial;
	
	pMaterial = NULL;
	
	strcpy(buffer, "res/models/");
	strcat(buffer, pName);
	
	if ((fp = fopen(buffer, "r")) == NULL)
		return 1;
	
	if ((ln = malloc(sizeof(char))) == NULL)
	{
		fclose(fp);
		return 1;
	}
	
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
				fclose(fp);
				free(tmp);
				return 1;
			}
			ln[size - 1] = (char)fgetc(fp);
		} // while
		ln[size - 1] = '\0';
		
		// treat line data
		if (strstr(ln, "newmtl ") != NULL)
		{
			if (pMaterial != NULL)
			{
				fnAddMaterial(pModel, pMaterial);
			}
			
			if ((pMaterial = malloc(sizeof(material_t))) == NULL)
			{
				fclose(fp);
				free(ln);
				return 1;
			} // if
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
			fclose(fp);
			return 1;
		}
	} // while
	fclose(fp);
	free(ln);
	
	fnAddMaterial(pModel, pMaterial);
	return 0;
} // fnGetMaterials

vector_t *fnStrToVector(char *str)
{
	vector_t *pVector;
	if ((pVector = malloc(sizeof(vector_t))) != NULL)
	{
		if (sscanf(str, "%*s %f %f %f", &pVector->fVectC[0], &pVector->fVectC[1], &pVector->fVectC[2]) == EOF)
		{
			free(pVector);
			return NULL;
		} // if
		else
		{
			pVector->iVectI = 0;
			pVector->pNext = NULL;
			return pVector;
		} // else
	} // if
	else
		return NULL;
} // fnStrToVector

model_t *fnGetModel(char *pName)
{
	FILE		*fp;
	char		*ln, *tmp, buffer[50], mat[50];
	size_t		size;
	model_t		*pModel;
	face_t		*pFace;
	int		iV1, iV2, iV3, iN;
	
	strcpy(buffer, "res/models/");
	strcat(buffer, pName);
	
	if ((fp = fopen(buffer, "r")) == NULL)
		return NULL;
	
	if ((ln = malloc(sizeof(char))) == NULL)
	{
		fclose(fp);
		return NULL;
	} // if
	
	if ((pModel = malloc(sizeof(model_t))) == NULL)
	{
		fclose(fp);
		free(ln);
		return NULL;
	}
	
	pModel->pVertices = NULL;
	pModel->pNormals = NULL;
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
				fclose(fp);
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
			fnGetMaterials(pModel, buffer);
		} // get materials
		else if (ln[0] == 'v' && ln[1] == ' ')
		{
			if (fnAddVertice(pModel, fnStrToVector(ln)))
			{
				fclose(fp);
				free(ln);
				return NULL;
			} // if
		} // get vertices
		else if (ln[0] == 'v' && ln[1] == 'n')
		{
			if (fnAddNormal(pModel, fnStrToVector(ln)))
			{
				fclose(fp);
				free(ln);
				return NULL;
			}
		} // get normals
		else if (strstr(ln, "usemtl ") != NULL)
		{
			sscanf(ln, "usemtl %s", mat);
		} // set material for faces
		else if (strstr(ln, "f ") != NULL)
		{
			if ((pFace = malloc(sizeof(face_t))) == NULL)
			{
				fclose(fp);
				free(ln);
				return NULL;
			} // if
			sscanf(ln, "f %d//%*d %d//%*d %d//%d", &iV1, &iV2, &iV3, &iN);
			pFace->pVertice1 = fnGetVector(pModel->pVertices, iV1);
			pFace->pVertice2 = fnGetVector(pModel->pVertices, iV2);
			pFace->pVertice3 = fnGetVector(pModel->pVertices, iV3);
			pFace->pNormal = fnGetVector(pModel->pNormals, iN);
			pFace->pMaterial = fnGetMaterial(pModel->pMaterials, mat);
			fnAddFace(pModel, pFace);
		} // get faces
		
		size = 1;
		free(ln);
		
		if ((ln = malloc(size)) == NULL)
		{
			return NULL;
		}
	} // while
	
	fclose(fp);
	free(ln);
	return pModel;
} // fnGetModel

model_t *fnGetModels(void)
{
	DIR		*dp;
	struct dirent	*ep;
	model_t		*pModel, *pModels;
	
	pModel = NULL;
	pModels = NULL;
	
	if ((dp = opendir("res/models")) == NULL)
		return NULL;
	
	while ((ep = readdir(dp)) != NULL)
	{
		if (strstr(ep->d_name, ".obj") != NULL)	// treat .obj file
		{
			printf("Loading Model from file: %s...\n", ep->d_name);
			if ((pModel = fnGetModel(ep->d_name)) == NULL)
			{
				closedir(dp);
				printf("Error while getting model\n");
				return NULL;
			}
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
	model_t *pModel;
	for (pModel = pModels; pModel != NULL; pModel = pModel->pNext)
	{
		fnMakeDisplayList(pModel, 0.0f, 0.0f, 0.0f, pModel->iDispListId);
	} // for
} // fnBuildDefaultLists
