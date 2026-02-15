using System;
using System.Runtime.InteropServices;

namespace MyDll1Native
{
    /// <summary>
    /// P/Invoke wrapper for the native MyDll1.dll (used by PowerPoint and Excel add-ins).
    /// </summary>
    public static class MyDll1
    {
        private const string DllName = "MyDll1.dll";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        private static extern uint MyDll1_GetVersion();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        private static extern int MyDll1_Initialize();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        private static extern void MyDll1_Shutdown();

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        private static extern int MyDll1_GetThirdPartyCheck();

        /// <summary>Returns version as (major, minor, patch).</summary>
        public static (int Major, int Minor, int Patch) GetVersion()
        {
            uint v = MyDll1_GetVersion();
            return ((int)((v >> 16) & 0xFF), (int)((v >> 8) & 0xFF), (int)(v & 0xFF));
        }

        /// <summary>Initialize the native engine. Call once after loading the add-in.</summary>
        public static int Initialize() => MyDll1_Initialize();

        /// <summary>Shutdown the native engine. Call before unloading the add-in.</summary>
        public static void Shutdown() => MyDll1_Shutdown();

        /// <summary>Returns 1 if third-party (OpenCV) check passes; 0 otherwise.</summary>
        public static int GetThirdPartyCheck() => MyDll1_GetThirdPartyCheck();
    }
}
