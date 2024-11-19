#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject *cvarint_encode(PyObject *self, PyObject *args) {
  PyObject* py_n;
  unsigned long long n;
  unsigned char out[10]; // varints can be a max of 10 bytes

  // assign n and check if malloc fail
  if (!PyArg_ParseTuple(args, "O", &py_n)) { return NULL; }

  // try conversion to c u_long_long
  n = PyLong_AsUnsignedLongLong(py_n);
  if (n == (unsigned long long)-1 && PyErr_Occurred()) { return NULL; }

  int idx = 0;
  while (n) {
    int part = n & 0x7f;
    n = n >> 7;
    if (n != 0) {
      part |= 0x80;
    }
    out[idx] = part;
    idx++;
  }
 
  PyObject* bytes = PyBytes_FromStringAndSize((const char*)out, idx);

  return bytes;
}

static PyObject *cvarint_decode(PyObject *self, PyObject *args) {
  const char* bytes;
  Py_ssize_t bytes_size;

  // parse args into a byte array
  if (!PyArg_ParseTuple(args, "y#", &bytes, &bytes_size)) { return NULL; }

  unsigned long long n = 0;
  int shift = 0;

  for (Py_ssize_t i = 0; i < bytes_size; i++) {
    unsigned char byte = bytes[i];
    n |= ((unsigned long long)(byte & 0x7f)) << shift;
    if (!(byte & 0x80)) { break; } // if no continuation byte, exit loop
    shift += 7;
  }

  PyObject* num = PyLong_FromUnsignedLongLong(n);
  if (!num) { return NULL; }

  return num;
}

static PyMethodDef CVarintMethods[] = {
    {"encode", cvarint_encode, METH_VARARGS, "Encode an integer as varint."},
    {"decode", cvarint_decode, METH_VARARGS,
     "Decode varint bytes to an integer."},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef cvarintmodule = {
    PyModuleDef_HEAD_INIT, "cvarint",
    "A C implementation of protobuf varint encoding", -1, CVarintMethods};

PyMODINIT_FUNC PyInit_cvarint(void) { return PyModule_Create(&cvarintmodule); }
