#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>
using namespace std;

class Window {
public:
    virtual void draw() = 0;
    virtual ~Window() = default;
};

class Button {
public:
    virtual void draw() = 0;
    virtual ~Button() = default;
};

class Shader {
public:
    virtual void compile() = 0;
    virtual ~Shader() = default;
};

class OpenGLWindow: public Window {
public:
    void draw() override {
        cout << "OpenGL: dibujando ventana" << endl;
    }
};

class OpenGLButton: public Button {
public:
    void draw() override {
        cout << "OpenGL: dibujando boton" << endl;
    }
};

class OpenGLShader: public Shader {
public:
    void compile() override {
        cout << "OpenGL: compilando shader GLSL" << endl;
    }
};

class VulkanWindow: public Window {
public:
    void draw() override {
        cout << "Vulkan: dibujando ventana con command buffers" << endl;
    }
};

class VulkanButton: public Button {
public:
    void draw() override {
        cout << "Vulkan: dibujando boton con pipeline grafico" << endl;
    }
};

class VulkanShader: public Shader {
public:
    void compile() override {
        cout << "Vulkan: compilando shader a SPIR-V" << endl;
    }
};

class RenderFactory {
public:
    virtual unique_ptr<Window> createWindow() = 0;
    virtual unique_ptr<Button> createButton() = 0;
    virtual unique_ptr<Shader> createShader() = 0;
    virtual ~RenderFactory() = default;
};

class OpenGLFactory: public RenderFactory {
public:
    unique_ptr<Window> createWindow() override {
        return make_unique<OpenGLWindow>();
    }

    unique_ptr<Button> createButton() override {
        return make_unique<OpenGLButton>();
    }

    unique_ptr<Shader> createShader() override {
        return make_unique<OpenGLShader>();
    }
};

class VulkanFactory: public RenderFactory {
public:
    unique_ptr<Window> createWindow() override {
        return make_unique<VulkanWindow>();
    }

    unique_ptr<Button> createButton() override {
        return make_unique<VulkanButton>();
    }

    unique_ptr<Shader> createShader() override {
        return make_unique<VulkanShader>();
    }
};

class App {
    unique_ptr<RenderFactory> factory;
public:
    App(unique_ptr<RenderFactory> factory) {
        cambiarFactory(move(factory));
    }

    void cambiarFactory(unique_ptr<RenderFactory> nueva) {
        if (nueva == nullptr)
            throw invalid_argument("Fabrica nula");

        factory = move(nueva);
    }

    void ejecutar() {
        auto window = factory->createWindow();
        auto button = factory->createButton();
        auto shader = factory->createShader();

        window->draw();
        button->draw();
        shader->compile();
    }
};

int main() {
    App app(make_unique<OpenGLFactory>());
    app.ejecutar();

    cout << "\nCambio de plataforma" << endl;

    app.cambiarFactory(make_unique<VulkanFactory>());
    app.ejecutar();
}
