#include <thread>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "Call_Me.h"
#include "Hook.h"
#include "Zygisk.hpp"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

class MyModule : public zygisk::ModuleBase {
public:
	void onLoad(Api *api, JNIEnv *env) override {
		this->api = api;
		this->env = env;
	}

	void preAppSpecialize(AppSpecializeArgs *args) override {
		auto package_name = env->GetStringUTFChars(args->nice_name, nullptr);
		auto app_data_dir = env->GetStringUTFChars(args->app_data_dir, nullptr);
		preSpecialize(package_name, app_data_dir);
		env->ReleaseStringUTFChars(args->nice_name, package_name);
		env->ReleaseStringUTFChars(args->app_data_dir, app_data_dir);
	}

	void postAppSpecialize(const AppSpecializeArgs *) override {
		if (enable_hack) {
			std::thread hack_thread(hack_prepare, game_data_dir, data, length);
			hack_thread.detach();
		}
	}

private:
    Api *api = nullptr;
    JNIEnv *env = nullptr;
    bool enable_hack = false;
    char *game_data_dir = nullptr;
    void *data = nullptr;
    size_t length = 0;

    void preSpecialize(const char *package_name, const char *app_data_dir) {
        if (strcmp(package_name, "com.Gaggle.fun.GooseGooseDuck") == 0) {

            LOGI("detect game: %s", package_name);
            enable_hack = true;
            game_data_dir = strdup(app_data_dir);

#if defined(__i386__)
            const char *path = "zygisk/x86.so";
#elif defined(__x86_64__)
            const char *path = "zygisk/x86_64.so";
#elif defined(__arm__)
            const char *path = "zygisk/armeabi-v7a.so";
#elif defined(__aarch64__)
            const char *path = "zygisk/arm64-v8a.so";
#endif

            int dirfd = api->getModuleDir();
            int fd = openat(dirfd, path, O_RDONLY);
            if (fd != -1) {
                struct stat sb{};
                if (fstat(fd, &sb) == 0) {
                    length = sb.st_size;
                    data = mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
                    if (data == MAP_FAILED) {
                        LOGW("mmap failed");
                        data = nullptr;
                    }
                }
                close(fd);
            } else {
                LOGW("Unable to open so file: %s", path);
            }
        } else {
            api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
        }
    }
};

REGISTER_ZYGISK_MODULE(MyModule)

EGLBoolean (*orig_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
	eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
	eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);
	
	static bool setup = false;
	static bool menu = true;
	
	if (!setup)
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		SetDarkBlueUITheme();
		
		io.Fonts->AddFontFromMemoryTTF(Inter, sizeof(Inter), 28.f, NULL, io.Fonts->GetGlyphRangesVietnamese());
		interbold = io.Fonts->AddFontFromMemoryTTF(InterBold, sizeof(InterBold), 28.f, NULL, io.Fonts->GetGlyphRangesVietnamese());
		
		io.KeyMap[ImGuiKey_UpArrow] = 19;
		io.KeyMap[ImGuiKey_DownArrow] = 20;
		io.KeyMap[ImGuiKey_LeftArrow] = 21;
		io.KeyMap[ImGuiKey_RightArrow] = 22;
		io.KeyMap[ImGuiKey_Enter] = 66;
		io.KeyMap[ImGuiKey_Backspace] = 67;
		io.KeyMap[ImGuiKey_Escape] = 111;
		io.KeyMap[ImGuiKey_Delete] = 112;
		io.KeyMap[ImGuiKey_Home] = 122;
		io.KeyMap[ImGuiKey_End] = 123;
		
		ImGui_ImplOpenGL3_Init("#version 300 es");
		ImGui::GetStyle().ScaleAllSizes(3.0f);
		loadImages();
		setup = true;
	}
	
	ImGuiIO& io = ImGui::GetIO();
	static bool WantTextInputLast = false;
	if (io.WantTextInput && !WantTextInputLast) ShowSoftKeyboardInput();
	WantTextInputLast = io.WantTextInput;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplAndroid_NewFrame(glWidth, glHeight);
	ImGui::NewFrame();
	
	ui_dpi_scale = ImLerp(ui_dpi_scale , menu ? 1.1f : 0.4f, 0.5f * (10.0f * ImGui::GetIO().DeltaTime));
	
	if (ui_dpi_scale <= 1.0f && ui_dpi_scale >= 0.5f) window_flags = menu ? ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar : ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
	if (ui_dpi_scale < 0.5f) window_flags = menu ? ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar : ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;
	
	if (ui_dpi_scale <= 1.0f && ui_dpi_scale >= 0.5f) ImGui::SetNextWindowSize(ImVec2(1280 * ui_dpi_scale, 640 * ui_dpi_scale));
	if (ui_dpi_scale < 0.5f) ImGui::SetNextWindowSize(ImVec2(700 * ui_dpi_scale - 100, 600 * ui_dpi_scale - 80));

	if (ImGui::Begin(OBFUSCATE("Menu"), nullptr, window_flags))
	{
		ImGui::Columns(2);
		ImGui::SetColumnOffset(1, 170);
		
		CircleImage(Logo.textureId, 120);
		if (ImGui::IsItemHovered() && IsClickOrHold() == 0) menu = !menu;
		
		static ImVec4 active = to_vec4(0, 150, 255, 255);
		static ImVec4 inactive = to_vec4(0, 0, 0, 0);
        static int Tab = 2;

        ImGui::Spacing();
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.6f);

        ImGui::PushStyleColor(ImGuiCol_Button, Tab == 1 ? active : inactive);
        if (ImGui::Button(oxorany("A"), ImVec2(120, 50))) Tab = 1;
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Button, Tab == 2 ? active : inactive);
        if (ImGui::Button(oxorany("Log"), ImVec2(120, 50))) Tab = 2;
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        ImGui::NextColumn();
        ImVec2 contentRegion = ImGui::GetContentRegionAvail();
        ImVec2 textSize = ImGui::CalcTextSize(window_name);
        float cursorX = ImGui::GetCursorPosX();
        float textPosX = cursorX + (contentRegion.x - textSize.x) / 2.0f;
        ImGui::PushFont(interbold);
        if (ui_dpi_scale > 0.5f) ImGui::SetCursorPosX(textPosX);
        ImGui::Text(window_name);
        ImGui::PopFont();

        if (Tab == 1) {
            ImGui::Text(oxorany("Zoom Multiplie"));
            ImGui::SliderInt(oxorany("##PZoom"), &zoom, 1, 100);

            ImGui::Text(oxorany("Speed Multiplie"));
            ImGui::SliderInt(oxorany("##PSpeed"), &speed, 5, 20);

            ImGui::Checkbox(oxorany("NoFog"), &noFog);
            ImGui::Checkbox(oxorany("NoClip"), &noClip);
            ImGui::Checkbox(oxorany("NoCD"), &noCoolDown);

            if (ImGui::Button(oxorany("Call Emergency"))) {
                callBell = true;
            }
        }

        if (Tab == 2) {
            ImGui::Text(oxorany("Debug Logs:"));
            ImGui::BeginChild("DebugRegion", ImVec2(0, 400), true);
            for (auto& line : GetDebugLogs()) {
                ImGui::TextUnformatted(line.c_str());
            }
            ImGui::EndChild();
        }

        ImGui::End();
	}
	
	ImGui::Render();
	ImGui::EndFrame();
	
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &glWidth);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &glHeight);
	
	io.KeysDown[io.KeyMap[ImGuiKey_UpArrow]] = false;
	io.KeysDown[io.KeyMap[ImGuiKey_DownArrow]] = false;
	io.KeysDown[io.KeyMap[ImGuiKey_LeftArrow]] = false;
	io.KeysDown[io.KeyMap[ImGuiKey_RightArrow]] = false;
	io.KeysDown[io.KeyMap[ImGuiKey_Enter]] = false;
	io.KeysDown[io.KeyMap[ImGuiKey_Backspace]] = false;
	io.KeysDown[io.KeyMap[ImGuiKey_Delete]] = false;
	io.KeysDown[io.KeyMap[ImGuiKey_Escape]] = false;
	io.KeysDown[io.KeyMap[ImGuiKey_Home]] = false;
	io.KeysDown[io.KeyMap[ImGuiKey_End]] = false;
	return orig_eglSwapBuffers(dpy, surface);
}

