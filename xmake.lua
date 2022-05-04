includes("VC-LTL5.lua")

add_rules("mode.debug", "mode.release")

set_warnings("more")

add_defines("WIN32", "_WIN32")
add_defines("UNICODE", "_UNICODE", "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_DEPRECATE")

if is_mode("release") then
    add_defines("NDEBUG")
    add_cxflags("/O2", "/Os", "/Gy", "/MT", "/EHsc", "/fp:precise")
    add_ldflags("/DYNAMICBASE", "/LTCG")
end

add_cxflags("/utf-8")

add_links("gdiplus", "kernel32", "user32", "gdi32", "winspool", "comdlg32")
add_links("advapi32", "shell32", "ole32", "oleaut32", "uuid", "odbc32", "odbccp32")

target("minhook")
    set_kind("static")
    add_files("minhook/src/**.c")
    add_includedirs("minhook/include", {public=true})

target("chrome_plus")
    set_kind("shared")
    set_targetdir("$(buildir)/release")
    set_basename("version")
    add_deps("minhook")
    add_files("src/*.cpp")
    add_files("src/*.rc")
    add_links("user32")
    after_build(function (target)
        os.rm("$(buildir)/release/version.exp")
        os.rm("$(buildir)/release/version.lib")
    end)
