#include "GUICollection.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include <array>
#include <iostream>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// Enable/Disable use of GUICollection interface instead of raw ImGui
#define USE_GUICOLLECTION 1

// Our state
bool show_another_window = false;
bool show_demo_window = true;
gui::Vec4 clear_color(0.45f, 0.55f, 0.60f, 1.00f);

#if USE_GUICOLLECTION
class HelloWorldWindow : public gui::Window
{
private:
    float f = 0.0f;
    int counter = 0;
public:
    HelloWorldWindow()
        : Window{ "Hello, world!", gui::Vec2{ 400, 300 } } // Define a window called "Hello, world!" with the specified dimensions.
    {
    }

    void DrawContents() override
    {
        gui::Text("This is some useful text.");                 // Display some text (you can use a format strings too)
        gui::Checkbox("Demo Window", &show_demo_window);        // Edit bools storing our window open/close state
        gui::Checkbox("Another Window", &show_another_window);

        gui::VarSlider("float", &f, 0.0f, 1.0f);                   // Edit 1 float using a slider from 0.0f to 1.0f
        gui::VarColor("clear color", &clear_color);             // Edit 4 floats representing a color

        if (gui::Button("Button"))                              // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        gui::SameLine();
#ifdef GUI_FEATURE_CPP20_FORMAT
        gui::Text("counter = {}", counter);
#else
        gui::Text("counter = %d", counter);
#endif

        {
            // Cycle the color of the framerate text depending on the counter
            gui::Vec4 framerateColor(
                static_cast<float>(counter % 3 == 0),
                static_cast<float>(counter % 3 == 1),
                static_cast<float>(counter % 3 == 2),
                1.0f
            );
            // Apply the color to text only if the counter has been incremented
            gui::SetStyleColor textColor(gui::FLAG_STYLE_COLOR::TEXT, framerateColor, counter > 0);

            // Notice the color applying to the framerate text once the button has been pressed
#ifndef IMGUI_DISABLE
            ImGuiIO& io = ImGui::GetIO();
            static float highestFPS{};
            if (io.Framerate > highestFPS)
                highestFPS = io.Framerate;
#ifdef GUI_FEATURE_CPP20_FORMAT
            gui::Text("Application average {:.3f} ms/frame ({:.1f} FPS) (highest {:.1f} FPS)", 1000.0f / io.Framerate, io.Framerate, highestFPS);
#else
            gui::Text("Application average %.3f ms/frame (%.1f FPS) (highest {%.1f} FPS)", 1000.0f / io.Framerate, io.Framerate, highestFPS);
#endif
#endif
        }

        // Notice the color not applying anymore to this text, because the SetStyleColor object went out of scope. Indentation also helps with seeing which styles apply.
        gui::Text("Always black text!");
    }
};

void DrawHelloWorldWindow()
{
    static HelloWorldWindow window; // Allocate storage for the Hello World window
    window.Draw();                  // Draw the window and its contents
}
#else
void DrawHelloWorldWindow()
{
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    // Cycle the color of the framerate text depending on the counter
    ImVec4 framerateColor(
        static_cast<float>(counter % 3 == 0),
        static_cast<float>(counter % 3 == 1),
        static_cast<float>(counter % 3 == 2),
        1.0f
    );
    // Apply the color to text only if the counter has been incremented
    if (counter > 0)
        ImGui::PushStyleColor(ImGuiCol_Text, framerateColor);

    // Notice the color applying to the framerate text once the button has been pressed
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    if (counter > 0)
        ImGui::PopStyleColor(); // ImGuiCol_Text

    // You can imagine it's pretty hard to tell what styles apply to this text if more style pushes and pops happen above
    gui::Text("Always black text!");

    ImGui::End();
}
#endif

#if USE_GUICOLLECTION
class AnotherWindow : public gui::Window
{
private:
    std::string name = "Name";
    int profession = 1;
    int age = 24;
    std::vector<std::string> thingsILike = { "Gaming", "Sleeping" };
public:
    AnotherWindow()
        : Window{ "Another Window", gui::Vec2{ 300, 250 } }
    {
    }

