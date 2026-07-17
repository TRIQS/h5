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
Low-level bridge between :mod:`h5.archive` and the compiled ``_h5py`` module.

This module translates the high-level, dict-like archive operations into calls
on the C++ HDF5 handles exposed by the compiled ``h5._h5py`` extension: opening
groups, reading and writing datasets and attributes, and creating groups and
soft links. It is an implementation detail -- users interact with
:class:`~h5.archive.HDFArchive` rather than with this layer directly.
"""

import numpy,string
from .archive import *
from . import _h5py as h5
 
class HDFArchiveGroupBasicLayer:
    """
    Low-level group operations backing :class:`~h5.archive.HDFArchiveGroup`.

    Wraps a single HDF5 group handle from the compiled ``h5._h5py`` extension and
    exposes the primitive operations (querying, reading/writing datasets and
    attributes, creating groups and soft links) that the high-level dict-like
    interface is built on. 
    
    This is the base class of :class:`~h5.archive.HDFArchiveGroup`. Users should not 
    instantiate it directly.

    Parameters
    ----------
    parent : HDFArchive or HDFArchiveGroup
        Object whose ``_group`` handle and ``options`` are used as the base.
    subpath : str
        Name of the subgroup to open; if empty, ``parent``'s group is reused.
    """
    _class_version = 1

    def __init__(self, parent, subpath ):
        self.options = parent.options
        self._group = parent._group.open_group(subpath) if subpath else parent._group
        self.ignored_keys = [] 
        self.cached_keys = list(self._group.keys())

    def _init_root(self, descriptor, open_flag) :
        if descriptor is None:
            try :
                fich = h5.File()
            except :
                print("Cannot open the HDF file in memory")
                raise
        elif isinstance(descriptor, bytes):
            try :
                fich = h5.File(descriptor)
            except :
                print("Cannot open the HDF file in memory from the provided bytes")
                raise
        else:
            assert(isinstance(descriptor, str))
            try :
                fich = h5.File(descriptor, open_flag)
            except :
                print("Cannot open the HDF file %s"%descriptor)
                raise

        # checking the version
        if open_flag not in ['r','r+','a'] :
            self._version = self._class_version
        else :
            try :
                self._version = int(fich.attrs['HDFArchive_Version'])
            except :
                self._version = 1
            if self._version > self._class_version :
                raise IOError("File %s is too recent for this version of HDFArchive module"%descriptor)
        self._group = h5.Group(fich)

    def is_group(self,p) :
        """
        Return whether ``p`` names a subgroup of this group.

        Parameters
        ----------
        p : str
            Entry name (must be non-empty and not start with ``'/'``).

        Returns
        -------
        bool
            True if ``p`` exists and is an HDF5 subgroup.
        """
        assert len(p)>0 and p[0]!='/'
        return p in self.cached_keys and self._group.has_subgroup(p)

    def is_data(self,p) :
        """
        Return whether ``p`` names a dataset (leaf) of this group.

        Parameters
        ----------
        p : str
            Entry name (must be non-empty and not start with ``'/'``).

        Returns
        -------
        bool
            True if ``p`` exists and is an HDF5 dataset.
        """
        assert len(p)>0 and p[0]!='/'
        return p in self.cached_keys and self._group.has_dataset(p)

    def write_attr (self, key, val) :
        """
        Write an HDF5 attribute on this group.

        Parameters
        ----------
        key : str
            Attribute name.
        val : object
            Attribute value.
        """
        self._group.write_attribute(key, val)

    def _read (self, key):
        return h5.h5_read_bare(self._group, key)

    def _write(self, key, val) :
        h5.h5_write_bare(self._group, key, val)
        # keep the key cache in sync so the dataset is visible in the same session
        self.cached_keys.append(key)

    def _flush(self):
        if bool(self._group): self._group.get_file().flush()

    def create_group (self,key):
        """
        Create a new empty subgroup.

        Parameters
        ----------
        key : str
            Name of the subgroup to create.
        """
        self._group.create_group(key)
        self.cached_keys.append(key)

    def create_softlink (self,target_key,key,delete_if_exists=True):
        """
        Create an HDF5 soft link ``key`` pointing at ``target_key``.

        Parameters
        ----------
        target_key : str
            Path the link should point to.
        key : str
            Name of the soft link to create.
        delete_if_exists : bool, optional
            If True (default), replace an existing entry named ``key``.
        """
        self._group.create_softlink(target_key,key,delete_if_exists)
        self.cached_keys.append(key)

    def keys(self) :
        """
        Return the names of the entries in this group.

        Returns
        -------
        list of str
            The (cached) keys of the group's subgroups and datasets.
        """
        return self.cached_keys

    def _clean_key(self,key, report_error=False) :
        if report_error and key not in self.cached_keys :
             raise KeyError("Key %s is not in archive !!"%key)
        if key in self.cached_keys :
          # FIXME
          # del self._group[key]
          self.cached_keys.remove(key)
        else: raise KeyError("Key %s is not in archive !!"%key)

