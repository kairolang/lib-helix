-- Project setup
set_project("core")
set_version("0.0.1-alpha+5932a", {soname = false})
set_description("Kairo Core Lib")
add_rules("mode.debug", "mode.release")

-- ABI and runtime detection
local function get_abi()
    if os.host() == "windows" then
        return "msvc"
    elseif os.host() == "linux" then
        return "gnu"
    elseif os.host() == "macosx" then
        return "llvm"
    end
    return "unknown"
end

local function get_runtime(abi)
    if abi == "msvc" then
        return "MT"
    else
        return "static" -- For non-MSVC, attempt static where possible
    end
end

-- Build configuration
local function setup_debug()
    set_symbols("debug")
    set_optimize("none")
    add_defines("DEBUG", "H_DEBUG_M")
    set_warnings("all", "extra")
end

local function setup_release()
    set_symbols("hidden")
    set_optimize("aggressive")
    add_defines("NDEBUG")
    set_warnings("none")
end

local function setup_build_folder(abi)
    set_targetdir("$(buildir)/$(mode)/$(arch)-" .. abi .. "-$(os)/lib")
end

local function setup_env(abi, runtime)
    if os.host() == "windows" then
        add_cxflags("/MT")
    elseif os.host() == "macosx" then
        add_ldflags("-lc++") -- macOS default C++ runtime
    else
        add_ldflags("-static-libstdc++")
    end

    if is_mode("debug") then
        setup_debug()
    else
        setup_release()
    end

    set_policy("build.across_targets_in_parallel", true)
    set_runtimes(runtime)
end

-- Source and header setup
local function core_setup()
    add_includedirs("core")
    add_headerfiles("core/**.h")
    add_headerfiles("core/**.hh")
    add_headerfiles("core/**.tpp")
    
end

-- Determine ABI and runtime
local abi = get_abi()
local runtime = get_runtime(abi)

-- Setup environment and build folder
setup_build_folder(abi)
setup_env(abi, runtime)
core_setup()

-- Main library target
target("lib-helix")
    set_targetdir("$(buildir)/$(mode)/$(arch)-" .. abi .. "-$(os)/lib")