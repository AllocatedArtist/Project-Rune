#ifndef BUFFER_H_
#define BUFFER_H_

enum class BufferType {
  kBufferTypeVertex,
  kBufferTypeIndex,
};

class Buffer {
public:
  Buffer() = default;
  Buffer(const BufferType& type);
  ~Buffer();

  void Bind();
  void Unbind();

  void BufferData(const unsigned long long& size, const void* data);
  void BufferSubData(const unsigned long long& offset, const unsigned int& size, const void* data);
private:
  unsigned int id_;
  BufferType type_;
};

#endif