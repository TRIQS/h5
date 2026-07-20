# Copyright (c) 2019-2020 Simons Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Dict-like Python interface to HDF5 files.

This module implements the high-level, dictionary-like API for reading and
writing HDF5 archives. Values are stored and retrieved by key like a Python
``dict``.

Standard containers (:class:`list`, :class:`tuple`, :class:`dict`) are
supported out of the box, and custom classes can be made archive-compatible by
registering them with :mod:`h5.formats`.
"""

import sys,numpy, warnings, json
from importlib import import_module
from .archive_basic_layer import HDFArchiveGroupBasicLayer
from .formats import register_class, register_backward_compatibility_method, get_format_info

# -------------------------------------------
#
#  Various wrappers for basic python types.
#
# --------------------------------------------
class List:
    """
    Archive wrapper for a Python ``list``.

    Adapts a list to the archive reduce/reconstruct protocol so it can be stored
    as an HDF5 subgroup: elements are keyed by their (stringified) index on write
    and reassembled in order on read. 
    
    Instances are created internally by :class:`HDFArchiveGroup` and the class is 
    registered with :mod:`h5.formats`.

    Parameters
    ----------
    ob : list
        The list to wrap.
    """
    def __init__(self,ob) :
        self.ob = ob
    def __reduce_to_dict__(self) :
        """Return the elements as a ``{index -> value}`` dict for storage."""
        return {str(n):v for n,v in enumerate(self.ob)}
    @classmethod
    def __factory_from_dict__(cls, name, D) :
        """Rebuild the list from a ``{index -> value}`` dict ``D``."""
        return [x for n,x in sorted([(int(n), x) for n,x in list(D.items())])]

class Tuple:
    """
    Archive wrapper for a Python ``tuple``.

    Behaves like :class:`List` but reconstructs a ``tuple`` on read. 
    
    Instances are created internally by :class:`HDFArchiveGroup` and the class 
    is registered with :mod:`h5.formats`.

    Parameters
    ----------
    ob : tuple
        The tuple to wrap.
    """
    def __init__(self,ob) :
        self.ob = ob
    def __reduce_to_dict__(self) :
        """Return the elements as a ``{index -> value}`` dict for storage."""
        return {str(n):v for n,v in enumerate(self.ob)}
    @classmethod
    def __factory_from_dict__(cls, name, D) :
        """Rebuild the tuple from a ``{index -> value}`` dict ``D``."""
        return tuple(x for n,x in sorted([(int(n), x) for n,x in list(D.items())]))

class Dict:
    """
    Archive wrapper for a Python ``dict``.

    Adapts a dict to the archive reduce/reconstruct protocol, storing each
    ``key -> value`` pair in an HDF5 subgroup. 
    
    Instances are created internally by :class:`HDFArchiveGroup` and the class 
    is registered with :mod:`h5.formats`.

    Parameters
    ----------
    ob : dict
        The dict to wrap.
    """
    def __init__(self,ob) :
        self.ob = ob
    def __reduce_to_dict__(self) :
        """Return a shallow copy of the mapping for storage."""
        # Only string keys are supported, Non-string keys are rejected
        bad = sorted({type(k).__name__ for k in self.ob if not isinstance(k, str)})
        if bad : raise TypeError("HDFArchive can only store dicts with string keys, found key type(s): %s"%", ".join(bad))
        return dict(self.ob)
    @classmethod
    def __factory_from_dict__(cls, name, D) :
        """Rebuild the dict from the stored ``{key -> value}`` mapping ``D``."""
        return {n:x for n,x in list(D.items())}

register_class(List)
register_backward_compatibility_method('PythonListWrap', 'List')

register_class(Tuple)
register_backward_compatibility_method('PythonTupleWrap', 'Tuple')

register_class(Dict)
register_backward_compatibility_method('PythonDictWrap', 'Dict')

# -------------------------------------------
#
#  A view of a subgroup of the archive
#
# --------------------------------------------

class HDFArchiveGroup(HDFArchiveGroupBasicLayer):
    """
    A view on a subgroup of an :class:`HDFArchive`.

    Exposes a dict-like interface (``__getitem__``, ``__setitem__``,
    ``__delitem__``, ``__iter__``, ``keys``, ``values``, ``items``) over the
    keys of one HDF5 group. On read, registered Python classes are
    automatically reconstructed via their ``__factory_from_dict__`` (see
    :mod:`h5.formats`); use :meth:`get_raw` to bypass reconstruction.

    Ordering
    --------
    Group members are iterated in ascending name order (HDF5 is queried with
    ``H5_ITER_INC``), which is deterministic and stable across HDF5/Python
    versions -- unlike the previous native iteration order.

    A ``dict`` (or any object reconstructed via ``__reduce_to_dict__`` /
    ``__factory_from_dict__`` whose result depends on ordering) round-trips in
    its original insertion order rather than name order: the key order is
    recorded in a ``__dict_key_order__`` attribute on write and replayed on
    read. Archives that lack this attribute (older files, or files written by
    other tools) fall back to name order. On read the attribute must list
    exactly the group's keys; a mismatch indicates a corrupt or externally
    edited archive and raises :class:`ValueError`.

    Only string keys are supported for stored ``dict`` objects; non-string keys
    raise :class:`TypeError` on write rather than being silently stringified.

    Parameters
    ----------
    parent : HDFArchive or HDFArchiveGroup
        The archive or group this view is opened relative to.
    subpath : str
        Name of the subgroup to open. If empty, this group aliases ``parent``'s
        group (used for the root view).
    """
    _wrappedType = {
        list : List,
        tuple : Tuple,
        dict : Dict
    }
    _MaxLengthKey = 500

    def __init__(self, parent, subpath) :
        # We want to hold a reference to the parent group, if we are not at the root
        # This will prevent a premature destruction of the root HDFArchive object
        if not self is parent: self.parent = parent
        self.options = parent.options
        HDFArchiveGroupBasicLayer.__init__(self, parent, subpath)
        self.options = parent.options
        self.key_as_string_only = self.options['key_as_string_only']
        self._reconstruct_python_objects = self.options['reconstruct_python_object']
        self.is_top_level = False

    #-------------------------------------------------------------------------
    def __contains__(self,key) :
        """
        Return whether ``key`` is an entry of the group.

        Parameters
        ----------
        key : str
            Entry name to test.

        Returns
        -------
        bool
            True if ``key`` is present.
        """
        return key in list(self.keys())

    #-------------------------------------------------------------------------
    def values(self) :
        """
        Iterate over the values stored in the group.

        Returns
        -------
        generator
            A generator yielding each value in the group (reconstructed as a
            Python object where applicable, like :meth:`__getitem__`).
        """
        def res() :
            for name in list(self.keys()) :
                yield self[name]
        return res()

   #-------------------------------------------------------------------------
    def items(self) :
        """
        Iterate over the ``(key, value)`` pairs stored in the group.

        Returns
        -------
        generator
            A generator yielding ``(key, value)`` tuples, with values
            reconstructed as in :meth:`__getitem__`.
        """
        def res() :
            for name in list(self.keys()):
                yield name, self[name]
        return res()

    #-------------------------------------------------------------------------
    def __iter__(self) :
        """
        Iterate over the keys of the group, like a dictionary.

        Yields
        ------
        str
            Each key in the group.
        """
        def res() :
            for name in list(self.keys()) :
                yield name
        return res()

    #-------------------------------------------------------------------------
    def __len__(self) :
        """
        Return the number of keys in the group.

        Returns
        -------
        int
            The number of entries (subgroups and datasets) in this group.
        """
        return  len(list(self.keys()))

    #-------------------------------------------------------------------------
    def update(self,object_with_dict_protocol):
        """
        Copy all ``(key, value)`` pairs of a mapping into the group.

        Parameters
        ----------
        object_with_dict_protocol : mapping
            Any object exposing an ``items()`` method (e.g. a ``dict`` or another
            :class:`HDFArchiveGroup`). Each pair is written via ``self[key] = value``.
        """
        for k,v in list(object_with_dict_protocol.items()) : self[k] = v

    #-------------------------------------------------------------------------
    def __delitem__(self,key) :
        """
        Remove the entry ``key`` from the group.

        Parameters
        ----------
        key : str
            Name of the entry to delete.

        Raises
        ------
        KeyError
            If ``key`` is not present.
        """
        self._clean_key(key,True)

    #-------------------------------------------------------------------------
    def __setitem__(self,key,val) :
        """
        Write ``val`` into the group under ``key``.

        ``list``/``tuple``/``dict`` values are wrapped for HDF5 storage. Objects
        exposing ``__write_hdf5__`` or ``__reduce_to_dict__`` use those protocols.
        Numpy arrays and scalars are written directly; another :class:`HDFArchiveGroup` 
        is copied recursively.

        Parameters
        ----------
        key : str
            Entry name. Must not contain ``'/'``.
        val : object
            Value to store.

        Raises
        ------
        KeyError
            If ``key`` already exists and overwriting is disabled.
        """
        assert '/' not in key, "/ can not be part of a key"

        if key in list(self.keys()) :
            if self.options['do_not_overwrite_entries'] : raise KeyError("key %s already exist."%key)
            self._clean_key(key) # clean things

        # Transform list, dict, etc... into a wrapped type that will allow HDF reduction
        if type(val) in self._wrappedType: val = self._wrappedType[type(val)](val)

        # write the attributes
        def write_attributes(g) :
           """Use the _hdf5_format_ if it exists otherwise the class name"""
           ds = val._hdf5_format_ if hasattr(val,"_hdf5_format_") else val.__class__.__name__
           try :
             get_format_info(ds)
           except :
             err = """
               You are trying to store an object of type "%s", with the format "%s".
               This format is not registered, so you will not be able to reread the class.
               Didn't you forget to register your class in h5.formats?
               """ %(val.__class__.__name__,ds)
             raise IOError(err)
           g.write_attr("Format", ds)

        if hasattr(val,'__write_hdf5__') : # simplest protocol
            val.__write_hdf5__(self._group,key)
            self.cached_keys.append(key) # I need to do this here
            # Should be done in the __write_hdf5__ function
            #SubGroup = HDFArchiveGroup(self,key)
            #write_attributes(SubGroup)
        elif hasattr(val,'__reduce_to_dict__') : # Is it a HDF_compliant object
            d = val.__reduce_to_dict__()
            if not isinstance(d,dict) : raise ValueError(" __reduce_to_dict__ method does not return a dict. See the doc !")
            self.create_group(key) # create a new group
            SubGroup = HDFArchiveGroup(self,key)
            for k, v in list(d.items()) : SubGroup[k] = v
            write_attributes(SubGroup)
            # Preserve the key order of the reduced dict, which HDF5 does not (it
            # iterates group members by name). This covers plain dicts and any
            # __factory_from_dict__ class whose reconstruction depends on the order.
            SubGroup.write_attr('__dict_key_order__', json.dumps(list(d.keys()), ensure_ascii=False))
        elif isinstance(val,numpy.ndarray) : # it is a numpy
            try :
               self._write( key, numpy.array(val,copy=1,order='C') )
            except RuntimeError:
               print("HDFArchive is in trouble with the array %s"%val)
               raise
        elif isinstance(val, HDFArchiveGroup) : # will copy the group recursively
            # we could add this for any object that has .items() in fact...
            self.create_group(key) # create the target group first (open_group requires it to exist)
            SubGroup = HDFArchiveGroup(self, key)
            for k,v in list(val.items()) : SubGroup[k]=v
        else : # anything else... expected to be a scalar
            try :
               self._write( key, val)
            except:
               raise #ValueError, "Value %s\n is not of a type suitable to storage in HDF file"%val
        self._flush()

    #-------------------------------------------------------------------------
    def get_raw (self,key):
        """
        Return the entry at ``key`` without reconstructing a Python object.

        Unlike :meth:`__getitem__`, no registered class is reconstructed. 
        A subgroup is returned as a plain :class:`HDFArchiveGroup`.

        Parameters
        ----------
        key : str
            Name of the entry in this group.

        Returns
        -------
        HDFArchiveGroup or object
            The raw subgroup or dataset value.
        """
        return self.__getitem1__(key,False)

    #-------------------------------------------------------------------------
    def __getitem__(self,key) :
        """
        Return the entry at ``key``, reconstructing a Python object when possible.

        If ``key`` contains ``'/'`` it is treated as a path and traversed through
        the intermediate subgroups. Registered classes are reconstructed via their
        ``__factory_from_dict__`` (see :mod:`h5.formats`).

        Parameters
        ----------
        key : str
            Entry name, or a ``'/'``-separated path relative to this group.

        Returns
        -------
        object
            The stored value, a reconstructed Python object, or an
            :class:`HDFArchiveGroup` for a subgroup.

        Raises
        ------
        KeyError
            If ``key`` does not exist in the group.
        """
        # If the key contains /, grabs the subgroups
        if '/' in key:
            a,l =self, key.split('/')
            for s in l[:-1]: a = a.get_raw(s)
            return a[l[-1]]
        return self.__getitem1__(key,self._reconstruct_python_objects)

    #-------------------------------------------------------------------------
    def __getitem1__(self, key, reconstruct_python_object, hdf5_format = None) :
        """
        Internal helper implementing :meth:`__getitem__` for a single key.

        Parameters
        ----------
        key : str
            Entry name (no ``'/'`` traversal).
        reconstruct_python_object : bool
            If True, reconstruct a registered Python class from the entry;
            otherwise return the raw subgroup or dataset.
        hdf5_format : str, optional
            Format string to use instead of reading it from the entry; used to
            propagate backward-compatibility formats during recursion.

        Returns
        -------
        object
            The reconstructed object, the raw value, or an :class:`HDFArchiveGroup`.

        Raises
        ------
        KeyError
            If ``key`` does not exist or is of unknown type.
        """
        if key not in self :
            raise KeyError("Key %s does not exist."%key)

        if self.is_group(key) :
            SubGroup = HDFArchiveGroup(self,key) # View of the subgroup
            bare_return = lambda: SubGroup
        elif self.is_data(key) :
            bare_return = lambda: self._read(key)
        else :
            raise KeyError("Key %s is of unknown type !!"%key)

        if not reconstruct_python_object : return bare_return()

        # try to find the format
        if hdf5_format is None:
            hdf5_format = self._group.read_hdf5_format_from_key(key)
            if hdf5_format == "":
                return bare_return()

        try :
            fmt_info = get_format_info(hdf5_format)
        except KeyError:
            warnings.warn(f"The hdf5 format {hdf5_format} is not recognized. Returning as a group. Did you forget to import this python class ?")
            return bare_return()

        r_class_name  = fmt_info.classname
        r_module_name = fmt_info.modulename
        r_readfun = fmt_info.read_fun
        if not (r_class_name and r_module_name) : return bare_return()
        try:
            r_class = getattr(import_module(r_module_name),r_class_name)
        except KeyError:
            raise RuntimeError("I cannot find the class %s to reconstruct the object !"%r_class_name)
        if r_readfun:
            return r_readfun(self._group, key)
        if hasattr(r_class,"__factory_from_dict__"):
            assert self.is_group(key), "__factory_from_dict__ requires a subgroup"
            reconstruct = lambda k: SubGroup.__getitem1__(k, reconstruct_python_object, fmt_info.backward_compat.get(k, None))
            values = {k: reconstruct(k) for k in SubGroup._reconstruction_key_order()}
            return r_class.__factory_from_dict__(key, values)

        raise ValueError("Impossible to reread the class %s for group %s and key %s"%(r_class_name,self, key))

    #-------------------------------------------------------------------------
    def _reconstruction_key_order(self):
        """Order the keys for python-object reconstruction. Reduced objects store
        their key order at write time (see __setitem__); when present it must list
        exactly the group's keys, and we return that order. Absent (older archives,
        files from other tools) we keep the group's deterministic name order."""
        order_str = self._group.read_attribute('__dict_key_order__')
        if not order_str : return list(self.keys())
        order = json.loads(order_str)
        if set(order) != set(self.keys()) :
            raise ValueError("Corrupt archive: __dict_key_order__ %r does not match the group keys %r"%(order, list(self.keys())))
        return order

    #---------------------------------------------------------------------------
    def __str__(self) :
        """
        Return a human-readable listing of the group's entries.

        Returns
        -------
        str
            One line per entry, marking each as a subgroup or data.
        """
        def pr(name) :
            if self.is_group(name) :
                return "%s : subgroup"%name
            elif self.is_data(name) : # can be an array of a number
                return "%s : data "%name
            else :
                raise ValueError("oopps %s"%name)

        s= "HDFArchive%s with the following content:\n"%(" (partial view)" if self.is_top_level else '')
        s+='\n'.join([ '  '+ pr(n) for n in list(self.keys()) ])
        return s

    #-------------------------------------------------------------------------
    def __repr__(self) :
        """Return the same listing as :meth:`__str__`."""
        return self.__str__()

    # These two methods are necessary for "with"
    def __enter__(self):
        """Enter a ``with`` block and return this group."""
        return self
    def __exit__(self, type, value, traceback):
        """Exit a ``with`` block (no-op for a subgroup view)."""
        pass

