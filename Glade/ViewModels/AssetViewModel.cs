using CommunityToolkit.Mvvm.ComponentModel;
using Glade.Store;
using System;

namespace Glade.ViewModels
{
  internal class AssetViewModel : ObservableObject
  {
    private string _name;
    private string _filename;

    public Guid Id { get; }
    public string Name
    {
      get => _name;
      set => SetProperty(ref _name, value);
    }

    public string Filename
    {
      get => _filename;
      set => SetProperty(ref _filename, value);
    }

    public AssetViewModel(Asset asset)
    {
      Id = asset.Id;
      _name = asset.Name;
      _filename = asset.Filename;
    }
  }
}
