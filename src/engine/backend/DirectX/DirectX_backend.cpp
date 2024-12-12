
// Implements:
#include <engine/core/backend_hook.hpp>

// Dependencies:
#include <engine/core/engine.hpp>

// Dependencies (3rd_party):
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
/*
    On hold until Raylib backend is fully implemented: now implementing 3D scene features.
*/
namespace jbx {

    // DirectX backend context
    struct Engine_Context {
        HWND                    window;
        IDXGISwapChain*         swap_chain;
        ID3D11Device*           device;
        ID3D11DeviceContext*    immediate_context;
        ID3D11RenderTargetView* render_target_view;
        s32x2                   size;
        f32x4                   clear_color;
        bool                    should_run;

        Engine_Context()
        :   should_run(true),
            clear_color(0.2f, 0.2f, 0.2f, 1.0f),
            window(nullptr),
            swap_chain(nullptr),
            device(nullptr),
            immediate_context(nullptr),
            render_target_view(nullptr) {
        }
    };

    /*
        Internal helper functions to create the main window:
    */

    static inline LRESULT
    main_window_procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    static inline HWND
    create_main_window(HINSTANCE hInstance, int nCmdShow) {
        Unique<Engine_Config>& config = get_context<Engine_Config>();

        // Setup the window class:
        WNDCLASSEX window_class;
        window_class.cbSize         = sizeof(WNDCLASSEX);
        window_class.style          = CS_HREDRAW | CS_VREDRAW;
        window_class.lpfnWndProc    = main_window_procedure;
        window_class.cbClsExtra     = 0;
        window_class.cbWndExtra     = 0;
        window_class.hInstance      = hInstance;
        window_class.hIcon          = NULL;
        window_class.hCursor        = LoadCursor(NULL, IDC_ARROW);
        window_class.hbrBackground  = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        window_class.lpszMenuName   = NULL;
        window_class.lpszClassName  = "Custom_DX_Window";
        window_class.hIconSm        = NULL;

        HRESULT result = RegisterClassEx(&window_class);
        ERROR_IF(result == 0, "Failed to register window class!");

        // Create the window
        HWND window_handle = CreateWindow(
            window_class.lpszClassName,
            config->window_title.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            config->window_width,
            config->window_height,
            NULL,
            NULL,
            window_class.hInstance,
            NULL
        );

        ERROR_IF(!window_handle, "Unable to create a window handle!");

        ShowWindow(window_handle, nCmdShow);
        UpdateWindow(window_handle);

        return window_handle;
    }

    void
    initialize_and_start_backend() {
        Unique<Engine_Config>&  config  = get_context<Engine_Config>();
        Unique<Engine_Context>& context = get_context<Engine_Context>();

        context->window = create_main_window(config->instance, config->cmd_show);

        DXGI_SWAP_CHAIN_DESC sd               = {};
        sd.BufferCount                        = 1;
        sd.BufferDesc.Width                   = config->window_width;
        sd.BufferDesc.Height                  = config->window_height;
        sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator   = config->desired_framerate;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow                       = context->window;
        sd.SampleDesc.Count                   = 1;
        sd.SampleDesc.Quality                 = 0;
        sd.Windowed                           = TRUE;

        HRESULT result = D3D11CreateDeviceAndSwapChain(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            0,
            NULL,
            0,
            D3D11_SDK_VERSION,
            &sd,
            &context->swap_chain,
            &context->device,
            NULL,
            &context->immediate_context
        );

        ERROR_IF(FAILED(result), "Failed to Create D3D11 Device!");

        // Create the render target view
        ID3D11Texture2D* pBackBuffer = nullptr;
        result = context->swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        ERROR_IF(FAILED(result), "Failed to get the swap chain buffer!");

        result = context->device->CreateRenderTargetView(pBackBuffer, NULL, &context->render_target_view);
        ERROR_IF(FAILED(result), "Failed to create render target!");

        pBackBuffer->Release();
        context->immediate_context->OMSetRenderTargets(1, &context->render_target_view, NULL);

        // Setup the viewport
        D3D11_VIEWPORT vp;
        vp.Width       = static_cast<float>(config->window_width);
        vp.Height      = static_cast<float>(config->window_height);
        vp.MinDepth    = 0.0f;
        vp.MaxDepth    = 1.0f;
        vp.TopLeftX    = 0.0f;
        vp.TopLeftY    = 0.0f;

        context->immediate_context->RSSetViewports(1, &vp);

        // Main loop:
        MSG msg = { 0 };
        while (context->should_run) {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    context->should_run = false;
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                context->immediate_context->ClearRenderTargetView(
                    context->render_target_view,
                    &(context->clear_color.x)
                );

                context->swap_chain->Present(1, 0);
            }
        }
    }


    void
    set_clear_color(const u8x4& rgba_color) {
        log_warn("{ set_clear_color } not implemented!");
    }

    bool
    is_key_pressed(Keyboard_Key key) {
        log_warn("{ is_key_pressed } not implemented!");
        return true;
    }

    void
    draw_rect(const Rect& rect, const Color& fill_color) {
        log_warn("{ draw_rect } not implemented!");
    }

    Texture
    load_texture(const std::string& texture_file_name) {
        log_warn("{ load_texture } not implemented!");
        return Texture(
            static_cast<int>(0),
            f32x4(0, 0, 42, 42)
        );
    }

    void
    draw_texture(Texture& texture, const Rect& entity_rect) {
        log_warn("{ draw_texture } not implemented!");
    }

    Sound
    load_sound(const std::string& sound_file_name, f32 volume, f32 pitch) {
        log_warn("{ load_sound } not implemented!");
        return Sound(
            0,
            1.0f,
            1.0f
        );
    }

    void
    play_sound(Sound& sound) {
        log_warn("{ play_sound } not implemented!");
    }

    Font
    load_font(const std::string& font_file_name, int font_size) {
        log_warn("{ load_font } not implemented!");
        return Font(0);
    }

    void
    draw_text(const Text& text, f32x2 position) {
        log_warn("{ draw_text } not implemented!");
    }

} // jbx
