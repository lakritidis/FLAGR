from distutils.core import setup
from setuptools import find_packages

DESCRIPTION = "PyFLAGR is a Python package for aggregating ranked preference lists from multiple sources."
LONG_DESCRIPTION = "FLAGR is a high performing, modular library for rank aggregation. To ensure the highest possible performance, the core FLAGR library is written in C++ and implements a wide collection of unsupervised rank aggregation methods. Its modular design allows third-party programmers to implement their own algorithms and easily rebuild the entire library. FLAGR can be built as a standard application, or as a shared library (so or dll). In the second case, it can be linked from other C/C++ programs, or even from programs written in other languages (e.g. Python, PHP, etc.). In this context, PyFLAGR is a Python library that links to FLAGR and allows a developer to exploit the efficient FLAGR implementations from a standard Python program."

setup(
    name='pyflagr',
    version='1.0.2',
    description=DESCRIPTION,
    long_description=LONG_DESCRIPTION,
    long_description_content_type='text/markdown',
    author="Leonidas Akritidis",
    author_email="lakritidis@ihu.gr",
    maintainer="Leonidas Akritidis",
    maintainer_email="lakritidis@ihu.gr",
    packages=find_packages(),
    url='https://github.com/lakritidis/FLAGR',
    install_requires=["pandas"],
    license="Apache",
    keywords=[
        "rank aggregation", "rank fusion", "data fusion", "unsupervised learning", "information retrieval",
        "metasearch", "metasearch engines", "borda count", "condorcet", "kendall", "spearman"],
    py_modules=["flagr"],
    package_data={'': ['flagr.so', 'flagr.dll']}
)
