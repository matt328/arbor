using Glade.Store;

namespace Glade.Dialog
{
  internal readonly struct AssetDialogConfig
  {
    public required AssetType AssetType { get; init; }
    public required string Caption { get; init; }
    public string? InitialDirectory { get; init; }
  }
}