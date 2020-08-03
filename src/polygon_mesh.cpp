#include "polygon_mesh.hpp"

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<fstream>
#include<assert.h>
#include<iostream>
#include<set>

#include "kdtree.hpp"
#include "ray.hpp"

#include "triangle.hpp"
#include "shader.hpp"
#include "camera.hpp"



PolygonMesh::PolygonMesh(const std::string& path, Shader * shader) : tree_(nullptr), 
                                                                     vao_(0),
                                                                      vbo_(0)
{
    shader_ = shader;
    model_ = glm::mat4(1.0f);

    /* Read the .obj file */
    if (path.substr(path.length() - 4) != ".obj") {
        assert("Cannot read PolygonMesh .obj");
        return;
    }
    else 
    {
        std::ifstream input_file_stream(path, std::ios::in);

        std::vector<glm::vec3> vertices, normals;
        std::vector<glm::vec2> uvs;
        std::vector<unsigned int> vertex_indices, uv_indices, normal_indices;

        for (std::string buffer; input_file_stream >> buffer;) {
            //std::cout << "buffer" << buffer << std::endl;
            
            if (buffer == "v") {
                float x, y, z;
                input_file_stream >> x >> y >> z;
                vertices.push_back(glm::vec3(x, y, z));
            }
            else if (buffer == "vn") { 
                float x, y, z;
                input_file_stream >> x >> y >> z;
                normals.push_back(glm::vec3(x, y, z));
            }
            else if (buffer == "vt") {
                float x, y;
                input_file_stream >> x >> y;
                uvs.push_back(glm::vec2(x, y));
            }
            else if (buffer == "f") {
                unsigned int vertex_index[3], uv_index[3], normal_index[3];
                for (auto i = 0; i < 3; ++i) {
                    char trash_char;
                    input_file_stream   >> vertex_index[i] >> trash_char
                                        >> uv_index[i] >> trash_char
                                        >> normal_index[i];
                    --vertex_index[i];
                    --uv_index[i];
                    --normal_index[i];

                    vertices_.push_back({ vertices[vertex_index[i]], uvs[uv_index[i]], normals[normal_index[i]]});
                }
 
                // Build triangles for ray tracer
                const std::vector<glm::vec3> points = { vertices[vertex_index[0]], vertices[vertex_index[1]], vertices[vertex_index[2]]};
                objects_.push_back(new Triangle(points, normals[normal_index[0]]));
            }
            
		}

        input_file_stream.close();

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

    std::cout << "Setting up mesh" << std::endl;
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
 
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glBindVertexArray(0);

}

bool PolygonMesh::IsHit(Ray & ray, float & t) const
{
    float temp_t;
    std::set<float> t_list;
    //return tree_->IsHit(ray, t); ; /// to implement later, it hits but doesn't give correct t
    for (auto object : objects_) {
        if (object->IsHit(ray, temp_t)) {
            t_list.insert(temp_t);
        }
    }
    if (t_list.size() == 0) return false;
    //std::cout << "set size: " << t_list.size() << std::endl;
    t = *t_list.begin();
    return true;
}

void PolygonMesh::Draw() const {
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vertices_.size());
    glBindVertexArray(0);
}