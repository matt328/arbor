using System.Collections.ObjectModel;

namespace Glade.Store
{
  internal class EditorStore
  {
    public ObservableCollection<Asset> Models { get; set; } = [];
    public ObservableCollection<Asset> Animations { get; set; } = [];
    public ObservableCollection<Asset> Skeletons { get; set; } = [];
  }
}
