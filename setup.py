# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = "0.0.1"

ext_modules = [
    Pybind11Extension(
        "piper_phonemize_cpp",
        ["src/python.cpp"],
        define_macros=[("VERSION_INFO", __version__)],
        include_dirs=["espeak-ng/build/include"],
        library_dirs=["espeak-ng/build/lib"],
        libraries=["espeak-ng"],
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
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
)
