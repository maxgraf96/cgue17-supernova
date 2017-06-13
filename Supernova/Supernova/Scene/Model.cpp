#include <iostream>
#include <glm\glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include<memory>

#include "Model.hpp"
#include "SceneObject.hpp"
#include "../Textures/TextureLoader.hpp"
#include "../Materials/Material.hpp"


using namespace supernova;
using namespace supernova::scene;

Model::Model()
	: SceneObject(glm::mat4(1.0f)) {
}

Model::Model(glm::mat4& matrix, string const &path)
	: SceneObject(matrix) {
	this->loadModel(path);
}

Model::~Model() {

}

void Model::draw(Shader* shader) {
	for (GLuint i = 0; i < this->meshes.size(); i++) {
		this->meshes[i].draw(shader);
	}
}

void Model::draw() {};

void Model::loadModel(string const &path) {
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate);

	/* Check if scene and root node are not null */
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "ASSIMP ERROR: " << import.GetErrorString() << endl;
	}
	/* Get directory of model */
	this->directory = path.substr(0, path.find_last_of('/'));

	this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	/* loop through all of the node's meshes */
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}
	/* loop through all meshes of this node's children */
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		// recursive
		this->processNode(node->mChildren[i], scene);
	}
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		/* Check if texture was already loaded into memory */
		for (unsigned int j = 0; j < alreadyLoadedTextures.size(); j++) {
			if (std::strcmp(alreadyLoadedTextures[j].path.C_Str(), str.C_Str()) == 0) {
				textures.push_back(alreadyLoadedTextures[j]);
				skip = true;
				break;
			}
		}
		/* If texture isn't loaded, load it */
		if (!skip) {
			Texture texture;
			string testo = (directory + "/" + str.C_Str());
			texture.id = textureLoader.load(testo.c_str());
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			this->alreadyLoadedTextures.push_back(texture);
		}
	}
	return textures;
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	// material for if the mesh has no texture defined
	std::unique_ptr<Material> noTextureMaterial;
	aiColor3D colorAmbient;
	aiColor3D colorDiffuse;
	aiColor3D colorSpecular;

	for (GLuint i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		// position
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		// texture coordinates
		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			// Note: Assimp allows 8 texture coordinates per vertex, we only use the first
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.textureCoordinates = vec;
		}
		else {
			vertex.textureCoordinates = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}

	/* Indices */
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	/* Textures */
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// Diffuse
		vector<Texture> diffuseMaps = this->loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// Specular
		vector<Texture> specularMaps = this->loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		material->Get(AI_MATKEY_COLOR_AMBIENT, colorAmbient);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, colorDiffuse);
		material->Get(AI_MATKEY_COLOR_SPECULAR, colorSpecular);
		noTextureMaterial = std::make_unique<Material>(
			glm::vec3(colorAmbient.r, colorAmbient.b, colorAmbient.g),
			glm::vec3(colorDiffuse.r, colorDiffuse.b, colorDiffuse.g), 
			glm::vec3(colorSpecular.r, colorSpecular.b, colorSpecular.g),
			16.0f
			);
	}

	return Mesh(vertices, indices, textures, noTextureMaterial.get());
}
