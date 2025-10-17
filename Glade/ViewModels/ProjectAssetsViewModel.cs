using Glade.Command;
using Glade.Dialog;
using Glade.Store;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows.Input;

namespace Glade.ViewModels
{
  internal partial class ProjectAssetsViewModel : ObservableObject
  {
    private readonly EditorStore _store;
    private readonly IAssetDialogService _assetDialogService;

    public ObservableCollection<AssetViewModel> ModelVMs { get; } = [];
    public ObservableCollection<AssetViewModel> AnimationVMs { get; } = [];
    public ObservableCollection<AssetViewModel> SkeletonVMs { get; } = [];

    public ICommand AddModelCommand { get; }

    public ProjectAssetsViewModel(EditorStore store, IAssetDialogService assetDialogService)
    {
      _store = store;
      _assetDialogService = assetDialogService;
      foreach (var m in _store.Models) ModelVMs.Add(new AssetViewModel(m));
      foreach (var a in _store.Animations) AnimationVMs.Add(new AssetViewModel(a));
      foreach (var s in _store.Skeletons) SkeletonVMs.Add(new AssetViewModel(s));

      _store.Models.CollectionChanged += (_, e) =>
      {
        if (e.NewItems != null)
        {
          foreach (Asset model in e.NewItems)
          {
            ModelVMs.Add(new AssetViewModel(model));
          }
        }
        if (e.OldItems != null)
        {
          foreach (Asset model in e.OldItems)
          {
            var vm = ModelVMs.FirstOrDefault(x => x.Id == model.Id);
            if (vm != null)
            {
              ModelVMs.Remove(vm);
            }
          }
        }
      };

      AddModelCommand = new RelayCommand(_ => AddModel());
    }

    private async void AddModel()
    {
      var config = new AssetDialogConfig
      {
        AssetType = AssetType.Model,
        Caption = "Import Model",
        InitialDirectory = _store.LastModelPath,
      };

      var asset = await _assetDialogService.ShowNewAssetDialog(config);
      if (asset != null)
      {
        _store.Models.Add(asset);
      }
    }
  }
}