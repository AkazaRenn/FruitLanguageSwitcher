using System.Runtime.InteropServices;

namespace App.Interop;
internal static partial class UIAutomation {
    [StructLayout(LayoutKind.Sequential)]
    public struct CaretInfo {
        public double X;
        public double Y;
        public double Height = 0;

        public CaretInfo(double x, double y, double height) {
            X = x;
            Y = y;
            Height = height;
        }
    }

    [LibraryImport("Core.dll", EntryPoint = "GetCaretPosition", StringMarshalling = StringMarshalling.Utf16)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool GetCaretPosition(out CaretInfo caretInfo);
}
