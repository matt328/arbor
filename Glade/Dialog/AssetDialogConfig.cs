using Glade.Store;
using System.Collections.Generic;

namespace Glade.Dialog
{
  internal readonly struct AssetDialogConfig
  {
    public required AssetType AssetType { get; init; }
    public required string Caption { get; init; }
    public List<string> FileTypes { get; init; }
  }
}