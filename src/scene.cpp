#ifndef _SCENE_H_
#define _SCENE_H_
#include "inc.h"
#include "texture.h"
#include <vector>
#include <map>

using namespace std;
using namespace glm;

// w większości standardowy OpenGLowy boilerplate

class Timer;
class Mesh
{
public:
	string Name;
	int MaterialIndex;
	vector<vec3> Vertices;
	vector<vec3> Normals;
	vector<vec2> TextureCoords;
	vector<vec3> Tangents;
	vector<vec3> Bitangents;
	vector<uvec3> Indices;
	vector<float> Distance;
private:
	GLuint VAO;
	GLuint Buffers[2];
	unsigned int count;
public:
	GLuint GPUVBO[3];
public:
	bool GenOGLMesh()
    {
        GLuint VSize = Vertices.size() * 3 * sizeof(GLfloat);
        GLuint NSize = Normals.size() * 3 * sizeof(GLfloat);
        GLuint TSize = TextureCoords.size() * 2 * sizeof(GLfloat);
        GLuint ISize = Indices.size() * 3 * sizeof(GLuint);
        GLuint TGSize = Tangents.size() * 3 * sizeof(GLfloat);
        GLuint BTGSize = Bitangents.size() * 3 * sizeof(GLfloat);
        GLuint DSize = Distance.size() * sizeof(GLfloat);
        count = Indices.size() * 3;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(2, Buffers);
        glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
        glBufferData(GL_ARRAY_BUFFER, VSize + NSize + TSize + DSize, NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, VSize, (const void*)&Vertices[0]);
        glBufferSubData(GL_ARRAY_BUFFER, VSize, NSize, (const void*)&Normals[0]);
        glBufferSubData(GL_ARRAY_BUFFER, VSize + NSize, TSize, (const void*)&TextureCoords[0]);
        glBufferSubData(GL_ARRAY_BUFFER, VSize + NSize + TSize, DSize, (const void*)&Distance[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)VSize);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(VSize + NSize));
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(VSize + NSize + TSize));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ISize, (const void*)&Indices[0], GL_STATIC_DRAW);

        glBindVertexArray(0);
        return true;
    }
	bool GenOGLStructuredData()
    {
        vector<vec4> verts;
        vector<vec4> norms;
        vector<uvec4> idxs;
        for (int i = 0; i < Vertices.size(); i++)
            verts.push_back(vec4(Vertices[i], 0.0));
        for (int i = 0; i < Normals.size(); i++)
            norms.push_back(vec4(Normals[i], 0.0));
        for (int i = 0; i < Indices.size(); i++)
            idxs.push_back(vec4(Indices[i], 0.0));
        GLuint VSize = verts.size() * 4 * sizeof(GLfloat);
        GLuint NSize = norms.size() * 4 * sizeof(GLfloat);
        GLuint ISize = idxs.size() * 4 * sizeof(GLuint);
        cout << "size: " << verts.size() << "," << norms.size() << "," << idxs.size() << endl;
        glGenBuffers(3, GPUVBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, GPUVBO[0]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, VSize, (const GLvoid*)&verts[0], GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, GPUVBO[0]);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, GPUVBO[1]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, NSize, (const GLvoid*)&norms[0], GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, GPUVBO[1]);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, GPUVBO[2]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, ISize, (const GLvoid*)&idxs[0], GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, GPUVBO[2]);
        return true;
    }
	void Render()
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (const void*)0);
        glBindVertexArray(0);
    }
};

class Material
{
public:
	float Ka[3];
	float Kd[3];
	float Ks[3];
	float Shininess;
	float Opacity;
	string diffuseTex;
	string specularTex;
public:
	Material(){}
	void SetMaterial(GLuint program, map<string, Texture2D*> &m_Textures)
    {
        glUniform3fv(glGetUniformLocation(program, "Ka"), 1, Ka);
        glUniform3fv(glGetUniformLocation(program, "Kd"), 1, Kd);
        glUniform3fv(glGetUniformLocation(program, "Ks"), 1, Ks);
        glUniform1f(glGetUniformLocation(program, "Shininess"), Shininess);

        if (diffuseTex.size() > 0 && m_Textures.find(diffuseTex) != m_Textures.end())
        {
            glUniform1i(glGetUniformLocation(program, "HasDiffuseTexture"), 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_Textures[diffuseTex]->OGLTexture);
            glUniform1i(glGetUniformLocation(program, "DiffuseTexture"), 0);
        }
        else
            glUniform1i(glGetUniformLocation(program, "HasDiffuseTexture"), 0);

        if (specularTex.size() > 0 && m_Textures.find(specularTex) != m_Textures.end())
        {
            glUniform1i(glGetUniformLocation(program, "HasSpecularTexture"), 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_Textures[specularTex]->OGLTexture);
            glUniform1i(glGetUniformLocation(program, "SpecularTexture"), 1);
        }
        else
            glUniform1i(glGetUniformLocation(program, "HasSpecularTexture"), 0);
    }
};

