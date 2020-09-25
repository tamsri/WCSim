#include "polygon_mesh.hpp"

#include<fstream>
#include<assert.h>
#include<iostream>
#include<set>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/matrix.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "kdtree.hpp"
#include "ray.hpp"

#include "triangle.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "radiation_pattern.hpp"


PolygonMesh::PolygonMesh(const RadiationPattern & radiaton_pattern)
{
    shader_ = Object::default_shader_;
    model_ = glm::mat4(1.0f);
    // Create Vertex
    const glm::vec3 x_axis{1.0f, 0.0f, 0.0f};
    const glm::vec3 y_axis{0.0f, 1.0f, 0.0f};
    const glm::vec3 z_axis{0.0f, 0.0f, 1.0f};
    auto pattern = radiaton_pattern.pattern_;
    const auto min_g = radiaton_pattern.min_gain_;
    const auto range_g = radiaton_pattern.max_gain_ - 2.0f*min_g;
    float step = .5;
    bool is_dB = true;
    const float min_linear = pow(10, radiaton_pattern.min_gain_ / 10);
    const float max_linear = pow(10, radiaton_pattern.max_gain_ / 10);
    const float min_max_linear = max_linear - min_linear;
    const float min_dB = radiaton_pattern.min_gain_;
    const float max_dB = radiaton_pattern.max_gain_;
    const float range = (max_dB - 2 * min_dB);
    for(float phi = 0; phi < 180; phi+= step)
        for (float theta = 0; theta < 360; theta += step) {
            float gain_1 = pattern[theta][phi];
            float gain_2 = pattern[theta + step][phi];
            float gain_3 = pattern[theta + step][phi + step];
            float gain_4 = pattern[theta][phi + step];
            
            if (is_dB) {
                gain_1 = (gain_1 + abs(min_dB)) / range;
                gain_2 = (gain_2 + abs(min_dB)) / range;
                gain_3 = (gain_3 + abs(min_dB)) / range;
                gain_4 = (gain_4 + abs(min_dB)) / range;
            }
            else {
                gain_1 = pow(10, gain_1 / 10) / (min_max_linear);
                gain_2 = pow(10, gain_2 / 10) / (min_max_linear);
                gain_3 = pow(10, gain_3 / 10) / (min_max_linear);
                gain_4 = pow(10, gain_4 / 10) / (min_max_linear);
            }
            
            //std::cout << "gaains: " << gain_1 << " " << gain_2 << " " << gain_3 << " " << gain_4 << std::endl;
            
           /* glm::vec3 d1 =  glm::rotateY(x_axis, glm::radians((float)theta)) + glm::rotateZ(y_axis, -glm::radians((float)phi));
            glm::vec3 d2=   glm::rotateY(x_axis, glm::radians((float)theta+step)) + glm::rotateZ(y_axis, -glm::radians((float)phi));
            glm::vec3 d3 =  glm::rotateY(x_axis, glm::radians((float)theta+step)) + glm::rotateZ(y_axis, -glm::radians((float)phi + step));
            glm::vec3 d4 =  glm::rotateY(x_axis, glm::radians((float)theta)) + glm::rotateZ(y_axis, -glm::radians((float)phi+step));*/
           
            glm::vec3 d1 = glm::normalize( glm::vec3(cos(glm::radians((float)theta)), cos(glm::radians((float)phi)), sin(glm::radians((float)theta))) );
            glm::vec3 d2 = glm::normalize(glm::vec3(cos(glm::radians((float)theta + step)), cos(glm::radians((float)phi)), sin(glm::radians((float)theta + step))) );
            glm::vec3 d3 = glm::normalize(glm::vec3(cos(glm::radians((float)theta + step)), cos(glm::radians((float)phi + step)), sin(glm::radians((float)theta + step))) );
            glm::vec3 d4 = glm::normalize(glm::vec3(cos(glm::radians((float)theta)), cos(glm::radians((float)phi+step)), sin(glm::radians((float)theta))) );
            
            //glm::vec3 d2 = glm::normalize(glm::rotateY(x_axis, glm::radians((float)theta)) + glm::rotateZ(y_axis, glm::radians(float(phi)))) / 2.0f;
            //glm::vec3 d3 = glm::normalize(glm::rotateY(x_axis, glm::radians((float)theta)) + glm::rotateZ(y_axis, glm::radians(float(phi)))) / 2.0f;
            //glm::vec3 d4 = glm::normalize(glm::rotateY(x_axis, glm::radians((float)theta)) + glm::rotateZ(y_axis, glm::radians(float(phi)))) / 2.0f;

            
            glm::vec3 p1 = gain_1 * d1;
            glm::vec3 p2 = gain_2 * d2;
            glm::vec3 p3 = gain_3 * d3;
            glm::vec3 p4 = gain_4 * d4;
            
            glm::vec3 t1_n = glm::cross(glm::normalize(p2 - p1), glm::normalize(p3 - p1));
            glm::vec3 t2_n = glm::cross(glm::normalize(p1 - p4), glm::normalize(p3 - p4));
            // First Triangle
            vertices_.push_back({ p1*10.0f, glm::vec3{1.0f}, t1_n });
            vertices_.push_back({ p2*10.0f, glm::vec3{1.0f}, t1_n });
            vertices_.push_back({ p3*10.0f, glm::vec3{1.0f}, t1_n });
            // Second Triangle
            vertices_.push_back({ p4*10.0f, glm::vec3{1.0f}, t2_n });
            vertices_.push_back({ p1*10.0f, glm::vec3{1.0f}, t2_n });
            vertices_.push_back({ p3*10.0f, glm::vec3{1.0f}, t2_n });
        }
    SetupMesh();
    //std::cout << "ver: " << vertices_.size() << std::endl;
} 

