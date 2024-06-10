#include <android/log.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>
#include <memory>

#include <glm/geometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Cube.h"
#include "ShaderProgram.h"

#define LOG_TAG "OpenGLES"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

EGLDisplay display;
EGLSurface surface;
EGLContext context;

int width;
int height;

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

glm::vec3 cameraPos(0, 2, -6);

std::unique_ptr<Cube> cube;
std::unique_ptr<ShaderProgram> shaderProgram;

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <string>

void initEGL(ANativeWindow* window)
{
    EGLint attribList[] =
    {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
    };

    EGLint numConfigs;
    EGLConfig config;
    EGLint majorVersion, minorVersion;

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("Failed to get EGL display");
        return;
    }

    eglInitialize(display, &majorVersion, &minorVersion);
    if (!eglChooseConfig(display, attribList, &config, 1, &numConfigs)) {
        LOGE("Failed to choose EGL config");
        return;
    }

    surface = eglCreateWindowSurface(display, config, window, NULL);
    if (surface == EGL_NO_SURFACE)
    {
        LOGE("Error creating EGL surface");
        return;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);

    const EGLint context_attrib_list[] = {
            // request a context using Open GL ES 2.0
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attrib_list);
    if (context == EGL_NO_CONTEXT)
    {
        LOGE("Error creating EGL context");
        return;
    }

    if (!eglMakeCurrent(display, surface, surface, context))
    {
        LOGE("Error making EGL context current");
        return;
    }
}

void initRender(AAssetManager* assetManager)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        LOGE("OpenGL error: 0x%x", error);
        return;
    }

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    if (glGetError() != GL_NO_ERROR)
    {
        LOGE("Failed to get OpenGL viewport");
        return;
    }

    const GLubyte* renderer = glGetString(GL_RENDERER);
    if (glGetError() != GL_NO_ERROR) {
        LOGE("Failed to get OpenGL renderer");
        return;
    }

    auto readShader = [assetManager](const char* path)
    {
        AAsset* asset = AAssetManager_open(assetManager, path, AASSET_MODE_BUFFER);
        off_t length = AAsset_getLength(asset);
        char* buffer = new char[length + 1];
        AAsset_read(asset, buffer, length);
        buffer[length] = '\0';
        std::string shaderSource(buffer);
        delete[] buffer;
        AAsset_close(asset);

        return shaderSource;
    };

    shaderProgram = std::make_unique<ShaderProgram>(readShader("vertexShader.hlsl").c_str(), readShader("fragmentShader.hlsl").c_str());
    shaderProgram->use();

    projection = glm::perspective(glm::radians(45.0), (double)width / (double)height, 0.1, 100.0);
    view = glm::lookAt(cameraPos, glm::vec3(0,0,0), glm::vec3(0, 1, 0));

    shaderProgram->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    shaderProgram->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    shaderProgram->setVec3("lightPos", cameraPos);
    shaderProgram->setVec3("viewPos", cameraPos);

    shaderProgram->setMatrix("view",view);
    shaderProgram->setMatrix("projection", projection);

    cube = std::make_unique<Cube>(shaderProgram->ID);
}

void render()
{
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    shaderProgram->use();



    shaderProgram->setMatrix("model", model);

    cube->draw(shaderProgram->ID);

    if (!eglSwapBuffers(display, surface))
    {
        EGLint error = eglGetError();
        LOGE("eglSwapBuffers error: %d", error);
    }
}

glm::vec2 prevPos;

int32_t handleInput(struct android_app* app, AInputEvent* event)
{
    int32_t eventType = AInputEvent_getType(event);
    switch(eventType){
        case AINPUT_EVENT_TYPE_MOTION:
            switch(AInputEvent_getSource(event))
            {
                case AINPUT_SOURCE_TOUCHSCREEN:
                    int action = AKeyEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
                    switch(action)
                    {
                        case AMOTION_EVENT_ACTION_DOWN:
                            prevPos = {AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};
                            break;
                        case AMOTION_EVENT_ACTION_MOVE:
                            glm::vec2 currPos = {AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};
                            auto direction = (currPos - prevPos) * 0.1f;

                            glm::mat4 transform = glm::mat4 (1.0f);
                            transform = glm::rotate(transform, glm::radians(direction.x), glm::vec3(0.0f, 1.0f, 0.0f));
                            transform = glm::rotate(transform, glm::radians(-direction.y), glm::vec3(1.0f, 0.0f, 0.0f));

                            model = transform * model;

                            prevPos = currPos;
                            break;
                    }
                    break;
            }
            break;
    }

    return 0;
}

bool isInitializing = true;

void android_main(struct android_app* application)
{
    LOGI("%s", glGetString(GL_VERSION));

    application->onInputEvent = handleInput;
    application->onAppCmd = [](android_app *app, int32_t cmd)
    {
        if (cmd == APP_CMD_INIT_WINDOW)
            isInitializing = false;
    };

    while (isInitializing)
    {
        int events;
        struct android_poll_source* source;

        while ((ALooper_pollAll(0, NULL, &events, (void **) &source)) >= 0)
        {
            if (source != NULL)
            {
                source->process(application, source);
            }
        }
    }

    initEGL(application->window);
    initRender(application->activity->assetManager);

    bool isRunning = true;
    while (isRunning)
    {
        int events;
        struct android_poll_source* source;

        render();

        while ((ALooper_pollAll(0, NULL, &events, (void **) &source)) >= 0)
        {
            if (source != NULL)
            {
                source->process(application, source);
            }

            if (application->destroyRequested != 0)
            {
                LOGI("android_main: destroy requested");
                eglDestroySurface(display, surface);
                eglTerminate(display);
                isRunning = false;
            }
        }
    }
}
