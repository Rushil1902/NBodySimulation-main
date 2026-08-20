#include "Mesh.hpp"

Mesh::Mesh(const MeshData& data){
    indexCount = data.indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(float), data.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size()*sizeof(unsigned int),data.indices.data(),GL_STATIC_DRAW);

    //Vertex attribute
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Normal Attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // UV attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

};

Mesh::~Mesh(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);


}

void Mesh::draw() const{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,indexCount,GL_UNSIGNED_INT, 0);    
}