    void DrawContents() override
    {
        gui::Text("Hello from another window!");
        gui::Separator();

        // Text manipulation
        if (gui::TextBox("Name", &name, gui::FLAG_INPUT_TEXT::ENTER_RETURNS_TRUE))
            std::cout << "Hello " << name << "!\n";

        // Render a read-only text box that can accept a number payload
        gui::TextBoxReadOnly("Age", std::to_string(age));
        gui::PayloadTarget<int>("NUMBER", [&age = age](int number) { age = number; std::cout << "Happy birthday! x" << number << '\n'; });
        gui::Spacing();

        // Render a rectangular area that can accept a number payload
        gui::PayloadTargetRect<int>("NUMBER", gui::Vec2{ gui::GetAvailableContentRegion().x * 0.67f, 30 }, "Drag a number here!", [](int number) { std::cout << "Lucky number " << number << "!\n"; });

        // Demonstration of 1 way to render a dropdown selection menu
        bool isIneligible = (age < 18);
        {
            gui::Disabled disabledIfIneligible{ isIneligible };
            std::array<const char*, 3> professions{ "Programmer", "Engineer", "Scientist" };
            if (gui::Combo{ "Job", professions, &profession })
                std::cout << "Job changed to " << professions[profession] << '\n';
        }
        if (isIneligible)
        {
            gui::SetStyleColor textColor{ gui::FLAG_STYLE_COLOR::TEXT, gui::Vec4{ 1.0f, 0.0f, 0.0f, 1.0f } };
            gui::Text("Ineligible for a job due to age restrictions");
        }

        // Render some numbered buttons that can be dragged into the age boxes above
        for (int i{ 2 }; i <= 64; i *= 2)
        {
            gui::Button(std::to_string(i), gui::Vec2{ 30, 30 });
            if (gui::PayloadSource payloadSource{ "NUMBER", i })
                gui::Text("Dragging the number {}", i); // Drawing anything while a PayloadSource object is still alive will instead draw to the cursor tooltip
            if (i < 64)
                gui::SameLine();
        }

        // Demonstration of drawing a std::vector with resize options
        gui::VarContainer("I Like", &thingsILike, [](std::string& thing) {
            // Demonstration of renaming an item that is not a text box (in this case, a selectable)
            // Right click the selectable and select "Rename" to start renaming` it!
            gui::Renamable::Wrap(thing,
                [&thing]() { gui::Selectable(thing); },
                [&thing](const char* input) { thing = input; }
            );
        });

        if (gui::Button("Close Me"))
            show_another_window = false;
    }
};

