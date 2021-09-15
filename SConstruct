import kconfiglib
import os
import multiprocessing
from pathlib import Path
import platform
import subprocess


def PhonyTargets(
    target,
    action,
    depends,
    env=None,
):
    # Creates a Phony target
    if not env:
        env = DefaultEnvironment()
    t = env.Alias(target, depends, action)
    env.AlwaysBuild(t)


class SillySpawnForUselessWindows:
    def ourspawn(self, sh, escape, cmd, args, env):
        newargs = ' '.join(args[1:])
        cmdline = cmd + " " + newargs
        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        proc = subprocess.Popen(cmdline, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, startupinfo=startupinfo, shell=False, env=env)
        data, err = proc.communicate()
        rv = proc.wait()
        if rv:
            print("=====")
            print(err)
            print("=====")
        return rv


def correct_spawn(env):
    if platform.system() == 'Windows':
        buf = SillySpawnForUselessWindows()
        buf.ourenv = env
        env['SPAWN'] = buf.ourspawn


def getKconfig(kconfig):
    if os.path.isfile('sdkconfig'):
        config = kconfiglib.Kconfig(kconfig)
        config.load_config('sdkconfig')
    else:
        print('Not configured!')
        exit(1)
    return config


def generate_sdkconfig_header(target, source, env):
    content = """
#ifndef SDKCONFIG_H_INCLUDED
#define SDKCONFIG_H_INCLUDED
// Automatically generated file. Do not edit.

    """
    for config in list(
            map(lambda x: getKconfig(str(x)),
                [kconf for kconf in source if 'Kconfig' in str(kconf)])):
        config.load_config('sdkconfig')

        for (key, sym) in [(x, config.syms[x]) for x in config.syms.keys()]:
            if sym.type == kconfiglib.BOOL and sym.str_value == 'y':
                content += "#define CONFIG_{} 1\n".format(key)
            elif sym.type == kconfiglib.INT:
                content += "#define CONFIG_{} {}\n".format(key, sym.str_value)
            elif sym.type == kconfiglib.STRING:
                if key in ['LV_USER_DATA_FREE', 'LV_MEM_CUSTOM_FREE', 'LV_MEM_CUSTOM_ALLOC', 'LV_TICK_CUSTOM_SYS_TIME_EXPR']:
                    value = sym.str_value.replace('"', '')
                else:
                    value = f'"{sym.str_value}"'
                content += '#define CONFIG_{} {}\n'.format(key, value)

    content += "#endif"

    with open(str(target[0]), "w") as f:
        f.write(content)


MINGW = platform.system() == "Windows"
PROGRAM = "simulated.exe" if MINGW else "simulated"
MAIN = "main"
SIMULATOR = 'simulator'
COMPONENTS = "components"
LVGL = f"{COMPONENTS}/lvgl"
FREERTOS = f'{SIMULATOR}/freertos-simulator'
CJSON = f'{SIMULATOR}/cJSON'
B64 = f'{SIMULATOR}/b64'

CFLAGS = [
    "-Wall",
    "-Wextra",
    "-Wno-unused-function",
    "-g",
    "-O0",
    "-DLV_CONF_INCLUDE_SIMPLE",
    "-DLV_LVGL_H_INCLUDE_SIMPLE",
    '-DprojCOVERAGE_TEST=1',
    "-DLV_KCONFIG_IGNORE",
    "-Wno-unused-parameter",
    "-static-libgcc",
    "-static-libstdc++",
]
LDLIBS = ["-lmingw32", "-lSDL2main",
          "-lSDL2"] if MINGW else ["-lSDL2"] + ['-lpthread']

CPPPATH = [
    COMPONENTS, f'{SIMULATOR}/port', f'#{MAIN}',
    f"#{MAIN}/config", f"#{SIMULATOR}", B64, CJSON,
    f"#{LVGL}",
    f"#{LVGL}/lvgl",
]


def main():
    num_cpu = multiprocessing.cpu_count()
    SetOption('num_jobs', num_cpu)
    print("Running with -j {}".format(GetOption('num_jobs')))

    env_options = {
        "ENV": os.environ,
        "CC": ARGUMENTS.get('cc', 'gcc'),
        "ENV": os.environ,
        "CPPPATH": CPPPATH,
        'CPPDEFINES': [],
        "CCFLAGS": CFLAGS,
        "LIBS": LDLIBS,
    }

    env = Environment(**env_options, tools=['mingw'] if MINGW else None)
    correct_spawn(env)
    env.Tool('compilation_db')

    gel_env = env
    gel_selected = ['pagemanager', 'collections', 'data_structures']
    (gel_objects, include) = SConscript(
        f'{COMPONENTS}/gel/SConscript', exports=['gel_env', 'gel_selected'])
    env['CPPPATH'] += [include]

    freertos_env = env
    (freertos, include) = SConscript(
        f'{FREERTOS}/SConscript', exports=['freertos_env'])
    env['CPPPATH'] += [include]

    sdkconfig = env.Command(
        f"{SIMULATOR}/sdkconfig.h",
        [str(filename) for filename in Path(
            'components').rglob('Kconfig')] + ['sdkconfig'],
        generate_sdkconfig_header)

    sources = Glob(f'{SIMULATOR}/*.c')
    sources += Glob(f'{SIMULATOR}/port/*.c')
    sources += Glob(f'{SIMULATOR}/indev/*.c')
    sources += Glob(f'{SIMULATOR}/display/*.c')
    sources += [File(filename) for filename in Path('main/model').rglob('*.c')]
    sources += [File(filename)
                for filename in Path('main/config').rglob('*.c')]
    sources += [File(filename) for filename in Path('main/view').rglob('*.c')]
    sources += [File(filename)
                for filename in Path('main/controller').rglob('*.c')]
    sources += [
        File(filename) for filename in Path(f'{LVGL}/customization').rglob('*.c')
    ]
    sources += [
        File(filename) for filename in Path(f'{LVGL}/lvgl/src').rglob('*.c')
    ]
    sources += [File(f'{CJSON}/cJSON.c')]
    sources += [File(f'{B64}/encode.c'),
                File(f'{B64}/decode.c'), File(f'{B64}/buffer.c')]

    prog = env.Program(PROGRAM, sources + freertos + gel_objects)
    PhonyTargets('run', './simulated', prog, env)
    env.Alias('mingw', prog)
    env.CompilationDatabase('build/compile_commands.json')


main()
