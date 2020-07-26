#include "polygon_mesh.hpp"

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<fstream>
#include<assert.h>
#include<iostream>


#include "kdtree.hpp"
#include "ray.hpp"

#include "triangle.hpp"
#include "shader.hpp"
#include "camera.hpp"

PolygonMesh::PolygonMesh(const std::string& path, Shader * shader)
{
    shader_ = shader;
    model_ = glm::mat4(1.0f);
    /* Read the .obj file */
    if (path.substr(path.length() - 4) != ".obj") {
        assert("Cannot read PolygonMesh .obj");
    }
    else 
    {
        std::ifstream input_file_stream(path, std::ios::in);
        
		for (std::string buffer; input_file_stream >> buffer;) {
            //std::cout << "buffer" << buffer << std::endl;
            if (buffer == "v") {
                float x, y, z;
                input_file_stream >> x >> y >> z;
                vertices_.push_back(glm::vec3(x, y, z));
            }
            else if (buffer == "vn") { 
                float x, y, z;
                input_file_stream >> x >> y >> z;
                normals_.push_back(glm::vec3(x, y, z));
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

	}
    for (auto & face : faces_) {
        //auto points = glm::vec3(vertices_[face[0]], vertices_[face[1]], vertices_[face[2]]);
        /*objects_.push_back(
            new Triangle(points)
        );*/
    }
    tree_ = new KDTree(objects_);
    SetupMesh();
}

bool PolygonMesh::LoadObj(const std::string& path, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& uvs, std::vector<glm::vec3>& normals)
{

    return true;
}

void PolygonMesh::SetupMesh()
{
    std::cout << "DEBUG OBJ" << std::endl;

    for (auto& ver : vertices_) {
        std::cout << ver.x << ',' << ver.y << ',' << ver.z << std::endl;
    }
    std::cout << "Setting up mesh" << std::endl;
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);
   
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(vao_, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

  
    // vertex positions
    //glEnableVertexAttribArray(vao_);
    //glVertexAttribPointer(vao_, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // safe end
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool PolygonMesh::IsHit(Ray & ray) const
{
    return tree_->IsHit(ray);
}

void PolygonMesh::Draw() const {
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vertices_.size());
    glBindVertexArray(0);

}