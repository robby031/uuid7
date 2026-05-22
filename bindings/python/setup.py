from setuptools import setup, find_packages

setup(
    name="uuid7",
    version="1.0.0",
    description="UUID v7 library with monotonic ordering - Python binding",
    packages=find_packages(),
    package_data={
        "uuid7": [
            "libuuid7.so",
            "libuuid7.dylib",
            "uuid7.dll",
        ]
    },
    python_requires=">=3.7",
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
)