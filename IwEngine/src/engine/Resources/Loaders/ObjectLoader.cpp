#include "iw/engine/Resources/Loaders/ObjectLoader.h"
#include "iw/util/io/File.h"
#include "iw/log/logger.h"

iwm::vector3 ReadVector3(
	char* token);

iwm::vector2 ReadVector2(
	char* token);

void ReadFace(
	char* token,
	unsigned int* faces,
	int& index);

template<>
IwEngine::Object IwUtil::Load<IwEngine::Object>(
	const char* fileName)
{
	IwEngine::Object obj(fileName);

	std::vector<std::string> lines = IwUtil::ReadFileLines(fileName);

	int vertCount = 0;
	int normCount = 0;
	int textCount = 0;
	int faceCount = 0;
	for (auto str : lines) {
		char* token = std::strtok(&str[0u], " ");
		if (token == NULL) {
			continue;
		}

		else if (token[0] == 'v') {
			if (token[1] == 't') {
				textCount++;
			}

			else if (token[1] == 'n') {
				normCount++;
			}

			else {
				vertCount++;
			}
		}

		else if (token[0] == 'f') {
			token = std::strtok(NULL, " //");
			while (token != NULL) {
				faceCount++;
				token = std::strtok(NULL, " //");
			}
		}
	}

	obj.Vertices = new iwm::vector3[vertCount];
	obj.Normals  = new iwm::vector3[normCount];
	obj.Uvs      = new iwm::vector2[textCount];
	obj.Faces    = new unsigned int[faceCount];

	int vi = 0, ni = 0, ti = 0, fi = 0;

	for (auto& str : lines) {
		char* token = std::strtok(&str[0u], " ");
		if (token == NULL) {
			continue;
		}

		else if (token[0] == 'v') {
			switch (token[1]) {
				case 't': obj.Uvs[ti++]      = ReadVector2(token); break;
				case 'n': obj.Normals[ni++]  = ReadVector3(token); break;
				default:  obj.Vertices[vi++] = ReadVector3(token); break;
			}
		}

		else if (token[0] == 'f') {
			ReadFace(token, obj.Faces, fi);
		}
	}

	return obj;
}

iwm::vector3 ReadVector3(
	char* token)
{
	token = std::strtok(NULL, " ");
	float x = std::strtof(token, NULL);

	token = std::strtok(NULL, " ");
	float y = std::strtof(token, NULL);

	token = std::strtok(NULL, " ");
	float z = std::strtof(token, NULL);

	return { x, y, z };
}

iwm::vector2 ReadVector2(
	char* token)
{
	token = std::strtok(NULL, " ");
	float x = std::strtof(token, NULL);

	token = std::strtok(NULL, " ");
	float y = std::strtof(token, NULL);

	return { x, y };
}

void ReadFace(
	char* token, 
	unsigned int* faces,
	int& index)
{	
	token = std::strtok(NULL, " //");
	while(token != NULL) {
		faces[index++] = std::strtoul(token, NULL, 0);
		token = std::strtok(NULL, " //");
	}
}
