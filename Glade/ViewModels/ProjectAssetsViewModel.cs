using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Glade.Dialog;
using Glade.Logging;
using Glade.Store;
using Microsoft.Extensions.DependencyInjection;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;

namespace Glade.ViewModels
{
  internal partial class ProjectAssetsViewModel : ObservableObject
  {
    private readonly EditorStore _store;
    private readonly IAssetDialogService _assetDialogService;

    public ObservableCollection<AssetViewModel> ModelVMs { get; } = [];
    public ObservableCollection<AssetViewModel> AnimationVMs { get; } = [];
    public ObservableCollection<AssetViewModel> SkeletonVMs { get; } = [];

    public ProjectAssetsViewModel(EditorStore store, IAssetDialogService assetDialogService)
    {
      _store = store;
      _assetDialogService = assetDialogService;
      foreach (var m in _store.Models) ModelVMs.Add(new AssetViewModel(m));
      foreach (var a in _store.Animations) AnimationVMs.Add(new AssetViewModel(a));
      foreach (var s in _store.Skeletons) SkeletonVMs.Add(new AssetViewModel(s));

      SyncCollections(_store.Models, ModelVMs);
      SyncCollections(_store.Animations, AnimationVMs);
      SyncCollections(_store.Skeletons, SkeletonVMs);
    }

    [RelayCommand]
    private async Task AddModel()
    {

      App.Services.GetRequiredService<LogStore>().Add(LogLevel.Info, "AddModel command invoked.");

      var config = new AssetDialogConfig
      {
        AssetType = AssetType.Model,
        Caption = "Import Model",
        FileTypes = [".trm", "*"]
      };

      var asset = await _assetDialogService.ShowNewAssetDialog(config);
      if (asset != null)
      {
        _store.Models.Add(asset);
      }
    }

    [RelayCommand]
    private async Task AddAnimation()
    {
      var config = new AssetDialogConfig
      {
        AssetType = AssetType.Animation,
        Caption = "Import Animation",
        FileTypes = [".ozz", "*"]
      };

      var asset = await _assetDialogService.ShowNewAssetDialog(config);
      if (asset != null)
      {
        _store.Animations.Add(asset);
      }
    }

    [RelayCommand]
    private async Task AddSkeleton()
    {
      var config = new AssetDialogConfig
      {
        AssetType = AssetType.Skeleton,
        Caption = "Import Skeleton",
        FileTypes = [".ozz", "*"]
      };

      var asset = await _assetDialogService.ShowNewAssetDialog(config);
      if (asset != null)
      {
        _store.Skeletons.Add(asset);
      }
    }

    private static void SyncCollections(
    ObservableCollection<Asset> source,
    ObservableCollection<AssetViewModel> target)
    {
      source.CollectionChanged += (_, e) =>
      {
        if (e.NewItems is not null)
        {
          foreach (Asset asset in e.NewItems)
            target.Add(new AssetViewModel(asset));
        }

        if (e.OldItems is not null)
        {
          foreach (Asset asset in e.OldItems)
          {
            var vm = target.FirstOrDefault(x => x.Id == asset.Id);
            if (vm != null)
              target.Remove(vm);
          }
        }
      };
    }
  }
}