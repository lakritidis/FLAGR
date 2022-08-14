from distutils.core import setup
from setuptools import find_packages

DESCRIPTION = 'PyFLAGR is a Python package for aggregating ranked preference lists from multiple sources.'
LONG_DESCRIPTION =  'FLAGR is a high performing, open-source, modular and cross-platform library for rank aggregation. '\
                    'The core FLAGR library is written in C++ and implements a wide collection of unsupervised rank ' \
                    'aggregation methods. Its modular design allows third-party programmers to implement their own ' \
                    'algorithms and easily integrate them within the library. FLAGR can be built as a standard ' \
                    'console application, or as a shared library (that is, .so or .dll file). In the second case, it ' \
                    'can be linked from other C/C++ programs, or from programs written in other languages (like ' \
                    'Python, R, PHP, etc.).\n\n' \
                    'In this context, PyFLAGR is a Python library that links to FLAGR and allows an analyst to exploit '\
                    'the efficient FLAGR implementations from a standard Python program.\n\n\n' \
                    'FLAGR Website: [http://flagr.mywork.gr/](http://flagr.mywork.gr/)\n\n' \
                    'GitHub repository: [https://github.com/lakritidis/FLAGR](https://github.com/lakritidis/FLAGR)\n\n'

setup(
    name='pyflagr',
    version='1.0.5',
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
    package_data={'': ['flagr.so', 'flagr.dll', 'libgcc_s_seh-1.dll', 'libstdc++-6.dll']}
)