class Scene
{
public:
	vector<Mesh> m_Meshes;
	vector<Material> m_Materials;
	map<string, Texture2D*> m_Textures;
private:
	Scene(){}
public:
	Scene(string Filename)
	{
		LoadFromFile(Filename);
		calculateNormalDistances();
		GenOGLMeshes();
	}
	~Scene()
	{
	}
	void RenderScene(GLuint program)
	{
		int meshesNum = m_Meshes.size();
		for (int idx = 0; idx < meshesNum; idx++)
		{
			m_Materials[m_Meshes[idx].MaterialIndex].SetMaterial(program, m_Textures);
			m_Meshes[idx].Render();

		}
	}
	int GetMeshesNum()
	{
		return m_Meshes.size();
	}
	int GetMaterialsNum()
	{
		return m_Materials.size();
	}
	Mesh* GetMesh(int index)
	{
		return &m_Meshes[index];
	}
	Material* GetMaterial(int index)
	{
		return &m_Materials[index];
	}
	void SetMaterial(GLuint program, int index)
	{
		m_Materials[index].SetMaterial(program, m_Textures);
	}
private:
	bool LoadFromFile(string &Filename)
    {
        Assimp::Importer importer;
        const aiScene *pScene=importer.ReadFile(Filename,
                                                aiProcessPreset_TargetRealtime_Quality
                                                | aiProcess_PreTransformVertices
        );

        if (!pScene)
        {
            cerr << "Assimp: cannot open file!" << endl;
            return false;
        }
        m_Meshes.resize(pScene->mNumMeshes);
        for (unsigned int i = 0; i < m_Meshes.size(); i++)
        {
            const aiMesh* paiMesh = pScene->mMeshes[i];
            InitMesh(i, paiMesh);
        }
        return InitMaterial(pScene);

    };
	void InitMesh(int Index, const aiMesh *paiMesh)
    {
        m_Meshes[Index].MaterialIndex = paiMesh->mMaterialIndex;
        const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
        for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
        {
            const aiVector3D* pPos = &(paiMesh->mVertices[i]);
            const aiVector3D* pNormal = paiMesh->HasNormals() ? &(paiMesh->mNormals[i]) : &Zero3D;
            const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
            const aiVector3D* pTangent = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mTangents[i]) : &Zero3D;
            const aiVector3D* pBitangent = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mBitangents[i]) : &Zero3D;
            m_Meshes[Index].Vertices.push_back(vec3(pPos->x, pPos->y, pPos->z));
            m_Meshes[Index].Normals.push_back(vec3(pNormal->x, pNormal->y, pNormal->z));
            m_Meshes[Index].TextureCoords.push_back(vec2(pTexCoord->x, pTexCoord->y));
            m_Meshes[Index].Tangents.push_back(vec3(pTangent->x, pTangent->y, pTangent->z));
            m_Meshes[Index].Bitangents.push_back(vec3(pBitangent->x, pBitangent->y, pBitangent->z));
        }

        for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
        {
            const aiFace Face = paiMesh->mFaces[i];
            if (Face.mNumIndices != 3) continue;
            m_Meshes[Index].Indices.push_back(vec3(Face.mIndices[0], Face.mIndices[1], Face.mIndices[2]));
        }
    }
	bool InitMaterial(const aiScene* pScene)
    {
        m_Materials.resize(pScene->mNumMaterials);
        for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
        {
            const aiMaterial* pMaterial = pScene->mMaterials[i];
            for (unsigned int j = 0; j < pMaterial->mNumProperties; j++)
            {
                const aiMaterialProperty* pMaterialProperty = pMaterial->mProperties[j];
                if (pMaterialProperty->mKey == aiString(string("$clr.ambient")))
                    memcpy(m_Materials[i].Ka, pMaterialProperty->mData, sizeof(float)* 3);
                else if (pMaterialProperty->mKey == aiString(string("$clr.diffuse")))
                    memcpy(m_Materials[i].Kd, pMaterialProperty->mData, sizeof(float)* 3);
                else if (pMaterialProperty->mKey == aiString(string("$clr.specular")))
                    memcpy(m_Materials[i].Ks, pMaterialProperty->mData, sizeof(float)* 3);
                else if (pMaterialProperty->mKey == aiString(string("$mat.shininess")))
                    memcpy(&(m_Materials[i].Shininess), pMaterialProperty->mData, sizeof(float));
                else if (pMaterialProperty->mKey == aiString(string("$mat.opacity")))
                {
                    memcpy(&(m_Materials[i].Opacity), pMaterialProperty->mData, sizeof(float));
                }
            }
            if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
            {
                aiString Path;
                if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
                {
                    string path = Path.C_Str();
                    m_Materials[i].diffuseTex = path;
                    auto it = m_Textures.find(path);
                    if (it == m_Textures.end())
                        m_Textures[path] = new Texture2D(Path.C_Str());
                }
            }
            if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
            {
                aiString Path;
                if (pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
                {
                    string path = Path.C_Str();
                    m_Materials[i].specularTex = path;
                    auto it = m_Textures.find(path);
                    if (it == m_Textures.end())
                        m_Textures[path] = new Texture2D(Path.C_Str());
                }
            }
        }
        return true;
    }
	void calculateNormalDistances()
    {
        for (int i = 0; i < m_Meshes.size(); i++)
        {
            m_Meshes[i].Distance.resize(m_Meshes[i].Vertices.size());
            for (int j = 0; j < m_Meshes[i].Vertices.size(); j++)
            {
                vec3 org = m_Meshes[i].Vertices[j];
                vec3 dir = -m_Meshes[i].Normals[j];
                float delta = 1e-3;
                m_Meshes[i].Distance[j] = 0;
                for (int k = 0; k < m_Meshes[i].Indices.size(); k++)
                {
                    vec3 index = m_Meshes[i].Indices[k];
                    vec3 d1 = m_Meshes[i].Vertices[index.x];
                    vec3 d2 = m_Meshes[i].Vertices[index.y];
                    vec3 d3 = m_Meshes[i].Vertices[index.z];
                    mat3 A = mat3(d1.x - d2.x, d1.y - d2.y, d1.z - d2.z, d1.x - d3.x, d1.y - d3.y, d1.z - d3.z, dir.x, dir.y, dir.z);
                    vec3 b = vec3(d1.x - org.x, d1.y - org.y, d1.z - org.z);
                    if (abs(determinant(A)) < 1e-08)
                        continue;
                    mat3 Ainv = inverse(A);
                    vec3 v = Ainv*b;
                    if (0 <= v.x && 0 <= v.y && v.x + v.y <= 1 && v.z >= 1e-3)
                    {
                        if (v.z > m_Meshes[i].Distance[j])
                            m_Meshes[i].Distance[j] = v.z;
                    }
                }
            }
        }
    }

    // void calculateNormalDistances()
    // {
    //     for (int j = 0; j < this->vertices.size(); j++)
    //     {
    //         vec3 P1 = this->vertices[j].Position;
    //         vec3 N1 = this->vertices[j].Normal;

    //         for (int k = 0; k < this->vertices.size(); k++)
    //         {
    //             vec3 P2 = this->vertices[k].Position;
    //             if (P2 == P1)
    //                 continue;

    //             if (checkIsOnNormalExtension(P1, N1, P2))
    //             {
    //                 this->vertices[j].NormalDistance = distance(P1, P2);
    //             }
    //         }
    //     }
    // }

    // bool checkIsOnNormalExtension(glm::vec3 P1, glm::vec3 N1, glm::vec3 P2)
    // {
    //     float allowedError = 1e-3;
    //     glm::vec3 P1P2 = P2 - P1;
    //     float crossProduct = glm::cross(P1P2, N1);
    //     return glm::abs(crossProduct) < 1e-6;
    // }
	bool GenOGLMeshes()
    {
        bool res = true;
        for (int idx = 0; idx < m_Meshes.size(); idx++)
        {
            res &= m_Meshes[idx].GenOGLMesh();
            res &= m_Meshes[idx].GenOGLStructuredData();
        }
        return res;
    }
};

#endif