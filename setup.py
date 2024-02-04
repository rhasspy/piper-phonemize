import platform
from pathlib import Path
import shutil
import sys

# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup
from setuptools.command.install import install

_DIR = Path(__file__).parent
_ESPEAK_DIR = _DIR / "install"
_LIB_DIR = _DIR / "lib" / f"Linux-{platform.machine()}"
_ONNXRUNTIME_DIR = _LIB_DIR / "onnxruntime"

__version__ = "1.2.0"

class CustomInstallCommand(install):
    """Customized setuptools install command to copy espeak-ng-data and specific DLLs."""

    def run(self):
        super().run()

        # This custom installation step is only necessary for Windows
        if platform.system() == "Windows":
            self.copy_espeak_ng_data()
            self.copy_specific_dlls()

    def copy_espeak_ng_data(self):
        source_data_dir = _ESPEAK_DIR / "share" / "espeak-ng-data"
        target_data_dir = Path(sys.prefix) / "Lib" / "site-packages" / "piper_phonemize" / "espeak-ng-data"

        if source_data_dir.exists():
            if target_data_dir.exists():
                shutil.rmtree(target_data_dir)
            shutil.copytree(source_data_dir, target_data_dir)
            print(f"Copied espeak-ng-data to: {target_data_dir}")

    def copy_specific_dlls(self):
        dll_files = [
            _ESPEAK_DIR / "bin" / "espeak-ng.dll",
            _ESPEAK_DIR / "lib" / "onnxruntime.dll"
        ]

        target_lib_dir = Path(sys.prefix) / "Library" / "bin" # Windows specific
        if not target_lib_dir.exists():
            print(f"Error: {target_lib_dir} does not exist and DLLs were not copied.")
            exit(1)

        for dll_path in dll_files:
            if dll_path.exists():
                shutil.copy(dll_path, target_lib_dir)
                print(f"Copied {dll_path} to: {target_lib_dir}")
            else:
                print(f"Error: {dll_path} does not exist and was not copied.")
                exit(1)


ext_modules = [
    Pybind11Extension(
        "piper_phonemize_cpp",
        [
            "src/python.cpp",
            "src/phonemize.cpp",
            "src/phoneme_ids.cpp",
            "src/tashkeel.cpp",
        ],
        define_macros=[("VERSION_INFO", __version__)],
        include_dirs=[str(_ESPEAK_DIR / "include"), str(_ONNXRUNTIME_DIR / "include")],
        library_dirs=[str(_ESPEAK_DIR / "lib"), str(_ONNXRUNTIME_DIR / "lib")],
        libraries=["espeak-ng", "onnxruntime"],
    ),
]

setup(
    name="piper_phonemize",
    version=__version__,
    author="Michael Hansen",
    author_email="mike@rhasspy.org",
    url="https://github.com/rhasspy/piper-phonemize",
    description="Phonemization libary used by Piper text to speech system",
    long_description="",
    packages=["piper_phonemize"],
    package_data={
        "piper_phonemize": [
            str(p) for p in (_DIR / "piper_phonemize" / "espeak-ng-data").rglob("*")
        ]
        + [str(_DIR / "libtashkeel_model.ort")]
    },
    include_package_data=True,
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
)
