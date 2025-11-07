#include <glad/glad.h>
#include <stdlib.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

static HMODULE libgl;

static void* get_proc(const char *namez) {
    void* p = (void*)wglGetProcAddress(namez);
    if(p == 0 ||
        (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
        (p == (void*)-1) )
    {
        p = (void*)GetProcAddress(libgl, namez);
    }
    return p;
}
#endif

// Function pointers
PFNGLCLEARCOLORPROC glad_glClearColor = NULL;
PFNGLCLEARPROC glad_glClear = NULL;
PFNGLGETSTRINGPROC glad_glGetString = NULL;
PFNGLVIEWPORTPROC glad_glViewport = NULL;

int gladLoadGLLoader(GLADloadproc load) {
#if defined(_WIN32)
    libgl = LoadLibraryA("opengl32.dll");
    if(libgl == NULL) return 0;
    
    glad_glClearColor = (PFNGLCLEARCOLORPROC)get_proc("glClearColor");
    glad_glClear = (PFNGLCLEARPROC)get_proc("glClear");
    glad_glGetString = (PFNGLGETSTRINGPROC)get_proc("glGetString");
    glad_glViewport = (PFNGLVIEWPORTPROC)get_proc("glViewport");
    
    return glad_glClearColor != NULL;
#else
    return 0;
#endif
}
