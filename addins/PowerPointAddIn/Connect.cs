using System;
using System.Runtime.InteropServices;
using Extensibility;
using MyDll1Native;

namespace PowerPointAddIn
{
    [ComVisible(true)]
    [Guid("A1B2C3D4-E5F6-7890-ABCD-111111111111")]
    [ClassInterface(ClassInterfaceType.None)]
    public class Connect : IDTExtensibility2
    {
        public void OnConnection(object application, ext_ConnectMode connectMode, object addInInst, ref Array custom)
        {
            try
            {
                int init = MyDll1.Initialize();
                if (init != 0)
                    System.Diagnostics.Debug.WriteLine("PowerPointAddIn: MyDll1_Initialize returned " + init);
                else
                    System.Diagnostics.Debug.WriteLine("PowerPointAddIn: MyDll1 loaded and initialized.");
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine("PowerPointAddIn OnConnection: " + ex.Message);
            }
        }

        public void OnDisconnection(ext_DisconnectMode disconnectMode, ref Array custom)
        {
            try
            {
                MyDll1.Shutdown();
                System.Diagnostics.Debug.WriteLine("PowerPointAddIn: MyDll1 shutdown.");
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine("PowerPointAddIn OnDisconnection: " + ex.Message);
            }
        }

        public void OnAddInsUpdate(ref Array custom) { }

        public void OnStartupComplete(ref Array custom) { }

        public void OnBeginShutdown(ref Array custom) { }
    }
}
