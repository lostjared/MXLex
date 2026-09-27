#!/usr/bin/env python3
# /// script
# requires-python = ">=3.11"
# dependencies = ["pcons>=0.24"]
# ///
"""Pcons build for MXLex.

Usage::

    uvx pcons -B build/pcons --reconfigure
    uvx pcons -B build/pcons VARIANT=debug
    uvx pcons -B build/pcons PROGRAMS=0
    uvx pcons -B build/pcons all install

Set ``PCONS_INSTALL_PREFIX`` to stage somewhere other than ``dist`` and
``PCONS_FINAL_PREFIX`` when the final runtime prefix differs from the staging
location.
"""

from pathlib import Path

from pcons import Project, find_c_toolchain, get_platform, get_var
from pcons.core.subst import SourcePath, TargetPath

VERSION = "0.1.0"

project_dir = Path(__file__).parent.resolve()
platform = get_platform()


def option(name: str, default: bool = False) -> bool:
    """Read an ON/OFF build option."""
    return get_var(name, "1" if default else "0").lower() in (
        "1",
        "on",
        "true",
        "yes",
    )


project = Project("MXLex", root_dir=project_dir)
env = project.Environment(toolchain=find_c_toolchain())
env.cxx.set_standard(23)
env.set_variant(get_var("VARIANT", "release"))
env.cxx.flags.extend(["-Wall", "-Wextra", "-pedantic"])

if platform.is_linux:
    # DrvFS rounds copy2-preserved mtimes down, leaving installed outputs
    # older than their inputs. GNU install gives outputs fresh timestamps.
    env.install.copycmd = ["install", "-D", SourcePath(), TargetPath()]

mxlex = project.StaticLibrary(
    "mxlex",
    env,
    sources=[
        project_dir / "source" / "token.cpp",
        project_dir / "source" / "scanner.cpp",
    ],
)
mxlex.public.include_dirs.append(project_dir)

if option("PROGRAMS", default=True):
    test_lex = project.Program(
        "test-lex",
        env,
        sources=[project_dir / "main.cpp"],
    )
    test_lex.link(mxlex)

final_prefix = get_var(
    "PCONS_FINAL_PREFIX",
    get_var("PCONS_INSTALL_PREFIX", str(project_dir / "dist")),
)
pc_file = project.generate_pc_file(
    mxlex,
    version=VERSION,
    description="C++23 lexical analyzer library",
    install_prefix=final_prefix,
)
public_headers = sorted((project_dir / "MXLex").glob("*.hpp"))

project.Alias(
    "install",
    project.Install("lib", [mxlex], mode=0o644),
    project.Install("include/MXLex", public_headers, mode=0o644),
    project.Install("lib/pkgconfig", [pc_file], mode=0o644),
)
