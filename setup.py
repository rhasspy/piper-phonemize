# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext
from pathlib import Path
from setuptools import setup

_DIR = Path(__file__).parent

__version__ = "0.0.1"

ext_modules = [
    Pybind11Extension(
        "piper_phonemize_cpp",
        ["src/python.cpp", "src/phonemize.cpp", "src/phoneme_ids.cpp"],
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
    package_data={
        "piper_phonemize": [
            str(p) for p in (_DIR / "piper_phonemize" / "espeak-ng-data").rglob("*")
        ]
    },
    include_package_data=True,
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
)
