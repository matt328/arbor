using Glade.Store;
using System.Threading.Tasks;

namespace Glade.Dialog
{
  internal interface IAssetDialogService
  {
    /// <summary>
    /// Shows the asset creation dialog and returns the chosen Asset, or null if cancelled.
    /// </summary>
    Task<Asset?> ShowNewAssetDialog(AssetDialogConfig config);
  }
}
