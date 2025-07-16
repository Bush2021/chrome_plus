includes("VC-LTL5.lua")

add_rules("mode.debug", "mode.release")

set_warnings("more")

add_defines("WIN32", "_WIN32", "UNICODE", "_UNICODE")
add_cxflags("/utf-8")
set_languages("c++20")
set_fpmodels("precise") -- Default

if is_mode("release") then
    set_exceptions("none")
    set_optimize("smallest")
    set_runtimes("MD")
    add_defines("NDEBUG")
    add_cxflags("/MP", "/fp:precise")
    add_ldflags("/DYNAMICBASE", "/LTCG")
end

if is_mode("debug") then
    set_optimize("none")
    set_runtimes("MTd")
    set_symbols("debug")
    add_defines("_DEBUG")
    add_cxflags("/MP")
    add_ldflags("/DYNAMICBASE")
end

target("detours")
    set_kind("static")
    add_files("detours/src/*.cpp|uimports.cpp")
    add_includedirs("detours/src", {public=true})

target("chrome_plus")
    set_kind("shared")
    set_targetdir("$(builddir)/$(mode)")
    set_basename("version")
    add_deps("detours")
    add_files("src/*.cc")
    add_files("src/*.rc")
    add_links("onecore", "propsys", "oleacc")
    after_build(function (target)
        if is_mode("release") then
            for _, file in ipairs(os.files("$(builddir)/release/*")) do
                if not file:endswith("dll") then
                    os.rm(file)
                end
            end
        end
    end)