using Interop;
using System;
using System.Runtime.InteropServices;

namespace App {
    internal class Program {

        [StructLayout(LayoutKind.Sequential)]
        public struct MSG {
            public IntPtr hwnd;
            public uint message;
            public IntPtr wParam;
            public IntPtr lParam;
            public uint time;
            public int pt_x;
            public int pt_y;
        }

        [DllImport("user32.dll")]
        static extern bool GetMessage(out MSG lpMsg, IntPtr hWnd, uint wMsgFilterMin, uint wMsgFilterMax);

        [DllImport("user32.dll")]
        static extern bool TranslateMessage(ref MSG lpMsg);

        [DllImport("user32.dll")]
        static extern IntPtr DispatchMessage(ref MSG lpMsg);

        static void Main(string[] args) {
            Interop.Core core = new();
            core.OnEvent += Obj_OnSomethingHappened;
            MSG msg;
            while (GetMessage(out msg, IntPtr.Zero, 0, 0)) {
                TranslateMessage(ref msg);
                DispatchMessage(ref msg);
            }
        }
        private static void Obj_OnSomethingHappened() {
            Console.WriteLine("App: Event received");
        }

    }
}
