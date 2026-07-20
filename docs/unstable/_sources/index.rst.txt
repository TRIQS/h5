.. _welcome:

h5
**

.. sidebar:: h5 |PROJECT_VERSION|

   This is the homepage of h5 |PROJECT_VERSION|.
   For changes see the :doc:`changelog page <ChangeLog>`.

   .. image:: _static/logo_github.png
      :width: 75%
      :align: center
      :target: https://github.com/TRIQS/h5


``h5`` is a high-level C++20 interface to the `HDF5 <https://www.hdfgroup.org/solutions/hdf5/>`_
library, together with a dict-like Python interface built on top of it. It provides RAII
resource management, generic read/write for STL containers and multi-dimensional arrays,
and an extensible ADL-based mechanism to make your own types HDF5-serializable.

It is a part of the `TRIQS <https://triqs.github.io>`_ project.

To get started, see the :doc:`installation guide <install>` or learn how to use ``h5`` in the 
:ref:`documentation`.


.. toctree::
   :maxdepth: 2
   :hidden:

   install
   documentation
   issues
   ChangeLog
   about
