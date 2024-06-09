uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

attribute vec3 vPosition;
void main()
{
    gl_Position = projection * view * model * vec4(vPosition, 1.0);
}