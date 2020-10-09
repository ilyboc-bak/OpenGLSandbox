#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_opengl3.h"
#include "IMGUI/imgui_impl_glfw.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // or COMPAT

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "ERROR!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    {
        float positions[] = {
            -2.0f, -1.5f, 0.0f, 0.0f, // bottom left
             2.0f, -1.5f, 1.0f, 0.0f, // bottom right
             2.0f,  1.5f, 1.0f, 1.0f, // top right
            -2.0f,  1.5f, 0.0f, 1.0f  // top left 
        };

        unsigned int indices[6] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);
        IndexBuffer ib(indices, 6);

        glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0, 0));

        Shader shader("res/shaders/BasicTexture.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

        Texture texture("res/textures/Donut.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
        ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        float r = 0.0f;
        float increment = 0.05f;
        glm::vec3 translationA(0.5f, 0, 0);
        glm::vec3 translationB(0.5f, 0, 0);
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);

				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				//shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va, ib, shader);
                GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
            }
			
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);

				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				//shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va, ib, shader);
				GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
			}
            

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = +0.05f;

            r += increment;

            // legacy openGL
            /*glBegin(GL_TRIANGLES);
            glVertex2f(-0.5f, -0.5f);
            glVertex2f( 0.0f,  0.5f);
            glVertex2f( 0.5f, -0.5f);
            glEnd();*/

			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::SliderFloat3("TranslationA", &translationA.x, -1.5f, 1.5f);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::SliderFloat3("TranslationB", &translationB.x, -1.5f, 1.5f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}