# -------------------------------------------
#
#  The main class
#
# --------------------------------------------

class HDFArchive(HDFArchiveGroup):
    r"""
    Top-level handle to an HDF5 file.

    Opens a local path, a remote URL (read-only), an in-memory buffer, or a fresh 
    in-memory file, and exposes its contents through the :class:`HDFArchiveGroup` 
    dict-like interface. 
    
    Supports ``with`` blocks and a context-managed lifetime. 
    
    The underlying file is closed in ``__del__`` / ``__exit__``.

    Parameters
    ----------
    descriptor : str or bytes, optional
        * If ``descriptor`` is a simple string, it is interpreted as a local file name.
        * If ``descriptor`` is a remote url (e.g.
          ``http://ipht.cea.fr/triqs/data/single_site_bethe.output.h5``) then the h5
          file is downloaded as a temporary file and opened. In that case,
          ``open_flag`` must be ``'r'`` (read-only). The temporary file is deleted at exit.
        * If ``descriptor`` is a bytes object, the bytes are interpreted as an hdf5
          file and opened in memory only. Here ``open_flag`` must keep its default ``'a'``.
        * If ``descriptor`` is None (default), a new hdf5 file is created in memory only.
          Here ``open_flag`` must keep its default ``'a'``.
    open_flag : str, optional
        Opening mode, one of ``'r'`` (read-only), ``'w'`` (truncate/write) or ``'a'`` 
        (read-write, default). Memory files require ``'a'``.
    key_as_string_only : bool, optional
        If True (default), keys are stored as plain strings.
    reconstruct_python_object : bool, optional
        If True (default), registered Python classes are reconstructed on read. If False, 
        entries are returned as raw datasets / subgroups.
    init : iterable of (key, value), optional
        Any generator of ``(key, value)`` tuples, e.g. ``dict.items()``. The archive is 
        filled with these values on construction.

    Examples
    --------
    >>> # retrieve a remote archive (in read-only mode)
    >>> h = HDFArchive('http://ipht.cea.fr/triqs/data/single_site_bethe.output.h5')
    >>>
    >>> # full copy of an archive
    >>> HDFArchive(f, 'w', init = HDFArchive(fmp, 'r').items())
    >>>
    >>> # partial copy of the file fmp, keeping only the key 'G'
    >>> HDFArchive(f, 'w', init = [(k, v) for (k, v) in HDFArchive(fmp, 'r') if k in ['G']])
    >>>
    >>> # faster: objects are retrieved lazily (generator instead of list)
    >>> HDFArchive(f, 'w', init = ((k, v) for (k, v) in HDFArchive(fmp, 'r') if k in ['G']))
    >>>
    >>> # partial copy with on-the-fly processing via a function P
    >>> HDFArchive(f, 'w', init = ((k, P(v)) for (k, v) in HDFArchive(fmp, 'r') if k in ['G']))
    """
    _class_version = 1

    def __init__(self, descriptor = None, open_flag = 'a', key_as_string_only = True,
            reconstruct_python_object = True, init = {}):
        assert isinstance(descriptor,(str,bytes)) or descriptor is None, "descriptor must be a string or bytes"
        assert open_flag in ['r','w','a'], "Invalid mode"

        if isinstance(descriptor, bytes) or descriptor is None:
            assert open_flag == 'a', "Memory files require read-write mode 'a'"
            self._init_root(descriptor, None)
            LocalFileName = "MemoryBuffer"

        elif isinstance(descriptor, str):
            import os,os.path

            # If it is a url, retrieve it and check mode is read only
            import urllib.request
            try:
                LocalFileName, http_message = urllib.request.urlretrieve(descriptor)
                # a url must be read only
                assert open_flag == 'r', "You retrieve a distant Url %s which is not local, so it must be read-only. Use 'r' option"%descriptor
            except ValueError: # Not a valid URL -> Local File
                LocalFileName, http_message = descriptor, None

            if open_flag == 'w':
                # destroys the file, ignoring errors
                try: os.remove(os.path.abspath(LocalFileName))
                except OSError: pass

            self._init_root(LocalFileName, open_flag)

        self.options = {'key_as_string_only' : key_as_string_only,
                        'do_not_overwrite_entries' : False,
                        'reconstruct_python_object': reconstruct_python_object,
                        'UseAlpsNotationForComplex'  : True
                        }
        HDFArchiveGroup.__init__(self,self,"")
        self.is_top_level = True
        for k,v in init : self[k]=v

    def as_bytes(self):
      """
      Serialize the underlying HDF5 file to an in-memory byte buffer.

      Returns
      -------
      bytes
          A copy of the whole HDF5 file, suitable e.g. for MPI broadcast or for
          re-opening with ``HDFArchive(buffer)``.
      """
      return self._group.get_file().as_buffer()

    def __del__(self):
      """Flush and close the underlying HDF5 file when the archive is destroyed."""
      # We must ensure the root group is closed before closing the file
      if hasattr(self, '_group'):
          self._flush()
          del self._group

    # These two methods are necessary for "with"
    def __enter__(self):
      """Enter a ``with`` block and return this archive."""
      return self

    def __exit__(self, type, value, traceback):
      """Flush and close the underlying HDF5 file on leaving a ``with`` block."""
      self._flush()
      del self._group

#--------------------------------------------------------------------------------

class HDFArchiveInert:
    """
    A fake class for the node in MPI. It does nothing, but permits to write simply::

        a = mpi.bcast(H['a'])  # run on all nodes

    - ``__getitem__`` returns self so that ``H['a']['b']`` is ok...
    - ``__setitem__`` does nothing.
    """
    def HDFArchive_Inert(self):
        """No-op placeholder; this class intentionally performs no I/O."""
        pass

    def __getitem__(self,x)   :
        """Return ``self`` so that chained access such as ``H['a']['b']`` is a no-op.

        Parameters
        ----------
        x : object
            Ignored key.

        Returns
        -------
        HDFArchiveInert
            This same inert object.
        """
        return self
    
    def __setitem__(self,k,v) :
        """Ignore the assignment (no-op)."""
        pass

#--------------------------------------------------------------------------------


