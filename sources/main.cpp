#include <iostream>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl2.h>

int main ()
{
    // Initialize SDL
    if ( SDL_Init( SDL_INIT_VIDEO ) != 0 )
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,
                         SDL_GL_CONTEXT_PROFILE_CORE );

    // Create window
    SDL_Window * window =
        SDL_CreateWindow( "ImGui SDL OpenGL", 100, 100, 640, 480,
                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
    if ( ! window )
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext( window );
    if ( ! glContext )
    {
        std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError()
                  << std::endl;
        SDL_DestroyWindow( window );
        SDL_Quit();
        return 1;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO & io = ImGui::GetIO();
    ( void )io;
    ImGui_ImplSDL2_InitForOpenGL( window, glContext );
    ImGui_ImplOpenGL3_Init( "#version 330" );

    // Main loop
    bool quit = false;
    while ( ! quit )
    {
        // Handle events
        SDL_Event event;
        while ( SDL_PollEvent( &event ) )
        {
            ImGui_ImplSDL2_ProcessEvent( &event );

            if ( event.type == SDL_QUIT )
            {
                quit = true;
            }
        }

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame( window );
        ImGui::NewFrame();

        // Create ImGui window
        ImGui::Begin( "Hello, world!" );

        // Add some text
        ImGui::Text( "This is some text!" );

        // End ImGui window
        ImGui::End();

        // Render ImGui frame
        glClear( GL_COLOR_BUFFER_BIT );
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
        SDL_GL_SwapWindow( window );
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext( glContext );
    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
