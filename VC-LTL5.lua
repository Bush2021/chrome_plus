target("VC-LTL-5")
    set_kind("phony")
    before_build("windows", function (target)
        local function find_in_file()
            for _, dir in ipairs(os.dirs("$(projectdir)/*")) do
                name = dir:match(".*\\(.*)")
                if name:find("VC%-LTL") then
                    return dir .. [[\build\native\]]
                end
            end
        end
        local function find_in_reg()
            return vformat("$(reg HKEY_CURRENT_USER\\Code\\VC-LTL;Root)")
        end
        local VC_LTL_Root = find_in_file() or find_in_reg()
        if not VC_LTL_Root or #VC_LTL_Root == 0 then
            return
        end
        local WindowsTargetPlatformMinVersion = "6.0.6000.0"
        cprint("${color.warning}VC-LTL Path : %s", VC_LTL_Root)
        cprint("${color.warning}WindowsTargetPlatformMinVersion : %s", WindowsTargetPlatformMinVersion)
        import("core.tool.toolchain")
        local tcname = target:toolchain() or "msvc"
        local toolinst = toolchain.load(tcname)
        local runenvs = toolinst:runenvs()
        local includepath = VC_LTL_Root .. [[TargetPlatform\header;]] .. VC_LTL_Root .. [[TargetPlatform\]] .. WindowsTargetPlatformMinVersion..[[\header;]]
        runenvs.INCLUDE = includepath .. (runenvs.INCLUDE or "")
        local arch = target:arch()
        local archpath = "Win32"
        if arch ~= "x86" then
            archpath = arch
        end
        cprint("${color.warning}Platform : %s", archpath)
        local libpath = VC_LTL_Root .. [[TargetPlatform\]] .. WindowsTargetPlatformMinVersion..[[\lib\]] .. archpath .. ";"
        runenvs.LIB = libpath .. (runenvs.LIB or "")
        -- print(runenvs.INCLUDE)
        -- print(runenvs.LIB)
    end)