using System;

namespace Glade.Store
{
  internal enum AssetType
  {
    Model,
    Animation,
    Skeleton
  }

  internal class Asset
  {
    public Guid Id { get; set; }
    public required string Name { get; set; }
    public required string Filename { get; set; }
    public required AssetType AssetType { get; set; }
  };
}
