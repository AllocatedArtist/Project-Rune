#vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 fragTexCoords;

uniform mat4 model;
uniform mat4 viewProjection;

out vec3 fragNormal;

void main() {
  fragNormal = aNormal;
  fragTexCoords = aTexCoords;
  gl_Position = viewProjection * model * vec4(aPos, 1.0);
}

#fragment 
#version 330 core

out vec4 fragColor;
uniform sampler2D texture0;

uniform vec3 fragBaseColor = vec3(0.0);

in vec2 fragTexCoords;
in vec3 fragNormal;

void main() {

  vec4 texColor = texture(texture0, fragTexCoords);

  if (texColor == vec4(0.0, 0.0, 0.0, 1.0)) {
    texColor = vec4(1.0);
  }


  fragColor = texColor * vec4(fragBaseColor, 1.0) * vec4(fragNormal, 1.0);  
}

