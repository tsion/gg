#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SOIL/SOIL.h>
#include <iostream>

const char vertexSource[] = R"(
  #version 150

  in vec3 position;
  in vec3 color;
  in vec2 texcoord;

  out vec3 Color;
  out vec2 Texcoord;

  uniform mat4 trans;
  uniform vec3 overrideColor;

  void main(void) {
    Texcoord = texcoord;
    Color = overrideColor * color;
    gl_Position = trans * vec4(position, 1.0);
  }
)";

const char fragmentSource[] = R"(
  #version 150

  in vec3 Color;
  in vec2 Texcoord;

  out vec4 outColor;

  uniform sampler2D texKitten;
  uniform sampler2D texPuppy;

  void main(void) {
    vec4 colKitten = texture(texKitten, Texcoord);
    vec4 colPuppy = texture(texPuppy, Texcoord);
    outColor = vec4(Color, 1.0) * mix(colKitten, colPuppy, 0.5);
  }
)";

GLuint compileShader(GLenum shaderType, const char* shaderSource) {
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderSource, nullptr);
  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (!status) {
    char buffer[512];
    glGetShaderInfoLog(shader, 512, NULL, buffer);
    std::cerr << buffer << '\n';
    std::abort();
  } else {
    return shader;
  }
}

int main() {
  // Initialize SDL.
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // 8-bit stencil buffer.
  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_Window* window = SDL_CreateWindow(
      "gg", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
      SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  // Initialize GLEW.
  glewExperimental = GL_TRUE;
  glewInit();

  // Initialize OpenGL.
  glEnable(GL_DEPTH_TEST);

  // Create Vertex Array Object.
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create a Vertex Buffer Object and copy the vertex data to it.
  GLfloat vertices[] = {
  // X      Y      Z     R     G     B     S     T
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

    // Floor vertices.
    -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
     1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
  };
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Create and compile the vertex and fragment shaders.
  GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
  GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

  // Link the vertex and fragment shaders into a shader program.
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  // Specify the layout of the vertex data.
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        nullptr);

  GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void*)(3 * sizeof(float)));

  GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void*)(6 * sizeof(float)));

  // Set up textures.
  GLuint textures[2];
  glGenTextures(2, textures);

  int width, height;
  unsigned char* image;

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  image = SOIL_load_image("sample.png", &width, &height, 0, SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image);
  SOIL_free_image_data(image);
  glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  image = SOIL_load_image("sample2.png", &width, &height, 0, SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image);
  SOIL_free_image_data(image);
  glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // TODO(tsion): Test mipmaps.
  // glGenerateMipmap(GL_TEXTURE_2D);

  // Set up camera vectors for view transformation.
  glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec3 cameraPosition = glm::vec3(2.5f, 2.5f, 2.0f);
  glm::vec3 cameraDirection;
  float horizontalAngle = 3.8f;
  float verticalAngle = -0.58f;
  float cameraSpeed = 4.0f;
  float cameraRotationSpeed = 0.025f;

  // Calculate projection transformation.
  glm::mat4 proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 10.0f);

  GLint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");
  GLint uniTrans = glGetUniformLocation(shaderProgram, "trans");

  SDL_Event windowEvent;
  bool quit = false;
  float lastTime = (float)SDL_GetTicks() / 1000.0f;

  bool forwardHeld  = false;
  bool backwardHeld = false;
  bool leftHeld     = false;
  bool rightHeld    = false;
  bool upHeld       = false;
  bool downHeld     = false;

  while (true) {
    float currentTime = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    while (SDL_PollEvent(&windowEvent)) {
      switch (windowEvent.type) {
        case SDL_QUIT:
          quit = true;
          break;

        case SDL_KEYDOWN:
          switch (windowEvent.key.keysym.sym) {
            case SDLK_w: forwardHeld  = true; break;
            case SDLK_s: backwardHeld = true; break;
            case SDLK_a: leftHeld     = true; break;
            case SDLK_d: rightHeld    = true; break;
            case SDLK_r: upHeld       = true; break;
            case SDLK_f: downHeld     = true; break;
          }
          break;

        case SDL_KEYUP:
          switch (windowEvent.key.keysym.sym) {
            case SDLK_w: forwardHeld  = false; break;
            case SDLK_s: backwardHeld = false; break;
            case SDLK_a: leftHeld     = false; break;
            case SDLK_d: rightHeld    = false; break;
            case SDLK_r: upHeld       = false; break;
            case SDLK_f: downHeld     = false; break;
            case SDLK_ESCAPE:
              quit = true;
              break;
          }
          break;

        case SDL_MOUSEMOTION:
          horizontalAngle +=
              windowEvent.motion.xrel * deltaTime * cameraRotationSpeed;
          horizontalAngle = std::fmod(horizontalAngle, 2 * glm::pi<float>());

          verticalAngle -=
              windowEvent.motion.yrel * deltaTime * cameraRotationSpeed;
          verticalAngle = glm::clamp(
              verticalAngle,
              -glm::pi<float>() / 2.0f + 0.001f,
              glm::pi<float>() / 2.0f - 0.001f);

          break;
      }
    }

    if (quit) { break; }

    glm::vec3 direction;
    if (forwardHeld) {
      direction += glm::vec3(cameraDirection.x, cameraDirection.y, 0.0f);
    }
    if (backwardHeld) {
      direction -= glm::vec3(cameraDirection.x, cameraDirection.y, 0.0f);
    }
    if (leftHeld)  { direction += glm::cross(cameraUp, cameraDirection); }
    if (rightHeld) { direction -= glm::cross(cameraUp, cameraDirection); }
    if (upHeld)    { direction += cameraUp; }
    if (downHeld)  { direction -= cameraUp; }
    if (direction != glm::vec3()) {
      cameraPosition += glm::normalize(direction) * deltaTime * cameraSpeed;
    }

    // Clear the screen to white.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate model transformation.
    glm::mat4 model;
    model = glm::rotate(model, currentTime * 180.0f,
                        glm::vec3(0.0f, 0.0f, 1.0f));

    // Calculate view transformation.
    cameraDirection = glm::vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        cos(verticalAngle) * cos(horizontalAngle),
        sin(verticalAngle));
    glm::mat4 view = glm::lookAt(
        cameraPosition, cameraPosition + cameraDirection, cameraUp);

    // Calculate the overall combined transformation.
    glm::mat4 trans = proj * view * model;
    glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));

    // Draw the cube.
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glEnable(GL_STENCIL_TEST);

    // Draw the floor.
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF); // Write to stencil buffer
    glDepthMask(GL_FALSE); // Don't write to depth buffer
    glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)
    glDrawArrays(GL_TRIANGLES, 36, 6);

    // Draw the cube reflection.
    glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
    glStencilMask(0x00); // Don't write anything to stencil buffer
    glDepthMask(GL_TRUE); // Write to depth buffer
    model = glm::scale(glm::translate(model, glm::vec3(0, 0, -1)),
                       glm::vec3(1, 1, -1));
    trans = proj * view * model;
    glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
    glUniform3f(uniColor, 0.3f, 0.3f, 0.3f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);

    glDisable(GL_STENCIL_TEST);

    // Swap buffers.
    SDL_GL_SwapWindow(window);
  }

  glDeleteTextures(2, textures);
  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  SDL_GL_DeleteContext(context);
  SDL_Quit();

  return 0;
}
