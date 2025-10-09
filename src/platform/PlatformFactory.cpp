#include "platform/IPlatform.h"
#include <memory>

#ifdef __APPLE__
#include "platform/MacOSPlatform.h"
#elif defined(_WIN32)
#include "platform/WindowsPlatform.h"
#elif defined(__linux__)
#include "platform/LinuxPlatform.h"
#endif

namespace Platform
{

    std::unique_ptr<IPlatform> CreatePlatform()
    {
#ifdef __APPLE__
        return std::make_unique<MacOSPlatform>();
#elif defined(_WIN32)
        return std::make_unique<WindowsPlatform>();
#elif defined(__linux__)
        return std::make_unique<LinuxPlatform>();
#else
#error "Unsupported platform"
#endif
    }

} // namespace Platform
