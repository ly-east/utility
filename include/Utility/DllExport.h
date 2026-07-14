#ifndef UTILITY_DLLEXPORT_H
#define UTILITY_DLLEXPORT_H

#ifdef _WIN32
#ifdef DllExport
#undef DllExport
#endif
#if UTILITY_DLL
#define DllExport __declspec(dllexport)
#else
#define DllExport
#endif // UTILITY_DLL
#else
#ifdef DllExport
#undef DllExport
#endif
#define DllExport
#endif // _WIN32

#endif // UTILITY_DLLEXPORT_H
