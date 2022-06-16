from distutils.core import setup
from setuptools import find_packages

DESCRIPTION = "PyFLAGR is a Python package for aggregating ranked preference lists from multiple sources."
LONG_DESCRIPTION = "PyFLAGR is a Python package for aggregating ranked preference lists from multiple sources."

setup(
    name='pyflagr',
    version='1.0',
    description=DESCRIPTION,
    long_description=LONG_DESCRIPTION,
    long_description_content_type='text/markdown',
    author="Leonidas Akritidis",
    author_email="lakritidis@ihu.gr",
    maintainer="Leonidas Akritidis",
    maintainer_email="lakritidis@ihu.gr",
    packages=find_packages(),
    install_requires=["pandas==1.4.2"],
    license="Apache",
    keywords=[
        "rank aggregation", "rank fusion", "data fusion", "unsupervised learning", "information retrieval",
        "metasearch", "metasearch engines", "borda count", "condorcet", "kendall", "spearman"],
    py_modules=["flagr"],
    package_data={'': ['flagr.so', 'flagr.dll']}
)