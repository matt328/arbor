using Glade.Store;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Threading.Tasks;

namespace Glade.Dialog
{
  internal class AssetDialogService : IAssetDialogService
  {
    private readonly Window _window;

    public AssetDialogService(Window window)
    {
      _window = window;
    }

    public async Task<Asset?> ShowNewAssetDialog(AssetDialogConfig config)
    {
      var dialog = new AssetDialog(config, _window);
      if (_window.Content is FrameworkElement fe)
      {
        dialog.XamlRoot = fe.XamlRoot;
      }
      var result = await dialog.ShowAsync();

      if (result == ContentDialogResult.Primary)
      {
        return new Asset
        {
          Id = Guid.NewGuid(),
          Name = dialog.AssetName,
          Filename = dialog.Filename,
          AssetType = config.AssetType
        };
      }

      return null;
    }

  }
}
