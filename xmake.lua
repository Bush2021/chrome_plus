add_rules("mode.debug", "mode.release")

set_warnings("more")

add_defines("WIN32", "_WIN32", "UNICODE", "_UNICODE")
set_encodings("source:utf-8")
set_languages("c++23")
set_fpmodels("precise") -- Default
if is_mode("release") then
    set_exceptions("none")
    set_optimize("smallest")
    set_runtimes("MT")
    add_requires("vc-ltl5")
    add_defines("NDEBUG")
    add_cxflags("/MP")
    add_ldflags("/DYNAMICBASE")
    set_policy("build.optimization.lto", true)
end

if is_mode("debug") then
    set_runtimes("MTd")
    add_defines("_DEBUG")
    add_cxflags("/MP")
    add_ldflags("/DYNAMICBASE")
end

target("detours")
    set_kind("static")
    add_includedirs("detours/src", {public=true})
    add_files(
        "detours/src/detours.cpp",
        "detours/src/disasm.cpp",
        "detours/src/image.cpp",
        "detours/src/modules.cpp"
    )
    if is_arch("x86") then
        add_defines("_X86_")
        add_files("detours/src/disolx86.cpp")
    elseif is_arch("x64") then
        add_defines("_AMD64_")
        add_files("detours/src/disolx64.cpp")
    elseif is_arch("arm64") then
        add_defines("_ARM64_")
        add_files("detours/src/disolarm64.cpp")
    end

target("mini_gzip")
    set_kind("static")
    add_includedirs("mini_gzip", {public = true})
    add_files("mini_gzip/miniz.c", "mini_gzip/mini_gzip.c")

target("chrome_plus")
    set_kind("shared")
    set_targetdir("$(builddir)/$(mode)")
    set_basename("version")
    add_deps("detours")
    add_deps("mini_gzip")
    add_files("src/*.cc")
    add_files("src/*.rc")
    add_links("onecore", "propsys", "oleacc")
    if is_mode("release") then
        add_packages("vc-ltl5")
    end
    after_build(function (target)
        if is_mode("release") then
            for _, file in ipairs(os.files("$(builddir)/release/*")) do
                if not file:endswith("dll") then
                    os.rm(file)
                end
            end
        end
    end)