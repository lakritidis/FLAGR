from distutils.core import setup
from setuptools import find_packages

DESCRIPTION = 'PyFLAGR is a Python package for aggregating ranked preference lists from multiple sources.'
LONG_DESCRIPTION = 'The fusion of multiple ranked lists of elements into a single aggregate list is a well-studied '\
    'research field with numerous applications in Bioinformatics, recommendation systems, collaborative filtering, '\
    'election systems and metasearch engines.\n\n' \
    'FLAGR is a high performance, modular, open source library for rank aggregation problems. It implements baseline '\
    'and recent state-of-the-art aggregation algorithms that accept ranked preference lists and generate a single '\
    'consensus list of elements. A portion of these methods apply exploratory analysis techniques and belong to the '\
    'broad family of unsupervised learning techniques.\n\n' \
    'PyFLAGR is a Python library built on top of FLAGR library core. It can be easily installed with pip and used in '\
    'standard Python programs and Jupyter notebooks.\n\n\n' \
    'FLAGR Website: [https://flagr.site/](https://flagr.site/)\n\n' \
    'GitHub repository: [https://github.com/lakritidis/FLAGR](https://github.com/lakritidis/FLAGR)\n\n'

setup(
    name='pyflagr',
    version='1.0.8',
    description=DESCRIPTION,
    long_description=LONG_DESCRIPTION,
    long_description_content_type='text/markdown',
    author="Leonidas Akritidis",
    author_email="lakritidis@ihu.gr",
    maintainer="Leonidas Akritidis",
    maintainer_email="lakritidis@ihu.gr",
    packages=find_packages(),
    url='https://github.com/lakritidis/FLAGR',
    install_requires=["pandas", "matplotlib"],
    license="Apache",
    keywords=[
        "rank aggregation", "rank fusion", "data fusion", "unsupervised learning", "information retrieval",
        "metasearch", "metasearch engines", "borda count", "condorcet", "kendall", "spearman"],
    py_modules=["flagr"],
    package_data={'': ['flagr.so', 'flagr.dll', 'libgcc_s_seh-1.dll', 'libstdc++-6.dll']}
)
