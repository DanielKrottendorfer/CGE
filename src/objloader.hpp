#ifndef OBJLOADER_H
#define OBJLOADER_H

int loadOBJ(
	const char * path,
	std::vector<glm::vec4> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec4> & out_normals
);

#endif
