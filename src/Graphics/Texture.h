#ifndef TEXTURE_H_
#define TEXTURE_H_

class Texture {
public:
  Texture() = default;
  ~Texture();

  Texture& Create();
  void Load(const char* filename, const bool& flip);
  void BindSlot(const int& slot);
  void Unbind();
private:
  unsigned int texture_;
};

#endif