PolygonMesh::PolygonMesh(const std::string& path, Shader * shader) : tree_(nullptr),
                                                                     vao_(0),
                                                                      vbo_(0)
{
    transform_ = { glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) };
    shader_ = shader;
    model_ = glm::mat4(1.0f);

    LoadObj(path); // Create vertices, uv, normal
    tree_ = new KDTree(objects_);
    SetupMesh();
}

bool PolygonMesh::LoadObj(const std::string& path)
{
    // Load the obj file to triangles and vertices for visualisation
    if (path.substr(path.length() - 4) != ".obj") {
        assert("Cannot read PolygonMesh .obj");
        return false;
    } 
    else
    {
        std::ifstream input_file_stream(path, std::ios::in);

        std::vector<glm::vec3> vertices, normals;
        std::vector<glm::vec2> uvs;
        std::vector<unsigned int> vertex_indices, uv_indices, normal_indices;

        for (std::string buffer; input_file_stream >> buffer;) {
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
                    input_file_stream >> vertex_index[i] >> trash_char
                        >> uv_index[i] >> trash_char
                        >> normal_index[i];
                    --vertex_index[i];
                    --uv_index[i];
                    --normal_index[i];

                    vertices_.push_back({ vertices[vertex_index[i]], uvs[uv_index[i]], normals[normal_index[i]] });
                }

                // Build triangles for ray tracer
                const std::vector<glm::vec3> points = { vertices[vertex_index[0]], vertices[vertex_index[1]], vertices[vertex_index[2]] };
                objects_.push_back(new Triangle(points, normals[normal_index[0]]));
            }
        }
        input_file_stream.close();
    }
    return true;
}

void PolygonMesh::SetupMesh()
{
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

bool PolygonMesh::IsHit(Ray &ray, float & t) const
{
    float temp_t;
    std::set<float> t_list;
    //return tree_->IsClosestHit(ray, t); ; /// to implement later, it hits but doesn't give correct t
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

bool PolygonMesh::IsHit(Ray& ray, float& t, Triangle *& hit_triangle) const
{
    float temp_t;
    std::set<std::pair<float, Triangle *>> t_list;

    //return tree_->IsHit(ray, t); ; /// to implement later, it hits but doesn't give correct t
    for (auto object : objects_) {
        Triangle * test_triangle = nullptr;
        if (object->IsHit(ray, temp_t, test_triangle)) {
            t_list.insert(std::pair{ temp_t , test_triangle});
        }
    }
    if (t_list.size() == 0) return false;
    //std::cout << "set size: " << t_list.size() << std::endl;
    t = std::get<0>(*t_list.begin());
    hit_triangle = std::get<1>(*t_list.begin());
    return true;
}

bool PolygonMesh::IsHit(Ray& ray, std::set<std::pair<float, Triangle*>> & hit_triangles) const
{
    float temp_t;
    //std::vector<float, Triangle *> hit_list;

    //return tree_->IsHit(ray, t); ; /// to implement later, it hits but doesn't give correct t
    for (auto object : objects_) {
        Triangle* test_triangle = nullptr;
        if (object->IsHit(ray, temp_t, test_triangle)) {
            hit_triangles.insert(std::pair{ temp_t, test_triangle });
        }
    }
    //std::cout << "hit triangles : " << hit_triangles.size() << std::endl;
    if (hit_triangles.size() == 0) return false;
    return true;
}

std::vector<const Triangle*> PolygonMesh::GetObjects()
{
    return std::vector<const Triangle*>(objects_);
}

void PolygonMesh::UpdateTransform(Transform& transform) {
    transform_ = transform;
    model_ = glm::translate(glm::mat4(1.0f), transform_.position);
    //model_ = glm::scale(model_, transform.scale);
    //model_ = glm::rotate(model_, transform.rotation); // implement later
}

void PolygonMesh::Draw() const {
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vertices_.size());
    glBindVertexArray(0);
}