void DrawAnotherWindow()
{
    static AnotherWindow anotherWindow;           // Allocate storage for another window
    anotherWindow.SetIsOpen(show_another_window); // Show or not show the window accordingly (can also be managed by the window class itself)
    anotherWindow.Draw();                         // Ignores draw call if the internal flag is set to closed
}
#else
void DrawAnotherWindow()
{
    if (!show_another_window)
        return;
    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    ImGui::Separator();

    // Text manipulation
    static std::string name = "Name";
    if (ImGui::InputText("Name", &name, ImGuiInputTextFlags_EnterReturnsTrue))
        std::cout << "Hello " << name << "!\n";

    // Render a read-only text box that can accept a number payload
    static int age = 24;
    std::string ageStr = std::to_string(age);
    ImGui::InputText("Age", &ageStr, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget())
    {
        if (auto payload = ImGui::AcceptDragDropPayload("NUMBER"))
        {
            age = *reinterpret_cast<int*>(payload->Data);
            std::cout << "Happy birthday! x" << age << '\n';
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::Spacing();

    // Render a rectangular area that can accept a number payload
    ImVec2 topLeft = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2{ 250, 30 };
    const char* text = "Drag a number here!";
    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, topLeft + size, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg)), 5.0f);
    ImGui::InvisibleButton("##PayloadTarget", size);
    if (ImGui::BeginDragDropTarget())
    {
        if (auto payload = ImGui::AcceptDragDropPayload("NUMBER"))
        {
            age = *reinterpret_cast<int*>(payload->Data);
            std::cout << "Happy birthday! x" << age << '\n';
        }
        ImGui::EndDragDropTarget();
    }
    ImVec2 textSize = ImGui::CalcTextSize(text);
    ImGui::GetWindowDrawList()->AddText(topLeft + (size - textSize) * 0.5f, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), text);

    // Render a dropdown selection menu
    static int profession = 1;
    const char* professions[]{ "Programmer", "Engineer", "Scientist" };
    bool isIneligible = (age < 18);
    if (isIneligible)
        ImGui::BeginDisabled();
    if (ImGui::Combo("Job", &profession, professions, 3))
        std::cout << "Job changed to " << professions[profession] << '\n';
    if (isIneligible)
    {
        ImGui::EndDisabled();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f });
        ImGui::Text("Ineligible for a job due to age restrictions");
        ImGui::PopStyleColor();
    }

    // Render some numbered buttons that can be dragged into the age boxes above
    for (int i{ 2 }; i <= 64; i *= 2)
    {
        ImGui::Button(std::to_string(i).c_str(), gui::Vec2{ 30, 30 });
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("NUMBER", &i, sizeof(int));
            ImGui::Text("Dragging the number %d", i);
            ImGui::EndDragDropSource();
        }
        if (i < 64)
            ImGui::SameLine();
    }

    // Demonstration of renaming an item that is not a text box (in this case, a selectable)
    // Right click the selectable and select "Rename" to start renaming it!
    // (not gonna do the std::vector thing above cause I don't have the energy for that lmao)
    static bool isRenamingSelectable = false;
    static bool stealFocus = false;
    static std::string thinkingAbout = "Gaming";
    if (isRenamingSelectable)
    {
        ImGui::PushID("Rename");
        ImGui::SetNextItemWidth(-1.0f);
        if (stealFocus)
            ImGui::SetKeyboardFocusHere();

        if (ImGui::InputText("", &thinkingAbout, ImGuiInputTextFlags_EnterReturnsTrue))
            isRenamingSelectable = false;

        if (stealFocus)
        {
            ImGui::NavMoveRequestApplyResult();
            stealFocus = false;
        }
        if (!ImGui::IsItemFocused() || ImGui::IsKeyPressed(ImGuiKey_Escape))
            isRenamingSelectable = false;
        ImGui::PopID();
    }
    else
    {
        ImGui::Selectable(thinkingAbout.c_str());
        if (ImGui::BeginPopupContextItem("RenameContextMenu"))
        {
            if (ImGui::MenuItem("Rename"))
                isRenamingSelectable = stealFocus = true;
            ImGui::EndPopup();
        }
    }

    if (ImGui::Button("Close Me"))
        show_another_window = false;
    ImGui::End();
}
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
#ifndef IMGUI_DISABLE
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
#endif

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
#ifndef IMGUI_DISABLE
            ImGui_ImplGlfw_Sleep(10);
#endif
            continue;
        }

        // Start the Dear ImGui frame
#ifndef IMGUI_DISABLE
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
#endif

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        DrawHelloWorldWindow();

        // 3. Show another simple window.
        DrawAnotherWindow();

        // 4. Notifications
#ifdef GUI_EXTENSION_NOTIFICATIONS
        static int v{};
        if (gui::IsKeyPressed(gui::KEY::ENTER))
        {
            gui::Notification(gui::FLAG_NOTIF::INFO, "Very long super duper long this is kinda dumb Test Notif %d", ++v);
            if (v > 7)
                v = 0;
        }
        gui::RenderNotifications();
#endif

        // Rendering
#ifndef IMGUI_DISABLE
        ImGui::Render();
#endif
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
#ifndef IMGUI_DISABLE
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
#endif

        glfwSwapBuffers(window);
    }

    // Cleanup
#ifndef IMGUI_DISABLE
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
