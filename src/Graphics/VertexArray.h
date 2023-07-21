#ifndef VERTEX_ARRAY_H_
#define VERTEX_ARRAY_H_

enum class VertexFormat {
  kVertexFormatFloat3,
  kVertexFormatFloat2,
};

class VertexArray {
public:
  VertexArray() = default;
  ~VertexArray();

  void Create();

  void Bind();
  void Unbind();

  void VertexAttribute(const unsigned int& index, const VertexFormat& format, const unsigned long long& stride, void* offset);
private:
  unsigned int id_;
};


#endif