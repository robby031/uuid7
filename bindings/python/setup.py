from setuptools import setup, find_packages

setup(
    name="uuid7",
    version="1.0.1",
    description="UUID v7 library with monotonic ordering - Python binding",
    packages=find_packages(),
    setup_requires=["cffi>=1.0.0"],
    install_requires=["cffi>=1.0.0"],
    cffi_modules=["uuid7_ffi_build.py:ffi"],
    python_requires=">=3.7",
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
)
