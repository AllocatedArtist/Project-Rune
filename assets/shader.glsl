#vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 fragTexCoords;

uniform mat4 model;
uniform mat4 viewProjection;

void main() {
  fragTexCoords = aTexCoords;
  gl_Position = viewProjection * model * vec4(aPos, 1.0);
}

#fragment 
#version 330 core

out vec4 fragColor;
uniform sampler2D texture0;
in vec2 fragTexCoords;

void main() {
  fragColor = texture(texture0, fragTexCoords); 
}

