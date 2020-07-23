#include "polygon_mesh.hpp"

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<fstream>
#include<assert.h>

PolygonMesh::PolygonMesh(const std::string& path)
{
    /* Read the .obj file */
    if (path.substr(path.length() - 4) != ".obj") {
        assert("Cannot read PolygonMesh .obj");
    }
    else 
    {
        std::ifstream input_file_stream(path, std::ios::in);
		for (std::string buffer; input_file_stream >> buffer;) {
            if (buffer == "v") {
                float x, y, z;
                input_file_stream >> x >> y >> z;
                vertices_.emplace_back(x, y, z);
            }
            else if (buffer == "vn") { 
                float x, y, z;
                input_file_stream >> x >> y >> z;
                normal_.emplace_back(x, y, z);
            }
            else if (buffer == "f") {
                faces_.emplace_back(std::vector<int>(3, 0));
                for (auto i = 0; i < 3; ++i) {
                    input_file_stream >> faces_.back()[i];
                    faces_.back()[i]--;
                }
            }
		}
        input_file_stream.close();

        for (auto& face : faces_) {
        
        }
	}

}

void PolygonMesh::SetupMesh()
{
    //glGenVertexArrays(1, &vao_);
    //glGenBuffers(1, &vbo_);

    //glBindVertexArray(vao_);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    //glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW );

    //// vertex positions
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    //// vertex normals
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    //// vertex texture coords
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    //glBindVertexArray(0);
}