uintptr_t il2cppBase = 0;
void *Init_thread()
{
    sleep(1);
	Tools::Hook((void *) DobbySymbolResolver(OBFUSCATE("/system/lib/libandroid.so"), OBFUSCATE("ANativeWindow_getWidth")), (void *) _ANativeWindow_getWidth, (void **) &orig_ANativeWindow_getWidth);
	Tools::Hook((void *) DobbySymbolResolver(OBFUSCATE("/system/lib/libandroid.so"), OBFUSCATE("ANativeWindow_getHeight")), (void *) _ANativeWindow_getHeight, (void **) &orig_ANativeWindow_getHeight);

    while (il2cppBase == 0) {
        il2cppBase = KittyMemory::findIl2cppBase();
        if (!il2cppBase) {
            AddDebugLog("Waiting for libil2cpp.so to load...");
            sleep(1);
        }
    }
    AddDebugLog("libil2cpp.so loaded at %p", (void*)il2cppBase);
    sleep(1);
    Tools::Hook((void *) DobbySymbolResolver("/system/lib/libEGL.so", "eglSwapBuffers"), (void *) hook_eglSwapBuffers, (void **) &orig_eglSwapBuffers);

	Attach();
	PollUnicodeChars();
    TouchInput::Init();

    playerCollider = GetFieldOffset(oxorany("Assembly-CSharp.dll"), oxorany("Handlers.GameHandlers.PlayerHandlers"), oxorany("PlayableEntity") , oxorany("playerCollider"));

    Tools::Hook((void *) (uintptr_t) GetMethodOffset(oxorany("Assembly-CSharp.dll"), oxorany("Handlers.GameHandlers.SpecialBehaviour"), oxorany("FogOfWar") , oxorany("FixedUpdate"), 0), (void *) FogUpdate , (void **) &_FogUpdate);
    Tools::Hook((void *) (uintptr_t) GetMethodOffset(oxorany("Assembly-CSharp.dll"), oxorany("Handlers.GameHandlers.PlayerHandlers"), oxorany("LocalPlayer") , oxorany("FixedUpdate"), 0), (void *) PlayerUpdate , (void **) &_PlayerUpdate);
    Tools::Hook((void *) (uintptr_t) GetMethodOffset(oxorany("Assembly-CSharp.dll"), oxorany(""), oxorany("UICooldownButton") , oxorany("set_Cooldown"), 1), (void *) set_Cooldown , (void **) &_set_Cooldown);
    Tools::Hook((void *) (uintptr_t) GetMethodOffset(oxorany("Assembly-CSharp.dll"), oxorany("Handlers.GameHandlers.PlayerHandlers"), oxorany("PlayerController") , oxorany("Update"), 0), (void *) CallUpdate , (void **) &_CallUpdate);

    return nullptr;
}
