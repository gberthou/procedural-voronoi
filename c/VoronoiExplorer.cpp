#include <Python.h>

#include "VoronoiExplorer.h"

VoronoiExplorer::VoronoiExplorer(PyObject *voronoiModule, const std::string &filename, size_t density)
{
    auto constructor = PyObject_GetAttrString(voronoiModule, "VoronoiExplorer");

    auto _filename = PyUnicode_FromString(filename.c_str());
    auto _density = PyLong_FromSize_t(density);
    auto args = PyTuple_Pack(2, _filename, _density);
    _self = PyObject_CallObject(constructor, args);
    _loadChunk      = PyObject_GetAttrString(_self, "load_chunk");
    _unloadChunk    = PyObject_GetAttrString(_self, "unload_chunk");
    _keepOnlyChunks = PyObject_GetAttrString(_self, "keep_only_chunks");
    _loadedShapes   = PyObject_GetAttrString(_self, "loaded_shapes");
    _noise          = PyObject_GetAttrString(_self, "noise");
    _noiseGet       = PyObject_GetAttrString(_noise, "get");

    Py_DECREF(args);
    Py_DECREF(_filename);
    Py_DECREF(_density);
    Py_DECREF(constructor);
}

VoronoiExplorer::~VoronoiExplorer()
{
    Py_DECREF(_self);
    Py_DECREF(_loadChunk);
    Py_DECREF(_unloadChunk);
    Py_DECREF(_keepOnlyChunks);
    Py_DECREF(_loadedShapes);
    Py_DECREF(_noiseGet);
    Py_DECREF(_noise);
}

void VoronoiExplorer::LoadChunk(const Key &key)
{
    auto keyx = PyLong_FromSsize_t(key.keyx);
    auto keyy = PyLong_FromSsize_t(key.keyy);
    auto tuple = PyTuple_Pack(2, keyx, keyy);
    auto args = PyTuple_Pack(1, tuple);

    PyObject_CallObject(_loadChunk, args);

    Py_DECREF(args);
    Py_DECREF(tuple);
    Py_DECREF(keyx);
    Py_DECREF(keyy);
}

void VoronoiExplorer::UnloadChunk(const Key &key)
{
    auto keyx = PyLong_FromSsize_t(key.keyx);
    auto keyy = PyLong_FromSsize_t(key.keyy);
    auto tuple = PyTuple_Pack(2, keyx, keyy);
    auto args = PyTuple_Pack(1, tuple);

    PyObject_CallObject(_unloadChunk, args);

    Py_DECREF(args);
    Py_DECREF(tuple);
    Py_DECREF(keyx);
    Py_DECREF(keyy);
}

void VoronoiExplorer::KeepOnlyChunks(const std::vector<Key> &keys)
{
    auto keyset = PySet_New(NULL);
    for(const auto &key : keys)
    {
        auto keyx = PyLong_FromSsize_t(key.keyx);
        auto keyy = PyLong_FromSsize_t(key.keyy);
        auto tuple = PyTuple_Pack(2, keyx, keyy);
        PySet_Add(keyset, tuple);

        Py_DECREF(tuple);
        Py_DECREF(keyx);
        Py_DECREF(keyy);
    }
    auto args = PyTuple_Pack(1, keyset);
    
    PyObject_CallObject(_keepOnlyChunks, args);

    Py_DECREF(args);
    Py_DECREF(keyset);
}

void VoronoiExplorer::LoadedShapes(std::vector<VoronoiFace> &faces)
{
    auto list = PyObject_CallObject(_loadedShapes, NULL);
    Py_ssize_t size = PyList_Size(list);

    faces.clear();
    faces.reserve(size);

    for(Py_ssize_t i = 0; i < size; ++i)
    {
        auto shape = PyList_GetItem(list, i);
        auto vertices = PyObject_GetAttrString(shape, "vertices");
        if(!PyList_Check(vertices))
        {
            Py_DECREF(vertices);
            Py_DECREF(shape);
            continue;
        }
        Py_ssize_t lenVertices = PyList_Size(vertices);

        VoronoiFace face;

        for(Py_ssize_t j = 0; j < lenVertices; ++j)
        {
            auto vertex = PyList_GetItem(vertices, j);
            auto _x = PyTuple_GetItem(vertex, 0);
            auto _y = PyTuple_GetItem(vertex, 1);
            double x = PyFloat_AsDouble(_x);
            double y = PyFloat_AsDouble(_y);

            face.emplace_back(x, y);

            Py_DECREF(_x);
            Py_DECREF(_y);
            Py_DECREF(vertex);
        }
        Py_DECREF(vertices);
        Py_DECREF(shape);

        faces.push_back(face);
    }
}

double VoronoiExplorer::GetNoiseAt(double x, double y)
{
    auto _x = PyFloat_FromDouble(x);
    auto _y = PyFloat_FromDouble(y);
    auto args = PyTuple_Pack(2, _x, _y);

    auto _ret = PyObject_CallObject(_noiseGet, args);
    double ret = PyFloat_AsDouble(_ret);

    Py_DECREF(_ret);
    Py_DECREF(args);
    Py_DECREF(_x);
    Py_DECREF(_y);

    return ret;
}
