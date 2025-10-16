namespace Glade
{
  using System.Runtime.InteropServices;

  internal static partial class NativeMethods
  {
    [LibraryImport("Arbor.dll")]
    [UnmanagedCallConv(CallConvs = new System.Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
    public static partial void initialize();

    [LibraryImport("Arbor.dll")]
    [UnmanagedCallConv(CallConvs = new System.Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
    public static partial void shutdown();
  }
}
