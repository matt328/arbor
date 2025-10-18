namespace Glade
{
  using System.Runtime.InteropServices;

  [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
  public delegate void EngineLogCallback(int level, string message);

  internal static partial class NativeMethods
  {
    [LibraryImport("Arbor.dll")]
    [UnmanagedCallConv(CallConvs = new System.Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
    public static partial void initialize();

    [LibraryImport("Arbor.dll")]
    [UnmanagedCallConv(CallConvs = new System.Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
    public static partial void shutdown();

    [LibraryImport("Arbor.dll")]
    [UnmanagedCallConv(CallConvs = new System.Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
    public static partial void initializeLogging();

    [LibraryImport("Arbor.dll")]
    [UnmanagedCallConv(CallConvs = new System.Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
    public static partial void registerLogCallback(EngineLogCallback cb);
  }
}
